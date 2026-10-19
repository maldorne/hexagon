// balance -- admin tool to dump the numbers that balance a game: its races,
// classes, guilds, jobs, damaging and healing spells and skills, evaluated by the
// game's own code on a test monster, written as CSV files under
// /games/<game>/docs/balance/ (one folder per game).
//
//   balance [<game>]    the game of the room you are in by default

#include <mud/cmd.h>
#include <living/effects.h>
#include <translations/combat.h>

inherit CMD_BASE;

// every stat of the test monster; the files say it (baselines.csv)
#define BALANCE_STAT 13
#define BALANCE_LEVELS 20
#define BALANCE_POWERS 3
// the monster that stands for a caster or a player
#define DUMMY_PATH "/lib/monster.c"
// how long a level or a power set on the test caster is held (heart_beats)
#define HOLD_TIME 1000
// spells and skills evaluated in each call_out, to stay within the ticks of
// one call
#define EVALUATED_PER_STEP 2

private string format_decimal(float value, int scale);
private string category_text(object effect);

// the run in progress: only one at a time
private string game, out_dir;
private object admin, master, dummy;
private string * queue;
// the effects of the step in progress
private string * effect_ids;
private mapping effect_paths;
private int cursor;
private int running;
// the columns of the effects file that depend on the effects: their
// targeting data and the extra numbers some of them add; known after a first
// pass over all of them, which comes before the rows are written
private string * targeting_keys, * extra_keys;
private int scanning;

void setup()
{
  set_aliases(({ "balance" }));
  set_usage("balance [<game>]");
  set_help(
    "Dumps the numbers that balance a game as CSV files under\n" +
    "/games/<game>/docs/balance/: its races, classes, guilds, jobs, the spells and\n" +
    "skills that damage or heal, and the baselines (hit points, regeneration)\n" +
    "they are measured against. Everything is evaluated by the game's own code\n" +
    "on a test monster with every stat at " + BALANCE_STAT + ", a spell for each\n" +
    "level 1 to " + BALANCE_LEVELS + " and power 1 to " + BALANCE_POWERS + ".\n" +
    "\n" +
    "  balance            the game of the room you are in\n" +
    "  balance <game>     another game, by its folder name\n" +
    "\n" +
    "Files: races.csv, classes.csv, guilds.csv, jobs.csv, effects.csv, skills.csv,\n" +
    "baselines.csv, stat_bonuses.csv. The work is split in steps and takes a\n" +
    "few seconds; the command says when it is done. A game adds columns to\n" +
    "baselines.csv with query_balance_baselines(level) in its master.\n");
}

// ===== csv =====

/**
 * The damage type in English: the game names them in its language, the dump
 * does not.
 */
private string english_type(string type)
{
  string * mine, * english;
  int index;

  if (!stringp(type) || !strlen(type))
    return "";

  mine = RESISTANCES + AC_TYPES_STR;
  english = ({ "acid", "air", "electrical", "cold", "magic", "psionic",
    "missiles", "poison", "law", "water", "good", "disease", "fire", "evil",
    "death", "stone", "chaos", "slashing", "blunt", "piercing" });

  index = member_array(type, mine);
  return index == -1 ? type : english[index];
}

/**
 * The folder of the game being dumped.
 */
private string game_dir()
{
  return "/games/" + game + "/";
}

/**
 * One value as a CSV field: quoted when it holds a comma, a quote or a line
 * break. A float is written with one decimal.
 */
private string csv_field(mixed value)
{
  string text;

  if (value == nil)
    return "";

  if (floatp(value))
    text = format_decimal((float)value, 10);
  else
    text = "" + value;

  if (strsrch(text, ",") != -1 || strsrch(text, "\"") != -1 ||
      strsrch(text, "\n") != -1)
    text = "\"" + replace_string(text, "\"", "\"\"") + "\"";

  return text;
}

/**
 * A float with a fixed number of decimals, given as the power of ten that
 * scales it (10 for one decimal, 100 for two), rounded.
 */
