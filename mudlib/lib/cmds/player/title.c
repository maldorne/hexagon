// title - the titles somebody has been given, and which one they wear
//
// A title is never lost: this lists every one its owner has been granted,
// wears one by its number, or leaves them with none showing.

#include <mud/cmd.h>
#include <living/titles.h>
#include <language.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(_LANG_CMD_TITLE_ALIASES);
}

string query_usage()
{
  return _LANG_CMD_TITLE_SYNTAX;
}

string query_help()
{
  return _LANG_CMD_TITLE_HELP;
}

// The titles this game can put words to, in the order they were granted
private string * titles_of(object me)
{
  return handler(TITLES_HANDLER, me)->query_titles_of(me);
}

// Where a title came from, said in the reader's language
private string from_name(string from)
{
  switch (from)
  {
    case TITLE_FROM_CLASS: return _LANG_CMD_TITLE_FROM_CLASS;
    case TITLE_FROM_GUILD: return _LANG_CMD_TITLE_FROM_GUILD;
    case TITLE_FROM_JOB:   return _LANG_CMD_TITLE_FROM_JOB;
    case TITLE_FROM_DEED:  return _LANG_CMD_TITLE_FROM_DEED;
  }

  return from;
}

private string listing(object me)
{
  string * ids, ret;
  object titles;
  int i;

  titles = handler(TITLES_HANDLER, me);
  ids = titles_of(me);

  if (!sizeof(ids))
    return _LANG_CMD_TITLE_NONE_EARNED;

  ret = _LANG_CMD_TITLE_HEADER;

  for (i = 0; i < sizeof(ids); i++)
    ret += sprintf(_LANG_CMD_TITLE_LINE,
                   i + 1,
                   titles->query_title_text(ids[i], me->query_gender()),
                   from_name(titles->query_title_from(ids[i])),
                   (ids[i] == me->query_worn_title()) ? _LANG_CMD_TITLE_WORN : "");

  return ret + _LANG_CMD_TITLE_FOOTER;
}

int cmd(string str, object me, string verb)
{
  string * ids;
  int which;

  if (!str || !strlen(str))
  {
    tell_object(me, listing(me));
    return 1;
  }

  if (member_array(lower_case(str), _LANG_CMD_TITLE_NO_TITLE) != -1)
  {
    me->clear_worn_title();
    tell_object(me, _LANG_CMD_TITLE_CLEARED);
    return 1;
  }

  which = to_int(str);
  ids = titles_of(me);

  if (which < 1 || which > sizeof(ids))
  {
    notify_fail(_LANG_CMD_TITLE_NOT_YOURS);
    return 0;
  }

  me->wear_title(ids[which - 1]);
  tell_object(me, _LANG_CMD_TITLE_SET);
  return 1;
}
