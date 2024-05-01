
#include <basic/parse.h>
#include <language.h>

string * lng,
       * shrt,
       * adjs,
       * plu,
       * name;
mapping verb,
        plural,
        other_things;
int * cur_desc;

int is_room_item() { return 1; }

void create()
{
  adjs = ({ });
  lng = ({ "" });
  shrt = ({ "" });
  plu = ({ });
  name = ({ });
  verb = ([ ]);
  plural = ([ ]);
  cur_desc = ({ });
  other_things = ([ "smell" : 0, "taste" : 0, "read" : 0 ]);
} /* create() */

string short()
{
  string * ret;
  int i;

  ret = ({ });
  for (i = 0; i < sizeof(cur_desc); i++)
    ret += ({ shrt[cur_desc[i]] });

  return query_multiple_short(ret);
} /* short() */

string * pretty_short()
{
  string * ret;
  int i;

  ret = ({ });
  for (i = 0; i < sizeof(cur_desc); i++)
    ret += ({ shrt[cur_desc[i]] });
  return ret;
} /* pretty_short() */

string query_plural()
{
  string * ret;
  int i;

  ret = ({ });
  for (i = 0; i < sizeof(cur_desc); i++)
    ret += ({ pluralize(shrt[cur_desc[i]]) });
  return query_multiple_short(ret);
} /* query_plural() */

string * pretty_plural()
{
  string * ret;
  int i;

  ret = ({ });
  for (i = 0; i < sizeof(cur_desc); i++)
    ret += ({ pluralize(shrt[cur_desc[i]]) });
  return ret;
} /* query_plural() */

string long(varargs string s, int dark)
{
  int i;
  string ret;

  ret = "";

  for (i = 0; i < sizeof(cur_desc); i++)
  {
    if (!lng[cur_desc[i]])
      continue;

    ret += lng[cur_desc[i]];
  }

  if (ret == "")
    return _LANG_ITEM_ERROR;

  return wrap(ret, this_user() ? this_user()->query_cols() : 80, 1);
} /* long() */

// void set_long(string s) { lng[cur_desc] = s; }
// string query_long() { return lng[cur_desc]; }

int * query_cur_desc() { return cur_desc; }
mapping query_verbs() { return verb; }
string * query_plurals() { return map_indices(plural) + map_values(plural); }

string * query_lng() { return lng; }
string * query_shrt() { return shrt; }

int drop() { return 1; }
int get() { return 1; }

void setup_item(mixed nam, mixed long, varargs int no_plural)
{
  string * bits, s, real_long;
  int i;

  if (pointerp(long))
  {
    real_long = _LANG_ITEM_NOTHING_IMPORTANT;
    for (i = 0; i < sizeof(long);i+=2)
      if (long[i] != "long")
      {
        if (!other_things[long[i]])
          other_things[long[i]] = ([ sizeof(lng) : long[i+1] ]);
        else
          other_things[long[i]][sizeof(lng)]  = long[i+1];
      } else
      {
        real_long = long[i+1];
      }
    long = real_long;
  }

  if (pointerp(nam))
  {
    if (sizeof(nam) > 0)
      shrt += ({ nam[0] });
    for (i = 0; i < sizeof(nam); i++)
    {
      bits = explode(nam[i], " ");
      name += ({ (s=bits[sizeof(bits)-1]) });
      if (!verb[s])
      {
        verb[s] = ({ bits[0..sizeof(bits)-2], sizeof(lng) });
        if (!no_plural)
          plural[(s=pluralize(s))] = ({ bits[0..sizeof(bits)-2], sizeof(lng) });
      } else
      {
        verb[s] += ({ bits[0..sizeof(bits)-2], sizeof(lng) });
        if (!no_plural)
          plural[(s=pluralize(s))] +=
                   ({ bits[0..sizeof(bits)-2], sizeof(lng) });
      }
      if (no_plural)
        plu += ({ "no plural" });
      else
        plu += ({ s });
      adjs += bits[0..sizeof(bits)-2];
    }
    lng += ({ long });
    return ;
  }
  shrt += ({ nam });
  bits = explode(nam, " ");
  name += ({ (s=bits[sizeof(bits)-1]) });
  if (!verb[s])
  {
    verb[s] = ({ bits[0..sizeof(bits)-2], sizeof(lng) });
    if (!no_plural)
      plural[(s=pluralize(s))] = ({ bits[0..sizeof(bits)-2], sizeof(lng) });
  } else
  {
    /* Dey are both existant... */
    verb[s] += ({ bits[0..sizeof(bits)-2], sizeof(lng) });
    if (!no_plural)
      plural[(s=pluralize(s))] += ({ bits[0..sizeof(bits)-2], sizeof(lng) });
  }
  if (no_plural)
    plu += ({ "no plural" });
  else
    plu += ({ s });
  adjs += bits[0..sizeof(bits)-2];
  lng += ({ long });
} /* setup_item() */