private string format_decimal(float value, int scale)
{
  string frac;
  int whole, parts, digits;

  // the cast of a float to an int already rounds to the nearest
  parts = (int)(value * (float)scale);
  whole = parts / scale;
  frac = "" + (parts % scale);
  digits = strlen("" + scale) - 1;

  while (strlen(frac) < digits)
    frac = "0" + frac;

  return whole + "." + frac;
}

/**
 * Appends a row to a file of the output folder.
 */
private void put_row(string file, mixed * fields)
{
  string line;
  int i;

  line = "";
  for (i = 0; i < sizeof(fields); i++)
    line += (i ? "," : "") + csv_field(fields[i]);

  write_file(out_dir + file, line + "\n");
}

/**
 * Starts a file again, with its header row.
 */
private void start_file(string file, string * header)
{
  remove_file(out_dir + file);
  put_row(file, header);
}

// ===== what there is =====

/**
 * The files of a directory and of the directories under it (one level),
 * full paths with ".c", sorted.
 */
private string * files_under(string dir)
{
  string * names, * files, * more;
  int i, j;

  names = get_dir(dir + "*");
  files = ({ });

  if (!names)
    return files;

  for (i = 0; i < sizeof(names); i++)
  {
    if (file_size(dir + names[i]) == -2)
    {
      more = get_dir(dir + names[i] + "/*.c");
      for (j = 0; more && j < sizeof(more); j++)
        files += ({ dir + names[i] + "/" + more[j] });
    }
    else if (strlen(names[i]) > 2 && names[i][strlen(names[i]) - 2..] == ".c")
      files += ({ dir + names[i] });
  }

  return sort_array(files);
}

/**
 * Loads an object, nil when it does not compile.
 */
private object try_load(string path)
{
  object ob;

  catch(ob = load_object(path));
  return ob;
}

/**
 * A fresh test monster with every stat at BALANCE_STAT and no racial bonus.
 */
private object make_dummy()
{
  object ob;

  ob = clone_object(DUMMY_PATH);
  ob->set_str(BALANCE_STAT);
  ob->set_con(BALANCE_STAT);
  ob->set_dex(BALANCE_STAT);
  ob->set_int(BALANCE_STAT);
  ob->set_wis(BALANCE_STAT);
  ob->set_cha(BALANCE_STAT);
  ob->reset_bon_stats();
  return ob;
}

/**
 * The id of a class, race or guild file: its name without the folders and
 * the extension.
 */
private string id_of(string path)
{
  string * parts;

  parts = explode(path, "/");
  path = parts[sizeof(parts) - 1];
  return path[0..strlen(path) - 3];
}

// ===== steps =====

/**
 * The next step of the run, in a call_out of its own so no step runs out of
 * ticks.
 */
private void next_step()
{
  string step;

  if (!sizeof(queue))
  {
    running = 0;
    if (dummy)
      dummy->dest_me();
    tell_object(admin, "Balance dump of '" + game + "' done: " + out_dir + "\n");
    return;
  }

  step = queue[0];
  queue = queue[1..];
  call_out(step, 0);
}

/**
 * What a bonus of the stat gives, for the stat values 1 to 30.
 */
void step_stat_bonuses()
{
  int value;

  start_file("stat_bonuses.csv", ({ "value", "bonus" }));
  for (value = 1; value <= 30; value++)
    put_row("stat_bonuses.csv", ({ value, dummy->query_stat_bonus(value) }));

  next_step();
}

/**
 * The races of the game and what they give.
 */
