
#include <living/effects.h>
#include <user/xp.h>

int do_call_out_effect(mixed *params);

static int effect_hp;
static mixed *effects;
// effects that keep acting after their skill or spell ran:
// id -> ({ source, func, ticks left, data }), indices LASTING_*
static mapping lasting;

void create() 
{
  effects = ({ });
  lasting = ([ ]);
} /* create() */

// Para saber si está ejecutando algo o no
mixed query_effects() { return effects; }

int add_active_effect(int no_rnds, string type, string name, object callee,
           string func, mixed params) 
/* no_rounds: numero de turnos que durara el hechizo.
   type: tipo de efecto (generalmente "spell" (EFFECT_TYPE_SPELL), si permitimos multicasting sera 
     la escuela/esfera a la que pertenezca el hechizo).
   name: nombre del hechizo.
   callee: objeto del hechizo.
   func: funcion a la que tendremos que llamar (generalmente "hb_spell").
   params [0]: objetivo o lista de objetivos.
      [1]: 'quiet', si debemos dar mensajes o no.
      [2]: environment(caster).
      [3]: gp_cost <- gps a gastar si el hechizo se formula bien o no.
*/           
{
  int i,j;

  if (type == EFFECT_TYPE_SPELL)
    effect_hp = this_object()->query_hp();
  else
    effect_hp = -1;

  if ((i = member_array(type, effects)) != -1)
    if ((j = member_array(name, effects[i+1])) == -1)
      effects[i+1] += ({name, ({no_rnds, callee, func, params})});
    else
      effects[i+1][j+1] = ({no_rnds, callee, func, params});
  else
    effects += ({type, ({name, ({no_rnds, callee, func, params})})});
} /* add_active_effect() */

int remove_active_effect(string name) 
{
  int i, j;

  for (i = 0;i < sizeof(effects); i+=2)
    if ((j = member_array(name, effects[i+1])) != -1) 
    {
      if (effects[i+1][j+1][1])
        effects[i+1][j+1][1]->remove_me(this_object());

      effects[i+1] = delete(effects[i+1], j, 2);

      if (!sizeof(effects[i+1])) 
      {
        effects = delete(effects, i, 2);
        i -= 2;
      }
    }
  return 1;
} /* remove_active_effect() */

mixed query_active_effect(string name) 
{
  int i;

  if ((i=member_array(name, effects)) == -1)
    return 0;

  if (sizeof(effects[i+1]))
    return effects[i..i]+effects[0..1];

  return 0;
} /* query_active_effect() */

mixed *query_active_effect_type(string type) 
{
  int i, j;
  mixed *ret;

  if ((i=member_array(type, effects)) == -1)
    return ({ });

  ret = ({ });

  for (j=0;j<sizeof(effects);j+=2)
    ret += ({ effects[i], effects[i+1][j..j+1] });

  return ret;
} /* query_active_effect_type() */

