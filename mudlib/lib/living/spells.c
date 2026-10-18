
/*** *******************************************************************************

    /lib/living/spells.c

    Algunos cambios respecto al sistema anterior:
     - Las protecciones magicas se cuentan al lanzar el hechizo
       (ver /std/spells/base.c), con la diferencia de que ahora no se
       re-calculan en la funcion spell_damage();
     - En la funcion spell_damage se contaban las resistencias para hacer
       el calculo de un critico/pifia en el hechizo. (Es decir, se restaba
       la proteccion magica dos veces). El 'malus' por mala suerte (pifia)
       ya se calcula tb en el base.c, asi que dejamos spell_damage() como
       esta.
    Como hacer daño magico a un personaje:
     - En el caso de ser un arma, como siempre se ha hecho:
        add_attack("ataque_secundario","water",2,2,1);   (por ejemplo)
     - En caso de ser una trampa, una accion en una room o algo similar:
       Tenemos dos posibilidades para la funcion spell_damage()
       int spell_damage(int damage, string type, object caster);
          * Para los hechizos normales, donde caster es el formulador del hechizo
            target->spell_damage(damage,type,caster);
       int spell_damage(int damage, string type)
          * Para hacer daño de un tipo determinado (agua, fuego, etc) contando
            las resistencias magicas del personaje :)
            target->spell_damage(damage,type);

    Por neverbot 7/2001
 *** *******************************************************************************/

#include <living/spells.h>
#include <living/effects.h>
#include <language.h>

// the spells this living knows: id -> ({ path of the spell object }), see
// add_spell; every spell is an effect (BASE_EFFECT) cast with formular
mixed spells;
mixed minor_spheres;    /* Las escuelas tb son tratadas como esferas, las diferenciamos */
mixed major_spheres;    /* mirando cual es el spell_dir de nuestro gremio. */
mixed neutral_spheres;

/* spells.c original por Baldrick, julio '93
 *
 * Este archivo mantiene la lista de hechizos que cada personaje conoce,
 * y las acciones para comprobar los conocimientos de nuestras escuelas/esferas,
 * mas el util comando 'formular' :)
 *
 * 10/2026 - Spells are effects now, like skills: formular finds a known spell
 *           by its name and calls its cast_effect, neverbot
 */

void create()
{
  spells = ([ ]);
  minor_spheres = ({ });
  neutral_spheres = ({ });
  major_spheres = ({ });
}

void spell_commands()
{
  add_private_action("show_spells",  _LANG_SPELLS_CMDS_LIST);
  add_private_action("show_spheres", _LANG_SPELLS_CMDS_SPHERES);
  add_private_action("show_schools", _LANG_SPELLS_CMDS_SCHOOLS);
  add_private_action("cast",         _LANG_SPELLS_CMDS_CAST);
}

/**
 * The known spell named at the start of a text, by its name or an alias,
 * whole words only. The longest name wins, so a spell whose name begins
 * another's is not taken for it.
 * @param  str  what follows the verb: the spell's name, then its target
 * @return ({ path, the rest of the text }), or nil when no known spell starts it
 */
private mixed * match_spell(string str)
{
  string * ids, * names;
  string name, best_path, best_rest;
  object ob;
  int i, n, best;

  if (!mappingp(spells) || !str)
    return nil;

  str = lower_case(trim(str));
  ids = map_indices(spells);
  best = 0;

  for (i = 0; i < sizeof(ids); i++)
  {
    if (!arrayp(spells[ids[i]]) || !sizeof(spells[ids[i]]) ||
        !stringp(spells[ids[i]][S_OBJECT]))
      continue;

    ob = nil;
    catch(ob = load_object(spells[ids[i]][S_OBJECT]));
    if (!ob)
      continue;

    names = ({ ob->query_effect_name() }) + ob->query_aliases();
    for (n = 0; n < sizeof(names); n++)
    {
      name = lower_case(names[n]);
      if (strlen(name) <= best)
        continue;

      if (str == name)
      {
        best = strlen(name);
        best_path = spells[ids[i]][S_OBJECT];
        best_rest = "";
      }
      else if (strlen(str) > strlen(name) &&
               str[0..strlen(name)] == name + " ")
      {
        best = strlen(name);
        best_path = spells[ids[i]][S_OBJECT];
        best_rest = str[strlen(name) + 1..];
      }
    }
  }

  return best ? ({ best_path, best_rest }) : nil;
}

/*  Si el hechizo tiene una funcion help(), imprimira el string que se
 *  devuelve aqui.
 */
