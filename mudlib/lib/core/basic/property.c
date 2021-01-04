#define TICS_HB 5 /* cludge to get timed properties on things without heart_beats */
/* /global/basic/property */
/* Adding static properties. making it easer to use for spelleffetcs
 * and such wombles..
 * Baldrick, Jan '95
 * Hrmf.. now a hard one, a timed propertysystem. saveable
 */
 
mapping map_prop;
mapping timed_prop;
static mapping static_map_prop;
 
void create() 
{
  map_prop = ([ ]);
  static_map_prop = ([ ]);
  timed_prop = ([ ]);
}
 
int query_hb_counter()
{
  return time()/TICS_HB;
}

int query_hb_diff(int oldc)
{
  int hb_counter;
  hb_counter = query_hb_counter();
  if (hb_counter > oldc) 
    return hb_counter - oldc;
  else 
    return oldc - hb_counter;
}
 
int add_property(string var, mixed val) 
{
  if (!strlen(var))
    return 0;
  if (!map_prop)
    map_prop = ([ ]);
  map_prop[var] = val;
  return 1;
}

int add_static_property(string var, mixed val) 
{
  if (!strlen(var))
    return 0;
  if (!static_map_prop)
    /* this_object() because we have a crude thingy in here which is over-ridden in anything with it's own HB */
    static_map_prop = ([ ]);
  static_map_prop[var] = val;
  return 1;
}

int add_timed_property(string var, mixed val, int time)
{
  int i;

  if (!strlen(var))
    return 0;
  if (time < 1)
    return 0;
  if (!timed_prop)
    timed_prop = ([ ]);
  i = this_object()->query_hb_counter();
  timed_prop[var] = ({val, time, i});
  return 1;
}

int remove_property(string var) 
{
  if (!strlen(var))
    return 0;
  if (!map_prop)
    map_prop = ([ ]);
  map_prop = m_delete(map_prop, var);
  return 1;
}

int remove_static_property(string var) 
{
  if (!strlen(var))
    return 0;
  if (!static_map_prop)
    static_map_prop = ([ ]);
  static_map_prop = m_delete(static_map_prop, var);
  return 1;
}

int remove_timed_property(string var) 
{
  if (!strlen(var))
    return 0;
  if (!timed_prop)
    timed_prop = ([ ]);
  timed_prop = m_delete(timed_prop, var);
  return 1;
}
 
mixed query_old_property(string str) 
{
  if (!strlen(str))
    return 0;
  if (!map_prop)
    map_prop = ([ ]);
  if (undefinedp(map_prop[str]))
    return 0;
  return map_prop[str];
}


mixed query_static_property(string str) 
{
  if (!strlen(str))
    return 0;
  if (!static_map_prop)
    static_map_prop = ([ ]);
  if (undefinedp(static_map_prop[str]))
    return 0;
  return static_map_prop[str];
}

/* This is the hard part of the timing system.
 * It will check if the property is timed out before returning the value.
 * this will make the timingsystem sleeping while not in use.
 * Smart, idea from Taniwha, coded by Baldrick.
 * ehh tried to be coded.. wonder if it will work..
 */
mixed query_timed_property(string str) 
{
  int val, i;

  if (!strlen(str))
    return 0;

  if (!timed_prop)
    timed_prop = ([ ]);
  // if (!sizeof(timed_prop[str][0]));
  //   return 0;
  if (!timed_prop[str]) 
    return 0;
  i = timed_prop[str][1];
  if (this_object()->query_hb_diff(timed_prop[str][2]) > i)
  {
    remove_timed_property(str);
    return 0;
  }
  val = timed_prop[str][0];
  return val;
}
/* Added Taniwha 1995, by popular demand, time remaining */
/* Could do clever stuff like check for static & normals if it falls through */

int query_time_remaining(string str) 
{
  int i;
  int val;

  if (!strlen(str))
    return 0;
  if (!timed_prop)
    timed_prop = ([ ]);
  if (!timed_prop[str]) 
    return 0;
  i = timed_prop[str][1];
  val = i - (int)this_object()->query_hb_diff(timed_prop[str][2]) ;
  if ( val <= 0)
  {
      val = 0;
      remove_timed_property(str);
  }
  // timeleft = val;
  // return timeleft;
  return val;
}

/* Added by Baldrick April '95.
 * This is called upon every login or logout (not decided yet)
 * Will remove "old" timed properties to redice memoryusage.
 * The clue is to get through a bunch of stringpointers when you don't
 * know the string...
 */
