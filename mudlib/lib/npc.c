// npc.c — a full NPC: a monster plus the area-managed persistence layer.
//
// This is what the area system spawns. As a generic clone it has an empty
// setup() and takes all its data from a bestiary data template
// (apply_template); hand-authored NPCs that need persistence / area membership
// inherit this instead of /lib/monster.c. Plain /lib/monster.c stays lean:
// none of the persisted-NPC state lives on it.
//
// The persistence layer gives an area-managed NPC its census identity, its own
// savefile for mutable state, and death cleanup. It is inert until an area
// stamps an identity (set_uuid + set_npc_area_path + set_npc_game).

#include <living/persisted.h>
#include <areas/area.h>
#include <basic/money.h>
#include <translations/money.h>
#include <living/skills.h>
#include <npc/npc.h>

inherit monster   "/lib/monster.c";
// Inventory persistence, the same mixin players use: create_auto_load snapshots
// the carried items into a saveable map, load_auto_load rebuilds them. This is
// what lets an NPC keep its own equipment on its npc.o across reboots.
inherit autoload  "/lib/core/basic/auto_load.c";
// Behaviour components (schedule, guard, toll, ...) attached per NPC: the NPC
// is their entity. Each inherits /lib/npc/component.c and sits under here:
inherit entity    "/lib/core/ecs/entity.c";

#define NPC_COMPONENT_DIR "/lib/npc/components/"

// Persisted into the NPC's own savefile so a restored NPC knows who it is.
string npc_uuid;         // census identity; nil/"" => not a persisted NPC
string npc_game;         // game slug, for the savefile path
string npc_area_path;    // owning area (census controller)
string npc_poi;          // owning point of interest, if any
string npc_source;       // blueprint path this NPC was spawned from; the
                         // bestiary-template identity and census key. Lets the
                         // NPC describe itself without a census lookup.
mapping npc_auto_load;   // the NPC's carried inventory, encoded for save_object
                         // (same shape as a player's auto_load)
string npc_given_name;   // a generated citizen's proper name (lowercase).
                         // id.c's `name` is static (never saved), so the one bit
                         // of a generated NPC's identity that is not code/template
                         // derived lives here, in the npc.o. Language-neutral (a
                         // proper noun does not translate). Nil for template NPCs,
                         // whose name comes from apply_template.
// whether its template gives it a name of its own (see has_proper_name)
static int proper_name;
// whether its template makes it one particular person without a name of its
// own (see is_unique)
static int unique_person;

// For an anonymous monster (no uuid): the location whose bucket counts it, set
// when it is cloned. It is bookkeeping, not identity -- the animal may wander
// off, and on death it still decrements the bucket it was counted in.
string npc_monster_location;

// This individual's concrete workplace location file, resolved by the schedule
// component's "work" symbol. Assigned once and persisted in npc.o (the type-level
// timetable is generic; where this NPC actually works is per-individual). The
// area that rosters this NPC (population caps, scheduler) is npc_area_path -- the
// census controller -- and stays fixed even when the NPC walks into another area;
// where it physically is (its position) is tracked in the areas handler.
string npc_work;

void create()
{
  monster::create();

  npc_uuid = nil;
  npc_game = nil;
  npc_area_path = nil;
  npc_poi = nil;
  npc_source = nil;
  npc_auto_load = ([ ]);
  npc_given_name = nil;
  npc_work = nil;
  npc_monster_location = nil;
  entity::create();
}

int query_persisted();

// Where this NPC lives: the house its area's books list it in, or nil. The
// schedule component walks the NPC there at night via travel_to(query_home()).
string query_home()
{
  object area;
  mixed house;

  if (!query_persisted() || !npc_area_path)
    return nil;

  area = AREA_HANDLER->query_area(npc_area_path);
  house = area ? area->query_house_of(npc_uuid) : nil;
  // a household of a shared house lives in that house's location
  return (stringp(house) && strlen(house)) ? area->query_house_file(house) : nil;
}

string query_monster_location() { return npc_monster_location; }
void set_monster_location(string file) { npc_monster_location = file; }