void step_races()
{
  string * files;
  string rule;
  object race;
  int i;

  start_file("races.csv", ({ "race", "file", "playable", "str", "con", "dex",
    "int", "wis", "cha", "body_size", "regen_rule" }));

  files = files_under(game_dir() + "obj/races/");
  for (i = 0; i < sizeof(files); i++)
  {
    race = try_load(files[i]);
    if (!race || !race->query_is_race_ob())
      continue;

    dummy->reset_bon_stats();
    race->set_racial_bonuses(dummy);

    // a race may keep its members from regenerating where it likes
    rule = "default";
    if (function_object("query_regen_hp", race) != "/lib/race" ||
        function_object("query_regen_gp", race) != "/lib/race")
      rule = "custom";

    put_row("races.csv", ({ race->query_race_id(), files[i],
      race->query_playable() ? "yes" : "no", dummy->query_bonus_str(),
      dummy->query_bonus_con(), dummy->query_bonus_dex(),
      dummy->query_bonus_int(), dummy->query_bonus_wis(),
      dummy->query_bonus_cha(), race->query_body_size(), rule }));
  }

  dummy->reset_bon_stats();
  next_step();
}

/**
 * The classes of the game.
 */
void step_classes()
{
  string * files;
  object class_ob;
  int i;

  start_file("classes.csv", ({ "class", "file", "hit_dice", "gp_dice",
    "hp_bonus", "gp_bonus", "combat_bonus", "xp_cost", "xp_growth" }));

  files = files_under(game_dir() + "obj/classes/");
  for (i = 0; i < sizeof(files); i++)
  {
    class_ob = try_load(files[i]);
    if (!class_ob)
      continue;

    put_row("classes.csv", ({ class_ob->query_class_id(), files[i],
      class_ob->query_hit_dice(), class_ob->query_gp_dice(),
      class_ob->query_hp_bonus(), class_ob->query_gp_bonus(),
      class_ob->query_combat_bonus(), class_ob->query_level_xp_cost(),
      format_decimal(class_ob->query_level_xp_growth(), 100) }));
  }

  next_step();
}

/**
 * Writes one row for each spell and skill that the guilds or the jobs of the
 * game teach or relate to themselves.
 * @param  csv     the file to write
 * @param  dir     where their objects are, under the game folder
 * @param  guilds  1 for the guilds, 0 for the jobs
 */
private void write_abilities(string csv, string dir, int guilds)
{
  string * files, * ids;
  mapping abilities;
  object guild, spell;
  string classes, power, categories;
  int i, j, level;

  start_file(csv, ({ guilds ? "guild" : "job", "file", "class", "ability",
    "ability_kind", "level", "power", "categories", "related" }));

  files = files_under(game_dir() + dir);
  for (i = 0; i < sizeof(files); i++)
  {
    guild = try_load(files[i]);
    if (!guild || (guilds ? !guild->query_is_guild() : !guild->query_is_job()))
      continue;

    classes = implode(guild->query_legal_classes() + ({ }), "|");

    abilities = guild->query_ability_spells();
    ids = sort_array(map_indices(abilities));
    for (j = 0; j < sizeof(ids); j++)
    {
      spell = try_load(ids[j]);
      power = "";
      categories = "";
      if (guild->query_ability_spell_power(ids[j]))
        power = ({ "", "minor", "neutral", "major" })[guild->query_ability_spell_power(ids[j])];
      if (spell)
        categories = category_text(spell);

      put_row(csv, ({ id_of(files[i]), files[i], classes,
        guild->query_spell_id(ids[j]), "spell", abilities[ids[j]], power,
        categories, 0 }));
    }

    // spells tied to the guild or job but not taught: no level, related = 1
    ids = sort_array(guild->query_related_ability_spells());
    for (j = 0; j < sizeof(ids); j++)
    {
      spell = try_load(ids[j]);
      put_row(csv, ({ id_of(files[i]), files[i], classes,
        guild->query_spell_id(ids[j]), "spell", "", "",
        spell ? category_text(spell) : "", 1 }));
    }

    abilities = guild->query_ability_skills();
    ids = sort_array(map_indices(abilities));
    for (j = 0; j < sizeof(ids); j++)
    {
      level = abilities[ids[j]][0];
      put_row(csv, ({ id_of(files[i]), files[i], classes, ids[j],
        "skill", level, "", "", 0 }));
    }

    // skills tied to the guild or job but not taught: no level, related = 1
    ids = sort_array(guild->query_related_ability_skills());
    for (j = 0; j < sizeof(ids); j++)
      put_row(csv, ({ id_of(files[i]), files[i], classes, ids[j],
        "skill", "", "", "", 1 }));
  }
}

