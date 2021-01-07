
// old cute_look.c, now contents.c, neverbot 06/03

// #include <alignment.h>

#include <user/hud.h>
#include <translations/common.h>
#include <language.h>

/* returns an array of the form:
 * ({
 *   ({
 *     "foo",
 *     "bing",
 *   }),
 *   ({
 *     ({ <list of living objects with short "foo"> }),
 *     ({ <ditto for short "bing"> }),
     }),
 *   ({
 *     "s1",
 *     "s2",
 *   }),
 *   ({
 *     ({ <list of pointers to objects that have a short of "s1"> }),
 *     ({ <ditto for objects with a short of "s2"> }),
 *   }),
 *   ({ <pointers to invisible objects> }),
 * })
 * The first two arrays contain living objects, the second two contain
 * the rest, the last contains the pointers to invisible objects.
 * NB: this_player() is _not_ included in the array if (s)he is present.
 */

mixed query_inventory(object *obs) 
{
  mixed inv;
  object ob;
  mixed desc;
  int i,j,z;
  object me;

  me = real_this_player();

  inv = ({ ({ }), ({ }), ({ }), ({ }), ({ }), });

  if (!obs)
    obs = all_inventory(this_object());

  for (z = sizeof(obs)-1; z >= 0; z--)
  {
    ob = obs[z];
    if (!(desc = (string)ob->pretty_short()) || (desc == ""))
    {
      inv[4] += ({ ob });
      continue;
    }
    /* Remove comments when ready for hidden players - Radix
    if(interactive(ob) && (int)ob->query_hidden())
    {
       inv[4] += ({ ob });
       continue;
    }
    */
    if (pointerp(desc))
    {
      if (living(ob))
      {
        if (ob == me) 
          continue;
        for (j = 0; j < sizeof(desc); j++)
          if ((i = member_array(desc[j], inv[0])) >=0)
            inv[1][i] += ({ ob });
          else
          {
            inv[0] += ({ desc[j] });
            inv[1] += ({ ({ ob }), });
          }
        continue;
      }

      for (j = 0; j < sizeof(desc); j++)
        if ((i = member_array(desc[j], inv[2])) >= 0)
          inv[3][i] += ({ ob });
        else {
          inv[2] += ({ desc[j] });
          inv[3] += ({ ({ ob }) });
        }
      continue;
    }

    if (living(ob))
    {
      if (ob == me) 
        continue;
      if ((i = member_array(desc, inv[0])) >= 0)
      {
        inv[1][i] += ({ ob });
        continue;
      }
      inv[0] += ({ desc });
      inv[1] += ({ ({ ob }), });
      continue;
    } 

    if ((i = member_array(desc, inv[2])) >= 0)
    {
      inv[3][i] += ({ ob });
      continue;
    }

    inv[2] += ({ desc });
    inv[3] += ({ ({ ob }), });
  }
  return inv;
}

// important change in this function: now npcs and players are treated the same
// both shown in the same line, and both with colors if needed, see
// /lib/handlers.hud.c
string query_contents(string str, varargs object *obs) 
{
  int i, j, drunk;
  mixed inv;
  string ret, aux, color;
  int count, howmany;
  object me;

  ret = "";
  aux = "";
  color = "";
  drunk = 0;

  // sometimes the action is not started by a player
  // i.e. when coming out from the sea, you don't write any command
  // the action is started by the room itself
  // so maybe the mudlib do not know how to resolve 
  // this_player nor this_player(1) ??
  // TODO: check this in hexagon
  me = real_this_player();
  
  inv = query_inventory(obs);
  drunk = me->query_volume(0);

  if (!sizeof(inv[0]) && !sizeof(inv[2])) 
    return ret;
  
  if (str) 
    ret += str;
  
  howmany = count = sizeof(inv[0]);

  // check the list of living objects in the room
  for (i = 0; i < sizeof(inv[0]); i++) 
  {
    if (inv[0][i][0] < '0' && inv[0][i][0] > '9')
      j = sizeof(inv[1][i]) * ((random(drunk)/1000)+1);
    else
      j = sizeof(inv[1][i]);

    if (j > 1)
      ret += capitalize(query_num(j, 0)) + " ";

    color = (string)HUD->query_color(me, inv[1][i][0]);
    aux = (j > 1) ? inv[1][i][0]->pretty_plural() : inv[1][i][0]->query_cap_name();

    if (strlen(color))
      ret += color + aux + "%^RESET%^";
    else
      ret += aux;

    count--;
    if (count > 1)
       ret += ", ";
    if (count == 1)
       ret += " " + _LANG_AND + " ";
  }

  // Si hay varios tipos (howmany) o varios del mismo tipo (j)
  if ((howmany > 1) || (j > 1))
     ret += " " + _LANG_CONTENTS_ARE_HERE; 	
  if ((howmany == 1) && (j <= 1))
     ret += " " + _LANG_CONTENTS_IS_HERE; 	
    	
  // items  
  for (i = sizeof(inv[2]) - 1; i >= 0; i--) 
  {
    j = sizeof(inv[3][i]) * ((random(drunk)/1000)+1);
  
    if (j > 1) 
    {
      ret += capitalize(query_num(j, 20)) + " " +
             // capitalize((string)inv[3][i][0]->pretty_plural(inv[2][i])) + ".\n";
             capitalize((string)inv[3][i][0]->pretty_plural()) + ".\n";
      continue;
    }
    ret += capitalize(inv[2][i]) + ".\n";
  }  

  return ret;
}

void list_contents(string str) 
{
  write(query_contents(str));
}