string query_work() { return npc_work; }
void set_work(string file) { npc_work = file; }

// ---------------------------------------------------------------------------
// Components (the entity half is /lib/core/ecs/entity.c)
// ---------------------------------------------------------------------------

string query_component_dir() { return NPC_COMPONENT_DIR; }

// Whether this NPC's routine keeps it where it is (at home for the night), so
// it does not wander off.
int query_staying_put()
{
  object sched;

  sched = query_component_by_type("schedule");
  return sched ? (int)sched->query_staying_put() : 0;
}

/**
 * Whether this NPC's routine has it off work, resting at home or on its way
 * there.
 */
int query_resting()
{
  object sched;

  sched = query_component_by_type("schedule");
  return sched ? (int)sched->query_resting() : 0;
}

/**
 * The NPC has moved: pass it on to the components.
 */
void event_moved(object ob, object from, object dest)
{
  run_on_components("event_moved", ob, from, dest);
}

/**
 * Something arrived where this NPC stands: the monster reacts, then the
 * components do.
 */
void event_enter(object ob, varargs string mess, object from, mixed avoid)
{
  monster::event_enter(ob, mess, from, avoid);
  run_on_components("event_enter", ob, mess, from, avoid);
}

/**
 * Something left the place where this NPC stands: the monster reacts, then
 * the components do.
 */
void event_exit(object ob, varargs string mess, object dest, mixed avoid)
{
  monster::event_exit(ob, mess, dest, avoid);
  run_on_components("event_exit", ob, mess, dest, avoid);
}

// Guard proxy. The room's exit handler consults a posted guard through
// guardian_check / guardian_message on the NPC object; delegate to the guard
// component when this NPC carries one, or to the toll component of one who
// charges for the way. An NPC without it blocks nobody, so a
// non-guard consulted by mistake simply lets everyone through.
int guardian_check(object mover)
{
  object g;
  g = query_component_by_type("guard");
  if (!g)
    g = query_component_by_type("toll");
  return g ? g->check(mover) : 1;
}

string guardian_message()
{
  object g;
  g = query_component_by_type("guard");
  if (!g)
    g = query_component_by_type("toll");
  return g ? g->message() : nil;
}

// Forward weather / climate events to this NPC's components (a climate component
// can react to rain, cold, etc.).
void event_weather(object who, varargs int flag, int * values)
{
  run_on_components("event_weather", who, flag, values);
}

// The areas handler calls this at a schedule hour: hand it to the schedule
// component (if any), which walks the NPC to where it should be this hour.
void do_schedule(int hour)
{
  run_on_components("do_schedule", hour);
}

// Materialization counterpart of do_schedule: walk to where the hour's entry
// puts this NPC, for one that came back mid-errand with its route gone.
void resume_schedule(int hour)
{
  run_on_components("resume_schedule", hour);
}

// A fight in this NPC's room: join it the way any monster does, and let the
// components react (a bodyguard defends whoever it stands by).
void event_fight_in_progress(object ob, object * fighters)
{
  monster::event_fight_in_progress(ob, fighters);
  run_on_components("event_fight_in_progress", ob, fighters);
}

// A crime in this NPC's place (crimes handler): the components react to it.
// criminal is nil when nobody could name the culprit.
void event_crime(object who, object criminal, string type, object victim,
                 int points)
{
  run_on_components("event_crime", who, criminal, type, victim, points);
}

// The hide shadow tells whoever it hid that a search found them.
void event_revealed(object finder)
{
  run_on_components("event_revealed", finder);
}

// Give this NPC a generated proper name: store it (persisted in npc.o) and set
// it as the engine name (the find_living id), lowercased. Call on a freshly
// cloned NPC, before any template names it -- monster::set_name only takes the
// first name. A generated citizen's short/long still come from the template.
void set_given_name(string s)
{
  // stored lowercase, like every name in a savefile / the find_living id;
  // capitalise it only where it is displayed
  npc_given_name = s ? lower_case(s) : s;
  if (npc_given_name && strlen(npc_given_name))
    set_name(npc_given_name);
}

string query_given_name() { return npc_given_name; }

