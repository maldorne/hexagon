// edited by neverbot, 4'03
// added here the old /global/basic/print_object.c
// adjectives removed, never used them

#include <basic/parse.h>
#include <basic/id.h>

static string name,
       *alias,
       // *adjectives,
       *plurals;

/* name handleing stuff */
void set_name(string str) { name = str; }
string query_name() { return name; }
string query_cap_name() 
{
  if (!name)
   return "Alguien";
  return capitalize(name);
} 

void create() 
{
  alias = ({ });
  plurals = ({ });
  // adjectives = ({ });
  name = DEF_NAME;
}

/* alias stuff */
void set_aliases(string *str) 
{
  alias = str;
}

void add_alias(mixed str) 
{
  if (pointerp(str)) 
  {
    alias += str;
    return ;
  }

  if (member_array(str, alias) == -1)
    alias += ({ str });
}

int remove_alias(string str) 
{
  int i;
  if ((i=member_array(str, alias)) == -1)
    return 0;
  alias = delete(alias, i, 1);
  return 1;
} 

string *query_alias() { return alias; }

/*
int full_id(string str) {
  int loop;

  for(loop = 0; loop < sizeof(adjectives); loop++) {
  }
  return 0;
} 
*/

/* plural stuff */
void set_plurals(string *str) 
{
  plurals = str;
} 

void add_plural(mixed str) 
{ 

  if (pointerp(str))
    plurals += str;
  else
    plurals += ({ str });
} 

void remove_plural(string str) 
{
  int i;

  if ((i=member_array(str, plurals)) != -1)
    plurals = delete(plurals,i,1);
} 

void add_plurals(string *str) 
{
  plurals += str;
}

string *query_plurals() { return plurals; }

int id_plural(string str) 
{
  return (member_array(str, plurals) != -1);
} 

// id - function called by present() in order to identify an object
// int id( string an_id );

// The present() efunction calls id() to determine if a given object is named
// by a given string.  id() should return 1 if the object wishes to be known
// by the name in the string anId; it should return 0 otherwise.

int id(string str) 
{
  return ((str == name) || 
          (member_array(str, alias) != -1) || 
          id_plural(str));
} 

/*
void set_adjectives(string *str) 
{
  adjectives = str;
}

void add_adjective(mixed str) 
{
  int i;

  if (pointerp(str)) 
  {
    for (i=0;i<sizeof(str);i++)
      add_adjective(str[i]);
    return ;
  }
  if (stringp(str))
    str = explode(str, " ");
  for (i=0;i<sizeof(str);i++)
    if (member_array(str[i], adjectives) == -1)
      adjectives += ({ str[i] });
} 

void remove_adjective(mixed str) 
{
  int i;

  if (pointerp(str))
    for (i=0;i<sizeof(str);i++)
      remove_adjective(str[i]);
  else
    if ((i=member_array(str, adjectives)) != -1)
      adjectives = delete(adjectives, i, 1);
} 

string *query_adjectives() { return adjectives; }
*/

/* ok parse command stuff */
// string *parse_command_id_list() { return ({ name }) + alias; }
// string *parse_command_plural_id_list() { return plurals; }
// string *parse_command_adjectiv_id_list() { return adjectives; }
/*
object query_parse_id(mixed *arr) 
{
  if (arr[P_THING] == 0)
    return this_object();
  if (arr[P_THING] < 0) 
  { // specific object case 
    arr[P_THING]++;
    if (arr[P_THING] != 0)
      return nil;
    arr[P_THING] = -10321;
    return this_object();
  }
  arr[P_THING]--; // lots of objects case
  if (arr[P_THING] != 0)
    return this_object();
  arr[P_THING] = -10101;
  return this_object();
}

object query_frac_parse_id(mixed *arr) 
{
  if (arr[P_THING] < 0) 
  {
    arr[P_THING]++;
    if (arr[P_THING] != 0)
      return nil;
    arr[P_THING] = -10235;
    return nil; // it refered to me and I am pretty depressed about it.
                // I want to break free!
  }
  if (arr[P_THING] == 0)
    if ((arr[P_MAX_NUM]*arr[P_TOP])/arr[P_BOT] > arr[P_CUR_NUM]++)
      return this_object();
    else
      return nil;
  if ((arr[P_THING]*arr[P_TOP])/arr[P_BOT] > arr[P_CUR_NUM]++)
    return this_object();
  return nil;
}
*/

// stats added
mixed stats()
{
  return ({
            ({ "Name", name }),
            ({ "Alias", alias }),
            ({ "Plurals", plurals }),
          });
}
