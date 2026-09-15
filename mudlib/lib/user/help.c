// The help module. This will (should) give help on the required thingy.
// share and enjoy.
// nroff files removed, neverbot 06/2010

#include <mud/cmd.h>
#include <files/help.h>
#include <language.h>

#define help_dirs ({ "/lib/docs/mud/concepts/", \
                     "/lib/docs/mud/important/", \
                     "/lib/docs/helpdir/" })

#define creator_dirs ({ "/lib/docs/lfun/", \
                        "/lib/docs/efun/", \
                        "/lib/docs/coder/", \
                        "/lib/docs/driver/lpc/types/", \
                        "/lib/docs/driver/applies/", \
                        "/lib/docs/driver/concepts/", \
                        "/lib/docs/driver/lpc/constructs/", })


private int show_sections();
private int show_section(string name);
private int show_matches(string pattern);
private int show_topic(mixed * topic, string str);
private int may_read(string tier);
private string help_page(string name, string kind, string subtitle,
                         string body);

static int compare_strings(string a, string b)
{
  if (a == b) return 0;
  if (!a || !b) return 1;
  if (a > b) return 1;
  return -1;
}

// One page for every kind of help, so a document does not read like a file
// somebody printed by accident: a bar naming what is being read, what kind of
// thing it is, the text, and a bar to say where it ends. Both bars are as wide
// as the reader's screen.
private string help_page(string name, string kind, string subtitle,
                         string body)
{
  string out;
  int width;

  width = (int)this_object()->query_cols();

  out = sprintf("%p%|*s\n", '-', width,
                fix_string(_LANG_HELP_TITLE_BAR(name)));

  if (strlen(subtitle))
    out += subtitle + " " + _LANG_HELP_KIND_SEPARATOR + " " + kind + "\n\n";
  else
    out += kind + "\n\n";

  out += body;

  if (!strlen(out) || out[strlen(out) - 1] != '\n')
    out += "\n";

  return out + sprintf("%p%*s\n", '-', width, "");
}

// Whether somebody may read a document written for a given tier.
private int may_read(string tier)
{
  if (tier != HELP_TIER_CODER)
    return TRUE;

  return this_object()->player() && this_object()->player()->query_coder();
}

// What there is to read, by section. The whole index would be hundreds of
// words; the sections are what fits on a screen.
private int show_sections()
{
  string * names;
  string out;
  int i;

  names = (string *)HELP_HANDLER->query_sections();
  out = _LANG_HELP_SECTIONS_HEADER;

  for (i = 0; i < sizeof(names); i++)
  {
    string * topics;
    int shown;

    topics = (string *)HELP_HANDLER->query_section(names[i]);
    shown = sizeof(topics);

    if (!shown)
      continue;

    // a section nobody may read is not offered
    if (!may_read(((mixed *)HELP_HANDLER->query_topic(topics[0]))[1]))
      continue;

    out += sprintf("  %-14s %3d\n", names[i], shown);
  }

  out += _LANG_HELP_SECTIONS_FOOTER;

  this_object()->more_string(out, capitalize(_LANG_HELP_NAME));
  return 1;
}

// The documents of one section, with whatever each says it is about.
private int show_section(string name)
{
  string * topics;
  string out;
  int i;

  topics = (string *)HELP_HANDLER->query_section(name);
  out = "";

  for (i = 0; i < sizeof(topics); i++)
  {
    mixed * topic;

    topic = (mixed *)HELP_HANDLER->query_topic(topics[i]);
    if (!topic || !may_read(topic[1]))
      continue;

    out += sprintf("  %-18s %s\n", topics[i], topic[3]);
  }

  this_object()->more_string(
    help_page(name, _LANG_HELP_KIND_SECTION, "", out), capitalize(name));
  return 1;
}

// Every word that matches, for when only half of it is remembered.
private int show_matches(string pattern)
{
  string * words;
  string out;
  int i, shown;

  words = (string *)HELP_HANDLER->query_matching(pattern);
  out = "";

  for (i = 0; i < sizeof(words); i++)
  {
    mixed * topic;

    topic = (mixed *)HELP_HANDLER->query_topic(words[i]);
    if (!topic || !may_read(topic[1]))
      continue;

    out += sprintf("  %-18s %-10s %s\n", words[i], topic[2], topic[3]);
    shown++;
  }

  if (!shown)
  {
    notify_fail(_LANG_HELP_NO_MATCHES);
    return 0;
  }

  this_object()->more_string(_LANG_HELP_MATCHES + out, pattern);
  return 1;
}

