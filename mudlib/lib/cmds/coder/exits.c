// exits coder command
// Original chkexits by Radiks. Moved / extended by neverbot.
// Verifies exits of a room/location: when invoked without arguments
// it checks the current environment in detail; when given file paths
// or globs it scans every matching .c room, prints a one-line outcome
// per file and a final summary across the batch. With -v, every file
// in the batch is shown with the same detailed table as the no-args
// branch.

#include <mud/cmd.h>
#include <translations/exits.h>
#include <room/location.h>

inherit CMD_BASE;

#define R   "%^BOLD%^RED%^"
#define G   "%^BOLD%^GREEN%^"
#define C   "%^BOLD%^CYAN%^"
#define Y   "%^BOLD%^YELLOW%^"
#define RE  "%^RESET%^"

// exit-check status codes
#define EX_OK          0
#define EX_NO_DEST     1   // destination file does not exist
#define EX_NO_LOAD     2   // destination won't load
#define EX_NO_BACK     3   // destination has no back-exit toward us
#define EX_WRONG_BACK  4   // destination has a back-exit but in the wrong direction

void setup()
{
  set_aliases(({ "exits" }));
  set_usage("exits [-v] [files|globs]");
  set_help(
    "Checks exits in the current room/location, or in every room\n" +
    "matching the given file/glob list.\n" +
    "  -v   verbose: dump the full per-exit table for every file.\n" +
    "With no arguments the current environment is checked.\n");
}

// "self id" of an environment, in the form other rooms/locations will
// use when referencing it from their own exits.
private string self_id(object env)
{
  if (env->query_location())
    return env->query_file_name();
  return file_name(env) + ".c";
}

// Returns ({ ({ dir, target_path, status, detail }), ... }) for every
// exit of env. detail is "" except for EX_WRONG_BACK, where it carries
// the (wrong) back-direction that was found.
private mixed * check_exits_of(object env)
{
  mixed * dirs, * dirs2, * out;
  object dest;
  string err, me_id, match;
  int a, j;

  out  = ({ });
  dirs = env->query_dest_dir();

  if (!sizeof(dirs))
    return out;

  me_id = self_id(env);

  for (a = 0; a < sizeof(dirs); a += 2)
  {
    string dir, target;
    dir    = dirs[a];
    target = dirs[a + 1];

    // resolve destination object — handles both .c rooms and .o locations
    dest = nil;
    err  = catch(dest = env->query_dest_object(dir));

    if (err || !dest)
    {
      // distinguish "file missing" from "file present but won't load"
      if (target && strlen(target) >= 2 &&
          target[strlen(target)-2..] == ".c" && file_size(target) == -1)
        out += ({ ({ dir, target, EX_NO_DEST, "" }) });
      else
        out += ({ ({ dir, target, EX_NO_LOAD, "" }) });
      continue;
    }

    dirs2 = nil;
    err = catch(dirs2 = dest->query_dest_dir());

    if (err)
    {
      out += ({ ({ dir, target, EX_NO_LOAD, "" }) });
      continue;
    }

    match = "";
    if (sizeof(dirs2))
      for (j = 0; j < sizeof(dirs2); j += 2)
        if (dirs2[j + 1] == me_id)
          match = dirs2[j];

    if (!strlen(match))
      out += ({ ({ dir, target, EX_NO_BACK, "" }) });
    else if (OPPOSITES[dir] != match)
      out += ({ ({ dir, target, EX_WRONG_BACK, match }) });
    else
      out += ({ ({ dir, target, EX_OK, "" }) });
  }

  return out;
}

// Renders one exit row of the detailed table.
private string render_row(mixed * row)
{
  string dir, target, detail, line;
  int status;

  dir    = row[0];
  target = row[1];
  status = row[2];
  detail = row[3];

  line = sprintf("  %-9s -> %s ", dir, target);

  switch (status)
  {
    case EX_OK:         return line + G + "ok" + RE + ".\n";
    case EX_NO_DEST:    return line + R + "destination does not exist" + RE + ".\n";
    case EX_NO_LOAD:    return line + R + "destination won't load" + RE + ".\n";
    case EX_NO_BACK:    return line + R + "no backwards exit" + RE + ".\n";
    case EX_WRONG_BACK: return line + Y + "wrong opposite" + RE +
                               " (back: " + C + detail + RE + ").\n";
  }

  return line + "?\n";
}

// Full detailed table for a single env. Used by the no-args branch and
// by -v on the file-list branch.
private string render_table(object env, mixed * checks)
{
  object user;
  string ret, here;
  int i, ok_count;

  user = this_player()->user();
  here = file_name(env);

  ret  = sprintf("%p%|*s\n", '-', user->query_cols(), "");
  ret += " File: " + here;
  if (env->query_location())
    ret += "   (location " + env->query_file_name() + ")";
  else
    ret += "   (" + file_size(here + ".c") + " bytes)";
  ret += "\n";
  ret += sprintf("%p%|*s\n", '-', user->query_cols(), "");

  if (!sizeof(checks))
  {
    ret += " No exits.\n";
    ret += sprintf("%p%|*s\n", '-', user->query_cols(), "");
    return ret;
  }

  ok_count = 0;
  for (i = 0; i < sizeof(checks); i++)
  {
    ret += render_row(checks[i]);
    if (checks[i][2] == EX_OK) ok_count++;
  }

  ret += sprintf("%p%|*s\n", '-', user->query_cols(), "");
  ret += " " + sizeof(checks) + " exits, " + ok_count + " ok, " +
         (sizeof(checks) - ok_count) + " with issues.\n";
  ret += sprintf("%p%|*s\n", '-', user->query_cols(), "");

  return ret;
}