string help_spell(string str)
{
  mixed * match;

  match = match_spell(str);
  if (!match)
    return "";

  return (string)match[0]->help();
}

/*
 * Listado de los hechizos que conoce el personaje:
 */
int show_spells(string str)
{
  string * ids, * names;
  object ob;
  int i;

  if (!mappingp(spells))
    spells = ([ ]);

  if (!map_sizeof(spells))
  {
    tell_object(this_object(), _LANG_SPELLS_NONE_KNOWN);
    return 1;
  }

  ids = map_indices(spells);
  names = ({ });
  for (i = 0; i < sizeof(ids); i++)
  {
    ob = nil;
    if (arrayp(spells[ids[i]]) && sizeof(spells[ids[i]]))
      catch(ob = load_object(spells[ids[i]][S_OBJECT]));
    names += ({ ob ? capitalize(ob->query_effect_name()) : ids[i] });
  }

  tell_object(this_object(), _LANG_SPELLS_KNOWN_HEADER +
              "  " + implode(names, "\n  ") + "\n");
  return 1;
}

/* Formulara el hechizo despues de un round de formulacion. */
int cast(string str)
{
  int i;
  object guild;
  object *armours;
  mixed * match;

  // Añadido, neverbot 02/2006
  if (!mappingp(spells))
    spells = ([ ]);

  if (!map_sizeof(spells))
  {
    notify_fail(_LANG_SPELLS_NONE_KNOWN);
    return 0;
  }

  /* Comprobaremos estas propiedades en cada uno de los hb_spell del hechizo,
     asi que en el primer turno del hechizo parariamos.
     Podemos ahorrarnos estas comprobaciones.
     neverbot 7/01 */

  if ( this_object()->query_property(PROPERTY_STILLCASTING) )
  {
    notify_fail(_LANG_SPELLS_STILL_CASTING);
    return 0;
  }

  if (!strlen(str))
  {
    notify_fail(_LANG_SPELLS_CAST_USAGE);
    return 0;
  }

  /* Dead players should not cast spells!
   * Baldrick.
   */
  if (this_object()->query_dead())
  {
    notify_fail(_LANG_SPELLS_DEAD);
    return 0;
  }

  guild = this_object()->query_guild_ob() ?
          load_object(this_object()->query_guild_ob()) : nil;

  /* ******************************************************************************
    Es de suponer que los gremios que pueden formular tienen limitaciones al
    tipo de armadura que lleven...
    Esto puede ser un problema con los magos guerreros, en el 23 estaba limitado,
    asi que quiza habria que comprobar aqui el material del que esta fabricada
    la armadura del formulador.
    neverbot
  ** ******************************************************************************/

  // PENDIENTE: Actualizar con el sistema de malus a la hora de formular segun
  // el equipo puesto
  if (guild && (armours = this_object()->query_worn_ob()))
  {
    for (i = 0; i < sizeof(armours); i++)
    {
      if (!guild->query_legal_armour((string)armours[i]->query_armour_type_name()))
      {
        notify_fail(_LANG_SPELLS_ILLEGAL_ARMOUR);
        return 0;
      }
    }
  }

  match = match_spell(str);
  if (!match)
  {
    notify_fail(_LANG_SPELLS_UNKNOWN);
    return 0;
  }

  /*** Para solucion de problemas, añadido por neverbot para hacerlo igual que en el
       mud original ***/
  if (this_object()->query_coder())
    tell_object(this_object(), "DEBUG: " + match[0] + "\n");

  // the target is whatever follows the spell's name
  return (int)call_other(match[0], "cast_effect", match[1], this_object(), 0);
}
/* Cast */

mixed query_spells() { return spells; }

/**
 * Teaches this living a spell.
 * @param  name  the spell's id (its file name, see query_spell_id in /lib/guild.c)
 * @param  ob    the full path of the spell object
 */
int add_spell(string name, mixed ob)
{
  int i;
  mapping tmp;

  if (pointerp(spells))
  {
    tmp = ([ ]);
    for (i=0;i<sizeof(spells);i+=2)
      tmp[spells[i]] = spells[i+1];
    spells = tmp;
  }

  if (!mappingp(spells))
    spells = ([ ]);

  spells[name] = ({ ob });
  return 1;
}

int remove_spell(string name)
{
  spells = m_delete(spells, name);
  return 1;
}

int reset_spellarray()
{
  spells = ([ ]);
  // write ("Bing.\n");
  return 1;
}

// whether this living knows the spell with that id
int query_spell(string type)
{
  if (!mappingp(spells))
    return 0;
  return !!spells[type];
}