// Read one document out. The argument is named `str` because the "no help
// about" message reads it from the caller's scope.
private int show_topic(mixed * topic, string str)
{
  string text;

  text = read_file(topic[0]);

  if (!stringp(text) || !strlen(text))
  {
    notify_fail(_LANG_HELP_NO_HELP_ABOUT);
    return 0;
  }

  // the header says what the document answers to; it is not part of it
  while (strlen(text) && text[0] == '@')
  {
    int i;

    i = strsrch(text, "\n");
    if (i == -1)
      break;
    text = text[i + 1 ..];
  }

  this_object()->more_string(
    help_page(str,
              topic[1] == HELP_TIER_CODER ? (topic[2] == HELP_DRIVER_SECTION ||
                                             topic[2] == "kfun"
                                               ? _LANG_HELP_KIND_DRIVER
                                               : _LANG_HELP_KIND_CODER_TOPIC)
                                          : _LANG_HELP_KIND_TOPIC,
              topic[3], trim(text) + "\n"),
    capitalize(str));
  return 1;
}

int do_help(string str)
{
  string s, text;
  string * files;
  string * aux;
  int i, j;
  object ob;

  files = ({ });
  aux = ({ });

  if (!strlen(str))
  {
    return show_sections();
  }

  // reindex: the documents are read from disk, so they are indexed once and
  // remembered; a new one needs the index built again
  if (member_array(str, _LANG_HELP_REINDEX) != -1 &&
      this_object()->player() && this_object()->player()->query_coder())
  {
    write(_LANG_HELP_REINDEXED(HELP_HANDLER->build_index()));
    return 1;
  }

  // a pattern, for when the word is only half remembered
  if (strsrch(str, "*") != -1 || strsrch(str, "?") != -1)
    return show_matches(str);

  /*
  if (sscanf(str, "spell %s", s) == 1 )
  {
    if ((text = this_object()->help_spell(s)) && strlen(text))
      write(text);
    else
      write("You do not know that spell.\n");
    return 1;
  }

  if ((text = this_object()->help_spell(str)) && strlen(text))
  {
    write(text);
    return 1;
  }

  if (sscanf(str, "skill %s", s) == 1 )
  {
    if ((text = this_object()->help_skill(s)) && strlen(text))
      write(text);
    else
      write("You do not know that skill.\n");
    return 1;
  }

  if ((text = this_object()->help_skill(str)) && strlen(text))
  {
    write(text);
    return 1;
  }
  */

  // check if it is a cmd
  ob = load_object(CMD_HANDLER);
  if (ob)
  {
    text = ob->query_unaliased_cmd(str);
    aux = keys(ob->query_hash());

    // what we are really looking for _is_ a cmd
    if ((ob->query_hash()[text]) && (text = ob->query_hash()[text]["file"]))
    {
      // check if the player has permissions to run the cmd
      if (member_array(text, ob->query_available_cmds(this_object()->player())) != -1)
      {
        if (ob = load_object(text))
        {
          this_object()->more_string(
            help_page(str, _LANG_HELP_KIND_COMMAND, "",
                      _LANG_CMD_SYNTAX + ob->query_usage() + "\n\n" +
                      (ob->query_help() ? wrap(ob->query_help())
                                        : _LANG_HELP_CMD_NO_HELP) + "\n"),
            capitalize(str));
          return 1;
        }
      }
    }
  }

  // check if it is a skill the player can look up (by id or translated name)
  if ((text = this_object()->player()->help_skill(str)) && strlen(text))
  {
    this_object()->more_string(
      help_page(str, _LANG_HELP_KIND_SKILL, "", text + "\n"),
      capitalize(str));
    return 1;
  }

  // a section of the index, then a document of it
  if (member_array(str, (string *)HELP_HANDLER->query_sections()) != -1)
    return show_section(str);

  {
    mixed * topic;

    topic = (mixed *)HELP_HANDLER->query_topic(str);
    if (topic && may_read(topic[1]))
      return show_topic(topic, str);
  }

  if (member_array(str, _LANG_HELP_EMOTIONS) != -1)
  {
    s = SOUL_OBJECT->query_soul_list();
    if (!strlen(s))
      write(_LANG_HELP_NO_HELP_ABOUT);
    else
      this_object()->more_string(s, _LANG_HELP_MORE_EMOTIONS);
    return 1;
  }

  /*
  aux = ({ });

  // fill an array in the form 
  // file_name_i, directory_of_file_i, i+1, i+1, i+2, i+2, etc
  for (i = 0; i < sizeof(help_dirs); i++)
  {
    if (file_size(help_dirs[i]) != -2)
      continue;
    aux = get_dir(help_dirs[i]);
    for (j = 0; j < sizeof(aux); j++)
      files += ({ aux[j], help_dirs[i] });
  }

  if (this_object()->query_coder())
    for (i = 0; i < sizeof(creator_dirs); i++)
    {
      if (file_size(creator_dirs[i]) != -2)
        continue;
      aux = get_dir(creator_dirs[i]);
      for (j = 0; j < sizeof(aux); j++)
        files += ({ aux[j], creator_dirs[i] });
    }

  // the help corresponds to a file name
  if ((i = member_array(str, files)) != -1)
  {
    s = "%^GREEN%^Help for: "+str + "%^RESET%^\n\n";
    s += read_file(files[i+1] + files[i]);

    this_object()->more_string(s + "\n", "Help");
    return 1;
  }
  */

  // finally we check if the help about some inventory item exists
  {
    object * objs;
    int flag, loop;
    flag = 0;
    objs = find_match(str, this_player());

    // if we do not have items in the inventory that match the name,
    // finally we try with the help of the emotions
    if (!sizeof(objs))
    {
      s = (string)SOUL_OBJECT->help_soul(str);

      if (!strlen(s))
      {
        notify_fail(_LANG_HELP_NO_HELP_ABOUT);
        return 0;
      }

      s = help_page(str, _LANG_HELP_KIND_SOUL, "", wrap(s));
      this_user()->set_finish_func("end_of_help");
      this_user()->more_string(s, capitalize(str));
      return 1;
    }

    for (loop = 0; loop < sizeof(objs); loop++)
    {
      if (text = (string)objs[loop]->query_help())
      {
        write(_LANG_HELP_HELP_ABOUT);
        flag = 1;
      }
    }

    if (!flag)
      write(_LANG_HELP_NO_HELP_FOR_ITEM);

    return 1;
  }

  notify_fail(_LANG_HELP_NO_HELP_ABOUT);
  return 0;

} /* do_help() */