int do_active_effects(object attacker)
{
  int i, j, ok;
  string * skill_path;

  this_object()->remove_timed_property(PROPERTY_IS_CASTING);
  
  for ( i = sizeof(effects)-2; i>=0; i-=2)
    for ( j = sizeof(effects[i+1])-2; j>=0; j-=2) 
    {
      if ((effect_hp - (int)this_object()->query_hp()) > 
        ((int)this_object()->query_max_hp()/SP_DIV) )
      {
        tell_room(environment(this_object()), "¡"+
           (string)this_object()->query_cap_name()+
           " pierde la concentración!\n",({this_object()}));
        tell_object(this_object(),"¡Te estremeces y pierdes la concentración!\n");

        remove_active_effect(effects[i+1][j]);
        continue;
      }
      else
      {
        /*
        // Esta comprobacion la hacemos a cada paso del hb_spell, esta repetida
        //  desde alli llamamos a end_spell_now que se encarga del 
        //  remove_active_effect
        //  neverbot 7/2001
        
        if (effects[i] == EFFECT_TYPE_SPELL){
          if (this_object()->query_property("nocast") ||
            environment(this_object())->query_property("nocast") ||
            this_object()->query_property("passed out"))
          {

            remove_active_effect(effects[i+1][j]);
            continue;
          }
        }
        */
        // Si todo ha ido bien, llamamos a la funcion de cada turno
        // del hechizo. Pasamos los parametros, el atacante, 
        // y el tipo de efecto (effect/spell) 
        ok = do_call_out_effect( ({effects[i+1][j+1], attacker, effects[i] }) );

        // Si hemos devuelto un -1 debemos terminar el hechizo (algo no
        // ha ido bien
        if (ok <= 0)
        {
          // Anunque hemos fallado la ejecucion, lo hemos intentado, por lo que
          // ponemos el lock igualmente, neverbot 06/12
          // (unless the effect's object is gone, and there is nothing to lock)
          if (effects[i+1][j+1][SP_OBJECT])
            this_object()->add_timed_property("LOCK " + 
              base_name(effects[i+1][j+1][SP_OBJECT]), 
              1, 
              (effects[i+1][j+1][SP_OBJECT])->query_lockout_time() / 2 );
  
          remove_active_effect(effects[i+1][j]);

          if (this_object()->query_coder())
            tell_object(this_object(),"DEBUG: Hechizo interrumpido. "+
              "Valor devuelto: "+ok+"\n");
           return 0;
        }  

        // En el ultimo paso acabamos el hechizo.
        // Si hemos llegado hasta aqui es que hemos de gastar los gps
        // (espero que este bien hecho y no de demasiados problemas)
        if (!--effects[i+1][j+1][SP_NO_RNDS])
        {
          this_object()->adjust_gp(-(int)effects[i+1][j+1][SP_PARAM][SP_GP_COST]);
          
          this_object()->add_timed_property("LOCK " + 
            base_name(effects[i+1][j+1][SP_OBJECT]), 
            1, 
            (effects[i+1][j+1][SP_OBJECT])->query_lockout_time() );

          // tell_object(find_living("folken"), "[do_active_effects] aprendizaje de la dote: "+effects[i+1][0]+"\n");
        
          // Si es una dote (efecto de tipo "effect"), mejoramos el aprendizaje
          // ya que la hemos ejecutado correctamente
          // A skill is known by its id, the skill object's file name; the
          // effect name is the translated verb and only matches it in English.
          skill_path = explode(base_name(effects[i+1][j+1][SP_OBJECT]), "/");
          this_object()->update_skill_used_times(skill_path[sizeof(skill_path) - 1]);
          
          remove_active_effect(effects[i+1][j]);
        }
      }
    }

  return 0;
} /* do_active_effects() */

int do_call_out_effect(mixed *params) 
{
  int res, aux_xp, percentage;
  mapping types;

  aux_xp = 0;

  // the effect's object was destructed halfway (updated, say): nothing to run
  if (!params[0][SP_OBJECT])
    return 0;

  res = call_other(params[0][SP_OBJECT],
    params[0][SP_FUNC],
    this_object(),
    //       params[1],
    params[0][SP_PARAM],
    params[0][SP_NO_RNDS]);

  // Tipos de xp, neverbot 07/04
  if ((res > 0) && (params[2] == EFFECT_TYPE_SPELL))
  {
    // the class's magic xp percentage applies to what the spell did; a class
    // that does not say gets all of it, as with kill xp
    types = this_object()->query_xp_types();
    percentage = (types && !undefinedp(types[MAGIC_XP])) ? types[MAGIC_XP] : 100;
    aux_xp = res * percentage / 100;

    this_object()->adjust_xp(aux_xp);
  }

  // Devolvemos el resultado. Si algo ha ido mal tendremos un -1, y debemos
  // terminar el hechizo (si continuamos acabaremos gastando los gps)
  return res;
      
} /* do_effect() */

/**
 * Drops every effect in execution, as if each had been interrupted.
 */
void clear_active_effects()
{
  int i, j;
  string * names;

  names = ({ });

  for (i = 0; i < sizeof(effects); i += 2)
    for (j = 0; j < sizeof(effects[i+1]); j += 2)
      names += ({ effects[i+1][j] });

  for (i = 0; i < sizeof(names); i++)
    remove_active_effect(names[i]);

  this_object()->remove_timed_property(PROPERTY_IS_CASTING);
}

/**
 * Ends one lasting effect and tells its source why.
 *
 * The entry leaves the list before the hook runs, so the hook may start a new
 * effect with the same id.
 */
private void end_lasting_effect(string id, string reason)
{
  mixed * entry;
  string err;

  entry = lasting[id];

  if (!entry)
    return;

  lasting[id] = nil;

  // a source that is gone has nobody left to clean up
  if (!entry[LASTING_SOURCE])
    return;

  err = catch(call_other(entry[LASTING_SOURCE], LASTING_END_FUNC,
    this_object(), id, entry[LASTING_DATA], reason));

  if (err)
    debug("effects", "lasting end of " + id + " in " +
      object_name(this_object()) + ": " + err + "\n");
}