int modify_item(string str, mixed long)
{
  int i, j;

  if ((j = member_array(str, shrt)) == -1)
    return 0;
  /* Got a match... */
  if (pointerp(long))
  {
    for (i = 0; i < sizeof(long);i+=2)
      if (long[i] != "long")
      {
        if (!other_things[long[i]])
          other_things[long[i]] = ([ sizeof(lng) : long[i+1] ]);
        else
          other_things[long[i]][sizeof(lng)]  = long[i+1];
      } else
      {
        lng[j] = long[i+1];
      }
    return 1;
  }
  lng[j] = long;
  return 1;
}

int remove_item(string str, string new_long)
{
  int i;

  if ((i = member_array(str, shrt)) == -1)
    return 0;
  /* Ok, got him.  Now we need to track down all the bits.  Sigh. */
  /* this is a mess.  I am not sure I want to do it.  So I won't for now. */
}

// string *parse_command_id_list() { return name; }
// string *parse_command_plural_id_list() { return plu; }
// string *parse_command_adjectiv_id_list() { return adjs; }

object query_parse_id(mixed *arr)
{
  string *bits;
  mixed stuff;
  int i, j;

  // all case
  if (arr[P_THING] == 0)
  {
    bits = explode(arr[P_STR], " ");

    if (!(stuff = plural[bits[sizeof(bits)-1]]))
      if (!(stuff = verb[bits[sizeof(bits)-1]]))
        return nil;

    cur_desc = ({ });

    for (j = 0 ;j < sizeof(stuff); j+=2)
    {
      for (i = 0; i < sizeof(bits)-2; i++)
        if (member_array(bits[i], stuff[j]) == -1)
          break;

      if (i < sizeof(bits)-2)
        continue;

      cur_desc += ({ stuff[j+1] });
    }

    return this_object();
  }

  if (arr[P_THING] < 0)
  {
    // specific object case
    bits = explode(arr[P_STR], " ");

    if (!(stuff = verb[bits[sizeof(bits)-1]]))
      return nil;

    for (j = 0; j < sizeof(stuff); j+=2)
    {
      for (i = 0; i < sizeof(bits)-2; i++)
        if (member_array(bits[i], stuff[j]) == -1)
          break;

      if (i < (sizeof(bits) - 2) || ++arr[P_THING] != 0)
        continue;

      // Get the current thingy out of the list
      cur_desc = ({ stuff[j+1] });
      arr[P_THING] = -10321;
      return this_object();
    }

    return nil;
  }

  // Lots of objects case.  The objects are specified though.
  bits = explode(arr[P_STR], " ");

  if (!(stuff = plural[bits[sizeof(bits)-1]]))
    if (!(stuff = verb[bits[sizeof(bits)-1]]))
      return nil;

  cur_desc = ({ });

  for (j = 0; j < sizeof(stuff); j+=2)
  {
    for (i = 0; i < sizeof(bits)-2; i++)
      if (member_array(bits[i], stuff[j]) == -1)
        continue;

    if (i < sizeof(bits)-2)
      continue;

    cur_desc += ({ stuff[j+1] });
    arr[P_THING]--;

    if (arr[P_THING] <= 0)
    {
      arr[P_THING] = -10786;
      return this_object();
    }
  }

  return this_object();
}

int move(mixed dest, varargs mixed messin, mixed messout) { return 1; }
// int move() { return 1; }

int command_control(string command)
{
  int i;

  if (!other_things[command])
    return 0;
  for (i = 0; i < sizeof(cur_desc); i++)
    if (other_things[command][cur_desc[i]])
      write(process_string(other_things[command][cur_desc[i]]));
    else
      cur_desc = delete(cur_desc, i--, 1);
  return sizeof(cur_desc);
} /* command_control() */

mapping query_other_things() { return other_things; }