int end_of_help()
{
  return 1;
}


/*
string search_help(string str)
{
  if (file_size(NROFF_DIR+str+".o") > 0)
    return NROFF_DIR+str;
  if (this_player()->query_coder() && file_size(CNROFF_DIR+str+".o") > 0)
    return CNROFF_DIR+str;
}

string create_help(string str)
{
  int i;
  mixed cross_ref;

  for (i = 0;i < sizeof(help_dirs); i++)
    if (file_size(help_dirs[i]+str)>0)
    {
      NROFF_HAND->create_help(help_dirs[i], str);
      return NROFF_DIR+str;
      return help_dirs[i]+str;
    }

  if (this_player()->query_coder())
    for (i = 0;i < sizeof(creator_dirs); i++)
      if (file_size(creator_dirs[i]+str)>0)
      {
        NROFF_HAND->create_chelp(creator_dirs[i], str);
        return CNROFF_DIR+str;
        return creator_dirs[i]+str;
      }

  cross_ref = read_file("/lib/docs/cross_ref");
  cross_ref = explode(cross_ref, "%");

  if ((i=member_array(str,cross_ref))==-1)
    return 0;

  return extract(cross_ref[i+1],0,strlen(cross_ref[i+1])-2);
  // use extract, not explode
}
*/

mixed * stats()
{
  return ({ });
}