// One particular person, from a template with a proper name: such an NPC is
// weighed in the ranking as a player is.
int has_proper_name() { return proper_name; }

// One particular person, with a name of its own or not: it keeps its template's
// name, race and level instead of a generated citizen's.
int is_unique() { return proper_name || unique_person; }

// A generated citizen is shown by its kind, not its proper name, wherever
// query_cap_name drives the display -- room / glance lists, combat and death
// messages, emotes -- so it never reads like a player. Its proper name
// (npc_given_name) stays the find_living id and targeting key, and is surfaced
// only when you examine it. Only NPCs we gave a generated name are affected;
// every other object (fauna, uniques, players) keeps the default cap_name.
string query_cap_name()
{
  if (npc_given_name && strlen(npc_given_name))
    return query_short();
  return ::query_cap_name();
}

// When you examine a generated citizen directly its short reveals the name in
// full -- "<kind> (<Name> <House>)" -- while query_cap_name above keeps room
// lists and combat on the bare kind. `short()` is what the look command prints
// as the examine header; query_short (the raw kind) is left untouched so
// cap_name and the plural stay clean.
string short(varargs int dark)
{
  if (npc_given_name && strlen(npc_given_name))
    return query_short() + " (" + query_full_name() + ")";
  return ::short(dark);
}

// The room-contents grouping keys on pretty_short. Keep it on the bare kind so
// same-kind citizens collapse into one line (a count plus the plural) instead
// of each listing separately -- the name lives only on short() (the examine
// header) and must not leak into the grouping key.
string pretty_short(varargs int dark)
{
  if (npc_given_name && strlen(npc_given_name))
    return query_short();
  return ::pretty_short(dark);
}

// Generic NPC: no hard-coded content. A hand-authored subclass overrides this.
void setup()
{
}

int query_npc() { return 1; }

// ---------------------------------------------------------------------------
// Persisted-NPC identity and census
// ---------------------------------------------------------------------------

int query_persisted() { return npc_uuid && strlen(npc_uuid); }

string query_uuid() { return npc_uuid; }
void set_uuid(string s) { npc_uuid = s; }

string query_npc_game() { return npc_game; }
void set_npc_game(string s) { npc_game = s; }

string query_npc_area_path() { return npc_area_path; }
void set_npc_area_path(string s) { npc_area_path = s; }

string query_npc_poi() { return npc_poi; }
void set_npc_poi(string s) { npc_poi = s; }

string query_npc_source() { return npc_source; }
void set_npc_source(string s) { npc_source = s; }

// ---------------------------------------------------------------------------
// Persisted-NPC savefile (mutable per-NPC state)
// ---------------------------------------------------------------------------

// Persist this NPC to its own savefile. Inert for a non-persisted NPC; the
// area drives when this runs (on location unload).
int save_npc()
{
  object purse;
  string dir;

  if (!query_persisted() || !npc_game)
    return 0;

  dir = npc_save_dir(npc_game, npc_uuid);
  // save_object does not create directories; mkdir is recursive here
  mkdir(dir);
  // snapshot the carried inventory (weapons, armour, clothes, anything) into
  // the auto-load map so save_object persists it -- exactly as a player saves
  // its inventory
  npc_auto_load = create_auto_load(all_inventory(this_object()));
  // coin is the one thing the inventory snapshot does not carry: the money
  // object opts out of it, and what persists instead is money_array. Copy the
  // live purse into it before saving, exactly as player::save_me does.
  purse = present(MONEY_NAME, this_object());
  money_array = purse ? (mixed *)purse->query_money_array() : ({ });
  // refresh each live component's persisted attrs into component_info so
  // save_object writes their current state
  sync_component_info();
  return !catch(save_object(dir + NPC_SAVE_FILE, 1));
}

