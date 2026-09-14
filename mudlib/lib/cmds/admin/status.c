
// What the driver says about itself, from the status() kfun: uptime, memory,
// how much of every table is in use, and the same for a single object.

#include <std.h>
#include <mud/cmd.h>
#include <status.h>

inherit CMD_BASE;

private string amount(mixed value, int human);
private string row(string label, string value);
private int do_object(string name);

void setup()
{
  set_aliases(({ "status" }));
  set_usage("status [-h] [<object>]");
  set_help(
    "Report the state of the driver.\n" +
    "\n" +
    "  status            uptime, memory, tables, ports\n" +
    "  status -h         the same with sizes in K and M\n" +
    "  status <object>   what the driver knows about one object: when it\n" +
    "                    was compiled, how big its program and data are,\n" +
    "                    how many sectors it occupies and what callouts it\n" +
    "                    has pending\n" +
    "\n" +
    "The numbers come straight from the status() kernel function; the " +
    "fields are the ones listed in <status.h>.");
}

// A number, in bytes when asked for it
private string amount(mixed value, int human)
{
  if (!intp(value))
    return "-";

  if (!human)
    return "" + value;

  if (value >= 1024 * 1024)
    return "" + (value / (1024 * 1024)) + "." +
           ((value % (1024 * 1024)) * 10 / (1024 * 1024)) + "M";

  if (value >= 1024)
    return "" + (value / 1024) + "." + ((value % 1024) * 10 / 1024) + "K";

  return "" + value;
}

private string row(string label, string value)
{
  return sprintf("  %-22s %s\n", label, value);
}

// ===== status <object> =====
private int do_object(string name)
{
  object ob;
  mixed * st;
  string out;

  ob = find_object(name);
  if (!ob)
    catch(ob = load_object(name));

  if (!ob)
  {
    notify_fail("There is no object called " + name + ".\n");
    return 0;
  }

  st = status(ob);
  if (!st)
  {
    notify_fail("The driver says nothing about " + name + ".\n");
    return 0;
  }

  out = file_name(ob) + ":\n";
  out += row("compiled", intp(st[O_COMPILETIME])
                           ? ctime(st[O_COMPILETIME], 4) : "-");
  out += row("program", amount(st[O_PROGSIZE], 1) + " bytes");
  out += row("variables", amount(st[O_DATASIZE], 0));
  out += row("sectors", amount(st[O_NSECTORS], 0));
  out += row("callouts", "" + (pointerp(st[O_CALLOUTS])
                                 ? sizeof(st[O_CALLOUTS]) : 0));
  out += row("index", amount(st[O_INDEX], 0));
  out += row("undefined functions", st[O_UNDEFINED] ? "yes" : "no");
  out += row("special role", st[O_SPECIAL] ? "yes" : "no");

  write(out);
  return 1;
}

static int cmd(string str, object me, string verb)
{
  mixed * st;
  string * args;
  string out;
  int human;

  args = (str && strlen(str)) ? explode(str, " ") - ({ "" }) : ({ });

  if (sizeof(args) && args[0] == "-h")
  {
    human = 1;
    args = args[1 ..];
  }

  if (sizeof(args) > 1)
  {
    notify_fail("Usage: status [-h] [<object>]\n");
    return 0;
  }

  if (sizeof(args))
    return do_object(get_path(args[0]));

  st = status();

  out = "Driver " + st[ST_VERSION] + "\n" +
    row("started", ctime(st[ST_STARTTIME], 4)) +
    row("rebooted", ctime(st[ST_BOOTTIME], 4)) +
    row("uptime", (string)handler("people")->query_span(st[ST_UPTIME])) +
    "\n" +
    row("swap", amount(st[ST_SWAPUSED], 0) + " / " +
                amount(st[ST_SWAPSIZE], 0) + " sectors of " +
                amount(st[ST_SECTORSIZE], human) + " bytes") +
    row("swapped out", amount(st[ST_SWAPRATE1], 0) + " last minute, " +
                       amount(st[ST_SWAPRATE5], 0) + " last five") +
    row("static memory", amount(st[ST_SMEMUSED], human) + " / " +
                         amount(st[ST_SMEMSIZE], human)) +
    row("dynamic memory", amount(st[ST_DMEMUSED], human) + " / " +
                          amount(st[ST_DMEMSIZE], human)) +
    "\n" +
    row("objects", amount(st[ST_NOBJECTS], 0) + " / " +
                   amount(st[ST_OTABSIZE], 0)) +
    row("callouts", amount(st[ST_NCOSHORT], 0) + " short, " +
                    amount(st[ST_NCOLONG], 0) + " long, of " +
                    amount(st[ST_COTABSIZE], 0)) +
    row("users", amount(st[ST_NUSERS], 0) + " / " +
                 amount(st[ST_UTABSIZE], 0)) +
    row("editors", amount(st[ST_ETABSIZE], 0)) +
    "\n" +
    row("max string", amount(st[ST_STRSIZE], human)) +
    row("max array", amount(st[ST_ARRAYSIZE], 0)) +
    row("stack left", amount(st[ST_STACKDEPTH], 0)) +
    row("ticks left", amount(st[ST_TICKS], 0));

  write(out);
  return 1;
}
