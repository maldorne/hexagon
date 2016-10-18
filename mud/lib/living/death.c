/* 
 * Ok, this time it's this code I will walk through...
 * Baldrick, april '94
 * Death.c removed from combat.c to make it cleaner.
 * Retocado para Cc, neverbot 4/2003
 *
 */

#include <living/death.h>
#include <common/properties.h>
#include <basic/alignment.h>
#include <user/xp.h>
#include <item/money.h>

// Traido desde /global/living/living.c, neverbot 4/03
int dead;

// Funcion create añadida
void create()
{
  dead = 0;
}

void set_dead(int value)
{
  dead = value;
  return; 
}

int query_dead()
{
  return dead;
}

/* For compatibility: */
int query_alive()
{
  return !dead;
}

int do_kill(object victim)
{
  if (this_object()->query_race_ob())
    catch(this_object()->query_race_ob()->on_kill(this_object(), victim));
  if (this_object()->query_guild_ob())
    catch(this_object()->query_guild_ob()->on_kill(this_object(), victim));
  if (this_object()->query_class_ob())
    catch(this_object()->query_class_ob()->on_kill(this_object(), victim));
  if (this_object()->query_deity_ob())
    catch((this_object()->query_deity_ob())->on_kill(this_object(), victim));   
  if (this_object()->query_group_ob())
    catch((this_object()->query_group_ob())->on_kill(this_object(), victim));
  if (this_object()->query_race_group_ob())
    catch((this_object()->query_race_group_ob())->on_kill(this_object(), victim));
  if (this_object()->query_job_ob())
    catch((this_object()->query_job_ob())->on_kill(this_object(), victim));
  if (this_object()->query_city_ob())
    catch((this_object()->query_city_ob())->on_kill(this_object(), victim));

  return 1;
}

// prototypes:
static void actual_death(object killer);

