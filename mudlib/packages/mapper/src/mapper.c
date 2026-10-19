// mapper engine: reads the saved locations of an area as data (never loading
// them), keeps what is safe to show to players and writes it as JSON.

#include <language.h>

#define READER "/packages/mapper/src/reader.c"
// files read in each call_out, to stay within the ticks of one call
#define READ_PER_STEP 40
// folders listed in each call_out
#define DIRS_PER_STEP 20
#define FORMAT "mapper/1"

// the run in progress: only one at a time
private int running;
private object admin, reader;
private string game, area_id, out_file;
private int with_sub_areas;
// save folders of the dumped area and of the game's areas, with trailing slash
private string root_dir, areas_dir;
private string * pending_dirs, * files;
private int cursor;
// save folder -> nearest folder that holds an area.o, "" when none
private mapping nearest_cache;
// area save folder -> ({ name, parent area id })
private mapping area_info;
// location save file -> ({ id, name, area save folder, x, y, z, exits })
private mapping locations;
// area id -> ({ kind, label }) of the areas outside the dump
private mapping label_cache;

private string message(string text, mixed * values);
private string nearest_area(string dir);
private void finish();

/**
 * A language string with $1, $2, ... replaced by the values.
 */
private string message(string text, mixed * values)
{
  int i;

  for (i = 0; i < sizeof(values); i++)
    text = replace_string(text, "$" + (i + 1), "" + values[i]);

  return text;
}

/**
 * Whether a piece of text between two %^ markers is a colour code: a name in
 * capitals (BOLD, YELLOW, RESET...).
 */
private int is_colour_code(string part)
{
  int i;

  if (!strlen(part))
    return 1;

  for (i = 0; i < strlen(part); i++)
    if ((part[i] < 'A' || part[i] > 'Z') && part[i] != '_')
      return 0;

  return 1;
}

/**
 * The text without colour codes: every capitalised name between %^ markers
 * goes, whether codes come alone or chained (%^BOLD%^YELLOW%^).
 */
private string strip_colours(string text)
{
  string * parts;
  string result;
  int i;

  if (!stringp(text))
    return "";

  // a sentinel on each end, since explode drops a leading or trailing marker
  parts = explode("|" + text + "|", "%^");
  result = "";

  for (i = 0; i < sizeof(parts); i++)
    if (i == 0 || i == sizeof(parts) - 1 || !is_colour_code(parts[i]))
      result += parts[i];

  return result[1 .. strlen(result) - 2];
}

/**
 * Sorts strings ascending (merge sort: sort_array is quadratic).
 */
private string * sort_strings(string * list)
{
  string * left, * right, * merged;
  int middle, i, j;

  if (sizeof(list) < 2)
    return list;

  middle = sizeof(list) / 2;
  left = sort_strings(list[..middle - 1]);
  right = sort_strings(list[middle..]);
  merged = ({ });
  i = 0;
  j = 0;

  while (i < sizeof(left) && j < sizeof(right))
  {
    if (left[i] <= right[j])
      merged += ({ left[i++] });
    else
      merged += ({ right[j++] });
  }

  if (i < sizeof(left))
    merged += left[i..];

  if (j < sizeof(right))
    merged += right[j..];

  return merged;
}

/**
 * The folder part of a path, with trailing slash.
 */
private string folder_of(string path)
{
  string * parts;

  // explode drops the leading empty piece, so the slash is put back
  parts = explode(path, "/") - ({ "" });
  return "/" + implode(parts[..sizeof(parts) - 2], "/") + "/";
}

/**
 * Today as YYYY-MM-DD (UTC), from the clock.
 */
private string today()
{
  int days, era, doe, yoe, year, doy, mp, day, month;

  // days since 1970-01-01 -> civil date
  days = time() / 86400 + 719468;
  era = days / 146097;
  doe = days - era * 146097;
  yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
  year = yoe + era * 400;
  doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
  mp = (5 * doy + 2) / 153;
  day = doy - (153 * mp + 2) / 5 + 1;
  month = mp < 10 ? mp + 3 : mp - 9;

  if (month <= 2)
    year++;

  return year + "-" + (month < 10 ? "0" : "") + month + "-" +
         (day < 10 ? "0" : "") + day;
}

/**
 * The id of an area from its save folder: its path under the game's areas.
 */