/**
 * The guilds of the game and the spells and skills they teach.
 */
void step_guilds()
{
  write_abilities("guilds.csv", "obj/guilds/", 1);
  next_step();
}

/**
 * The jobs of the game and the spells and skills they teach.
 */
void step_jobs()
{
  write_abilities("jobs.csv", "obj/jobs/", 0);
  next_step();
}

/**
 * The categories of a spell with its level in each: "combat:5|elemental:6".
 */
private string category_text(object effect)
{
  mapping levels;
  string * ids, text;
  int i;

  levels = effect->query_category_levels();
  ids = sort_array(map_indices(levels));
  text = "";

  for (i = 0; i < sizeof(ids); i++)
    text += (i ? "|" : "") + ids[i] + ":" + levels[ids[i]];

  return text;
}

/**
 * Hit points of an npc and of a player of each class, and how fast both
 * regenerate, level by level; plus what the game adds.
 */
void step_baselines()
{
  string * class_files, * extra_keys, * header;
  mapping extras;
  object npc, player, class_ob;
  mixed * row;
  int * gains;
  int level, i, start_hp;

  class_files = files_under(game_dir() + "obj/classes/");
  header = ({ "level", "stat", "npc_hp" });
  gains = ({ });

  // the hp per level of each class, from the game's own function
  player = make_dummy();
  for (i = 0; i < sizeof(class_files); i++)
  {
    class_ob = try_load(class_files[i]);
    if (!class_ob)
      continue;

    header += ({ "player_hp_" + class_ob->query_class_id() });
    player->set_class_ob(class_files[i]);
    gains += ({ player->query_level_hp_gain() });
  }
  start_hp = player->query_max_hp();
  player->dest_me();

  extra_keys = ({ });
  if (master && function_object("query_balance_baselines", master))
  {
    extras = master->query_balance_baselines(1);
    if (mappingp(extras))
      extra_keys = sort_array(map_indices(extras));
  }

  header += ({ "regen_seconds_per_hp", "regen_seconds_per_gp" }) + extra_keys;
  start_file("baselines.csv", header);

  // an npc has constitution times level hit points (lib/monster.c)
  npc = make_dummy();

  for (level = 1; level <= BALANCE_LEVELS; level++)
  {
    npc->set_level(level);
    row = ({ level, BALANCE_STAT, npc->query_max_hp() });

    // a player starts with one hit point and gets a level's gain per level
    // (the first levels are given automatically, the gain is the same)
    for (i = 0; i < sizeof(gains); i++)
      row += ({ start_hp + level * gains[i] });

    row += ({ format_decimal(dummy->query_regen_seconds(dummy->query_con()), 100),
              format_decimal(dummy->query_regen_seconds(dummy->query_gp_main_stat()), 100) });

    extras = master && sizeof(extra_keys) ?
      master->query_balance_baselines(level) : ([ ]);
    for (i = 0; i < sizeof(extra_keys); i++)
      row += ({ extras[extra_keys[i]] });

    put_row("baselines.csv", row);
  }

  npc->dest_me();
  next_step();
}

// ===== spells and skills =====

/**
 * The spells (or skills) to evaluate, id -> path. The file and its header wait
 * for the first pass (see scan_effect), which finds the columns.
 */
private void start_effects(mapping found)
{
  effect_paths = found;
  effect_ids = sort_array(map_indices(found));
  cursor = 0;
  targeting_keys = ({ });
  extra_keys = ({ });
  scanning = 1;
}

/**
 * Writes the header of the effects file once the first pass has found the
 * columns: the fixed ones, the targeting data every effect declares and the
 * extra numbers of some.
 */
private void start_effects_file(string file)
{
  start_file(file, ({ "spell", "name", "file", "base", "categories", "kind" }) +
    targeting_keys + ({ "damage_type", "level", "power", "average", "minimum",
    "maximum", "rounds", "cost" }) + extra_keys + ({ "note" }));
}

