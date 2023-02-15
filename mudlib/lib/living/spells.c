
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

mixed spells;
mixed minor_spheres;    /* Las escuelas tb son tratadas como esferas, las diferenciamos */
mixed major_spheres;    /* mirando cual es el spell_dir de nuestro gremio. */
mixed neutral_spheres;

/* spells.c original por Baldrick, julio '93
 *
 * Este archivo mantiene la lista de hechizos que cada personaje conoce,
 * y las acciones para comprobar los conocimientos de nuestras escuelas/esferas,
 * mas el util comando 'formular' :)
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
  add_private_action("show_spells",  "hechizos");
  add_private_action("show_spheres", "esferas");
  add_private_action("show_schools", "escuelas");
  add_private_action("cast",         "formular");
}

/*  Si el hechizo tiene una funcion help(), imprimira el string que se
 *  devuelve aqui.
 */
string help_spell(string str)
{
  object guild;
  string spell_dir;

  if (!mappingp(spells))
    spells = ([ ]);

  if (!map_sizeof(spells))
    return "";

  if (!spells[str])
    return "";

  guild = this_player()->query_guild_ob();
  spell_dir = guild->query_spell_directory();

  return ((string)(spell_dir+spells[str][S_OBJECT])->help(str));
}

/*
 * Listado de los hechizos que conoce el personaje:
 */
int show_spells(string str)
{
  string *lista;

  if (!mappingp(spells))
    spells = ([ ]);

  if (!map_sizeof(spells))
  {
    tell_object(this_player(), "No conoces ningún hechizo.\n");
    return 1;
  }

  lista = m_indices(spells);
  tell_object(this_player(), "Conoces los siguientes hechizos:\n");

  printf("%#-*s\n", this_user()->query_cols(), implode(lista, "\n"));
  return 1;
}

/* Formulara el hechizo despues de un round de formulacion. */
int cast(string str)
{
  int i, j;
  string *s,s1;
  object guild;
  object *armours;
  string spell_dir;

  // Añadido, neverbot 02/2006
  if (!mappingp(spells))
    spells = ([ ]);

  if (!map_sizeof(spells))
  {
    tell_object(this_player(),"No conoces ningún hechizo.\n");
    return 1;
  }

  /* Comprobaremos estas propiedades en cada uno de los hb_spell del hechizo,
     asi que en el primer turno del hechizo parariamos.
     Podemos ahorrarnos estas comprobaciones.
     neverbot 7/01

  if ( this_player()->query_property("nocast") )
  {
    tell_object(this_player(), "Parece que ahora no puedes reunir el poder "
      "necesario para formular el hechizo.\n");
    return 1;
  }

  if (environment(this_player())->query_property("nocast"))
  {
    tell_object(this_player(),"Los poderes misticos parecen ser demasiado "
      "debiles aqui y la magia no afecta.\n");
    return 1;
  }

  */

  if ( this_player()->query_property(PROPERTY_STILLCASTING) )
  {
    tell_object(this_player(), "No puedes formular otro hechizo hasta que transcurra "+
      "algún tiempo.\n");
    return 1;
  }

  if (!strlen(str))
  {
    notify_fail("Uso: formular <hechizo> [<objetivo>]\n");
    return 0;
  }

  /* Dead players should not cast spells!
   * Baldrick.
   */
  if (this_player()->query_dead())
  {
    notify_fail("Los espíritus no pueden formular hechizos.\n");
    return 0;
  }

  guild = (object)this_player()->query_guild_ob();

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
  if (armours = this_player()->query_worn_ob())
  {
    for (i = 0; i < sizeof(armours); i++)
    {
      if (!guild->query_legal_armour((string)armours[i]->query_armour_type_name()))
      {
        notify_fail("No puedes formular hechizos mientras vistas esa armadura.\n");
        return 0;
      }
    }
   }

  s = explode(str, " ");
  s1 = s[0];
  j = 1;
  while (!spells[s1] && j < sizeof(s))
    s1 = implode(s[0..j++], " ");

  if (!spells[s1])
  {
    notify_fail("No conoces ningún hechizo llamado '"+str+"'.\n");
    return 0;
  }

  /* ok we found our spell...
   * cast it...
   * Or, at least try to ;)
   */
  /* the Spell_dir is used to reduce space in the spellarray */

  spell_dir = guild->query_spell_directory();

  /*** Para solucion de problemas, añadido por neverbot para hacerlo igual que en el
       mud original ***/
  if (this_player()->query_coder())
     tell_object(this_player(),"DEBUG: El archivo es: "+spell_dir + spells[s1][S_OBJECT]+".\n");

  return (int)call_other(spell_dir + spells[s1][S_OBJECT], "cast_spell", implode(s[j..sizeof(s)], " "));
}
/* Cast */