// Restore this NPC's state from its savefile. The caller sets the identity
// (uuid + game) first. Inert for a non-persisted NPC.
int restore_npc()
{
  int ok;

  if (!query_persisted() || !npc_game)
    return 0;

  ok = restore_object(npc_save_dir(npc_game, npc_uuid) + NPC_SAVE_FILE, 1);
  if (ok)
  {
    // what it can carry follows its strength, not the figure saved with it
    reset_carry_cap();

    // rebuild the saved inventory (each item cloned and its attributes applied)
    // inside us, so a restored NPC carries exactly what it was saved with
    load_auto_load(npc_auto_load, this_object());
    // id.c's `name` is static and was not restored; re-seed the find_living id
    // from the saved given name so a restored citizen answers to itself again
    if (npc_given_name && strlen(npc_given_name))
      set_name(lower_case(npc_given_name));
    // re-clone the NPC's behaviour components from the restored component_info
    init_components(component_info);
  }
  return ok;
}

// Delete the NPC's entire save folder on death: every file in it, then the
// folder itself. An NPC may accumulate several save files over time (as a
// player's folder does), so remove them all -- not just npc.o -- and leave no
// empty directory behind. Done inline within the NPC's own uuid folder on
// purpose: there is no general recursive-delete efun (too easy to misuse), and
// this only ever touches this NPC's folder.
void delete_npc_save()
{
  string dir;
  string * files;
  int i;

  if (!query_persisted() || !npc_game)
    return;

  dir = npc_save_dir(npc_game, npc_uuid);
  files = (string *)get_dir(dir + "*");
  for (i = 0; i < sizeof(files); i++)
    catch(remove_file(dir + files[i]));
  catch(rmdir(dir));
}

// ---------------------------------------------------------------------------
// Death and data template
// ---------------------------------------------------------------------------

// Free the census slot and POI vacancy, and drop the savefile, when this NPC
// dies. This runs from dest_me, not do_death: living::actual_death destructs
// the body, and a destructed object can no longer call_other, so the area
// must be notified from dest_me -- the last moment the NPC is still alive,
// just before ::dest_me() destroys it.
//
// dest_me also runs on a plain eviction/unload, where the NPC is not dead and
// its census entry must survive so it returns on the next load; the
// query_dead() guard (set by living::actual_death before it destructs us)
// tells the two apart. The diplomacy controller (city_ob) is already notified
// inside living::do_death, so it is not repeated here.
// Components hear of the death while the body is still standing: what they
// put on it goes into the corpse with the rest of its inventory.
int do_death(varargs object killer)
{
  if (!query_dead())
    run_on_components("died", killer);
  return monster::do_death(killer);
}

void dest_me()
{
  if (query_dead() && npc_area_path)
  {
    object area;

    area = AREA_HANDLER->query_area(npc_area_path);

    if (query_persisted())
    {
      // an individual: free its census row and remove its save folder
      if (area)
        catch(area->npc_died(npc_uuid));
      delete_npc_save();
    }
    else if (area && npc_source && npc_monster_location)
    {
      // an anonymous monster: take one off the bucket it was counted in
      catch(area->monster_died(npc_source, npc_monster_location));
    }
  }

  // components are objects of their own: they go with the NPC
  destruct_components();

  ::dest_me();
}

// Apply a bestiary data template (a plain mapping of field -> value) onto this
// generic NPC. The set of fields carried is deliberately explicit; extend it
// (here and in bestiary::extract_template) as more of the source's data needs
// to survive the room2loc conversion.
// A gendered field is either a single value or a per-gender wrapper: a mapping
// keyed by gender ([ gender: value ]) written by the bestiary for a source that
// varies by gender. Being a mapping marks it apart from a plain value (aliases
// and plurals are themselves lists). Keys are strings after the JSON round-trip
// (gender ids 0 neuter / 1 male / 2 female). Resolve against the caller's
// gender, falling back to male then any present value so a gender the field
// does not cover still yields a coherent string.
// One of the feelings of a template (loved, hated, liked, disliked), each key
// of the description handed to the matching add_<kind>.
private void add_feelings(string kind, mixed description)
{
  string * keys;
  int i;

  if (!mappingp(description))
    return;

  keys = map_indices(description);
  for (i = 0; i < sizeof(keys); i++)
    call_other(this_object(), "add_" + kind, keys[i], description[keys[i]]);
}

