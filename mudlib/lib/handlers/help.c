
// help.c -- the index of everything the help command can show.
//
// A document says what it answers to. Its first lines carry a header:
//
//   @verbs cominfo
//   @tier coder
//   @title what a character can type, and what answers
//
// and the rest is the text. The header is the only thing this handler reads
// to build the index, so a document and the words that reach it are written
// in the same place, in the same language, at the same time.
//
// Documents live in sections, one directory each, under /docs/<lang>/help/.
// The driver manual is indexed too, by file name and for coders only: those
// pages come from DGD and are not ours to annotate.
//
// The index is built once and kept; `help reindex` builds it again.

#include <files/help.h>
#include <mud/translations.h>

inherit "/lib/core/object";

// keyword -> ({ path, tier, section, title })
mapping topics;
// section -> ({ keyword, keyword, ... })
mapping sections;

int build_index();
private string lowered(string str);
private int match_pattern(string str, string pattern);
private void index_tree(string dir, string section, string tier,
                        int keep_existing);
private void index_driver();
private string * header_of(string path);

void create()
{
  // the index has to exist before ::create() runs setup()
  topics = ([ ]);
  sections = ([ ]);

  restore_object(HELP_SAVE_FILE, 1);

  ::create();
}

void setup()
{
  if (!map_sizeof(topics))
    build_index();
}

// Every letter in lower case. The mudlib's lower_case() only touches the
// first character, which is what a name needs and an index does not.
private string lowered(string str)
{
  int i;

  if (!stringp(str))
    return "";

  for (i = 0; i < strlen(str); i++)
    if (str[i] >= 'A' && str[i] <= 'Z')
      str[i] = str[i] + 'a' - 'A';

  return str;
}

// Glob matching for `*` (any run, including none) and `?` (one character).
// Recursive backtracking; both strings are short.
private int match_pattern(string str, string pattern)
{
  if (!strlen(pattern))
    return !strlen(str);

  if (pattern[0] == '*')
  {
    if (match_pattern(str, pattern[1 ..]))
      return TRUE;

    return strlen(str) && match_pattern(str[1 ..], pattern);
  }

  if (!strlen(str))
    return FALSE;

  if (pattern[0] != '?' && pattern[0] != str[0])
    return FALSE;

  return match_pattern(str[1 ..], pattern[1 ..]);
}

// The header lines of a document: everything before the first line that is
// not a @field. Only the head of the file is read; a manual page can be long.
private string * header_of(string path)
{
  string head;
  string * lines, * out;
  int i;

  head = read_file(path, 0, HELP_HEADER_BYTES);
  if (!stringp(head) || !strlen(head))
    return ({ });

  lines = explode(head, "\n");
  out = ({ });

  for (i = 0; i < sizeof(lines); i++)
  {
    string line;

    line = trim(lines[i]);
    if (!strlen(line))
      continue;
    if (line[0] != '@')
      break;

    out += ({ line });
  }

  return out;
}

// Read one directory of documents into the index. Every file is expected to
// carry a header; one without @verbs is indexed by its own name, so a
// document is never invisible just because somebody forgot the line.
private void index_tree(string dir, string section, string tier,
                        int keep_existing)
{
  string * files;
  int i, j;

  files = get_dir(dir);

  for (i = 0; i < sizeof(files); i++)
  {
    string path, title, own_tier;
    string * head, * verbs;

    // a dotfile is not a document
    if (files[i][0] == '.')
      continue;

    path = dir + files[i];

    // a subdirectory is a section of its own
    if (file_size(path) == -2)
    {
      index_tree(path + "/", files[i], tier, keep_existing);
      continue;
    }

    head = header_of(path);
    verbs = ({ });
    title = "";
    own_tier = tier;

    for (j = 0; j < sizeof(head); j++)
    {
      string field, value;

      if (sscanf(head[j], "@%s %s", field, value) != 2)
        continue;

      switch (field)
      {
      case "verbs":
        verbs += explode(value, " ") - ({ "" });
        break;
      case "tier":
        own_tier = trim(value);
        break;
      case "title":
        title = trim(value);
        break;
      }
    }

    if (!sizeof(verbs))
      verbs = ({ files[i] });

    for (j = 0; j < sizeof(verbs); j++)
    {
      string word;

      word = lowered(verbs[j]);

      // the English tree only answers for what the mud's own language does
      // not have
      if (keep_existing && topics[word])
        continue;

      topics[word] = ({ path, own_tier, section, title });

      if (!sections[section])
        sections[section] = ({ });
      if (j == 0)
        sections[section] |= ({ word });
    }
  }
}

// The DGD manual, indexed by file name. Its pages are the driver's own text.
private void index_driver()
{
  string root;
  string * dirs;
  int i;

  root = "/docs/" + HELP_DRIVER_LANG + "/driver/";
  if (file_size(root) != -2)
    return;

  dirs = get_dir(root);

  for (i = 0; i < sizeof(dirs); i++)
  {
    string path;

    if (dirs[i][0] == '.')
      continue;

    path = root + dirs[i];

    if (file_size(path) == -2)
      // the driver manual never takes a word the mud already answers with
      index_tree(path + "/", dirs[i], HELP_TIER_CODER, TRUE);
    else if (!topics[lowered(dirs[i])])
    {
      topics[lowered(dirs[i])] =
        ({ path, HELP_TIER_CODER, HELP_DRIVER_SECTION, "" });

      if (!sections[HELP_DRIVER_SECTION])
        sections[HELP_DRIVER_SECTION] = ({ });
      sections[HELP_DRIVER_SECTION] |= ({ lowered(dirs[i]) });
    }
  }
}

// Walk the documents and remember what answers to what.
int build_index()
{
  string root;

  topics = ([ ]);
  sections = ([ ]);

  root = "/docs/" + GLOBAL_COMPILE_LANG + "/help/";
  if (file_size(root) == -2)
    index_tree(root, HELP_GENERAL_SECTION, HELP_TIER_PLAYER, FALSE);

  // whatever the mud's language does not have, English still answers
  if (GLOBAL_COMPILE_LANG != "en" && file_size("/docs/en/help/") == -2)
    index_tree("/docs/en/help/", HELP_GENERAL_SECTION, HELP_TIER_PLAYER, TRUE);

  index_driver();

  save_object(HELP_SAVE_FILE, 1);
  return map_sizeof(topics);
}

// ---------------------------------------------------------------------------
// Asking
// ---------------------------------------------------------------------------

// What a word answers with, or nil. The tier is not checked here: the caller
// knows who is asking.
mixed * query_topic(string word)
{
  if (!stringp(word) || !strlen(word))
    return nil;

  return topics[lowered(word)];
}

// Every keyword matching a pattern of `*` and `?`, sorted.
string * query_matching(string pattern)
{
  string * out, * words;
  int i;

  out = ({ });
  words = map_indices(topics);
  pattern = lowered(pattern);

  for (i = 0; i < sizeof(words); i++)
    if (match_pattern(words[i], pattern))
      out += ({ words[i] });

  return sort_array(out);
}

string * query_sections() { return map_indices(sections); }

string * query_section(string name)
{
  if (!stringp(name) || !sections[name])
    return ({ });

  return sort_array(sections[name] + ({ }));
}

mapping query_topics() { return topics + ([ ]); }

int query_size() { return map_sizeof(topics); }