mixed query_spells() { return spells; }

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

int query_spell(string type)
{
  if (!mappingp(spells))
    return 0;
  return spells[type];
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

int show_spheres(string str)
{
  object guild;
  int written, i;

  // neverbot, 02/2006
  if (!mappingp(spells))
    spells = ([ ]);
  if (!map_sizeof(spells))
    return 0;

  guild = this_player()->query_guild_ob();

  written = 0;

  // if ( (guild->query_spell_directory() != CLERIC_ROOT) &&
  //      (guild->query_spell_directory() != DRUID_ROOT) )

  if ((!guild) || ((string)guild->query_spell_directory() != CLERIC_ROOT))
  {
    notify_fail("No necesitas esferas.\n");
    return 0;
  }

  /* Could format it better, but I don't care. */
  for ( i=0; i<sizeof(minor_spheres); i++, written++ )
    tell_object(this_player(), minor_spheres[i]+" (Menor)\n");
  for ( i=0; i<sizeof(neutral_spheres); i++, written++ )
    tell_object(this_player(), neutral_spheres[i]+" (Neutral)\n");
  for ( i=0; i<sizeof(major_spheres); i++, written++ )
    tell_object(this_player(), major_spheres[i]+" (Mayor)\n");

  if (!written)
  {
    notify_fail("No conoces ninguna esfera.\n");
    return 0;
  }
  return written;
} /* int show spheres..*/

int show_schools(string str)
{
  object guild;
  int written, i;

  // neverbot, 02/2006
  if (!mappingp(spells))
    spells = ([ ]);
  if (!map_sizeof(spells))
    return 0;

  guild = this_player()->query_guild_ob();

  // if ( (guild->query_spell_directory() != MAGE_ROOT) &&
  //      (guild->query_spell_directory() != BARD_ROOT) )

  if (!guild || (string)guild->query_spell_directory() != MAGE_ROOT)
  {
    notify_fail("No necesitas escuelas.\n");
    return 0;
  }

  written = 0;
  for ( i=0; i<sizeof(minor_spheres); i++, written++ )
    tell_object(this_player(), minor_spheres[i]+" (Menor)\n");
  for ( i=0; i<sizeof(neutral_spheres); i++, written++ )
    tell_object(this_player(), neutral_spheres[i]+" (Neutral)\n");
  for ( i=0; i<sizeof(major_spheres); i++, written++ )
    tell_object(this_player(), major_spheres[i]+" (Mayor)\n");

  if (!written)
  {
    notify_fail("No tienes escuelas.\n");
    return 0;
  }
  return written;
} /* int show schools..*/

int spell_damage(int damage, string type, object caster)
{
  int prot;
  int original;

  original = damage;
  prot = this_object()->query_res(type);

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

// Añadida por neverbot para comprobar la resistencia de cualquier personaje o npc
// a un tipo de daño (la usan todos los npcs de /oficios/wizards).
// Antes no habia una funcion de calculo de resistencias decente :P

int query_res(string tipo)
{
  return this_object()->query_resistance(tipo);
}

mixed *stats()
{
  return ({
    ({ "Spells", spells }),
    ({ "Minor Spheres", minor_spheres }),
    ({ "Major Spheres", major_spheres }),
    ({ "Neutral Spheres", neutral_spheres }),
          });
}