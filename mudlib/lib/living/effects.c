
#include <living/effects.h>
#include <user/xp.h>

int do_call_out_effect(mixed *params);

static int effect_hp;
static mixed *effects;

void create() 
{
  effects = ({ });
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
          if (this_object()->query_coder())
          {
            tell_object(this_object(),"DEBUG: Coste en gps: "+
                (int)effects[i+1][j+1][SP_PARAM][SP_GP_COST]+"\n");
          }         
          this_object()->adjust_gp(-(int)effects[i+1][j+1][SP_PARAM][SP_GP_COST]);
          
          this_object()->add_timed_property("LOCK " + 
            base_name(effects[i+1][j+1][SP_OBJECT]), 
            1, 
            (effects[i+1][j+1][SP_OBJECT])->query_lockout_time() );

          // tell_object(find_living("folken"), "[do_active_effects] aprendizaje de la dote: "+effects[i+1][0]+"\n");
        
          // Si es una dote (efecto de tipo "effect"), mejoramos el aprendizaje
          // ya que la hemos ejecutado correctamente
          this_object()->update_feat_used_times(effects[i+1][0]);
          
          remove_active_effect(effects[i+1][j]);
        }
      }
    }

  return 0;
} /* do_active_effects() */

int do_call_out_effect(mixed *params) 
{
  int res, aux_xp;

  aux_xp = 0;
  res = call_other(params[0][SP_OBJECT],
    params[0][SP_FUNC],
    this_object(),
    //       params[1],
    params[0][SP_PARAM],
    params[0][SP_NO_RNDS]);

  // Tipos de xp, neverbot 07/04
  if ((res > 0) && (params[2] == EFFECT_TYPE_SPELL))
  {
    aux_xp = res * (this_object()->query_xp_types()[MAGIC_XP]) / 100;

    this_object()->adjust_xp(res);
  }

  // Devolvemos el resultado. Si algo ha ido mal tendremos un -1, y debemos
  // terminar el hechizo (si continuamos acabaremos gastando los gps)
  return res;
      
} /* do_effect() */

mixed * stats()
{
  return ({ });
}
