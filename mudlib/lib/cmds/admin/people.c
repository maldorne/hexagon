
// Who is connected, in whatever detail is being asked for. The facts come from
// the people handler; this only chooses which of them to show and lays them out.

#include <std.h>
#include <mud/cmd.h>

inherit CMD_BASE;

private string columns(string * * rows);
private string * * rows_for(object * who, string * fields, string * heads);
private int any_guest(object * who);
private int do_view(object * who, string view);
private int do_net(object * who);
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
    "  people net      where they connect from, sorted by address, with\n" +
    "                  the connections sharing one counted\n" +
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

// Whether anybody in the list is visiting. The guest column is only worth a
// place on the screen when there is one.
private int any_guest(object * who)
{
  int i;

  for (i = 0; i < sizeof(who); i++)
  {
    mapping facts;

    facts = (mapping)handler("people")->query_facts(who[i]);
    if (strlen(facts["guest"]))
      return TRUE;
  }

  return FALSE;
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

  if (any_guest(who))
  {
    fields += ({ "guest" });
    heads += ({ "guest" });
  }

  write("" + sizeof(who) + " connected:\n" +
        columns(rows_for(who, fields, heads)));
  return 1;
}

// ===== people net =====
// Sorted by address, and every address more than one connection comes from is
// counted on its rows: two people arriving from the same place at the same
// time is the whole point of the view.
int cmp_address(object a, object b)
{
  mixed x, y;

  x = query_ip_number(a);
  y = query_ip_number(b);

  if (!stringp(x))
    x = "";
  if (!stringp(y))
    y = "";

  if (x == y)
    return 0;

  return (x > y) ? 1 : -1;
}

private int do_net(object * who)
{
  mapping seen;
  string * * rows;
  int i, guests;

  seen = ([ ]);
  for (i = 0; i < sizeof(who); i++)
  {
    mixed ip;

    ip = query_ip_number(who[i]);
    if (!stringp(ip))
      ip = "-";
    seen[ip] = (seen[ip] ? seen[ip] : 0) + 1;
  }

  who = sort_array(who, "cmp_address", this_object());
  guests = any_guest(who);

  rows = ({ ({ "", "name", "idle", "address", "host", "same" }) +
            (guests ? ({ "guest" }) : ({ })) });

  for (i = 0; i < sizeof(who); i++)
  {
    mapping f;

    f = (mapping)handler("people")->query_facts(who[i]);
    rows += ({ ({ f["editing"], f["cap_name"], f["idle"], f["ip"], f["host"],
                  seen[f["ip"]] > 1 ? "" + seen[f["ip"]] : "" }) +
               (guests ? ({ f["guest"] }) : ({ })) });
  }

  write("" + sizeof(who) + " connected:\n" + columns(rows));
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
           "  kind       " + f["kind"] +
           (strlen(f["guest"]) ? " (guest)" : "") + ", " + f["gender"] + "\n" +
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

  if (view == "net")
    return do_net(who);

  return do_view(who, view);
}