private string area_of_folder(string folder)
{
  string id;

  if (!strlen(folder) || strlen(folder) < strlen(areas_dir) ||
      folder[..strlen(areas_dir) - 1] != areas_dir)
    return "";

  id = folder[strlen(areas_dir)..];

  while (strlen(id) && id[strlen(id) - 1] == '/')
    id = id[..strlen(id) - 2];

  return id;
}

/**
 * The save folder of an area id.
 */
private string folder_of_area(string id)
{
  return areas_dir + id + "/";
}

/**
 * The nearest folder, this one or above, that holds an area.o; "" when none
 * inside the game's areas.
 */
private string nearest_area(string dir)
{
  string up;

  if (!undefinedp(nearest_cache[dir]))
    return nearest_cache[dir];

  if (strlen(dir) <= strlen(areas_dir))
    up = "";
  else if (file_size(dir + "area.o") > 0)
    up = dir;
  else
    up = nearest_area(folder_of(dir[..strlen(dir) - 2]));

  nearest_cache[dir] = up;
  return up;
}

/**
 * Reads the area save of a folder once: its name and its parent area.
 */
private mixed * area_data(string folder)
{
  mapping data;
  string name, parts, * pieces;

  if (area_info[folder])
    return area_info[folder];

  name = "";
  parts = "";
  data = nil;

  if (strlen(folder) && file_size(folder + "area.o") > 0)
    data = reader->read(folder + "area.o");

  if (data && stringp(data["exploration_name"]))
    name = strip_colours(data["exploration_name"]);

  // without a display name the folder is the name; "rooms" says nothing
  if (!strlen(name))
  {
    pieces = explode(area_of_folder(folder), "/");

    if (sizeof(pieces))
      name = pieces[sizeof(pieces) - 1];

    if (name == "rooms" && sizeof(pieces) > 1)
      name = pieces[sizeof(pieces) - 2];
  }

  if (data && stringp(data["parent_area"]))
    parts = area_of_folder(data["parent_area"]);

  area_info[folder] = ({ name, parts });
  return area_info[folder];
}

/**
 * The first location save of a folder that carries one of the components,
 * as the object path it names; "" when none does. Each component is
 * ({ component type, saved field }).
 */
private string named_object(string folder, mixed * kinds)
{
  mixed * entries, * kind;
  mapping data, info;
  string name;
  int i, j;

  entries = get_dir(folder + "*", -1);

  for (i = 0; i < sizeof(entries); i++)
  {
    name = entries[i][0];

    if (entries[i][1] < 0 || strlen(name) <= 2 ||
        name[strlen(name) - 2..] != ".o" || name == "area.o")
      continue;

    data = reader->read(folder + name);

    if (!data || !mappingp(data["components"]))
      continue;

    for (j = 0; j < sizeof(kinds); j++)
    {
      kind = kinds[j];
      info = data["components"][kind[0]];

      if (mappingp(info) && stringp(info[kind[1]]) && strlen(info[kind[1]]))
        return info[kind[1]];
    }
  }

  return "";
}

/**
 * The name players see for a guild or job object: its short, no colours.
 */
private string object_label(string path)
{
  object ob;
  string text;

  if (!strlen(path) || catch(ob = load_object(path)) || !ob)
    return "";

  text = nil;

  if (catch(text = ob->query_short()))
    return "";

  return strip_colours(text);
}

/**
 * A name in capitals-first form: hyphens become spaces.
 */
private string capitalise(string text)
{
  string first;

  text = replace_string(text, "-", " ");

  if (!strlen(text))
    return text;

  first = text[..0];

  if (first[0] >= 'a' && first[0] <= 'z')
    first[0] -= 'a' - 'A';

  return first + text[1..];
}

/**
 * How a viewer names the area outside the dump an exit leads to: ({ kind,
 * label }). Guild and job areas (their first path segment) are labelled with
 * the short of the guild or job their locations name; any other area with the
 * name of its top-level area, else its first path segment.
 */
