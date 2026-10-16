/*
 * record command: your criminal record, from the crimes handler
 * (/lib/handlers/crimes.c). For each citizenship you have offended in: your
 * status there, your points and your latest offences. Points fade with time.
 */

#include <mud/cmd.h>
#include <crimes.h>
#include <language.h>

inherit CMD_BASE;

// how many recent offences are listed under each citizenship
#define RECORD_SHOWN 5

void setup()
{
  set_aliases(_LANG_CMD_RECORD_ALIAS);
  set_usage(_LANG_CMD_RECORD_SYNTAX);
  set_help(_LANG_CMD_RECORD_HELP);
}

// The translated name of a value of a list, by its position in the original.
private string translated(string * originals, string * names, string value)
{
  int pos;

  pos = member_array(value, originals);
  return (pos >= 0 && pos < sizeof(names)) ? names[pos] : value;
}

static int cmd(string str, object me, string verb)
{
  object crimes;
  mapping record, entry;
  mixed * recent;
  string * cits;
  string ret;
  int i, j, first;

  crimes = handler("crimes", me);
  record = crimes->query_record(me->query_name());
  cits = map_indices(record);
  ret = "";

  for (i = 0; i < sizeof(cits); i++)
  {
    entry = record[cits[i]];

    ret += sprintf(_LANG_RECORD_LINE, capitalize(cits[i]),
      translated(CRIME_STATUSES, _LANG_RECORD_STATUS_NAMES,
                 crimes->query_status(me, cits[i])),
      entry["points"]);

    recent = entry["recent"];
    first = sizeof(recent) - RECORD_SHOWN;
    if (first < 0)
      first = 0;

    for (j = sizeof(recent) - 1; j >= first; j--)
      ret += sprintf(_LANG_RECORD_OFFENCE,
        translated(CRIME_TYPES, _LANG_RECORD_OFFENCE_NAMES, recent[j][0]),
        ctime(recent[j][1], 4),
        strlen(recent[j][2]) ? capitalize(recent[j][2]) : _LANG_RECORD_NOBODY,
        strlen(recent[j][3]) ? recent[j][3] : _LANG_RECORD_NOWHERE);

    ret += "\n";
  }

  if (!strlen(ret))
  {
    write(_LANG_RECORD_CLEAN);
    return 1;
  }

  write(_LANG_RECORD_HEADER + ret);
  return 1;
}
