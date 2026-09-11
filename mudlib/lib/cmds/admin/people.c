
// Who is connected, in whatever detail is being asked for. The facts come from
// the people handler; this only chooses which of them to show and lays them out.

#include <std.h>
#include <mud/cmd.h>

inherit CMD_BASE;

private string columns(string * * rows);
private string * * rows_for(object * who, string * fields, string * heads);
private int do_view(object * who, string view);
private int do_detail(object * who);

void setup()
{
  set_aliases(({ "people" }));
  set_usage("people [ quick | net | terms | dirs | all ] [<name>]");
  set_help(
    "Report on whoever is connected right now.\n" +
    "\n" +
    "  people          who they are: kind, race, guild, level, alignment\n" +
    "  people quick    the short version: how long they have been on\n" +
    "  people net      where they connect from, and how idle they are\n" +
    "  people terms    what their client says it is, and how big\n" +
    "  people dirs     where each of them stands, and a coder's own path\n" +
    "  people all      every fact known about each of them, one block each\n" +
    "\n" +
    "Any view takes a name after it and reports only the people whose name " +
    "begins with it: 'people net ba' for everybody from Ba- onwards, " +
    "'people all bob' for everything about Bob.\n" +
    "\n" +
    "A '*' beside a name means that person is in the editor and is not " +
    "reading what is said to them. Somebody still at the login prompt has no " +
    "character yet and shows as such.");
}

// Lay rows out in columns, the first row being the header.
private string columns(string * * rows)
{
  string out;
  int * width;
  int i, j, k;

  if (!sizeof(rows))
    return "";

  width = allocate_int(sizeof(rows[0]));
  for (i = 0; i < sizeof(rows); i++)
    for (j = 0; j < sizeof(rows[i]) && j < sizeof(width); j++)
    {
      k = strlen(rows[i][j], TRUE);
      if (k > width[j])
        width[j] = k;
    }

  out = "";
  for (i = 0; i < sizeof(rows); i++)
  {
    string line;

    line = "  ";
    for (j = 0; j < sizeof(rows[i]); j++)
      line += sprintf("%-*s ", width[j] + 1, rows[i][j]);
    out += line + "\n";

    if (!i)
    {
      line = "  ";
      for (j = 0; j < sizeof(width); j++)
      {
        for (k = 0; k < width[j]; k++)
          line += "-";
        line += "  ";
      }
      out += line + "\n";
    }
  }

  return out;
}

// One row per user, reading the named facts in the order given.
private string * * rows_for(object * who, string * fields, string * heads)
{
  string * * rows;
  int i, j;

  rows = ({ heads });
  for (i = 0; i < sizeof(who); i++)
  {
    mapping facts;
    string * row;

    facts = (mapping)handler("people")->query_facts(who[i]);
    row = ({ });
    for (j = 0; j < sizeof(fields); j++)
      row += ({ facts[fields[j]] ? facts[fields[j]] : "-" });
    rows += ({ row });
  }

  return rows;
}

// ===== the tables =====
private int do_view(object * who, string view)
{
  string * fields, * heads;

  // the name column carries the editor mark, so it is two fields wide
  if (view == "quick")
  {
    fields = ({ "editing", "cap_name", "on", "race", "guild" });
    heads = ({ "", "name", "on", "race", "guild" });
  }
  else if (view == "net")
  {
    fields = ({ "editing", "cap_name", "idle", "ip", "host" });
    heads = ({ "", "name", "idle", "address", "host" });
  }
  else if (view == "terms")
  {
    fields = ({ "editing", "cap_name", "terminal", "rows", "cols" });
    heads = ({ "", "name", "terminal", "rows", "cols" });
  }
  else if (view == "dirs")
  {
    fields = ({ "editing", "cap_name", "dir", "where" });
    heads = ({ "", "name", "working in", "standing in" });
  }
  else
  {
    fields = ({ "editing", "cap_name", "kind", "race", "guild", "level",
                "align", "ext_align", "idle" });
    heads = ({ "", "name", "kind", "race", "guild", "level", "align", "seems",
               "idle" });
  }

  write("" + sizeof(who) + " connected:\n" +
        columns(rows_for(who, fields, heads)));
  return 1;
}

// ===== people all =====
private int do_detail(object * who)
{
  string out;
  int i;

  out = "";
  for (i = 0; i < sizeof(who); i++)
  {
    mapping f;

    f = (mapping)handler("people")->query_facts(who[i]);
    out += f["cap_name"] + (f["editing"] == "*" ? "  (in the editor)" : "") +
           "\n" +
           "  kind       " + f["kind"] + ", " + f["gender"] + "\n" +
           "  race       " + f["race"] + "\n" +
           "  guild      " + f["guild"] + "\n" +
           "  level      " + f["level"] + "\n" +
           "  alignment  " + f["align"] + " real, " + f["ext_align"] +
           " as it seems\n" +
           "  connected  " + f["on"] + " ago, idle " + f["idle"] + "\n" +
           "  client     " + f["terminal"] + ", " + f["rows"] + " rows x " +
           f["cols"] + " cols\n" +
           "  from       " + f["ip"] + " (" + f["host"] + ")\n" +
           "  standing   " + f["where"] + "\n" +
           "  euid       " + (f["euid"] ? f["euid"] : "-") + "\n" +
           "  working in " + f["dir"] + "\n";
  }

  write(out);
  return 1;
}

static int cmd(string str, object me, string verb)
{
  object * who;
  string * args, view, prefix;

  args = (str && strlen(str)) ? explode(str, " ") - ({ "" }) : ({ });

  view = "";
  prefix = "";

  if (sizeof(args) &&
      (args[0] == "quick" || args[0] == "net" || args[0] == "terms" ||
       args[0] == "dirs" || args[0] == "all"))
  {
    view = args[0];
    args = args[1 ..];
  }

  if (sizeof(args) > 1)
  {
    notify_fail("Usage: people [ quick | net | terms | dirs | all ] " +
                "[<name>]\n");
    return 0;
  }
  if (sizeof(args))
    prefix = args[0];

  who = (object *)handler("people")->query_users(prefix);
  if (!sizeof(who))
  {
    notify_fail(strlen(prefix)
                  ? "Nobody connected is called " + prefix + "-something.\n"
                  : "Nobody is connected.\n");
    return 0;
  }

  if (view == "all")
    return do_detail(who);

  return do_view(who, view);
}