int do_death(object killed_by) 
{
  int i, dead_xp, tot;
  int aln_at, aln_df;
  float xp_adj;
  mapping damage_done;
  int att_level;
  int attackers, attacker_xp, aux_attacker_xp;
  object *attacker_list, *call_outed;

  /* Added to maybe fix an annoying bug..
   * The dead on login bug..
   * Baldrick, April '95.
   */
  if (this_object()->query_property(LOADING_PROP))
    return 0;

  // Death statistics - Radix April 1996
  catch(xp_adj = DEATH_HANDLER->update_statistics(this_object(), killed_by));
  
  if (!xp_adj) 
    xp_adj = 1.0;

  // Informamos al atacante de que ha habido una victima
  if (killed_by)
    killed_by->do_kill(this_object());

  if (this_object()->query_race_ob())
    catch(this_object()->query_race_ob()->on_death(this_object(), killed_by));
  if (this_object()->query_guild_ob())
    catch(this_object()->query_guild_ob()->on_death(this_object(), killed_by));
  if (this_object()->query_class_ob())
    catch(this_object()->query_class_ob()->on_death(this_object(), killed_by));
  if (this_object()->query_deity_ob())
    catch((this_object()->query_deity_ob())->on_death(this_object(), killed_by));   
  if (this_object()->query_group_ob())
    catch((this_object()->query_group_ob())->on_death(this_object(), killed_by));
  if (this_object()->query_race_group_ob())
    catch((this_object()->query_race_group_ob())->on_death(this_object(), killed_by));
  if (this_object()->query_job_ob())
    catch((this_object()->query_job_ob())->on_death(this_object(), killed_by));
  if (this_object()->query_city_ob())
    catch((this_object()->query_city_ob())->on_death(this_object(), killed_by));

  // Añadido por neverbot 3/2002, para el sistema de guerras entre ciudades
  if (killed_by)
    if ( (killed_by->query_player() && this_object()->query_player()) &&
       (killed_by->query_citizen() && this_object()->query_citizen()) && 
       (killed_by->query_citizenship() != this_object()->query_citizenship()) )
    catch(WAR_HANDLER->war_death(this_object(), killed_by));
   
  attacker_list = this_object()->query_attacker_list();
  call_outed = this_object()->query_call_outed();

  /* Added this because 0's in the attacker/call outed list sometimes
   * pop up and screw things up royally.  Monsters not dying, that sort of thing.
   * --WF, apr 95
   */
  attacker_list -= ({ 0 });
  call_outed -= ({ 0 });

  att_level = (int)this_object()->query_level();
  damage_done = (mapping)this_object()->query_damage_done();

  if (!damage_done)
    damage_done = ([ ]);
  
  for (i = 0; i < sizeof(attacker_list); i++)
  {
    attacker_list[i]->stop_fight(this_object());
    tot += damage_done[attacker_list[i]];
    // Las monturas no quitan xp, neverbot 05/2009
    if (!attacker_list[i]->query_ride())
        attackers++;
  }
  
  for (i=0;i<sizeof(call_outed);i++)
    call_outed[i]->stop_fight(this_object());

  /* ??? where is this from ???  
   * It is the victims XP, so it can be added to the killer..
   * Smart, and nice.. 
   */
  // Cambiado, ahora no se pierde toda la xp, sino un tercio
  dead_xp = (int)this_object()->query_xp() / 3;

  if (interactive(this_object()) && dead_xp)
    log_file("xp", ctime(time())+": "+
            this_object()->query_cap_name()+" "+
             "has lost '"+dead_xp+"' xp dying.\n");

  if (!tot) 
    tot = 1;

  /* this gives the total for each attacker.. */
  if (dead_xp != 0)
    attacker_xp = dead_xp / 20;
  if (attackers)
    // xp_adj is the multiplier to kill_xp returned by the 
    // death statistic keeper... Anirudh
    attacker_xp += (int)((float)this_object()->query_kill_xp() * xp_adj/(float)attackers);

  /* Que hace esto aqui?? El pk deberia dar xp... neverbot 2/04
  if (interactive(this_object())) 
    attacker_xp = 0;
  */

  /* Deliver the XP.. */
  for (i = 0; i < sizeof(attacker_list); i++) 
  {
    if (!attacker_list[i]) 
      continue;
    
    if (function_exists("adjust_xp", attacker_list[i]))
    {
      // Matar npcs/save/players de menor nivel no da xp, neverbot 07/04
      // Cambiado por un margen de hasta 2 niveles de diferencia, 03/09
      if (this_object()->query_level() + 2 < attacker_list[i]->query_level() )
      {
        aux_attacker_xp = 0;
      }
      else
      {
        // Tipos de xp, neverbot 07/04
        aux_attacker_xp = attacker_xp * attacker_list[i]->query_xp_types()[KILL_XP] / 100;

        // tell_object( attacker_list[i], "[Debug kill_xp]: attacker_xp: " + attacker_xp + 
        //                " aux_attacker_xp: " + aux_attacker_xp + "\n");

        attacker_list[i]->adjust_xp(aux_attacker_xp);
      }
    
      /* -----------------------------------------------
       * PENDIENTE: Alineamiento, mejorar esto con el tiempo
       * neverbot, nuevo sistema de ajuste de alineamiento
       * Aqui habra que hacer todas las comprobaciones 
       *  cuando alguien muere, ajustes de alineamiento 
       *  de los atacantes (real o externo si hay o no
       *  testigos, etc).
       ----------------------------------------------- */
    
      aln_at = attacker_list[i]->query_ext_align();
      aln_df = this_object()->query_ext_align();

      if (ALIGN_TABLE->query_enemies(aln_at, aln_df))
      {
        // A los malos les hacemos un 10% del alin de su 
        //  victima mas malos, a los buenos solo el 5% de
        //  su victima mas buenos (un bueno matando de por
        //  si ya es ilogico).

        // Atacante bueno, victima malo
        if (aln_at > 100)
          attacker_list[i]->adjust_real_align(-aln_df/20);
        else // Atacante malo, victima bueno
          attacker_list[i]->adjust_real_align(-aln_df/10);
       
      }
      // No son enemigos, hay que hacer algo malo :(
      else
      {
        // El atacante es bueno
        if (aln_at > 100)
        {
          attacker_list[i]->adjust_real_align(-100);
          // Poco a poco tambien baja el alineamiento externo
          if (aln_df > 100)
            attacker_list[i]->adjust_ext_align(-5);
        }
        else // El atacante es malo
          attacker_list[i]->adjust_real_align(50);
      }
    }
  }

  this_object()->set_dead(1);
  this_object()->set_wetness(0);

  if (this_object()->query_riding())
    this_object()->destruct_ride_shadow();
  
  /*
  if (OMIQ_HAND->omiq_in_progress() && interactive(this_object()))
    this_object()->add_property("OMIQ_KILLED",1);
  */

  if (killed_by) 
  {
    tell_room(environment(this_object()), "\n"+killed_by->query_cap_name()+
      " propina el golpe mortal a "+
      this_object()->query_cap_name()+".\n\n",({killed_by}));
    tell_player(killed_by,"Propinas el golpe mortal a "+
      this_object()->query_cap_name()+".");
  }
  else
    tell_room(environment(this_object()),this_object()->query_cap_name()+
        " ha muerto horriblemente.\n");

  if (environment())
    event(environment(), "death", killed_by, attacker_list, this_object());

  if (killed_by)
    actual_death(killed_by);
  else
    actual_death(this_object());
  return 0;
}

object make_corpse()
{
  object corpse;
  object money;
  mixed *usedstuff;
  string my_mess, room_mess;
  int i;

  my_mess = "";
  room_mess = "";

  corpse = clone_object(CORPSE_OB);
  corpse->set_owner(this_object()->query_name(), this_object());

  corpse->start_decay();

  // dk Until we make an informed decision here, I'm going to have corpses
  // unhold, unwear and unwield everything.  Otherwise, since some items have a
  // special effect on the room only if they're worn or held by a living
  // creature, creators would have to code each such item carefully to
  // distinguish the effects when worn or held by a corpse...we probly don't
  // want to do this, but even if we do, I don't want to deal with it right now.

  /* Un equip the player ? or corpse ? */
  usedstuff = (mixed *)this_object()->query_held_ob();