private mixed * external_label(string target_id, string target_folder)
{
  string * parts, * result;
  string kind, top, label;
  mapping data;

  if (label_cache[target_id])
    return label_cache[target_id];

  parts = explode(target_id, "/");
  label = "";

  if (parts[0] == "guilds")
  {
    kind = "guild";
    label = object_label(named_object(target_folder,
      ({ ({ "guild-room", "room_guild" }),
         ({ "guild-entrance", "entrance_guild" }) })));
  }
  else if (parts[0] == "jobs")
  {
    kind = "job";
    label = object_label(named_object(target_folder,
      ({ ({ "job-room", "room_job" }),
         ({ "job-entrance", "entrance_job" }) })));
  }
  else
  {
    kind = "area";
    top = areas_dir + parts[0] + "/";

    if (file_size(top + "area.o") > 0)
    {
      data = reader->read(top + "area.o");

      if (data && stringp(data["exploration_name"]))
        label = strip_colours(data["exploration_name"]);
    }

    if (!strlen(label))
      label = capitalise(parts[0]);
  }

  label_cache[target_id] = ({ kind, label });
  return label_cache[target_id];
}

/**
 * The name shown for a location: the home name, else the specific title,
 * else the original one.
 */
private string location_name(mapping data)
{
  mixed home;
  string name;

  name = "";

  if (mappingp(data["components"]))
  {
    home = data["components"]["home"];

    if (mappingp(home) && stringp(home["home_short"]))
      name = strip_colours(home["home_short"]);
  }

  if (!strlen(name))
    name = strip_colours(data["specific_short"]);

  if (!strlen(name))
    name = strip_colours(data["original_short"]);

  return name;
}

/**
 * Whether an exit is one players can see: hidden and secret ones are not.
 */
private int visible_exit(mixed exit)
{
  if (!pointerp(exit) || sizeof(exit) < 2 || !stringp(exit[0]))
    return 0;

  return !(stringp(exit[1]) && (exit[1] == "hidden" || exit[1] == "secret"));
}

/**
 * Lists a folder: its .o files into the files list and its sub-folders into
 * the pending ones.
 */
private void scan_folder(string dir)
{
  mixed * entries;
  string name;
  int i;

  entries = get_dir(dir + "*", -1);

  for (i = 0; i < sizeof(entries); i++)
  {
    name = entries[i][0];

    if (entries[i][1] == -2)
    {
      // another area's folder is not part of this dump unless asked for
      if (with_sub_areas || file_size(dir + name + "/area.o") <= 0)
        pending_dirs += ({ dir + name + "/" });
    }
    else if (strlen(name) > 2 && name[strlen(name) - 2..] == ".o" &&
             name != "area.o")
      files += ({ dir + name });
  }
}

/**
 * Keeps one location save, when it has the data the map needs.
 */
private void read_location(string path)
{
  mapping data;
  string id, folder, area_folder;
  int * where;

  data = reader->read(path);

  if (!data || !stringp(data["file_name"]) || !strlen(data["file_name"]))
    return;

  where = data["coordinates"];

  if (!pointerp(where) || sizeof(where) != 3)
    return;

  id = path[strlen(root_dir)..strlen(path) - 3];
  folder = folder_of(path);
  area_folder = nearest_area(folder);

  // a save outside any area saved belongs to the dumped one
  if (!strlen(area_folder))
    area_folder = root_dir;

  area_data(area_folder);

  locations[path] = ({ id, location_name(data), area_folder,
                       where[0], where[1], where[2],
                       mappingp(data["exits"]) ? data["exits"] : ([ ]) });
}

/**
 * The work of one step: list some folders (phase 0) or read some saves
 * (phase 1).
 */
private void run_step(int phase)
{
  int i, stop;

  if (phase == 0)
  {
    for (i = 0; i < DIRS_PER_STEP && sizeof(pending_dirs); i++)
    {
      scan_folder(pending_dirs[0]);
      pending_dirs = pending_dirs[1..];
    }

    if (sizeof(pending_dirs))
    {
      call_out("step", 0, 0);
      return;
    }

    files = sort_strings(files);
    cursor = 0;
    call_out("step", 0, 1);
    return;
  }

  stop = cursor + READ_PER_STEP;

  for (; cursor < stop && cursor < sizeof(files); cursor++)
    read_location(files[cursor]);

  if (cursor < sizeof(files))
    call_out("step", 0, 1);
  else
    finish();
}

/**
 * One step of the run, in a call_out of its own so none runs out of ticks.
 */
void step(int phase)
{
  string err;

  err = catch(run_step(phase));

  if (err)
  {
    running = 0;
    tell_object(admin, message(_LANG_MAPPER_FAILED, ({ err })));
  }
}