/* the sphere code.. added by Baldrick october '93 to use by
 * add_spell code in the guilds..*
 * Fubaring by Wonderflug, August 95.
 */

int add_sphere(string name, string power)
{
  if (!pointerp(minor_spheres))
    minor_spheres = ({ });
  if (!pointerp(major_spheres))
    major_spheres = ({ });
  if (!pointerp(neutral_spheres))
    neutral_spheres = ({ });

  switch( power )
  {
    case "menor":
      minor_spheres += ({ name, });
      break ;
    case "neutral":
      neutral_spheres += ({ name, });
      break;
    case "mayor":
      major_spheres += ({ name, });
      break;
    default:
      /* For backward compatibility. */
      neutral_spheres += ({ name, });
      break;
  }

  return 1;
}

int add_spheres(mixed list)
{
  int i;

  if (!pointerp(minor_spheres))
    minor_spheres = ({ });
  if (!pointerp(major_spheres))
    major_spheres = ({ });
  if (!pointerp(neutral_spheres))
    neutral_spheres = ({ });

  for ( i=0; i<sizeof(list); i++ )
  {
    if ( sizeof(list[i]) < 2 )
    {
      /* Again, for backward compatibility. */
      neutral_spheres += ({ list[i] });
      continue;
    }

    switch( list[i][1] )
    {
      case "menor":
        minor_spheres += ({ list[i][0] });
        break;
      case "mayor":
        major_spheres += ({ list[i][0] });
        break;
      case "neutral":
        neutral_spheres += ({ list[i][0] });
        break;
      default:
        break;
    } /* switch */
  } /* for */

  return 1;
}

/*
int remove_sphere(string name)
  {
  spheres = m_delete(spheres, name);
  return 1;
  }
*/
// Añadida por neverbot 7/01
int remove_sphere(string name)
{
  minor_spheres-=({name});
  major_spheres-=({name});
  neutral_spheres-=({name});
  return 1;
}

int reset_spheres()
{
  minor_spheres = ({ });
  major_spheres = ({ });
  neutral_spheres = ({ });
  return 1;
}

/* Devolvemos el conjunto de los tres niveles:
 */
mixed query_spheres()
{
  return minor_spheres + major_spheres + neutral_spheres + ({ });
}

/* Extensiones */
mixed query_minor_spheres() { return minor_spheres + ({ }); }
mixed query_neutral_spheres() { return neutral_spheres + ({ }); }
mixed query_major_spheres() { return major_spheres + ({ }); }

/* Devolvemos el poder de la esfera como 'int'
     0: el personaje no conoce esa esfera
     1: la conoce a 'menor'
     2: la conoce a 'neutral'
     3: la conoce a 'mayor'
*/
mixed query_sphere(string name)
{
  if ( member_array(name, minor_spheres) != -1 )
    return 1;
  if ( member_array(name, neutral_spheres) != -1 )
    return 2;
  if ( member_array(name, major_spheres) != -1 )
    return 3;

  return 0;
}

mixed query_minor_sphere(string name)
{
  if ( member_array(name, minor_spheres) != -1 )
    return 1;
  return 0;
}

mixed query_neutral_sphere(string name)
{
  if ( member_array(name, neutral_spheres) != -1 )
    return 1;
  return 0;
}

mixed query_major_sphere(string name)
{
  if ( member_array(name, major_spheres) != -1 )
    return 1;
  return 0;
}

/* Another extension. Just returns the string instead of a number. */
mixed query_sphere_level(string name)
{
  switch( query_sphere(name) )
  {
    case 1:
      return "menor";
    case 2:
      return "neutral";
    case 3:
      return "mayor";
    default:
      return 0;
  }
  return 0;
}

/*  Si name es un string (una sola esfera), devolvemos
       query_sphere_level() (nivel de la esfera)
    Si es un array, devolvemos el nivel de la esfera con mayor nivel de la lista.
*/
mixed query_sphere_power(mixed name)
{
  int i;
  int max;

  if (stringp(name))
    return query_sphere_level(name);
  if (arrayp(name))
  {
    max = 0;
    for (i = 0;i<sizeof(name);i++)
    {
      if (query_sphere(name[i])>max)
        max = query_sphere(name[i]);
    }
    switch(max)
    {
      case 1: return "menor";
      case 2: return "neutral";
      case 3: return "mayor";
      default: return 0;
    }
  }
  return 0;
}