private string file_tag(string colour, string text)
{
  return colour + "[" + text + "]" + RE;
}

// Short, human-readable label for a status (for the one-line per-file row).
private string status_label(mixed * c)
{
  switch (c[2])
  {
    case EX_NO_DEST:    return c[0] + ": missing dest";
    case EX_NO_LOAD:    return c[0] + ": won't load";
    case EX_NO_BACK:    return c[0] + ": no back-exit";
    case EX_WRONG_BACK: return c[0] + ": wrong opposite (" + c[3] + ")";
  }
  return c[0];
}

static int cmd(string str, object me, string verb)
{
  object env, ob;
  mixed * checks;
  string * files, * tokens;
  string err, ret, args;
  int verbose;
  int i, j;
  int total_files, ok_files, bad_files, skipped_files;
  int total_exits, ok_exits, bad_exits;

  env  = environment(me);
  args = str ? str : "";
  verbose = 0;

  // strip -v from anywhere in the arg list
  tokens = explode(args, " ") - ({ "" });
  if (sizeof(tokens) && member_array("-v", tokens) != -1)
  {
    verbose = 1;
    tokens -= ({ "-v" });
  }
  args = implode(tokens, " ");

  // ===== no args: detailed check of current environment =====
  if (!strlen(args))
  {
    if (!env)
    {
      write("You can't do that without an environment.\n");
      return 1;
    }

    write(render_table(env, check_exits_of(env)));
    return 1;
  }

  // ===== with args: expand path/globs, scan every matching room/location =====

  // any token that resolves to a directory is auto-expanded to <token>/*
  // so callers can drop the trailing wildcard.
  {
    string * raw, * expanded;
    int k;

    raw = explode(args, " ") - ({ "" });
    expanded = ({ });

    for (k = 0; k < sizeof(raw); k++)
    {
      string token, resolved;
      token = raw[k];
      // strip trailing slash so file_size() works on the directory itself
      while (strlen(token) > 1 && token[strlen(token)-1] == '/')
        token = token[0..strlen(token)-2];
      resolved = get_path(token);
      if (file_size(resolved) == -2)
        expanded += ({ token + "/*" });
      else
        expanded += ({ raw[k] });
    }

    args = implode(expanded, " ");
  }

  files = get_files(args);

  if (!sizeof(files))
  {
    write("No files matched '" + args + "'.\n");
    return 1;
  }

  ret = "";
  total_files = ok_files = bad_files = skipped_files = 0;
  total_exits = ok_exits = bad_exits = 0;

  for (i = 0; i < sizeof(files); i++)
  {
    string path, ext;
    int file_ok, file_bad;

    path = files[i];

    if (strlen(path) < 2)
      continue;
    ext = path[strlen(path)-2..];
    if (ext != ".c" && ext != ".o")
      continue;

    total_files++;

    ob = nil;
    if (ext == ".c")
    {
      err = catch(ob = load_object(path));
    }
    else
    {
      // .o: a location save file — load through the location handler
      err = catch(ob = load_object(LOCATION_HANDLER)->load_location(path));
    }

    if (err)
    {
      ret += " " + file_tag(R, "-") + " " + path + "   cannot load.\n";
      skipped_files++;
      continue;
    }

    if (!ob || !ob->query_room())
    {
      ret += " " + file_tag(Y, "-") + " " + path + "   not a room.\n";
      skipped_files++;
      continue;
    }

    checks = check_exits_of(ob);

    if (verbose)
      ret += render_table(ob, checks);

    file_ok = file_bad = 0;
    for (j = 0; j < sizeof(checks); j++)
      if (checks[j][2] == EX_OK) file_ok++;
      else                       file_bad++;

    total_exits += sizeof(checks);
    ok_exits    += file_ok;
    bad_exits   += file_bad;

    if (file_bad == 0)
    {
      ok_files++;
      if (!verbose)
        ret += " " + file_tag(G, "ok") + "  " + path + "   " +
               file_ok + "/" + sizeof(checks) + ".\n";
    }
    else
    {
      bad_files++;
      if (!verbose)
      {
        // surface the first issue inline so the line tells you what to
        // look at without having to re-run with -v
        string first;
        first = "";
        for (j = 0; j < sizeof(checks); j++)
          if (checks[j][2] != EX_OK)
          {
            first = status_label(checks[j]);
            break;
          }
        ret += " " + file_tag(R, "! ") + " " + path + "   " +
               file_ok + "/" + sizeof(checks) +
               " (" + first +
               (file_bad > 1 ? ", +" + (file_bad - 1) + " more" : "") +
               ").\n";
      }
    }
  }

  if (total_files == 0)
  {
    write("No .c / .o files matched '" + args + "'.\n");
    return 1;
  }

  // summary
  ret += "\n";
  ret += " Scanned " + total_files + " file" + (total_files == 1 ? "" : "s") +
         ": " + G + ok_files + " ok" + RE +
         (bad_files     ? ", " + R + bad_files + " with issues" + RE : "") +
         (skipped_files ? ", " + Y + skipped_files + " skipped" + RE : "") +
         ".\n";
  ret += " Exits:   " + total_exits + " checked, " +
         G + ok_exits + " ok" + RE +
         (bad_exits ? ", " + R + bad_exits + " broken" + RE : "") + ".\n";

  write(ret);
  return 1;
}