/**
 * The JSON of a string.
 */
private string quote(string text)
{
  return json_encode(text);
}

/**
 * Integers ascending, as a list of JSON numbers.
 */
private string levels_json(mixed * values)
{
  int * sorted;
  int i, j, value;
  string text;

  sorted = ({ });

  for (i = 0; i < sizeof(values); i++)
  {
    value = values[i];

    for (j = 0; j < sizeof(sorted) && sorted[j] < value; j++);

    sorted = sorted[0..j - 1] + ({ value }) + sorted[j..];
  }

  text = "";

  for (i = 0; i < sizeof(sorted); i++)
    text += (i ? "," : "") + sorted[i];

  return text;
}

/**
 * Appends a JSON list to the output file, one element per line, between its
 * opening and closing text.
 */
private void write_list(string open, string * items, string close)
{
  int i;

  write_file(out_file, open);

  for (i = 0; i < sizeof(items); i++)
    write_file(out_file, items[i] + (i < sizeof(items) - 1 ? "," : "") + "\n");

  write_file(out_file, close);
}

/**
 * Writes the map file from what was read.
 */
private void finish()
{
  mapping by_id, area_ids, seen_levels;
  string * paths, * ids, * dirs, * rows, * ext, * areas;
  string path, line, target_folder, dest, target_id;
  mixed * loc, * exit, * tag;
  int i, j;

  by_id = ([ ]);
  area_ids = ([ ]);
  seen_levels = ([ ]);
  paths = map_indices(locations);

  if (!sizeof(paths))
  {
    running = 0;
    tell_object(admin, message(_LANG_MAPPER_EMPTY,
      ({ "/games/" + game + "/areas/" + area_id })));
    return;
  }

  for (i = 0; i < sizeof(paths); i++)
  {
    loc = locations[paths[i]];
    by_id[loc[0]] = paths[i];
    area_ids[loc[2]] = 1;
    seen_levels[loc[5]] = 1;
  }

  ids = sort_strings(map_indices(by_id));
  rows = ({ });
  ext = ({ });

  for (i = 0; i < sizeof(ids); i++)
  {
    loc = locations[by_id[ids[i]]];
    dirs = sort_strings(map_indices(loc[6]));
    line = "";

    for (j = 0; j < sizeof(dirs); j++)
    {
      exit = loc[6][dirs[j]];

      if (!visible_exit(exit))
        continue;

      dest = exit[0];

      // only saved locations of this game's areas can be shown on a map
      if (strlen(dest) <= strlen(areas_dir) ||
          dest[..strlen(areas_dir) - 1] != areas_dir ||
          dest[strlen(dest) - 2..] != ".o" || file_size(dest) <= 0)
        continue;

      if (locations[dest])
      {
        line += (strlen(line) ? "," : "") + "{\"dir\":" + quote(dirs[j]) +
                ",\"to\":" + quote(locations[dest][0]) + "}";
        continue;
      }

      target_folder = nearest_area(folder_of(dest));

      if (!strlen(target_folder))
        continue;

      target_id = area_of_folder(target_folder);
      tag = external_label(target_id, target_folder);

      ext += ({ "{\"from\":" + quote(ids[i]) + ",\"dir\":" + quote(dirs[j]) +
                ",\"area\":" + quote(target_id) +
                ",\"kind\":" + quote(tag[0]) +
                ",\"label\":" + quote(tag[1]) +
                ",\"to\":" +
                quote(dest[strlen(target_folder)..strlen(dest) - 3]) + "}" });
    }

    rows += ({ "{\"id\":" + quote(ids[i]) + ",\"name\":" + quote(loc[1]) +
               ",\"area\":" + quote(area_of_folder(loc[2])) +
               ",\"x\":" + loc[3] + ",\"y\":" + loc[4] + ",\"z\":" + loc[5] +
               ",\"exits\":[" + line + "]}" });
  }

  // areas, sorted by id
  paths = map_indices(area_ids);
  by_id = ([ ]);

  for (i = 0; i < sizeof(paths); i++)
    by_id[area_of_folder(paths[i])] = paths[i];

  ids = sort_strings(map_indices(by_id));
  areas = ({ });

  for (i = 0; i < sizeof(ids); i++)
  {
    loc = area_data(by_id[ids[i]]);
    areas += ({ "{\"id\":" + quote(ids[i]) + ",\"name\":" + quote(loc[0]) +
                ",\"parent\":" + quote(loc[1]) + "}" });
  }

  remove_file(out_file);
  write_file(out_file,
    "{\"format\":" + quote(FORMAT) + ",\"game\":" + quote(game) +
    ",\"area\":" + quote(area_id) +
    ",\"name\":" + quote(area_data(root_dir)[0]) +
    ",\"generated\":" + quote(today()) + ",\n");

  write_list("\"areas\":[\n", areas, "],\n");
  write_file(out_file, "\"levels\":[" + levels_json(map_indices(seen_levels)) +
             "],\n");
  write_list("\"locations\":[\n", rows, "],\n");
  write_list("\"external\":[\n", ext, "]}\n");

  running = 0;

  if (file_size(out_file) <= 0)
  {
    tell_object(admin, message(_LANG_MAPPER_WRITE_FAIL, ({ out_file })));
    return;
  }

  tell_object(admin, message(_LANG_MAPPER_DONE,
    ({ out_file, sizeof(rows), sizeof(areas), sizeof(ext) })));
}

