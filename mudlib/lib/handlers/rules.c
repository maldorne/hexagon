// Rules handler: what the normas command of a guild or a job shows. First
// the part read from the guild or job object (who it admits, the skills it
// teaches, what the reader's next level costs), then the text of its rules
// book, rendered by the books handler as a library renders it, so the book
// on the shelves and normas say the same thing.
//
// Shared by the guild room and the job room (/lib/room/guilds/,
// /lib/room/jobs/), where members read it.

#include <living/skills.h>
#include <language.h>

inherit "/lib/core/object.c";

// The names of the races or classes listed by id: the game's own object for
// each when there is one, the bare id otherwise.
private string _names_of(string * ids, string dir)
{
  string * names;
  object ob;
  int i;

  names = ({ });
  for (i = 0; i < sizeof(ids); i++)
  {
    ob = nil;
    catch(ob = load_object(dir + ids[i] + ".c"));
    names += ({ (ob && ob->query_name()) ? (string)ob->query_name() : ids[i] });
  }

  return implode(names, ", ");
}

// What alignment a guild asks for, "" when it does not matter.
private string _alignment(object guild)
{
  switch ((int)guild->query_needed_ext_align())
  {
    case 1:
      return _LANG_RULES_ALIGN_NEUTRAL;
    case 100:
      return _LANG_RULES_ALIGN_GOOD;
    case -100:
      return _LANG_RULES_ALIGN_EVIL;
  }
  return "";
}

// Who may join: races, classes, alignment, whatever else the object checks,
// and whether the reader meets it.
private string _who(object social, object reader, string root)
{
  string text, extra;

  text = _LANG_RULES_WHO_HEADER;

  text += _LANG_RULES_RACES +
          (sizeof(social->query_legal_races()) ?
            _names_of(social->query_legal_races(), root + "obj/races/") :
            _LANG_RULES_ANY) + ".\n";
  text += _LANG_RULES_CLASSES +
          (sizeof(social->query_legal_classes()) ?
            _names_of(social->query_legal_classes(), "/lib/obj/classes/") :
            _LANG_RULES_ANY) + ".\n";

  if (social->query_is_guild() && strlen(_alignment(social)))
    text += _LANG_RULES_ALIGNMENT + _alignment(social) + ".\n";

  extra = social->query_requirements_text();
  if (extra && strlen(extra))
    text += _LANG_RULES_ALSO + extra + "\n";

  if (reader)
    text += social->query_legal_player(reader) ? _LANG_RULES_YOU_MEET :
                                                 _LANG_RULES_YOU_DO_NOT_MEET;

  return text;
}

// The display name of a skill, from its object, or its id.
private string _skill_name(string id, object place)
{
  mixed * data;
  object table, ob;

  table = table("skills", place);
  data = table ? table->query_skill_data(id) : nil;
  ob = nil;
  if (arrayp(data) && sizeof(data))
    catch(ob = load_object(data[SKILL_DATA_PATH]));

  return (ob && ob->query_effect_name()) ? capitalize(ob->query_effect_name()) :
                                           id;
}

// The skills a guild teaches, by the guild level that gives each.
private string _skills(object guild, object place)
{
  mapping skills;
  string * ids;
  string text;
  int i, level, top;

  skills = guild->query_guild_skills();
  if (!mappingp(skills) || !map_sizeof(skills))
    return "";

  ids = map_indices(skills);
  top = 0;
  for (i = 0; i < sizeof(ids); i++)
    if (skills[ids[i]][0] > top)
      top = skills[ids[i]][0];

  text = _LANG_RULES_SKILLS_HEADER;
  for (level = 0; level <= top; level++)
    for (i = 0; i < sizeof(ids); i++)
      if (skills[ids[i]][0] == level)
        text += sprintf("  %-28s %s\n", _skill_name(ids[i], place),
                        _LANG_RULES_SKILL_FROM_LEVEL);

  return text;
}

// The spells a guild teaches, by the guild level that gives each; members
// learn them with aprender in the guild room.
private string _spells(object guild)
{
  mapping spells;
  string * paths;
  string text;
  object ob;
  int i, level, top;

  spells = guild->query_guild_spells();
  if (!mappingp(spells) || !map_sizeof(spells))
    return "";

  paths = map_indices(spells);
  top = 0;
  for (i = 0; i < sizeof(paths); i++)
    if (spells[paths[i]] > top)
      top = spells[paths[i]];

  text = _LANG_RULES_SPELLS_HEADER;
  for (level = 0; level <= top; level++)
    for (i = 0; i < sizeof(paths); i++)
      if (spells[paths[i]] == level)
      {
        ob = nil;
        catch(ob = load_object(paths[i]));
        text += sprintf("  %-28s %s\n",
                        ob ? capitalize(ob->query_effect_name()) : paths[i],
                        _LANG_RULES_SKILL_FROM_LEVEL);
      }

  return text;
}