/**
 * Starts a lasting effect: something a skill or spell keeps doing every
 * heart_beat for a while after it finished running.
 *
 * Every tick calls source->func(this living, data, ticks left) (ticks down to
 * 1); the effect ends when the ticks run out, when func returns 0, when the
 * call errors or when the source is gone. On end, source->lasting_end(this
 * living, id, data, reason) runs, reason being one of LASTING_END_*. An
 * effect with the same id is ended first (LASTING_END_REPLACED). Lasting
 * effects do not keep other skills or spells from running.
 *
 * @return 1 if it started, 0 if an argument is missing
 */
int add_lasting_effect(string id, object source, string func, int ticks,
  mixed data)
{
  if (!strlen(id) || !source || !strlen(func) || ticks <= 0)
    return 0;

  if (lasting[id])
    end_lasting_effect(id, LASTING_END_REPLACED);

  lasting[id] = ({ source, func, ticks, data });

  // a resting npc has its heart_beat off, and nothing would tick
  if (!query_heart_beat())
    set_heart_beat(1);

  return 1;
}

/**
 * Ends the lasting effect with this id (LASTING_END_REMOVED).
 *
 * @return 1 if there was one
 */
int remove_lasting_effect(string id)
{
  if (!lasting[id])
    return 0;

  end_lasting_effect(id, LASTING_END_REMOVED);
  return 1;
}

/**
 * The lasting effect with this id, a copy of ({ source, func, ticks left,
 * data }) (indices LASTING_*), or nil.
 */
mixed * query_lasting_effect(string id)
{
  if (!lasting[id])
    return nil;

  return ({ }) + lasting[id];
}

/**
 * The ids of every lasting effect in force.
 */
string * query_lasting_effects()
{
  return map_indices(lasting);
}

/**
 * Ends every lasting effect, telling each source the reason (default
 * LASTING_END_CLEARED).
 */
void clear_lasting_effects(varargs string reason)
{
  string * ids;
  int i;

  if (!reason)
    reason = LASTING_END_CLEARED;

  ids = map_indices(lasting);

  for (i = 0; i < sizeof(ids); i++)
    end_lasting_effect(ids[i], reason);
}

/**
 * One tick of every lasting effect; the living's heart_beat calls it.
 */
void tick_lasting_effects()
{
  string * ids;
  mixed * entry;
  mixed result;
  string err;
  int i;

  if (!map_sizeof(lasting))
    return;

  ids = map_indices(lasting);

  for (i = 0; i < sizeof(ids); i++)
  {
    // an earlier tick may have ended this one
    entry = lasting[ids[i]];

    if (!entry)
      continue;

    if (!entry[LASTING_SOURCE])
    {
      end_lasting_effect(ids[i], LASTING_END_ERROR);
      continue;
    }

    result = nil;
    err = catch(result = call_other(entry[LASTING_SOURCE], entry[LASTING_FUNC],
      this_object(), entry[LASTING_DATA], entry[LASTING_TICKS]));

    // the tick may have destructed this living
    if (!this_object())
      return;

    // the tick ended or replaced its own effect, and that call decided
    if (lasting[ids[i]] != entry)
      continue;

    if (err)
    {
      debug("effects", "lasting tick of " + ids[i] + " in " +
        object_name(this_object()) + ": " + err + "\n");
      end_lasting_effect(ids[i], LASTING_END_ERROR);
    }
    else if (!result)
      end_lasting_effect(ids[i], LASTING_END_DONE);
    else if (--entry[LASTING_TICKS] <= 0)
      end_lasting_effect(ids[i], LASTING_END_EXPIRED);
  }
}

mixed * stats()
{
  mixed * ret;
  string * ids;
  int i;

  ret = ({ });
  ids = map_indices(lasting);

  for (i = 0; i < sizeof(ids); i++)
    ret += ({ ({ "Lasting effect " + ids[i] + " (nosave)",
      (lasting[ids[i]][LASTING_SOURCE] ?
        object_name(lasting[ids[i]][LASTING_SOURCE]) : "gone") + "->" +
      lasting[ids[i]][LASTING_FUNC] + ", " +
      lasting[ids[i]][LASTING_TICKS] + " ticks left", }) });

  return ret;
}