/**
 * What a macro of the headers of the game (or of the effects) stands for,
 * when it is a string: the path of a base such as CLERIC_SPELL.
 */
private string macro_value(string name)
{
  string * dirs, * files, * lines;
  string text, def, value;
  int i, j, k;

  dirs = ({ game_dir() + "include/", "/include/living/" });

  for (i = 0; i < sizeof(dirs); i++)
  {
    files = get_dir(dirs[i] + "*.h");
    for (j = 0; files && j < sizeof(files); j++)
    {
      text = read_file(dirs[i] + files[j]);
      if (!text || strsrch(text, "#define " + name + " ") == -1)
        continue;

      lines = explode(text, "\n");
      for (k = 0; k < sizeof(lines); k++)
        if (sscanf(lines[k], "#define %s %s", def, value) == 2 && def == name)
        {
          value = trim(value);
          if (strlen(value) > 1 && value[0] == '"')
            return value[1..strsrch(value[1..], "\"")];
          return nil;
        }
    }
  }

  return nil;
}

/**
 * The file a spell or skill inherits: the one of its first inherit
 * statement, a path or the macro that stands for one. Nil when it cannot be
 * told.
 */
private string base_of(string path)
{
  string * lines;
  string text, token;
  int i;

  text = read_file(path);
  if (!text)
    return nil;

  lines = explode(text, "\n");
  for (i = 0; i < sizeof(lines); i++)
  {
    if (sscanf(lines[i], "inherit %s;", token) != 1)
      continue;

    token = trim(token);
    if (strlen(token) > 1 && token[0] == '"')
      return token[1..strlen(token) - 2];

    return macro_value(token);
  }

  return nil;
}

/**
 * Sets the level and the power a cast is evaluated at on the test caster, the
 * way a scroll holds them.
 */
private void hold_cast(int level, int power)
{
  dummy->remove_timed_property(PROPERTY_CAST_LEVEL);
  dummy->remove_timed_property(PROPERTY_CAST_POWER);
  dummy->add_timed_property(PROPERTY_CAST_LEVEL, level, HOLD_TIME);
  if (power)
    dummy->add_timed_property(PROPERTY_CAST_POWER, power, HOLD_TIME);
}

/**
 * What a spell or skill does at one level and power, nil when it neither
 * damages nor heals (or fails to answer).
 */
private mapping evaluate(object effect, int level, int power)
{
  mapping balance;
  string error;

  hold_cast(level, power);
  error = catch(balance = effect->query_balance(dummy));
  if (error)
  {
    tell_object(admin, "balance: " + object_name(effect) + " level " + level +
      ": " + error + "\n");
    return nil;
  }

  return balance;
}

/**
 * First pass over a spell or skill: takes the names of its targeting data and
 * of the extra numbers it adds, to make the columns of the file.
 */
private void scan_effect(string path, int powers)
{
  mapping balance;
  object effect;
  string * keys;
  int i;

  effect = try_load(path);
  if (!effect || !function_object("query_balance", effect))
    return;

  balance = evaluate(effect, 1, powers ? 1 : 0);
  if (!balance)
    return;

  if (!sizeof(targeting_keys))
    targeting_keys = effect->query_targeting_keys();

  if (!mappingp(balance["extra"]))
    return;

  keys = map_indices(balance["extra"]);
  for (i = 0; i < sizeof(keys); i++)
    if (member_array(keys[i], extra_keys) == -1)
      extra_keys += ({ keys[i] });

  extra_keys = sort_array(extra_keys);
}

/**
 * Writes the rows of a spell or skill, one per level and power; nothing for
 * one that neither damages nor heals.
 *
 * @param  file    the file to write to
 * @param  powers  whether the power changes the numbers (a spell) or not
 */
