//
// A living's family: which house it belongs to, and who its people are.
//
// The living stores nothing about it: which house it is of, who it is married
// to, whose child it is -- all of it is asked of the families handler, which is
// where a family outlives the people in it, and which can be written to without
// anybody being in the world.
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

  id = this_object()->query_uuid();
  return (stringp(id) && strlen(id)) ? FAMILY_NPC + id : nil;
}

string query_family()
{
  string id;

  id = query_family_id();
  return id ? handler("families", this_object())->family_of(id) : nil;
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

// Join a house, or leave the one somebody is of with no surname. Only the
// register is written; the surname answering as a name is the one thing kept on
// the living, and it is not saved.
int set_family(string surname, varargs string display_name)
{
  string id, old;
  mixed shown;

  id = query_family_id();
  if (!id)
    return 0;

  old = query_family();

  if (!surname || !strlen(surname))
  {
    if (!old)
      return 1;

    handler("families", this_object())->member_married_out(id, "nowhere");

    // the surname answers as a name while it is theirs, so it stops answering
    // when it is not
    this_object()->remove_alias(lower_case(old));
    return 1;
  }

  // an NPC's cap name is its trade word, so the name it was given goes first
  if (!display_name)
  {
    shown = this_object()->query_given_name();
    if (!stringp(shown) || !strlen(shown))
      shown = this_object()->query_cap_name();
    display_name = stringp(shown) ? capitalize(shown) : id;
  }

  if (!handler("families", this_object())->add_member(surname, id, display_name))
    return 0;

  // somebody of a house answers to it: "look <surname>" finds one of them, the
  // same way a generated citizen answers to the name it was given
  this_object()->add_alias(lower_case(surname));

  return 1;
}

string query_spouse_id()
{
  string id;

  id = query_family_id();
  if (!id)
    return nil;

  return handler("families", this_object())->query_spouse(id);
}

string * query_parent_ids()
{
  string id;

  id = query_family_id();
  if (!id)
    return ({ });

  return (string *)handler("families", this_object())->query_parents(id);
}

string * query_children_ids()
{
  string id;

  id = query_family_id();
  if (!id)
    return ({ });

  return (string *)handler("families", this_object())->query_children(id);
}

// Everything the house has to do to somebody arriving in the world, called
// once they are otherwise finished: start_player for a player, the census after
// the template for an NPC.
//
// The surname answers as a name, and id.c keeps names static, so nothing on disk
// brings it back -- it has to be put there again on every arrival. And somebody
// who joined the house before they were named is written down by name now.
void start_family()
{
  mixed surname, given;

  surname = query_family();
  if (!stringp(surname) || !strlen(surname))
    return;

  this_object()->add_alias(lower_case(surname));

  given = this_object()->query_given_name();
  if (stringp(given) && strlen(given) && query_family_id())
    handler("families", this_object())->name_member(query_family_id(),
                                                    capitalize(given));
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