private mixed gender_value(mixed v, int g)
{
  string * ks;

  if (!mappingp(v))
    return v;

  if (v["" + g] != nil)
    return v["" + g];
  if (v["1"] != nil)
    return v["1"];

  ks = map_indices(v);
  return sizeof(ks) ? v[ks[0]] : nil;
}

/**
 * Stamp a data template onto this NPC.
 *
 * The template is re-applied on every materialization, not only the first,
 * because the fields it carries are held in variables save_object never
 * writes: name, aliases and plurals are static in the id code, short, long
 * and the main plural are static in the description code. A restored NPC
 * would otherwise come back with no name and no description at all. Stamping
 * every time also means editing a template updates every existing NPC of that
 * type, which is the point of keeping the type as data.
 *
 * `born` says this is the NPC's first materialization, and separates the two
 * halves of a template. Everything above the guard is presentation: not
 * persisted, so it has to be re-read each time. Everything below it is the
 * body -- race, class and a pinned level -- which save_object does keep, so
 * re-applying it on a restore ranges from pointless to destructive:
 * set_class_ob resets the level, the experience and the strength cap, which is
 * right when a living changes class and wrong when it is merely waking up.
 */
/**
 * Joins the guild a template names, under the same rules as a player. When
 * the guild refuses the NPC (race, class, stats, alignment), it stays out of
 * it and the refusal is written to the npc_guild log, so a template that
 * cannot qualify is noticed.
 *
 * @param guild the guild path from the template's social objects
 */
private void join_template_guild(string guild)
{
  mixed joined;

  set_guild_ob(guild);

  joined = query_guild_ob();
  if (stringp(joined) && joined == guild)
    return;

  log_file("npc_guild", "[" + ctime(time(), 4) + "] " +
    (query_npc_source() ? query_npc_source() : base_name(this_object())) +
    " (" + query_cap_name() + ") refused by " + guild +
    ": race " + (query_race_ob() ? query_race_ob() : "-") +
    ", class " + (query_class_ob() ? query_class_ob() : "-") +
    ", level " + query_level() + ", real align " + query_real_align() +
    ", str " + query_str() + " con " + query_con() + " dex " + query_dex() +
    " int " + query_int() + " wis " + query_wis() + " cha " + query_cha() +
    "\n");
}

/**
 * Teaches the NPC everything its guild teaches up to its guild level, on top
 * of the spells and skills its template lists (template option
 * guild_abilities). Called once the guild and its level are set, and on every
 * materialization since what is known is not saved with the NPC.
 *
 * @param t the NPC template
 */
private void apply_guild_abilities(mapping t)
{
  object guild;
  mapping taught;
  string * paths;
  string id;
  int i, level;

  if (!t["guild_abilities"] || !stringp(query_guild_ob()))
    return;

  guild = nil;
  catch(guild = load_object(query_guild_ob()));
  if (!guild)
    return;

  guild->grant_guild_skills(this_object());

  level = query_guild_level();
  taught = guild->query_guild_spells();
  paths = map_indices(taught);
  for (i = 0; i < sizeof(paths); i++)
  {
    id = guild->query_spell_id(paths[i]);
    if (taught[paths[i]] <= level && !query_spell(id))
      add_spell(id, paths[i]);
  }
}

