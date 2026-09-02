//
// A living's family: which house it belongs to, and who its people are.
//
// The living itself stores one thing, the surname, in the family slot of its
// social objects. Everything else -- who it is married to, whose child it is,
// what the house belongs to -- is asked of the families handler, which is where
// a family outlives the people in it.
//
// A member is named the same way whether it is an NPC or a player, so a family
// may hold both and nothing here has to know which it is looking at.
//

#include <living/family.h>

// The name this living answers to in a family register. An NPC is named by the
// uuid its census row is keyed on; a player by its own name. Anything with
// neither -- a monster, an unstamped clone -- is nobody's relative.
string query_family_id()
{
  mixed id;

  // no (string) casts anywhere here: that is a conversion kfun, not a type
  // assertion, and a living that answers none of these hands back nil
  if (this_object()->query_player())
  {
    id = this_object()->query_name();
    return stringp(id) ? FAMILY_PLAYER + id : nil;
  }

  id = this_object()->query_npc_uuid();
  return (stringp(id) && strlen(id)) ? FAMILY_NPC + id : nil;
}

string query_family()
{
  mixed surname;

  surname = this_object()->query_family_ob();
  return stringp(surname) ? surname : nil;
}

// Join a house. The register is the authority on who belongs where, so it is
// told first and the slot only records the answer.
int set_family(string surname, varargs string display_name)
{
  string game, id;
  mixed shown;

  id = query_family_id();
  game = game_name();
  if (!id || !strlen(game))
    return 0;

  if (!surname || !strlen(surname))
  {
    this_object()->set_family_ob(nil);
    return 1;
  }

  if (!display_name)
  {
    shown = this_object()->query_cap_name();
    display_name = stringp(shown) ? shown : id;
  }

  if (!FAMILY_HANDLER->add_member(game, surname, id, display_name))
    return 0;

  this_object()->set_family_ob(surname);
  return 1;
}

string query_spouse_id()
{
  string game, id;

  id = query_family_id();
  game = game_name();
  if (!id || !strlen(game))
    return nil;

  return FAMILY_HANDLER->query_spouse(game, id);
}

string * query_parent_ids()
{
  string game, id;

  id = query_family_id();
  game = game_name();
  if (!id || !strlen(game))
    return ({ });

  return (string *)FAMILY_HANDLER->query_parents(game, id);
}

string * query_children_ids()
{
  string game, id;

  id = query_family_id();
  game = game_name();
  if (!id || !strlen(game))
    return ({ });

  return (string *)FAMILY_HANDLER->query_children(game, id);
}

// Whether this living belongs to the named house. What a family door asks
// before it opens, and what any other family privilege will ask after it.
int is_family_member(string surname)
{
  string mine;

  if (!surname || !strlen(surname))
    return 0;

  mine = query_family();
  return (mine && mine == surname);
}