private void dump_effect(string file, string id, string path, int powers)
{
  mapping balance;
  object effect;
  string base;
  mixed * row;
  int level, power, last, i;

  effect = try_load(path);
  if (!effect || !function_object("query_balance", effect))
    return;

  if (!evaluate(effect, 1, powers ? 1 : 0))
    return;

  base = base_of(path);
  last = powers ? BALANCE_POWERS : 0;

  for (level = 1; level <= BALANCE_LEVELS; level++)
    for (power = powers ? 1 : 0; power <= last; power++)
    {
      balance = evaluate(effect, level, power);
      if (!balance)
        continue;

      row = ({ id, effect->query_effect_name(), path, base,
        category_text(effect), balance["kind"] });

      for (i = 0; i < sizeof(targeting_keys); i++)
        row += ({ balance[targeting_keys[i]] });

      row += ({ english_type(balance["damage_type"]), level,
        powers ? power : nil, balance["average"], balance["minimum"],
        balance["maximum"], balance["rounds"], balance["cost"] });

      // the extra numbers this effect has, empty for the columns it lacks
      for (i = 0; i < sizeof(extra_keys); i++)
        row += ({ mappingp(balance["extra"]) ?
                  balance["extra"][extra_keys[i]] : nil });

      row += ({ balance["note"] });
      put_row(file, row);
    }

  dummy->remove_timed_property(PROPERTY_CAST_LEVEL);
  dummy->remove_timed_property(PROPERTY_CAST_POWER);
}

/**
 * Goes through the next effects of the list, a few each time: first to find
 * the columns, then to write the rows.
 *
 * @param  step    the function to call again for the next ones
 * @param  powers  whether the power changes the numbers (a spell) or not
 */
private void step_effects(string step, string file, int powers)
{
  int end;

  end = cursor + EVALUATED_PER_STEP;
  for (; cursor < end && cursor < sizeof(effect_ids); cursor++)
    if (scanning)
      scan_effect(effect_paths[effect_ids[cursor]], powers);
    else
      dump_effect(file, effect_ids[cursor], effect_paths[effect_ids[cursor]],
        powers);

  if (scanning && cursor >= sizeof(effect_ids))
  {
    scanning = 0;
    cursor = 0;
    start_effects_file(file);
  }

  if (cursor < sizeof(effect_ids))
  {
    call_out(step, 0);
    return;
  }

  effect_ids = nil;
  next_step();
}

/**
 * The next spells of the game, a few each time.
 */
void step_spells()
{
  if (!effect_ids)
    start_effects(table("spells", master)->query_spells());

  step_effects("step_spells", "effects.csv", 1);
}

/**
 * The next skills of the game, a few each time.
 */
void step_skills()
{
  mapping rows, found;
  string * ids;
  int i;

  if (!effect_ids)
  {
    rows = table("skills", master)->query_skills();
    found = ([ ]);
    ids = map_indices(rows);
    for (i = 0; i < sizeof(ids); i++)
      if (stringp(rows[ids[i]][0]))
        found[ids[i]] = rows[ids[i]][0] + ".c";

    start_effects(found);
  }

  step_effects("step_skills", "skills.csv", 0);
}

// ===== the command =====

static int cmd(string str, object me, string verb)
{
  if (this_player(1) != this_user())
    return 0;

  if (running)
  {
    tell_object(me, "A balance dump is already running.\n");
    return 1;
  }

  game = strlen(str) ? trim(str) : game_name(me);
  if (!strlen(game) || file_size("/games/" + game + "/master.c") < 0)
  {
    tell_object(me, "There is no game '" + game + "'.\n" + query_usage() + "\n");
    return 1;
  }

  admin = me;
  master = try_load("/games/" + game + "/master.c");
  out_dir = "/games/" + game + "/docs/balance/";
  mkdir(out_dir);

  dummy = make_dummy();
  effect_ids = nil;

  queue = ({ "step_stat_bonuses", "step_races", "step_classes", "step_guilds",
             "step_jobs", "step_baselines", "step_spells", "step_skills" });
  running = 1;

  tell_object(me, "Dumping the balance of '" + game + "' into " + out_dir + "...\n");
  next_step();
  return 1;
}