/**
 * How well this living knows a category of spells, a sphere (divine magic) or
 * a school (arcane magic), as the effects ask it to price a spell (see
 * query_effect_category in /lib/effects/base_effect.c). A member's guild says
 * it; the categories added to the living itself (an NPC's) count when higher.
 * @return 0 not at all, 1 minor, 2 neutral, 3 major
 */
int query_effect_category(string name)
{
  object guild;
  int known, own;

  known = 0;
  guild = this_object()->query_guild_ob() ?
          load_object(this_object()->query_guild_ob()) : nil;
  if (guild)
    known = guild->query_category_knowledge(name);

  own = query_sphere(name);
  return own > known ? own : known;
}

/**
 * The same, as one of the EFFECT_CAT_* names.
 */
string query_effect_category_level(string name)
{
  switch (query_effect_category(name))
  {
    case 1:
      return EFFECT_CAT_MINOR;
    case 2:
      return EFFECT_CAT_NEUTRAL;
    case 3:
      return EFFECT_CAT_MAJOR;
  }
  return EFFECT_CAT_NONE;
}

/**
 * The categories this living casts from, of one kind, and how well it knows
 * each: the guild's and the living's own. The living's own lists never said
 * which kind each was, so they show under both.
 * @param  guild_list  the guild's sphere or school mapping
 * @return category -> EFFECT_CAT_*
 */
private mapping known_categories(mapping guild_list)
{
  mapping known;
  int i;

  known = mappingp(guild_list) ? ([ ]) + guild_list : ([ ]);

  for (i = 0; i < sizeof(minor_spheres); i++)
    known[minor_spheres[i]] = EFFECT_CAT_MINOR;
  for (i = 0; i < sizeof(neutral_spheres); i++)
    known[neutral_spheres[i]] = EFFECT_CAT_NEUTRAL;
  for (i = 0; i < sizeof(major_spheres); i++)
    known[major_spheres[i]] = EFFECT_CAT_MAJOR;

  return known;
}

// The listing of esferas and escuelas, each by the name the guild gives it.
private int show_categories(mapping known, string header, string none)
{
  object guild;
  string * names;
  string text;
  int i;

  if (!map_sizeof(known))
  {
    notify_fail(none);
    return 0;
  }

  guild = this_object()->query_guild_ob() ?
          load_object(this_object()->query_guild_ob()) : nil;

  names = map_indices(known);
  text = header;
  for (i = 0; i < sizeof(names); i++)
    text += "  " + (guild ? guild->query_category_name(names[i]) :
                            capitalize(names[i])) + " (" +
            _LANG_SPELLS_CATEGORY_LEVELS[query_effect_category(names[i])] + ")\n";

  tell_object(this_object(), text);
  return 1;
}

// esferas: the spheres of divine magic this living casts from.
int show_spheres(string str)
{
  object guild;

  guild = this_object()->query_guild_ob() ?
          load_object(this_object()->query_guild_ob()) : nil;

  return show_categories(known_categories(guild ? guild->query_spheres() : nil),
                         _LANG_SPELLS_SPHERES_HEADER, _LANG_SPELLS_NO_SPHERES);
}

// escuelas: the schools of arcane magic this living casts from.
int show_schools(string str)
{
  object guild;

  guild = this_object()->query_guild_ob() ?
          load_object(this_object()->query_guild_ob()) : nil;

  return show_categories(known_categories(guild ? guild->query_schools() : nil),
                         _LANG_SPELLS_SCHOOLS_HEADER, _LANG_SPELLS_NO_SCHOOLS);
}

int spell_damage(int damage, string type, varargs object caster)
{
  int prot;
  int original;

  original = damage;
  prot = this_object()->query_resistance(type);

  // Debug de informacion a los inmortales (neverbot 7/01)
  if ((prot != 0) && this_object()->query_coder())
     tell_object(this_object(), "DEBUG (spell_damage): Aplicada resistencia mágica "+
               "contra '"+type+"' ("+prot+"%).\n");

  // Calculamos al modificacion al daño segun resistencias:
  damage = (damage * (100 - prot)) / 100;

  if (original > 0 && damage == 0)
    damage = 1;
  if (original < 0 && damage == 0)
    damage = -1;

  if (!caster)
  {
    return (this_object()->query_hp() - this_object()->adjust_hp(-damage) );
    // return 1;
  }
  else
    return (this_object()->query_hp() - this_object()->adjust_hp(-damage, caster) );
  //return 1;
}

mixed * stats()
{
  return ({
    ({ "Spells", spells }),
    ({ "Minor Spheres", minor_spheres }),
    ({ "Major Spheres", major_spheres }),
    ({ "Neutral Spheres", neutral_spheres }),
          });
}