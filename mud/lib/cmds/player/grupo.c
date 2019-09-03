/*
 * Comando grupo - por Yvendur@Iluminado Enero 00 -
 * Retocado por Tormentor@Iluminado - 6 Febrero 2003.
 * El canal del grupo lo he trasladao al /global/communicate.c
 * Reforma completa del comando, optimizando codigo. Radkar@Iluminado, Enero 2004
 * 
 * Actualizado para Ciudad Capital, Folken@CcMud, Abril 2009
 *   - Traducido codigo de castellano a ingles
 *   - Separada la opcion lista en lista (ver miembros) y estado (ver salud de los miembros)
 *   - Permitida opcion de invitar a un player que no este en tu room
 */

#include <cmd.h>
#include <adventurer.h>
#include <room.h>
#include <chat.h>
#include <consents.h>

inherit CMD_BASE;

// Comandos disponibles:
string * leader_actions = ({ "nombre", "expulsar", "atacar", "mover", "lider", "líder",
                             "invitar", "finalizar" });
string * group_actions =  ({ "canal", "despedirse", "lista", "estado", "canal" });
string * general_actions = ({ "unirse", "crear" });
string * out_actions = ({ "ayuda" });

string * actions = leader_actions + group_actions + general_actions + out_actions;

protected int cmd (string str, object me, string verb)
{
  int i, cont = 0, contno = 0;
  object * obs, ob, sha;
  string group_name = "";
  string parameter, strtmp, dir;

  // int j;
  // string * exito, comando, nombres_grupo, direcs;
  // object * atras, * foll, * foll2, he, tipo;
  
  if (!me) 
      me = this_player();

  if (!str || str == "") 
  {
     if (me->query_adventurer())
     {
         tell_object(me, "Perteneces al grupo '"+me->query_adventurer_group_name()+"'.\n");
         tell_object(me, "Utiliza 'grupo ayuda' para más información.\n");
         return 1;
     }
        
     notify_fail("Uso: grupo <comando>\nUtiliza 'grupo ayuda' para más información.\n");
     return 0;
  }
  
  // Jugadores con menos de 12 horas no tienen permitido hacer grupos, Folken
  if (-me->query_time_on() < 43200)
  {
      notify_fail("No puedes participar en grupos hasta que tu personaje tenga al menos doce horas de juego.\n");
      return 0;
  }
  
  strtmp = str;
  sscanf(strtmp, "%s %s", str, parameter);
  
  // Opcion no contemplada en el comando
  if (member_array(str, actions) == -1)
  {
     notify_fail("Opción incorrecta.\nUtiliza 'grupo ayuda' para más información.\n");
     return 0;
  }

  // Intentas ejecutar una orden sin estar en el grupo.
  if (member_array(str, group_actions + leader_actions) != -1 && 
     !me->query_adventurer())
  {
     notify_fail("Para hacer eso debes pertenecer a un grupo.\n");
     return 0;
  }

  // Intentas ejecutar una orden de lider sin ser el lider.
  if (member_array(str, leader_actions) != -1 && 
     (me->query_adventurer_leader() != me))
  {
     notify_fail("Sólo el líder del grupo puede hacer eso.\n");
     return 0;
  }

  // Intentas ejecutar una orden de "sin grupo" estando en el
  if (member_array(str, general_actions) != -1 && 
      me->query_adventurer())
  {
     notify_fail("No puedes hacer eso si ya perteneces a un grupo.\n");
     return 0;
  }

  if (str == "ayuda")
  {
      string help = 
          "En un grupo varios jugadores se reunen para afrontar objetivos en común. "
          "Cada jugador puede crear su propio grupo una vez tenga al menos doce horas de juego, y " 
          "puede invitar a más jugadores a que se unan a él. Puedes utilizar el comando "
          "grupo seguido de los siguientes parámetros:\n\n"
          " - crear <nombre>     : Para crear un grupo.\n"
          " - nombre             : Cambiar el nombre del grupo.\n"
          " - finalizar          : Para dar por terminado un grupo.\n"
          " - expulsar <jugador> : Para sacar a un jugador.\n"
          " - despedirse         : Para abandonarlo voluntariamente.\n"
          " - invitar <jugador>  : Invitar a otro jugador a unirse a ti.\n"
          " - unirse <jugador>   : Para unirse al grupo de <jugador>.\n"
          /*
          " - mover <dir>        : Para mover el grupo en alguna dirección.\n"
          " - sigilar <dir>      : Para hacer que el grupo sigile en una dirección.\n"
          " - atacar <objetivo>  : Coordinar el ataque a un mismo objetivo.\n" 
          */
          " - líder <miembro>    : Para cambiar de líder del grupo.\n"
          " - lista              : Muestra la lista de miembros del grupo.\n"
          " - estado             : Muestra la salud de todos los miembros.\n"
          " - canal <mensaje>    : Para enviar un mensaje por el canal privado del grupo.\n"
          "                        También puedes usar: 'aventurero <mensaje>'\n\n";
            
      help = sprintf("%-=*s", me->query_cols(), help);
      
      tell_object(me, help);
      
      return 1;
  }
  
  if (str == "canal")
  {
      CHAT_HANDLER->do_channel(me->query_adventurer_channel(), parameter, me->query_cap_name());
      return 1;
  }

  // Creacion de un grupo
  if (str == "crear")
  {
     if (parameter != "")
     {
        if (strlen(parameter) > GROUP_NAME_LENGTH)
        {
            tell_object(me, "Aviso: nombre de grupo demasiado largo (ha sido acortado).\n");
            parameter = parameter[0..GROUP_NAME_LENGTH-1];
        }
     }
     if (parameter)
     { 
         group_name = parameter + "%^RESET%^";
         tell_player(me, "Ok. Creas el grupo "+parameter+".\n");
         tell_room(environment(me),"\n" + me->query_cap_name() + " crea el grupo " + group_name + ".\n", me);
     }
     else
     { 
         group_name = "Grupo de " + me->query_cap_name();
         tell_player(me, "Ok. Creas un grupo contigo como líder.\n");
         tell_room(environment(me), "\n" + me->query_cap_name() + " crea un grupo con el como líder.\n", me);
     }     
     
     sha = clone_object(ADVENTURER_LEADER_SH);
     sha->setup_shadow(me, group_name);
     
     CHAT_HANDLER->open_channel(me->query_adventurer_channel(), me);
     
     return 1;
  }

  // Finalizacion de un grupo
  if (str == "finalizar")
  {
     tell_player(me, "Ok. Das por finalizado tu grupo.\n");

     obs = me->query_adventurer_list();
     
     tell_room(environment(me), me->query_cap_name() + " da por finalizado su grupo.\n", obs);

     for (i = 0; i < sizeof(obs); i++)
     {
         if (obs[i] != me) 
             tell_object(obs[i], me->query_cap_name()+" da por finalizado el grupo.\n");

         CHAT_HANDLER->close_channel(obs[i]->query_adventurer_channel(),obs[i]);
         obs[i]->dest_adventurer_shadow();
     }
     return 1;
  }

  // Despedirse de un grupo
  if (str == "despedirse")
  {
     if (me == me->query_adventurer_leader())
     {
        tell_object(me, "Eres el líder del grupo, no puedes despedirte sin finalizarlo o ceder antes el liderazgo.\n");
        return 1;
     }
     
     tell_player(me, "Te despides del grupo.\n");
     
     obs = (me->query_adventurer_leader())->query_adventurer_list();
     
     tell_room(environment(me), me->query_cap_name() + " se despide de su grupo.\n", obs);

     obs -= ({ me });
     // obs -= ({ me->query_adventurer_leader() });

     for (i = 0; i < sizeof(obs); i++)
          tell_player(obs[i], me->query_cap_name() + " se despide del grupo.\n");

     // tell_object(me->query_adventurer_leader(), me->query_cap_name()+" se despide de tu grupo.\n");          

     CHAT_HANDLER->close_channel(me->query_adventurer_channel(),me);
     (me->query_adventurer_leader())->remove_adventurer(me);
     me->dest_adventurer_shadow();
     return 1;
  }
  
  // Ver los miembros del grupo
  if (str == "lista") 
  {
     string ret = "\nMiembros del grupo '"+me->query_adventurer_group_name()+"':\n";

     obs = (me->query_adventurer_leader())->query_adventurer_list();
     obs = sort_array(obs, "compare_obs", this_object());
     
     for (i = 0; i < sizeof(obs); i++)
     {
         ret += "\t" + obs[i]->query_cap_name();
         if (interactive(obs[i]))
         {
            if (obs[i]->query_gtitle()) 
                ret += " "+obs[i]->query_gtitle();
            else if(obs[i]->query_gender()==1) 
                ret += " el aventurero";
            else 
                ret += " la aventurera";
                                    
         }   
         else 
             ret += " (%^BOLD%^No jugador%^RESET%^)";

         ret += ".\n";
     }    

     ret +="\n";

     tell_object(me, ret);
     return 1;
  }

  // Ver la salud de los miembros del grupo
  if (str == "estado") 
  {
     string ret = "\nMiembros del grupo '"+me->query_adventurer_group_name()+"':\n";

     obs = (me->query_adventurer_leader())->query_adventurer_list();
     obs = sort_array(obs, "compare_obs", this_object());
     
     for (i = 0; i < sizeof(obs); i++)
     {
         ret += "\t" + obs[i]->query_cap_name();
         if (!interactive(obs[i]))
             ret += " (%^BOLD%^No jugador%^RESET%^)";

         if (!obs[i]->query_dead()) 
             ret += ", "+obs[i]->health_string();
         ret += ".\n";
     }    

     ret +="\n";

     tell_object(me, ret);
     return 1;
  }

  // Cambiarle el nombre al grupo
  if (str == "nombre")
  {
      if (!parameter)
      {
         notify_fail("Debes especificar un nombre de grupo.\n");
         return 0;
      }
            
      if (strlen(parameter) > GROUP_NAME_LENGTH)
      {
          notify_fail("Nombre de grupo demasiado largo.\n");  
          return 0;  
      }

      obs = me->query_adventurer_list();
      group_name = parameter+"%^RESET%^";
      
      tell_player(me,"Cambias el nombre del grupo a '"+group_name+"'.\n");

      for (i = 0; i < sizeof(obs); i++)
      {
         if (obs[i] != me) 
             tell_object(obs[i],me->query_cap_name()+" cambia el nombre del grupo a '"+group_name+"'.");     

          CHAT_HANDLER->close_channel(obs[i]->query_adventurer_channel(), obs[i]);
          obs[i]->set_adventurer_group_name(group_name);
          CHAT_HANDLER->open_channel(obs[i]->query_adventurer_channel(), obs[i]);
     }
     tell_room(environment(me), me->query_cap_name()+" cambia el nombre de su grupo a '"+group_name+"'.\n",obs);
     return 1;
  }
 
  // Comando invitar
  if (str == "invitar")
  {
    // Numero max de gente en el grupo (idea de Shaka&Shura). Radkar, Noviembre 2004
    if (sizeof(me->query_adventurer_list()) >= MAX_ADVENTURERS)
    {
        notify_fail("El grupo ya está completo.\n");
        return 0;     
    }      

    if (parameter == "all" || parameter == "todos") 
       obs = all_inventory(environment(me));
    else 
    {
       // obs = find_match(parameter, environment(me));
       ob = find_living(me->expand_nickname(parameter));
       if (ob && interactive(ob))
           obs = ({ ob, });
       else
           obs = ({ });
    }

    obs -= ({ 0 });

    for (i = 0, cont = 0, contno = 0; i < sizeof(obs); i++)
    {
      if (!living(obs[i])) 
        continue;
      if (obs[i] == me) 
        continue;
      if (obs[i]->query_hidden()) 
        continue;
      if (!interactive(obs[i]) )
        continue; 

      if (obs[i]->query_adventurer())
      { 
        contno++;
        tell_object(me, obs[i]->query_cap_name()+" ya pertenece a un grupo.\n"); 
        continue; 
      }

      if (obs[i]->query_timed_property(INVITED_PROP))
      { 
        contno++;
        tell_object(me, obs[i]->query_short()+" ya ha sido invitado a un grupo.\n"); 
        continue; 
      }

      if (obs[i]->query_consent(GROUP_CONSENT) == 0)
      {
        tell_object(me, obs[i]->query_cap_name()+" no quiere ser invitado a grupos.\n"); 
        contno++;
        continue;
      }

      tell_object(me, "Ok, "+obs[i]->query_cap_name()+" ha sido invitado a tu grupo.\n");

      obs[i]->add_timed_property(INVITED_PROP, me->query_name(), INVITED_PROP_DURATION);

      tell_player(obs[i], "Has sido invitado a formar parte del grupo de "+me->query_cap_name()+".\n");
      tell_object(obs[i], "Utiliza 'grupo unirse "+me->query_name()+"' para aceptar la invitación.\n");

      cont++;
    }

    if (!cont && !contno)
    {
      notify_fail("No hay nadie con ese nombre aquí.\n");
      return 0;
    }
    return 1;   
  }

  // Comando echar, Radkar Febrero 2003
  if (str == "expulsar")
  {
     obs = me->query_adventurer_list();
     
     for (i = 0; i < sizeof(obs); i++)
     {
         if (obs[i]->query_name() == me->expand_nickname(parameter))
         {
             if (obs[i] == me) 
             {
      	         notify_fail("No puedes expulsarte a ti mismo. Si quieres finalizar el grupo, utiliza 'grupo finalizar'.\n");
             	 return 0;
             }
             ob = obs[i];
             tell_player(me, "Expulsas a "+ob->query_cap_name()+" del grupo.\n");
             tell_player(ob, me->query_cap_name()+" te expulsa del grupo.\n");
             tell_room(environment(me),me->query_cap_name()+" expulsa a "+obs[i]->query_cap_name()+" de su grupo.\n",obs);
             
             me->remove_adventurer(ob); 
             obs -= ({ ob });
             obs -= ({ me });

             for (i = 0; i < sizeof(obs); i++)
                 tell_object(obs[i], me->query_cap_name()+" expulsa del grupo a "+ob->query_cap_name()+".\n");

             CHAT_HANDLER->close_channel(ob->query_adventurer_channel(), ob);
             ob->dest_adventurer_shadow();
             return 1;
         }
     }
     notify_fail("No hay nadie con ese nombre para poder expulsarlo del grupo.\n");
     return 0;
  }

  // Comando lider, Radkar Febrero 2003
  if ((str == "lider") || (str == "líder"))
  {
     if (!parameter)
     {
         notify_fail("Debes especificar quién quieres que sea el nuevo líder.\n");
         return 0;
     }
            
     if (me->expand_nickname(parameter) == me->query_name())
     {
         notify_fail("Ya eres el líder del grupo.\n");
         return 0;
     }
     
     obs = me->query_adventurer_list();
     obs -= ({ me });
     
     for (i = 0; i < sizeof(obs); i++)
         if (obs[i]->query_name() == me->expand_nickname(parameter)) 
            ob = obs[i];
         
     if (!ob)
     {
        notify_fail("No hay nadie en el grupo llamado "+me->expand_nickname(parameter)+".\n");
        return 0;
     }    
     
     if (!interactive(ob))
     {
         notify_fail("Sólo puedes cederle el liderazgo a jugadores.\n");
         return 0;
     }

     dir = me->query_adventurer_channel();
     obs = me->query_adventurer_list();
     
     tell_player(me, "Cedes el liderazgo del grupo a "+ob->query_cap_name()+".\n");
     tell_player(ob, me->query_cap_name()+" te cede el liderazgo del grupo.\n");
     tell_room(environment(me),me->query_cap_name()+" cede el liderazgo de su grupo a "+ob->query_cap_name()+".\n", obs);     

     obs -= ({ ob });
     obs -= ({ me });
     
     CHAT_HANDLER->close_channel(ob->query_adventurer_channel(), ob);
     CHAT_HANDLER->close_channel(me->query_adventurer_channel(), me);
     
     ob->dest_adventurer_shadow();
     me->dest_adventurer_shadow();
     
     sha = clone_object(ADVENTURER_SH);
     sha->setup_shadow(me, ob, ob->query_adventurer_group_name());
     CHAT_HANDLER->open_channel(me->query_adventurer_channel(), me);

     sha = clone_object(ADVENTURER_LEADER_SH);
     sha->setup_shadow(ob, "Grupo de "+ob->query_cap_name());
     CHAT_HANDLER->open_channel(ob->query_adventurer_channel(), ob);
     
     ob->add_adventurer(me);

     for (i = 0; i < sizeof(obs); i++)
     {
         tell_player(obs[i], me->query_cap_name()+" le cede el liderazgo del grupo a "+ob->query_cap_name()+".\n");
         CHAT_HANDLER->close_channel(obs[i]->query_adventurer_channel(), obs[i]);         
         obs[i]->set_adventurer_leader(ob);         
         obs[i]->set_adventurer_group_name("Grupo de "+ob->query_cap_name());
         CHAT_HANDLER->open_channel(obs[i]->query_adventurer_channel(), obs[i]);
         ob->add_adventurer(obs[i]);
     }

     // sacar_npc(me);
     return 1;
  }

  // Comando unirse
  if (str == "unirse")
  {    
      
      if (!me->query_timed_property(INVITED_PROP)) 
      {
   	      notify_fail("No has sido invitado a formar parte de ningún grupo.\n");
          return 0;
      }
      
      ob = find_living(me->query_timed_property(INVITED_PROP));
       
      if (!ob)
      {
	      notify_fail("Imposible localizar al líder del grupo.\n");
          return 0;
      }

      if (!interactive(ob))
      {
          notify_fail(ob->query_cap_name()+" no es un jugador.\n");
          return 0;
      }

      if (!ob->query_adventurer())
      {
          notify_fail(ob->query_cap_name() + " ya no pertenece a ningún grupo.\n");
          return 0;
      }
      
      if (ob->query_adventurer_leader() != ob)
      {
          notify_fail(ob->query_cap_name()+" ya no es el líder del grupo.\n");
          return 0;
      }

      // Numero max de gente en el grupo (idea de Shaka&Shura). Radkar, Noviembre 2004
      if (sizeof(ob->query_adventurer_list()) >= MAX_ADVENTURERS)
      {
          notify_fail("El grupo ya está completo.\n");
          return 0;     
      }
      
      if (me->query_timed_property(INVITED_PROP) == ob->query_name())
      {
          tell_player(me, "Ok, te unes al grupo de "+ob->query_cap_name()+".\n");
          me->remove_timed_property(INVITED_PROP);
          
          sha = clone_object(ADVENTURER_SH);
          sha->setup_shadow(me, ob, ob->query_adventurer_group_name());
          CHAT_HANDLER->open_channel(me->query_adventurer_channel(), me);
          ob->add_adventurer(me);

          tell_player(ob, me->query_cap_name()+" se une a tu grupo.\n");
         
          obs = ob->query_adventurer_list();
          tell_room(environment(me), me->query_cap_name()+" se une al grupo de "+ob->query_cap_name()+".\n", obs);

          obs -= ({ me });
          obs -= ({ ob });

          for (i = 0; i < sizeof(obs); i++)
              tell_object(obs[i], me->query_cap_name()+" se une al grupo.\n");
          return 1;
      }
  }

/*
  // Comando mover grupo
  if (str == "mover") 
  {
  
     if (environment(me) && environment(me)->query_property("no_grupo")) {
        notify_fail("No puedes mover a tu grupo dadas las circunstancias.\n");
        return 0;
     }     
     if (environment(me)->query_confusion()){
        notify_fail("No puedes mover el grupo en este tipo de habitacion.\n");
        return 0;
     }
    
     cont = 0;
     obs = me->query_adventurer_list();
     for (i=0;i<sizeof(obs);i++)
         if (environment(me) == environment(obs[i])) cont++;
          
     if (cont<=1){
        notify_fail("No hay ningun miembro de tu grupo aqui que puedas mover.\n");
        return 0;
     }
                          
     direcs = environment(me)->query_dest_dir();
     if( (i=member_array(parameter,dirs_legales))!=-1) {
         if(i<1) i = 0;
         if(i%2==0) dir = dirs_legales[i+1];
         else       dir = parameter;
                        
     }    
     if (member_array(dir,direcs)==-1){ 
        notify_fail("No puedes mover el grupo a "+dir+" ya que no es una salida valida.\n");
        return 0;
     }
     if (member_array(dir,environment(me)->query_exits_modified())!=-1) {
        notify_fail("No puedes mover a tu grupo en esta direccion.\n");
        return 0;
     }     

     he = environment(me);     
     atras=({ });
     
     // Si el lider no se puede mover, no se mueve nadie.
     if (!environment(me)->do_exit_command("",({dir,"none"}),me,0)){     
        notify_fail("No puedes dirigir el grupo ahi porque ni siquiera tu puedes moverte.\n");
        return 0;
     }
     
     // Ok el lider se ha movido
     foll = ({ });
     exito = ({ me });
     obs -= ({ me });
     foll += me->query_followers();
     foreach(tipo in obs){
         if (environment(tipo) != he) continue;
         if (!environment(tipo)->do_exit_command("",({dir,"none"}),tipo,0)){
            atras += ({ tipo });
         }    
         else { 
              foll += tipo->query_followers(); 
              exito += ({ tipo });
              if (tipo->query_hidden()) tipo->dest_hide_shadow();
         }      
     }
     
     foll -= ({ 0 });
     foll -= exito;
     foll -= atras;
     
     
     foll2 = ({ });
     foreach (tipo in foll){
         if (environment(tipo) != he){ continue; } // No estan en la room
         if (!environment(tipo)->do_exit_command("",({dir,"none"}),tipo,0)) { tell_room(environment(me),"No consigues seguir al grupo "+me->query_adventurer_group_name()+".\n"); continue; }         
	     else{ 
             if (tipo->query_hidden()) tipo->dest_hide_shadow();
             foll2 += ({ tipo });
         }    
     }                                      

     
     nombres_grupo=query_multiple_short(exito);
     exito -= ({ me });
     tell_object(me,"Mueves ("+me->query_adventurer_group_name()+") ("+nombres_grupo+") a "+dir+".\n");
     for (i=0;i<sizeof(exito);i++)
         tell_object(exito [i],me->query_cap_name()+" desplaza ("+me->query_adventurer_group_name()+") ("+nombres_grupo+") a "+dir+".\n"); 
        
    for(i=0;i<sizeof(atras);i++)
        tell_object(atras[i],me->query_cap_name()+" y el grupo "+me->query_adventurer_group_name()+" te dejan atras.\n");
    if (sizeof(atras)) tell_object(me,"Dejas atras a "+query_multiple_short(atras)+".\n");

    exito += ({ me });
    if (nombres_grupo != "") { 
       tell_room(he,"("+me->query_adventurer_group_name()+") "+nombres_grupo+" se desplazan al "+dir+".\n",exito);
       tell_room(environment(me),"("+me->query_adventurer_group_name()+") "+nombres_grupo+" llegan del "+load_object(ROOM_HAND)->query_opposite(dir)+".\n",exito);  
     }

     // Cambio esto, que no salga el mensaje separado sino agrupado. Radkar, Octubre 2004
     for (i=0;i<sizeof(foll2);i++){
         tell_object(foll2[i],"Sigues a ("+me->query_adventurer_group_name()+") ("+nombres_grupo+") al "+dir+".\n");
     }
     if (sizeof(foll2)){
        tell_room(he,query_multiple_short(foll2)+" persigue ("+me->query_adventurer_group_name()+") "+nombres_grupo+" al "+dir+".\n",foll2);
        tell_room(environment(foll2[0]),query_multiple_short(foll2)+" persigue a ("+me->query_adventurer_group_name()+") "+nombres_grupo+".\n",foll2);           
     }
     return 1;
  } 

  // Comando matar
  if (str == "atacar") {
     parameter = me->expand_nickname(parameter);
     if (parameter == "all" || parameter =="0" || parameter == "todo" ) {
       	notify_fail("No esta permitido esto.\n");
        return 0;
     }
     if (me->query_level() <= 5 && !me->query_guild_ob()) {// Ares
        notify_fail("Tal vez mas adelante...\n");
        return 0;
     }
     if (me->query_dead()) { // Ares
        notify_fail("De repente notas que tu aspecto etereo interacciona poco "
                            "con el mundo material. Tus companyeros de grupo parecen ignorarte.\n");
        return 0;
     }
     if (environment(me)->query_property("nokill")) {// Ares
        notify_fail("No puedes llevar a tu grupo al combate aqui y ahora.\n");
        return 0;
     }

     obs = me->query_adventurer_list();
     foll = ({ });
     group_name = me->query_adventurer_group_name();
     for (i=0;i<sizeof(obs);i++)
        if (environment(me) == environment(obs[i])) foll += ({ obs[i] });
     obs = find_match(parameter,environment(me));
     exito = ({});
          
          if(sizeof(obs)) {
              for(i=0;i<sizeof(obs);i++) { 
                 if(member_array(obs[i],foll) != -1) {
                    notify_fail("No puedes hacer este ataque contra un miembro de tu grupo !\n");  
                    return 0;  
                  } 
                  if(obs[i]->query_property("pacify_on")) {// Ares
                    notify_fail(obs[i]->query_short()+" parece demasiado inofensivo como para "
                               "molestarse en atacar.\n");
                    return 0;
                  }
                  if(!obs[i]->query_alive()) { //Ares
                     notify_fail("Empezais a golpear a "+(string)obs[i]->query_short()+", "
                               "pero no parece ni enterarse.\n");
                     return 0;
                  }
                  if (obs[i]->query_hidden() || obs[i]->query_hide_shadow()) continue;
                  exito += ({obs[i]});
               }
               if (sizeof(exito)){
               for(j=0;j<sizeof(foll);j++){
                 if (foll[j] == me) 
                    tell_object(foll[j],"Conduces a ("+group_name+") a la lucha contra "+query_multiple_short(exito)+".\n");
                 else   
                    tell_object(foll[j],me->query_cap_name()+" conduce a ("+group_name+") a la lucha contra "+query_multiple_short(exito)+".\n");                 
                 for (i=0;i<sizeof(exito);i++) foll[j]->attack_ob(exito[i]);                    
               }  
               }
               else{
                   notify_fail("No hay nadie aqui con ese nombre para que tu grupo ataque.\n");  
                   return 0;  
               }
          } 
          else { 
            notify_fail("No hay nadie aqui con ese nombre para que tu grupo ataque.\n");  
            return 0;  
          } 
        return 1;
  }
  // Comando sigilar
  if (str == "sigilar") {

     if (!me->query_property(HABILIDAD_SIGILAR)){
        notify_fail("No tienes la habilidad de poder dirigir a tu grupo de forma sigilosa.\n");
        return 0;
     }     
     if (environment(me) && environment(me)->query_property("no_grupo")) {
        notify_fail("No puedes sigilar con tu grupo dadas las circunstancias.\n");
        return 0;
     }     
     if (environment(me)->query_confusion()){
        notify_fail("No puedes sigilar con el grupo en este tipo de habitacion.\n");
        return 0;
     }
    
    obs = me->query_adventurer_list();
    
    cont = 0;
    foll = ({ });
    for (i=0;i<sizeof(obs);i++)
        if (environment(me) == environment(obs[i])){ cont++; foll += ({ obs[i] }); }
         
    if (cont<=1){
       notify_fail("No hay ningun miembro de tu grupo aqui que puedas sigilar con el.\n");
       return 0;
    }                  

    for (i=0;i<sizeof(foll);i++){
        if (!foll[i]->query_property(HABILIDAD_SIGILAR)){
           notify_fail(foll[i]->query_short()+" no tiene la habilidad de sigilar en grupo.\n");
           return 0;
        }
        if (member_array("sigilar",foll[i]->query_known_commands()) == -1){
           notify_fail("Algun miembro de tu grupo no sabe sigilar asi que no puedes mover tu grupo sigilosamente.\n");
           return 0;
        }
        if (sizeof(foll[i]->query_attacker_list())){
           notify_fail("No puedes mover el grupo sigilosamente cuando algunos de tus miembros estan luchando!\n");
           return 0;        
        }
    }
      
    he = environment(me);    
    SNEAK->sneak(parameter,me,1);
    
    if (environment(me)==he){ // El lider no se ha podido mover
       notify_fail("No consigues dirigir a tu grupo sigilosamente.\n");
       return 0;                         
    }
    
    foll -= ({ me });     
    exito = ({ });
    for (i=0;i<sizeof(foll);i++){
        if (!foll[i]->query_property("noguild")){
           SNEAK->sneak(parameter,foll[i],1);
           if (environment(foll[i]) == environment(me)) exito += ({ foll[i] });
        }
    }
     
    foll -= ({ 0 });
    foll -= exito;

    exito += ({ me });
    nombres_grupo=query_multiple_short(exito);
    exito -= ({ me });
    tell_object(me,"Mueves SIGILOSAMENTE ("+me->query_adventurer_group_name()+") a "+parameter+".\n");
    for (i=0;i<sizeof(exito);i++)
        tell_object(exito [i],me->query_cap_name()+" desplaza SIGILOSAMENTE ("+me->query_adventurer_group_name()+") a "+parameter+".\n"); 
        
    for (i=0;i<sizeof(foll);i++)
        tell_object(foll[i],me->query_cap_name()+" y el grupo "+me->query_adventurer_group_name()+" te dejan atras.\n");
    if (sizeof(foll)) tell_object(me,"Dejas atras a "+query_multiple_short(foll)+".\n");
    return 1;
  } 
    */
    return 0;
}   

int compare_obs(object ob1, object ob2)
{
    string s1 = ob1->query_name();
    string s2 = ob2->query_name();
    if(!s1) 
        return 1;
    if(!s2) 
        return -1;
    return strcmp(s1,s2);
}