// The spells and skills tied to a guild but not taught in its room: they are
// learned elsewhere. Empty when the guild declares none.
private string _related(object guild, object place)
{
  string * paths, * ids;
  string text;
  object ob;
  int i;

  text = "";

  ids = guild->query_guild_related_skills();
  if (sizeof(ids))
  {
    text += _LANG_RULES_RELATED_SKILLS_HEADER;
    for (i = 0; i < sizeof(ids); i++)
      text += "  " + _skill_name(ids[i], place) + "\n";
  }

  paths = guild->query_guild_related_spells();
  if (sizeof(paths))
  {
    if (strlen(text))
      text += "\n";
    text += _LANG_RULES_RELATED_SPELLS_HEADER;
    for (i = 0; i < sizeof(paths); i++)
    {
      ob = nil;
      catch(ob = load_object(paths[i]));
      text += "  " + (ob ? capitalize(ob->query_effect_name()) : paths[i]) + "\n";
    }
  }

  return text;
}

// What the reader's next level costs, when they belong.
private string _next_level(object social, object reader, string path)
{
  int next, cost, have;

  if (!reader)
    return "";

  if (social->query_is_guild())
  {
    if (reader->query_guild_ob() != path)
      return _LANG_RULES_LEVEL_HEADER + _LANG_RULES_NOT_MEMBER_GUILD;
    if ((int)reader->query_guild_level() >= (int)social->query_max_level())
      return _LANG_RULES_LEVEL_HEADER + _LANG_RULES_MAX_LEVEL;

    next = (int)reader->query_guild_level() + 1;
    cost = (int)social->query_next_level_xp(reader);
    have = (int)reader->query_xp();
    return _LANG_RULES_LEVEL_HEADER + _LANG_RULES_GUILD_COST;
  }

  if (reader->query_job_ob() != path)
    return _LANG_RULES_LEVEL_HEADER + _LANG_RULES_NOT_MEMBER_JOB;
  if ((int)reader->query_job_level() >= (int)social->query_max_level())
    return _LANG_RULES_LEVEL_HEADER + _LANG_RULES_MAX_LEVEL;

  next = (int)reader->query_job_level() + 1;
  cost = (int)social->query_next_level_xp(reader);
  have = (int)reader->query_job_xp();
  return _LANG_RULES_LEVEL_HEADER + _LANG_RULES_JOB_COST;
}

/**
 * The normas of a guild or a job, for one reader.
 *
 * @param  path    the guild or job object, as the place names it (the same
 *                 string the reader's guild or job is compared with)
 * @param  reader  who reads them; their own conditions and next level are
 *                 said when given
 * @param  place   where they are read, which says the game (races, skills)
 * @return         the whole text, or nil when the object does not load
 */
string query_rules(string path, object reader, object place)
{
  object social;
  string text, book, rendered, root;

  social = nil;
  if (path && strlen(path))
    catch(social = load_object(path));
  if (!social)
    return nil;

  root = place ? game_root(place) : "";

  text = "%^BOLD%^" + (social->query_is_guild() ? _LANG_RULES_TITLE_GUILD :
                                                  _LANG_RULES_TITLE_JOB) +
         (string)social->query_short() + "%^RESET%^\n\n" +
         _who(social, reader, root);

  if (social->query_is_guild() && strlen(_skills(social, place)))
    text += "\n" + _skills(social, place);

  if (social->query_is_guild() && strlen(_spells(social)))
    text += "\n" + _spells(social);

  if (social->query_is_guild() && strlen(_related(social, place)))
    text += "\n" + _related(social, place);

  if (strlen(_next_level(social, reader, path)))
    text += "\n" + _next_level(social, reader, path);

  book = social->query_rules_book();
  rendered = (book && strlen(book)) ? handler("books")->query_rendered_text(book) :
                                      nil;
  text += "\n" + sprintf("%p%*s", '-', 40, "") + "\n\n" +
          (rendered ? rendered : _LANG_RULES_NO_BOOK);

  return text;
}
