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
#include <maps/maps.h>
#include <areas/area.h>

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
  set_usage("exits [-v] [files|globs | sector <map> <x> <y> [z] | area <name>]");
  set_help(
    "Checks exits in the current room/location, in every room matching\n" +
    "a file/glob list, or in every location of a given sector or area.\n" +
    "\n" +
    "  exits                                current environment (detailed)\n" +
    "  exits <path|glob ...>                scan matching .c rooms / .o locations\n" +
    "  exits sector <map> <x> <y> [z]       scan the sector containing (x, y, z)\n" +
    "                                       in the named map (x, y, z signed;\n" +
    "                                       z defaults to 0). Also reports drift\n" +
    "                                       between sector.o and on-disk coord files.\n" +
    "  exits area <name>                    scan every location of the area.\n" +
    "\n" +
    "  -v   verbose: dump the full per-exit table for every scanned file.\n" +
    "\n" +
    "Game is inferred from the player's current environment for the sector\n" +
    "and area forms.\n");
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

  // for locations, also surface the area / map / sector context
  if (env->query_location())
  {
    object area;
    int * coords;
    string game;

    area   = env->query_area();
    coords = env->query_coordinates();
    game   = game_name(env);

    if (strlen(game) || area)
    {
      ret += " ";
      if (strlen(game))
        ret += "Game: " + game;
      if (strlen(game) && area)
        ret += "   ";
      if (area)
        ret += "Area: " + area->query_area_name();
      ret += "\n";
    }

    ret += " Map:  " + env->query_map_name();
    if (coords && sizeof(coords) == 3)
    {
      int sx, sy, sz;
      sx = coords[0] / 10 - (coords[0] < 0);
      sy = coords[1] / 10 - (coords[1] < 0);
      sz = coords[2] / 10 - (coords[2] < 0);
      ret += "   Coords (" + coords[0] + "," + coords[1] + "," + coords[2] +
             ")   Sector (" + sx + "," + sy + "," + sz + ")";
    }
    else
      ret += "   (no coordinates set)";
    ret += "\n";
  }

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