/**
 * Starts the dump of an area, by its folder (/games/<game>/areas/<area>).
 * The result comes later, to the admin. Returns an error text, or "" when the
 * run started.
 */
string update_map(object me, string dir, int sub_areas)
{
  string * parts;
  string save_dir;

  if (running)
    return _LANG_MAPPER_BUSY;

  while (strlen(dir) > 1 && dir[strlen(dir) - 1] == '/')
    dir = dir[..strlen(dir) - 2];

  parts = explode(dir, "/") - ({ "" });
  dir = "/" + implode(parts, "/");

  if (sizeof(parts) < 4 || parts[0] != "games" || parts[2] != "areas")
    return message(_LANG_MAPPER_BAD_DIR, ({ dir }));

  game = parts[1];
  area_id = implode(parts[3..], "/");
  areas_dir = "/save/games/" + game + "/locations/areas/";
  root_dir = areas_dir + area_id + "/";

  if (file_size(root_dir) != -2)
    return message(_LANG_MAPPER_NO_SAVES, ({ dir }));

  mkdir("/games/" + game + "/docs");
  mkdir("/games/" + game + "/docs/maps");
  out_file = "/games/" + game + "/docs/maps/" + implode(parts[3..], "-") + ".json";

  admin = me;
  with_sub_areas = sub_areas;
  reader = load_object(READER);
  pending_dirs = ({ root_dir });
  files = ({ });
  nearest_cache = ([ ]);
  area_info = ([ ]);
  locations = ([ ]);
  label_cache = ([ ]);
  cursor = 0;
  running = 1;

  tell_object(me, message(_LANG_MAPPER_START, ({ dir })));
  call_out("step", 0, 0);
  return "";
}

/**
 * The maps written, one line each, for one game or for all of them.
 */
string list_maps(string wanted)
{
  string * games, * names;
  mixed * entries;
  string text, dir;
  int i, j;

  if (strlen(wanted))
  {
    if (file_size("/games/" + wanted) != -2)
      return message(_LANG_MAPPER_NO_GAME, ({ wanted }));

    games = ({ wanted });
  }
  else
  {
    games = ({ });
    entries = get_dir("/games/*", -1);

    for (i = 0; i < sizeof(entries); i++)
      if (entries[i][1] == -2)
        games += ({ entries[i][0] });

    games = sort_strings(games);
  }

  text = "";

  for (i = 0; i < sizeof(games); i++)
  {
    dir = "/games/" + games[i] + "/docs/maps/";
    entries = get_dir(dir + "*.json", -1);

    if (!sizeof(entries))
      continue;

    names = ({ });
    for (j = 0; j < sizeof(entries); j++)
      names += ({ entries[j][0] });

    names = sort_strings(names);
    text += games[i] + ":\n";

    for (j = 0; j < sizeof(names); j++)
      text += message(_LANG_MAPPER_LIST_ROW,
        ({ names[j], file_size(dir + names[j]),
           ctime(get_dir(dir + names[j], -1)[0][2]) }));
  }

  return strlen(text) ? text : _LANG_MAPPER_NO_MAPS;
}
