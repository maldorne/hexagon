// Mail reader: one clone per session, driven by input_to while the player
// reads, writes and deletes letters. The mailbox itself lives in the postal
// handler; this object only keeps what the session needs: the letters as
// they were listed and the ones marked for deletion.
// Based on the CcMud mail client, neverbot 12/2010.
// Rewritten for Hexagon, neverbot 09/2026.

#include <files/postal.h>
#include <language.h>

inherit "/lib/core/object";

#define CHECK_OWNER_TIME 300

static object owner;
static string owner_name;
static mapping * letters;
static string * marked;
// the letter being written: to, cc, subject and the lines typed so far
static mapping draft;
static string * body;
// opened only to write one letter: close when it is sent
static int only_one;

private void show_prompt();
private void show_headers();
private void refresh_letters();
private string * recipient_names(string str);
private string query_signature();
private void start_letter(string * names, varargs string subject);
private void close_mailer();

void create()
{
  ::create();
  letters = ({ });
  marked = ({ });
  draft = ([ ]);
  body = ({ });
}

// Open the mailbox of the player, or go straight to writing a letter
// when recipients are given.
void start(object player, varargs string recipients)
{
  owner = player;
  owner_name = player->query_name();

  call_out("check_owner", CHECK_OWNER_TIME);

  refresh_letters();

  if (strlen(recipients))
  {
    only_one = 1;
    start_letter(recipient_names(recipients));
    return;
  }

  show_headers();
  show_prompt();
}

// Close a session whose player is gone or no longer answering to us.
void check_owner()
{
  if (!owner || !owner->user() || !owner->user()->query_input_to())
  {
    close_mailer();
    return;
  }

  call_out("check_owner", CHECK_OWNER_TIME);
}

private void close_mailer()
{
  dest_me();
}

// never cleaned away while someone is using it
int clean_up(varargs int flag)
{
  if (owner)
    return 1;

  return ::clean_up(flag);
}

private int valid_session()
{
  if (!owner || this_player() != owner)
  {
    close_mailer();
    return 0;
  }

  return 1;
}

private void refresh_letters()
{
  string * ids;
  int i;

  letters = POSTAL_D->query_letters(owner_name);
  ids = ({ });

  for (i = 0; i < sizeof(letters); i++)
    ids += ({ letters[i]["id"] });

  // forget marks on letters that are no longer there
  marked &= ids;
}

// Names typed by the player, split on spaces or commas, nicknames expanded.
private string * recipient_names(string str)
{
  string * names;
  int i;

  names = explode(replace_string(str, ",", " "), " ") - ({ "" });

  for (i = 0; i < sizeof(names); i++)
    names[i] = owner->expand_nickname(names[i]);

  return names;
}

private void show_prompt()
{
  write(_LANG_MAILER_PROMPT);
  input_to("mailbox_command");
}

private void show_headers()
{
  string mark, text;
  int i;

  if (!sizeof(letters))
  {
    write(_LANG_MAILER_EMPTY);
    return;
  }

  text = _LANG_MAILER_HEADERS_TITLE;

  for (i = 0; i < sizeof(letters); i++)
  {
    if (member_array(letters[i]["id"], marked) != -1)
      mark = _LANG_MAILER_MARK_DELETED;
    else if (!letters[i]["read"])
      mark = _LANG_MAILER_MARK_UNREAD;
    else
      mark = " ";

    text += sprintf("%s %3d  %-14s %-10s %s\n", mark, i + 1,
      capitalize(letters[i]["from"]), ctime(letters[i]["date"], 4)[0..7],
      letters[i]["subject"]);
  }

  write(text);
}

private string list_names(string * names)
{
  int i;
  string * result;

  result = ({ });

  for (i = 0; i < sizeof(names); i++)
    result += ({ capitalize(names[i]) });

  return implode(result, ", ");
}

private void read_letter(int number)
{
  mapping letter;
  string text;

  if (number < 1 || number > sizeof(letters))
  {
    write(_LANG_MAILER_NO_SUCH_LETTER);
    return;
  }

  letter = letters[number - 1];

  text = _LANG_MAILER_LETTER_FROM + _LANG_MAILER_LETTER_TO;
  if (sizeof(letter["cc"]))
    text += _LANG_MAILER_LETTER_CC;
  text += _LANG_MAILER_LETTER_DATE + _LANG_MAILER_LETTER_SUBJECT + "\n" +
          letter["body"] + "\n";

  write(text);

  if (!letter["read"])
  {
    POSTAL_D->mark_read(owner_name, letter["id"]);
    letter["read"] = 1;
  }
}

// Letter numbers written as "3", "1-4", "2,5" or "2 5".
private int * parse_numbers(string str)
{
  string * parts;
  int * numbers;
  int i, from, to;

  parts = explode(replace_string(str, ",", " "), " ") - ({ "" });
  numbers = ({ });

  for (i = 0; i < sizeof(parts); i++)
  {
    if (sscanf(parts[i], "%d-%d", from, to) == 2)
    {
      for (; from <= to; from++)
        numbers += ({ from });
    }
    else if (sscanf(parts[i], "%d", from) == 1)
      numbers += ({ from });
  }

  return numbers;
}