// Scan a pre-built list of file paths (.c rooms or .o locations) through
// check_exits_of, accumulating per-file and per-exit counters.  Returns
// ({ rendered_text, total_files, ok_files, bad_files, skipped_files,
//    total_exits, ok_exits, bad_exits }).
private mixed * scan_files(string * files, int verbose)
{
  object ob;
  mixed * checks;
  string err, ret, ext, path;
  int i, j;
  int total_files, ok_files, bad_files, skipped_files;
  int total_exits, ok_exits, bad_exits;

  ret = "";
  total_files = ok_files = bad_files = skipped_files = 0;
  total_exits = ok_exits = bad_exits = 0;

  for (i = 0; i < sizeof(files); i++)
  {
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
      err = catch(ob = load_object(path));
    else
      err = catch(ob = load_object(LOCATION_HANDLER)->load_location(path));

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

  return ({ ret, total_files, ok_files, bad_files, skipped_files,
            total_exits, ok_exits, bad_exits });
}

// Render the trailing summary common to all batch scans.  Optional
// extra_lines is appended verbatim between the file and exit totals.
private string render_summary(mixed * totals, string extra_lines)
{
  string ret;
  int total_files, ok_files, bad_files, skipped_files;
  int total_exits, ok_exits, bad_exits;

  total_files   = totals[1];
  ok_files      = totals[2];
  bad_files     = totals[3];
  skipped_files = totals[4];
  total_exits   = totals[5];
  ok_exits      = totals[6];
  bad_exits     = totals[7];

  ret  = "\n";
  ret += " Scanned " + total_files + " file" + (total_files == 1 ? "" : "s") +
         ": " + G + ok_files + " ok" + RE +
         (bad_files     ? ", " + R + bad_files + " with issues" + RE : "") +
         (skipped_files ? ", " + Y + skipped_files + " skipped" + RE : "") +
         ".\n";

  if (extra_lines && strlen(extra_lines))
    ret += extra_lines;

  ret += " Exits:   " + total_exits + " checked, " +
         G + ok_exits + " ok" + RE +
         (bad_exits ? ", " + R + bad_exits + " broken" + RE : "") + ".\n";
  return ret;
}

// "exits sector <map> <x> <y> [z]" — scan every location of the sector
// containing (x, y, z) in the given map, plus a drift cross-check
// between the sector.o positions and the on-disk per-coord files.
private int do_sector(object me, string * tokens, int verbose)
{
  object sector;
  mapping positions;
  string game, map_name, sector_path, sector_file, coord_file_pattern, extra;
  string * files, * disk_files, * disk_only, * idx_only, * pos_files;
  mixed * totals;
  int x, y, z, sx, sy, sz, k;

  if (sizeof(tokens) < 4 || sizeof(tokens) > 5)
  {
    write("Usage: exits sector <map> <x> <y> [z]\n");
    return 1;
  }

  map_name = tokens[1];
  if (sscanf(tokens[2], "%d", x) != 1 ||
      sscanf(tokens[3], "%d", y) != 1)
  {
    write("Sector coordinates must be integers (signed allowed).\n");
    return 1;
  }
  z = 0;
  if (sizeof(tokens) == 5 && sscanf(tokens[4], "%d", z) != 1)
  {
    write("Sector z must be an integer (signed allowed).\n");
    return 1;
  }

  game = game_name(me);
  if (!strlen(game))
  {
    write("Cannot infer game from your current environment. " +
          "Stand inside a game first.\n");
    return 1;
  }

  sx = x / 10 - (x < 0);
  sy = y / 10 - (y < 0);
  sz = z / 10 - (z < 0);

  sector_path = "/save/games/" + game + "/maps/" + map_name + "/" +
                sx + "/" + sy + "/" + sz + "/";
  sector_file = sector_path + "sector.o";

  if (file_size(sector_path) != -2)
  {
    write("No sector directory at " + sector_path + ".\n");
    return 1;
  }
  if (file_size(sector_file) < 0)
  {
    write("Sector directory exists but has no sector.o (" + sector_file + ").\n");
    return 1;
  }

  sector = load_object(MAPS_HANDLER)->create_sector(sector_path);
  positions = sector->query_positions();
  pos_files = map_values(positions);

  // drift cross-check: per-coord files in the dir vs sector.o positions
  coord_file_pattern = sector_path + "*_*_*.o";
  disk_files = get_dir(coord_file_pattern);
  disk_files = disk_files ? disk_files : ({ });
  disk_files -= ({ "sector.o" });

  // sector.o keys are "x_y_z"; on-disk file names are "x_y_z.o".
  // Build comparable string sets.
  {
    string * idx_keys, * disk_keys;
    idx_keys  = map_indices(positions);
    disk_keys = ({ });
    for (k = 0; k < sizeof(disk_files); k++)
    {
      string base;
      base = disk_files[k];
      if (strlen(base) >= 2 && base[strlen(base)-2..] == ".o")
        base = base[0..strlen(base)-3];
      disk_keys += ({ base });
    }

    idx_only  = idx_keys  - disk_keys;
    disk_only = disk_keys - idx_keys;
  }

  // scan all locations the sector knows about
  files = pos_files;
  totals = scan_files(files, verbose);

  // header
  write(" Sector " + map_name + " (" + sx + "," + sy + "," + sz +
        ") in " + game + ": " + sizeof(pos_files) + " indexed location" +
        (sizeof(pos_files) == 1 ? "" : "s") + ".\n");
  write(totals[0]);

  // drift extra-lines for the summary
  extra = "";
  if (sizeof(idx_only) || sizeof(disk_only))
  {
    extra = " " + Y + "Drift:" + RE +
            " " + sizeof(idx_only) + " in index without coord file" +
            (sizeof(idx_only) ? " (" + implode(idx_only, ", ") + ")" : "") +
            ", " + sizeof(disk_only) + " coord file without index entry" +
            (sizeof(disk_only) ? " (" + implode(disk_only, ", ") + ")" : "") +
            ".\n";
  }

  write(render_summary(totals, extra));
  return 1;
}

// "exits area <name>" — scan every location of the area, resolved
// against the current player's game.
private int do_area(object me, string * tokens, int verbose)
{
  object area;
  mapping locations;
  string game, name, area_path;
  string * files;
  mixed * totals;

  if (sizeof(tokens) != 2)
  {
    write("Usage: exits area <name>\n");
    return 1;
  }

  name = tokens[1];
  game = game_name(me);
  if (!strlen(game))
  {
    write("Cannot infer game from your current environment. " +
          "Stand inside a game first.\n");
    return 1;
  }

  area_path = "/save/games/" + game + "/locations/areas/" + name + "/rooms/";

  if (file_size(area_path) != -2)
  {
    write("No area directory at " + area_path + ".\n");
    return 1;
  }

  area = load_object(AREA_HANDLER)->create_area(area_path);
  locations = area->query_locations();
  files = map_indices(locations);

  totals = scan_files(files, verbose);

  write(" Area " + name + " in " + game + ": " + sizeof(files) +
        " location" + (sizeof(files) == 1 ? "" : "s") + ".\n");
  write(totals[0]);
  write(render_summary(totals, ""));
  return 1;
}

static int cmd(string str, object me, string verb)
{
  object env;
  string * files, * tokens;
  string args;
  mixed * totals;
  int verbose;

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

  // ===== "sector ..." / "area ..." subcommands =====
  if (sizeof(tokens) && tokens[0] == "sector")
    return do_sector(me, tokens, verbose);
  if (sizeof(tokens) && tokens[0] == "area")
    return do_area(me, tokens, verbose);

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

  totals = scan_files(files, verbose);

  if (totals[1] == 0)
  {
    write("No .c / .o files matched '" + args + "'.\n");
    return 1;
  }

  write(totals[0]);
  write(render_summary(totals, ""));
  return 1;
}