void traverse_timed_properties()
{
  int i, koo;
  string *koofs;

  koofs = map_indices (timed_prop);
  for (i=0;i<sizeof(koofs);i++)
  {
    // Fix by Wonderflug, we don't want the first character
    // in the string :)  first element of the mapping element
    //koo = koofs[i][1];
    koo = timed_prop[koofs[i]][1];
    if (this_object()->query_hb_diff(timed_prop[koofs[i]][2]) > koo)
    {
      remove_timed_property(koofs[i]);
    } /* if */
  } /* for */
} /* void traverse.. */
 
int query_property_exists(string str) 
{
  if (!map_prop)
    map_prop = ([ ]);
  return (member_array(str, map_indices(map_prop))) != -1;
}

int query_static_property_exists(string str) 
{
  if (!static_map_prop)
    static_map_prop = ([ ]);
  return (member_array(str, map_indices(static_map_prop))) != -1;
}

int query_timed_property_exists(string str) 
{
  if (!timed_prop)
  {
    timed_prop = ([ ]);
      return 0;
  }
  if (!strlen(str)) 
    return 0;
  if (!sizeof(map_indices(timed_prop))) 
    return 0;
  query_timed_property(str); // Taniwha, so they expire
  return (member_array(str, map_indices(timed_prop))) != -1;
}
 
mapping query_properties() { return map_prop ; }

mapping query_static_properties() { return static_map_prop ; }

mapping query_timed_properties() { return timed_prop; }

int add_properties(mapping map)
{
  map_prop += map;
}

int add_static_properties(mapping map) 
{
  static_map_prop += map;
}

int add_timed_properties(mapping map) 
{
  timed_prop += map;
}

mixed query_property(string str) 
{
  if (!strlen(str))
    return 0;
  if (query_timed_property_exists(str)) 
    return query_timed_property(str);
  if (query_static_property_exists(str)) 
    return query_static_property(str);
  return query_old_property(str); 
}

/* Added by Hamlet -- If we're going to save timed properties, let's
   have them restore to something worthwhile. :)
*/
mapping freeze_timed_properties(mapping wabbit) 
{
  string *ind;
  int i;
  int timeleft;
  mapping ret;

  ret = ([ ]);

  if (!wabbit || !m_sizeof(wabbit))  
    return ([ ]);
  
  ind = map_indices(wabbit);

  for (i = 0; i < sizeof(ind); i++) 
  {
    timeleft = wabbit[ind[i]][1] -
                 (int)this_object()->query_hb_diff(wabbit[ind[i]][2]);
    if (timeleft > 0)
      ret[ind[i]] = ({ wabbit[ind[i]][0], timeleft });
  }
  return ret;
}

mapping thaw_timed_properties(mapping turtle) 
{
  string *ind;
  int i;
  mapping ret;

  ret = ([ ]);

  if (!turtle || !m_sizeof(turtle))  
    return ([ ]);

  ind = map_indices(turtle);

  for (i = 0; i < sizeof(ind); i++)
    ret[ind[i]] = ({ turtle[ind[i]][0], turtle[ind[i]][1],
                     this_object()->query_hb_counter()
                  });

  return ret;
}

/*
 * Added to be cute and be efficient
 *   Wahooka -- May '97
 */
mixed adjust_property(string str, mixed val, varargs int flag) 
{
  if (!map_prop || !val) 
    return 0;
  if (!map_prop[str]) 
    return map_prop[str] = val;
  if (flag == -1)
    return map_prop[str] -= val;
  return map_prop[str] += val;
}

mixed adjust_static_property(string str, mixed val, varargs int flag) 
{
  if (!static_map_prop || !val) 
    return 0;
  if (!static_map_prop[str]) 
    return static_map_prop[str] = val;
  if (flag == -1)
    return static_map_prop[str] -= val;
  return static_map_prop[str] += val;
}

/* Don't know if the following will work, can bother to check and see
 * if it might break something right now...

varargs mixed adjust_timed_property(string str, mixed val, int time) {
  if (!timed_prop || !timed_prop[str]) return 0;
  if (val)
    timed_prop[str][0] += val;
  if (time)
    timed_prop[str][1] += time;
  return timed_prop[str];
}
*/

mixed stats()
{
  return ({
		({ "Properties", map_prop, }),
		({ "Timed Properties", timed_prop, }),
		({ "Static Properties (nosave)", static_map_prop, }),
	  });
}