private void mark_letters(string str, int deleting)
{
  int * numbers;
  int i, count;

  numbers = parse_numbers(str);

  if (!sizeof(numbers))
  {
    write(_LANG_MAILER_WHICH_LETTERS);
    return;
  }

  for (i = 0; i < sizeof(numbers); i++)
  {
    if (numbers[i] < 1 || numbers[i] > sizeof(letters))
      continue;

    if (deleting)
      marked = (marked - ({ letters[numbers[i] - 1]["id"] })) +
               ({ letters[numbers[i] - 1]["id"] });
    else
      marked -= ({ letters[numbers[i] - 1]["id"] });

    count++;
  }

  if (!count)
    write(_LANG_MAILER_NO_SUCH_LETTER);
  else if (deleting)
    write(_LANG_MAILER_MARKED);
  else
    write(_LANG_MAILER_UNMARKED);
}

private void show_groups()
{
  mapping groups;
  string * keys;
  string text;
  int i;

  groups = POSTAL_D->query_groups(owner_name);
  keys = map_indices(groups);

  if (!sizeof(keys))
  {
    write(_LANG_MAILER_NO_GROUPS);
    return;
  }

  text = _LANG_MAILER_GROUPS_TITLE;

  for (i = 0; i < sizeof(keys); i++)
    text += sprintf("  %-14s %s\n", keys[i], list_names(groups[keys[i]]));

  write(text);
}

// "grupo nombre nombre -nombre": add the plain names, remove the ones
// written after a minus sign.
private void edit_group(string str)
{
  string * words, * adding, * removing, * done;
  string group;
  int i;

  words = explode(replace_string(lower_case(str), ",", " "), " ") - ({ "" });
  group = words[0];
  adding = ({ });
  removing = ({ });

  for (i = 1; i < sizeof(words); i++)
    if (words[i][0] == '-')
    {
      if (strlen(words[i]) > 1)
        removing += ({ words[i][1..] });
    }
    else
      adding += ({ words[i] });

  if (!sizeof(adding) && !sizeof(removing))
  {
    write(_LANG_MAILER_GROUP_SYNTAX);
    return;
  }

  if (sizeof(adding))
  {
    done = POSTAL_D->add_to_group(owner_name, group, adding);
    if (sizeof(done))
      write(_LANG_MAILER_GROUP_ADDED);
    if (sizeof(adding - done))
      write(_LANG_MAILER_GROUP_NOT_ADDED);
  }

  if (sizeof(removing))
  {
    done = POSTAL_D->remove_from_group(owner_name, group, removing);
    if (sizeof(done))
      write(_LANG_MAILER_GROUP_REMOVED);
    else
      write(_LANG_MAILER_GROUP_NOT_REMOVED);
  }
}

private void show_help()
{
  write(_LANG_MAILER_HELP);
}

void mailbox_command(string str)
{
  string verb, args;
  int number;

  if (!valid_session())
    return;

  if (!str)
    str = "";

  str = trim(str);

  if (!strlen(str))
  {
    show_prompt();
    return;
  }

  if (sscanf(str, "%d", number) == 1 && ("" + number) == str)
  {
    read_letter(number);
    show_prompt();
    return;
  }

  if (sscanf(str, "%s %s", verb, args) != 2)
  {
    verb = str;
    args = "";
  }

  verb = lower_case(verb);
  args = trim(args);

  if (member_array(verb, _LANG_MAILER_CMD_LIST) != -1)
  {
    refresh_letters();
    show_headers();
  }
  else if (member_array(verb, _LANG_MAILER_CMD_WRITE) != -1)
  {
    if (!strlen(args))
      write(_LANG_MAILER_WRITE_TO_WHOM);
    else
    {
      start_letter(recipient_names(args));
      return;
    }
  }
  else if (member_array(verb, _LANG_MAILER_CMD_REPLY) != -1)
  {
    if (sscanf(args, "%d", number) != 1 || number < 1 || number > sizeof(letters))
      write(_LANG_MAILER_NO_SUCH_LETTER);
    else
    {
      start_letter(({ letters[number - 1]["from"] }),
        _LANG_MAILER_REPLY_PREFIX + letters[number - 1]["subject"]);
      return;
    }
  }
  else if (member_array(verb, _LANG_MAILER_CMD_FORWARD) != -1)
  {
    string rest;
    mapping recipients;
    mapping letter;
    string * delivered;

    if (sscanf(args, "%d %s", number, rest) != 2 || number < 1 ||
        number > sizeof(letters))
      write(_LANG_MAILER_FORWARD_SYNTAX);
    else
    {
      recipients = POSTAL_D->expand_recipients(owner_name,
        recipient_names(rest));

      if (sizeof(recipients["unknown"]))
        write(_LANG_MAILER_UNKNOWN_RECIPIENTS);

      if (sizeof(recipients["found"]))
      {
        letter = letters[number - 1];
        delivered = POSTAL_D->post_mail(owner_name, recipients["found"], ({ }),
          _LANG_MAILER_FORWARD_PREFIX + letter["subject"],
          _LANG_MAILER_FORWARD_BODY + letter["body"]);
        if (sizeof(delivered))
          write(_LANG_MAILER_SENT);
        else
          write(_LANG_MAILER_NOT_SENT);
      }
    }
  }
  else if (member_array(verb, _LANG_MAILER_CMD_DELETE) != -1)
    mark_letters(args, 1);
  else if (member_array(verb, _LANG_MAILER_CMD_UNDELETE) != -1)
    mark_letters(args, 0);
  else if (member_array(verb, _LANG_MAILER_CMD_GROUPS) != -1)
  {
    if (!strlen(args))
      show_groups();
    else
      edit_group(args);
  }
  else if (member_array(verb, _LANG_MAILER_CMD_QUIT) != -1)
  {
    if (sizeof(marked))
    {
      number = POSTAL_D->remove_letters(owner_name, marked);
      write(_LANG_MAILER_DELETED);
    }

    write(_LANG_MAILER_CLOSED);
    close_mailer();
    return;
  }
  else if (member_array(verb, _LANG_MAILER_CMD_HELP) != -1)
    show_help();
  else
    write(_LANG_MAILER_UNKNOWN_COMMAND);

  show_prompt();
}