  if (sizeof(usedstuff)) 
  {
    my_mess = "Tu cuerpo pierde sus últimos alientos de vida";
    room_mess = (string)this_object()->query_cap_name() +
                " cae al suelo destrozad"+this_object()->query_vocal();
  } 

  for (i = 0; i < sizeof(usedstuff); i++)
  {
    /* Have to use this instead. */
    if (objectp(usedstuff[i]))
    {
      object ok;
      ok = usedstuff[i];
      this_object()->unhold_ob(ok);
      ok->move(environment(this_object()));
    } /* if (usedstuff.. */
  } /* for.. */

  /* Reset the array (to be sure ) */
  usedstuff = ({ });

  usedstuff = (mixed *)this_object()->query_worn_ob();

  if (sizeof(usedstuff))
    if (stringp(my_mess)) 
    {
      my_mess += ", y tus armas caen al suelo.";
      room_mess += ", y sus" + 
                   " armas caen de sus" +
                   " manos y chocan contra el suelo";
    } 
    else 
    {
      my_mess += "Tus pertenencias caen de tus manos";
      room_mess += "Las pertenencias de "+this_object()->query_cap_name() + 
                   " caen de sus manos";
    }

  for (i = 0; i < sizeof(usedstuff); i++)
  {
    if (objectp(usedstuff[i]))
    {
      this_object()->unwear_ob(usedstuff[i]);
      /*Shall they loose it or keep it in the corpse I */

      // usedstuff[i]->move(environment());
    } /* if (usedstuff.. */
  } /* for.. */

  /* Let the corpse loose everything */
  /* But they won't loose it, can put it back later tho.. */
  /*
  obs = all_inventory(this_object());
  for (i=0; i<sizeof(obs); i++)
    obs[i]->move(environment());
  */

  if (stringp(room_mess)) 
  {
    tell_room(environment(this_object()),room_mess+".\n", this_object());
    tell_player(this_object(), my_mess+".\n");
  }

  // Comprobacion añadida, para evitar que el objeto del dinero con
  // cero monedas se lleve al cuerpo (y al coger objetos del cuerpo
  // tengamos el molesto 'Coges  de cuerpo.'
  money = present(MONEY_NAME, this_object());
  if ((money) && (money->query_value() == 0))
  {
    money->dest_me();
  }

  // AS for now, the corpse contains the remaining stuff.
  // all_inventory(this_object())->move(corpse);
  {
    object * inv;
    inv = all_inventory(this_object());

    for (i = 0; i < sizeof(inv); i++)
      inv[i]->move(corpse);
  }

  corpse->move(environment());
  return corpse;
} /* Make corpse */

//? shouldnt need to call this... so make it static, just in case 

static void actual_death(object initiator)
{
  object ob, ob2;
  // Radix made this so it didn't HAVE to create a corpse, 
  // so creators could mask this in their NPCs
  // Feb. 28, 1996
  object corp;
  corp = make_corpse();

  // dw if second_life returns false... do stupid things... 

  if (!((int)this_object()->second_life(corp, initiator))) 
  {
    if (corp)
      corp->move(environment(this_object()));

    // dw dest the ones that stick around... 
    // This is slightly useless now, the stuff is in the corpse
    // Good for the stuff that for some reason couldn't be moved tho
    ob = first_inventory(this_object());
    while(ob) 
    {
      ob2 = next_inventory(ob);
      ob->dest_me();
      ob = ob2;
    }
    this_object()->dest_me();
  }
  
  if (!this_object()) 
    return;
  if (!this_object()->query_player()) 
    return;
  
  // if (!this_object()->query_coder()) 
  {
    object room;
    if (this_object()->query_real_align() < 0)
      room = load_object(LIMBO_ENTRANCE);
      // room = load_object(HELL_ENTRANCE); PENDIENTE: Cielo e infierno completos
    else
      room = load_object(LIMBO_ENTRANCE);
      // room = load_object(HEAVEN_ENTRANCE); PENDIENTE: Cielo e infierno completos

    if (!room)
    {
      tell_object(this_object(), "Por algún oscuro deseo de los poderes divinos, "+
        "tu alma debe continuar en este plano de existencia.\n");
      return;
    }
      
    tell_room(environment(this_object()), this_object()->query_cap_name()+" da un grito desgarrador "+
      "mientras su alma abandona este mundo.\n", this_object());
    tell_object(this_object(), "Tu espíritu se eleva sobre tu cuerpo y abandona este mundo.\n");
    
    this_object()->move(room);
    this_object()->do_look();

    if (this_object()->query_real_align() < 0)
      tell_room(room, this_object()->query_cap_name() + " llega al infierno para ser "+
        "castigado por sus pecados en vida.\n", this_object());
    else // PENDIENTE: hacer tambien el cielo :P
      tell_room(room, this_object()->query_cap_name() + " llega al infierno para ser "+
        "castigado por sus pecados en vida.\n", this_object());
  }
                           
} /* void actual death */

// Stats añadidos
mixed stats()
{
  return ({ ({ "Is Dead", dead, }), });
}
