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

// What somebody is called in full: their own name, and the house they are of
// when they have one. An NPC's own name is the one it was generated with; a
// player's is its name. Nil for anybody who answers to neither.
string query_full_name()
{
  mixed personal;
  string surname;

  personal = this_object()->query_given_name();
  if (!stringp(personal) || !strlen(personal))
    personal = this_object()->query_cap_name();
  if (!stringp(personal) || !strlen(personal))
    return nil;

  personal = capitalize(personal);
  surname = query_family();

  return (surname && strlen(surname)) ? personal + " " + surname : personal;
}

// Join a house. The register is the authority on who belongs where, so it is
// told first and the slot only records the answer.
int set_family(string surname, varargs string display_name)
{
  string game, id, old;
  mixed shown;

  id = query_family_id();
  game = game_name();
  if (!id || !strlen(game))
    return 0;

  if (!surname || !strlen(surname))
  {
    // the surname answers as a name while it is theirs, so it stops answering
    // when it is not
    old = query_family();
    if (old && strlen(old))
      this_object()->remove_alias(lower_case(old));

    this_object()->set_family_ob(nil);
    if (this_object()->query_persisted())
      this_object()->save_npc();
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

  // somebody of a house answers to it: "look copperfen" finds one of them, the
  // same way a generated citizen answers to the name it was given
  this_object()->add_alias(lower_case(surname));

  // the register has written its side; without this the two disagree the
  // moment the location unloads, and somebody wakes up disowned by a house
  // that still counts them
  if (this_object()->query_persisted())
    this_object()->save_npc();

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

// Everything the house has to do to somebody arriving in the world, called
// once they are otherwise finished: start_player for a player, the census after
// the template for an NPC.
//
// So far that is one thing: the surname answers as a name, and id.c keeps names
// static, so nothing on disk brings it back -- it has to be put there again on
// every arrival.
void start_family()
{
  string surname;

  surname = query_family();
  if (surname && strlen(surname))
    this_object()->add_alias(lower_case(surname));
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