void apply_template(mapping t, varargs int born)
{
  int g;

  if (!t)
    return;

  // A fixed template carries its gender; a bimodal one leaves the choice to
  // the caller (set before apply), so read whatever gender is in effect and
  // index the per-gender fields by it. A named individual keeps its own gender
  // over a single-gender template's.
  if (t["gender"] && !npc_given_name)
    set_gender(t["gender"]);
  g = query_gender();

  proper_name = t["proper_name"] ? 1 : 0;
  unique_person = t["unique"] ? 1 : 0;

  if (t["name"])
    set_name(gender_value(t["name"], g));
  if (t["short"])
    set_short(gender_value(t["short"], g));
  if (t["long"])
    set_long(gender_value(t["long"], g));
  if (t["main_plural"])
    set_main_plural(gender_value(t["main_plural"], g));
  if (t["aliases"])
    set_aliases(gender_value(t["aliases"], g));
  if (t["plurals"])
    set_plurals(gender_value(t["plurals"], g));

  // A named individual was named before the template (set_name keeps the first
  // name it is given), so the template's name, the kind word, would be lost:
  // it answers to it as an alias instead, to its trade as well as to its name.
  if (npc_given_name && t["name"])
    add_alias(gender_value(t["name"], g));
  if (t["align"])
    set_real_align(t["align"]);
  if (t["swims"])
    set_swims(1);

  // Starting coin, handed over once. What the NPC earns or spends afterwards is
  // its own, and persists: the purse itself opts out of the auto-load snapshot,
  // so what is saved is money_array, refreshed from the live money object in
  // save_npc and put back into one by start_money when it enters the world --
  // the two hooks a player uses. The base and spread mean two NPCs of a type do
  // not carry the same purse.
  if (born && mappingp(t["money"]) && !undefinedp(t["money"]["base"]))
  {
    mapping money;
    int amount;

    money = t["money"];
    amount = money["base"];
    if (!undefinedp(money["spread"]) && money["spread"] > 0)
      amount += random(money["spread"]);

    if (amount > 0)
      adjust_money(amount, money["type"] ? money["type"] : BASE_COIN);
  }

  // What the type knows how to do and how to be. Re-applied every
  // materialization: none of it rides in the npc.o, and repeating is a no-op.
  // Either a list of ids, at the value the skills table starts them at, or
  // ([ "<id>": <value> ]) to set each one: an ability for a percentage skill,
  // a level for a levelled one (ignored by a fixed one).
  if (pointerp(t["skills"]))
  {
    mixed skills;
    int i;

    skills = t["skills"];
    for (i = 0; i < sizeof(skills); i++)
      if (stringp(skills[i]))
        add_known_skill(skills[i], 1);
  }
  else if (mappingp(t["skills"]))
  {
    string * ids;
    int i;

    ids = map_indices(t["skills"]);
    for (i = 0; i < sizeof(ids); i++)
    {
      if (intp(t["skills"][ids[i]]))
        set_skill_value(ids[i], t["skills"][ids[i]], 1);
      else
        add_known_skill(ids[i], 1);
    }
  }

  // Spells are named to their casting object, so this one is a mapping rather
  // than a list: ([ "<spell>" : "/path/to/spell.c" ]).
  if (mappingp(t["spells"]))
  {
    string * names;
    int i;

    names = map_indices(t["spells"]);
    for (i = 0; i < sizeof(names); i++)
      if (stringp(t["spells"][names[i]]))
        add_spell(names[i], t["spells"][names[i]]);
  }

  // Whom it loves and hates, which decides whom it fights, and whom it likes
  // and dislikes, which only decides what it says on seeing them arrive. All in
  // the shape matches_group reads: race, guild, city...
  add_feelings("loved", t["loved"]);
  add_feelings("hated", t["hated"]);
  add_feelings("liked", t["liked"]);
  add_feelings("disliked", t["disliked"]);
  // it joins any fight against a player in its room
  if (t["join_fights"])
    set_join_fights();

  if (t["liked_message"])
    set_liked_message(gender_value(t["liked_message"], g));
  if (t["disliked_message"])
    set_disliked_message(gender_value(t["disliked_message"], g));

  // Behaviour the type carries: ([ "<component>" : ([ attrs ]) ]). This is how
  // a template says its people can do something a data field cannot describe --
  // bring the dead back, keep a shop -- without a blueprint of their own.
  if (mappingp(t["components"]))
  {
    string * types;
    int i;

    types = map_indices(t["components"]);
    for (i = 0; i < sizeof(types); i++)
      add_component(types[i],
                    mappingp(t["components"][types[i]])
                      ? t["components"][types[i]] : ([ ]));
  }

  // Where the NPC is willing to drift to when idle, and how often. It is how
  // the type behaves, like its chatter, so it is read again every
  // materialization and the template's list replaces whatever was saved.
  if (pointerp(t["move_zones"]))
  {
    mixed zones;
    int i;

    zones = t["move_zones"];
    if (sizeof(query_move_zones()))
      remove_move_zone(query_move_zones() + ({ }));
    for (i = 0; i < sizeof(zones); i++)
      add_move_zone(zones[i]);
  }

  if (mappingp(t["move_after"]) && !undefinedp(t["move_after"]["after"]))
    set_move_after(t["move_after"]["after"],
                   undefinedp(t["move_after"]["rand"]) ?
                     0 : t["move_after"]["rand"]);

  // one particular person is weighed in the ranking whenever it comes into
  // the world, not only when it is born and its level set
  if (proper_name && !born)
    call_out("check_ranking", 5);

  // Idle chatter and the addressed variant. Each block is a chance plus the
  // flat weight/message list load_chat itself takes; the chance is per type and
  // really does vary, so it is stored rather than assumed. The chatter is the
  // type's words, not the person's: it is loaded afresh from the template every
  // time, over whatever the save kept, so a changed template reaches everyone.
  if (mappingp(t["chat"]) && pointerp(t["chat"]["lines"]) &&
      sizeof(t["chat"]["lines"]))
  {
    clear_chat();
    load_chat(t["chat"]["chance"], t["chat"]["lines"]);
  }
  if (mappingp(t["a_chat"]) && pointerp(t["a_chat"]["lines"]) &&
      sizeof(t["a_chat"]["lines"]))
  {
    clear_achat();
    load_a_chat(t["a_chat"]["chance"], t["a_chat"]["lines"]);
  }

  // How the world reads this type: its own value, or else its race's. Set
  // outright here, since perceived alignment is not saved and a restored body
  // gets its race back without the race's share being added again.
  if (!undefinedp(t["ext_align"]))
    set_ext_align(t["ext_align"]);
  else if (query_race_ob())
    set_ext_align(load_object(query_race_ob())->query_ext_align());

  // One particular person is exactly what its template says, so the pinned
  // level, deity, guild and job follow the template on every materialization,
  // not only at birth. Guild and job take them only if they meet their rules.
  if (!born && is_unique())
  {
    mapping social;

    social = mappingp(t["social_obs"]) ? t["social_obs"] : ([ ]);
    if (t["level"] && query_class_ob() && query_class_level() != t["level"])
      set_level(t["level"]);
    // the deity before the guild, which may ask for one
    if (social["deity"] && query_deity_ob() != social["deity"])
      set_deity_ob(social["deity"]);
    if (social["guild"] && query_guild_ob() != social["guild"])
      join_template_guild(social["guild"]);
    if (!undefinedp(t["guild_level"]) && query_guild_ob() &&
        t["guild_level"] != query_guild_level())
      adjust_guild_level(t["guild_level"] - query_guild_level());
    if (social["job"] && query_job_ob() != social["job"])
      set_job_ob(social["job"]);
    if (!undefinedp(t["job_level"]) && query_job_ob() &&
        t["job_level"] != query_job_level())
      adjust_job_level(t["job_level"] - query_job_level());
  }

  // an NPC that is not born keeps its guild and level: it is the time to
  // give it what the guild teaches
  apply_guild_abilities(t);

  if (!born)
    return;

  // Every social object the type belongs to, stored as the raw paths the
  // accessors return. Race and class live here too: they are social objects
  // like the rest, not a special case. Citizenship is absent by design -- an
  // NPC takes its nationality from the area it is born in, not from its type.
  if (mappingp(t["social_obs"]))
  {
    mapping social;

    social = t["social_obs"];
    if (social["race"])       set_race_ob(social["race"]);
    if (social["class"])      set_class_ob(social["class"]);
    if (social["race_group"]) set_race_group_ob(social["race_group"]);
    if (social["group"])      set_group_ob(social["group"]);
    if (social["job"])        set_job_ob(social["job"]);
    if (social["deity"])      set_deity_ob(social["deity"]);
  }

  // Roll the eight stats in the type's range, so two NPCs of a type are not the
  // same creature, then let any individually pinned stat override its roll.
  // Both halves are optional: a type with neither keeps what the race gave it.
  if (mappingp(t["random_stats"]) &&
      !undefinedp(t["random_stats"]["low"]) &&
      !undefinedp(t["random_stats"]["high"]))
    set_random_stats(t["random_stats"]["low"], t["random_stats"]["high"]);

  if (mappingp(t["stats"]))
  {
    mapping fixed;

    fixed = t["stats"];
    if (!undefinedp(fixed["str"])) set_str(fixed["str"]);
    if (!undefinedp(fixed["con"])) set_con(fixed["con"]);
    if (!undefinedp(fixed["dex"])) set_dex(fixed["dex"]);
    if (!undefinedp(fixed["int"])) set_int(fixed["int"]);
    if (!undefinedp(fixed["wis"])) set_wis(fixed["wis"]);
    if (!undefinedp(fixed["cha"])) set_cha(fixed["cha"]);
    if (!undefinedp(fixed["wil"])) set_wil(fixed["wil"]);
    if (!undefinedp(fixed["per"])) set_per(fixed["per"]);
  }

  // How this trade's body differs from the average of its place, as deltas over
  // whatever the roll produced. The area decides how strong its people are, so
  // a template says a farmer has heavier arms than his neighbours, never that
  // he has a strength of twelve.
  if (mappingp(t["stat_modifiers"]))
  {
    mapping mods;
    string * names;
    int i, v;

    mods = t["stat_modifiers"];
    names = map_indices(mods);
    for (i = 0; i < sizeof(names); i++)
    {
      if (!intp(mods[names[i]]))
        continue;

      switch (names[i])
      {
      case "str":
        v = query_real_str() + mods[names[i]]; set_str(v < 1 ? 1 : v); break;
      case "con":
        v = query_real_con() + mods[names[i]]; set_con(v < 1 ? 1 : v); break;
      case "dex":
        v = query_real_dex() + mods[names[i]]; set_dex(v < 1 ? 1 : v); break;
      case "int":
        v = query_real_int() + mods[names[i]]; set_int(v < 1 ? 1 : v); break;
      case "wis":
        v = query_real_wis() + mods[names[i]]; set_wis(v < 1 ? 1 : v); break;
      case "cha":
        v = query_real_cha() + mods[names[i]]; set_cha(v < 1 ? 1 : v); break;
      case "wil":
        v = query_real_wil() + mods[names[i]]; set_wil(v < 1 ? 1 : v); break;
      case "per":
        v = query_real_per() + mods[names[i]]; set_per(v < 1 ? 1 : v); break;
      }
    }
  }

  if (t["level"])
    set_level(t["level"]);

  // The guild comes last among the body: it judges the NPC as it would a
  // player joining (race, class, stats, alignment), so it needs them set.
  // Joining starts at guild level one, hence the level after it.
  if (mappingp(t["social_obs"]) && t["social_obs"]["guild"])
    join_template_guild(t["social_obs"]["guild"]);
  if (!undefinedp(t["guild_level"]) && query_guild_ob() &&
      t["guild_level"] != query_guild_level())
    adjust_guild_level(t["guild_level"] - query_guild_level());

  // the job was set with the other social objects; joining starts at job
  // level one
  if (!undefinedp(t["job_level"]) && query_job_ob() &&
      t["job_level"] != query_job_level())
    adjust_job_level(t["job_level"] - query_job_level());

  // the guild and its level are settled: it knows what the guild teaches
  apply_guild_abilities(t);

  // How the world reads this type. Perceived alignment accumulates from the
  // social objects, so this comes after them to have the last word.
  if (!undefinedp(t["ext_align"]))
    set_ext_align(t["ext_align"]);

  // weight is not applied here: set_race_ob above already set the body weight
  // from the race, which is where it belongs

  // hp and gp start full, so only the maxima are stored
  if (t["max_hp"])
  {
    set_max_hp(t["max_hp"]);
    set_hp(query_max_hp(), this_object());
  }
  if (t["max_gp"])
  {
    set_max_gp(t["max_gp"]);
    set_gp(query_max_gp());
  }

  if (!undefinedp(t["wimpy"]))
    set_wimpy(t["wimpy"]);
  if (!undefinedp(t["aggressive"]))
    set_aggressive(t["aggressive"]);


}