private string short_subject(string subject)
{
  if (strlen(subject) > MAX_MAIL_SUBJECT_LENGTH)
    return subject[0..MAX_MAIL_SUBJECT_LENGTH - 1];

  return subject;
}

// A coder's signature, from .sig or .signature in their home directory,
// closes every letter they write.
private string query_signature()
{
  string file, text;
  int i;

  if (!owner->query_coder())
    return "";

  for (i = 0; i < 2; i++)
  {
    file = "/home/" + owner_name + "/" + ({ ".sig", ".signature" })[i];

    if (file_exists(file) && strlen(text = read_file(file)))
      return "\n" + text + (text[strlen(text) - 1] == '\n' ? "" : "\n");
  }

  return "";
}

// Writing a letter: recipients, then subject, then the lines of the body,
// then who gets a copy.
private void start_letter(string * names, varargs string subject)
{
  mapping recipients;

  recipients = POSTAL_D->expand_recipients(owner_name, names);

  if (sizeof(recipients["unknown"]))
    write(_LANG_MAILER_UNKNOWN_RECIPIENTS);

  if (!sizeof(recipients["found"]))
  {
    write(_LANG_MAILER_NOBODY_TO_WRITE);
    if (only_one)
      close_mailer();
    else
      show_prompt();
    return;
  }

  draft = ([ "to" : recipients["found"], "cc" : ({ }) ]);
  body = ({ });

  if (strlen(subject))
  {
    draft["subject"] = short_subject(subject);
    write(_LANG_MAILER_WRITE_BODY);
    input_to("letter_line");
    return;
  }

  write(_LANG_MAILER_ASK_SUBJECT);
  input_to("letter_subject");
}

void letter_subject(string str)
{
  if (!valid_session())
    return;

  str = trim(str ? str : "");

  if (!strlen(str))
    str = _LANG_MAILER_NO_SUBJECT;

  draft["subject"] = short_subject(str);
  write(_LANG_MAILER_WRITE_BODY);
  input_to("letter_line");
}

private void letter_done()
{
  if (only_one)
    close_mailer();
  else
  {
    refresh_letters();
    show_prompt();
  }
}

void letter_line(string str)
{
  if (!valid_session())
    return;

  if (!str)
    str = "";

  if (str == _LANG_MAILER_BODY_CANCEL)
  {
    write(_LANG_MAILER_DISCARDED);
    letter_done();
    return;
  }

  if (str == _LANG_MAILER_BODY_END)
  {
    if (!sizeof(body))
    {
      write(_LANG_MAILER_DISCARDED);
      letter_done();
      return;
    }

    write(_LANG_MAILER_ASK_CC);
    input_to("letter_cc");
    return;
  }

  body += ({ str });
  input_to("letter_line");
}

void letter_cc(string str)
{
  mapping recipients;
  string * delivered;

  if (!valid_session())
    return;

  str = trim(str ? str : "");

  if (strlen(str))
  {
    recipients = POSTAL_D->expand_recipients(owner_name,
      recipient_names(str));

    if (sizeof(recipients["unknown"]))
      write(_LANG_MAILER_UNKNOWN_RECIPIENTS);

    draft["cc"] = recipients["found"];
  }

  delivered = POSTAL_D->post_mail(owner_name, draft["to"], draft["cc"],
    draft["subject"], implode(body, "\n") + "\n" + query_signature());

  if (sizeof(delivered))
    write(_LANG_MAILER_SENT);
  else
    write(_LANG_MAILER_NOT_SENT);

  draft = ([ ]);
  body = ({ });
  letter_done();
}
