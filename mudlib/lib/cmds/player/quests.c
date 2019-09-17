#include <standard.h>
#include <cmd.h>

#include <quests.h>
#include <consents.h>

inherit CMD_BASE;

void setup()
{
  position = 0;
}

protected int cmd(string str, object me, string verb) 
{
  string text;
  mapping quests;
  int i;
  object quest;
  int can_finish_any = 0;

  quests = me->query_quests();

  if (!str || (str == ""))
  {
    if (!sizeof(quests))
    {
      tell_object(me, "Aún no has aceptado ningún trabajo.\n");
      return 1;
    }

    text = "Has aceptado los siguientes trabajos:\n\n";

    for (i = 0; i < sizeof(quests); i+=2)
    {
      quest = load_object(quests[i]);

      if (!quest)
        continue;

      text += "\t[%^BOLD%^"+(i/2+1)+"%^RESET%^] "+quest->query_quest_title();
      
      switch(quest->query_quest_type())
      {
        case QUEST_TYPE_EXPLORE:
          text += " (exploración)";
          break;
        case QUEST_TYPE_BEAT:
          text += " (enfrentamiento)";
          break;
        case QUEST_TYPE_KILL:
        case QUEST_TYPE_GET:
          text += " (" + quests[i+1] + " de " + quest->query_how_many_to_achieve() + ")"; 
          break;
        default:
          break;
      }

      switch (quest->query_quest_category())
      {
        case QUEST_GUILD:
          {
            // text += " %^GREEN%^(gremio)%^RESET%^";
            object guild = load_object(quest->query_provider_path());
            if (guild)
              text += " (%^GREEN%^"+guild->query_short()+"%^RESET%^)";            
          }
          break;
        case QUEST_JOB:
          {
            // text += " %^GREEN%^(oficio)%^RESET%^";
            object job = load_object(quest->query_quest_job());
            if (job)
              text += " (%^GREEN%^"+job->query_short()+"%^RESET%^)";            
          }
          break;
        case QUEST_EPIC:
          text += " (%^RED%^épica%^RESET%^";
          break;
        case QUEST_HISTORY:
          text += " (%^RED%^historia%^RESET%^";
          break;
        default:
        case QUEST_BASIC:
          break;
      }

      // Si ya esta terminada
      if ( ((quest->query_quest_type() == QUEST_TYPE_GET) || 
            (quest->query_quest_type() == QUEST_TYPE_KILL) || 
            (quest->query_quest_type() == QUEST_TYPE_EXPLORE)) &&
          (quests[i+1] >= quest->query_how_many_to_achieve()) ) 
      {
        text += " (%^BOLD%^RED%^terminada%^RESET%^)"; 
      }
      
      if (me->query_coder())
        text += " (" + base_name(quest) + ")";

      text += "\n";

      // completed quest which do not need the original provider to be finished
      if (me->has_completed_quest(quests[i]))
      {
        if (!quest->query_provider_needed())
          can_finish_any = 1;
        else
        {
          object provider = load_object(quest->query_provider_path());
          if (provider && provider->query_book() && present(provider->query_name(), me))
            can_finish_any = 1;
        }
      }
    }
  
    text += "\nEscribe '%^BOLD%^"+me->query_verb()+" info <número>%^RESET%^' para ver más información, o\n"
            "        '%^BOLD%^"+me->query_verb()+" borrar <número>%^RESET%^' para abandonarla.\n"
            "        '%^BOLD%^"+me->query_verb()+" compartir <número> <quién>%^RESET%^' para compartirla.\n";

    if (me->query_coder())
    {
      text += "        '%^BOLD%^"+me->query_verb()+" reset%^RESET%^' para borrar tu registro "
          "de misiones realizadas (sólo programadores).\n";
    }

    if (can_finish_any)
    {
      text += "\n[%^BOLD%^YELLOW%^?%^RESET%^] Tienes "+me->query_verb()+" terminadas. Escribe\n"
          "        '%^BOLD%^"+me->query_verb()+" finalizar <número>%^RESET%^' para terminarlas.\n";
    }

    tell_object(me, text);

    me->set_trivial_action();
    return 1;
  }
  else
  {
    // the input is 'quests <something>'
    string what = "";
    int which = 0;  
    string partner = ""; // quests compatir <num> <objective> : partner is the player objective
    object provider;  
    object destination; // for sharing quests

    if (str == "reset")
    {
      if (!me->query_coder())
      {
        tell_object(me, "Eso sólo pueden hacerlo los programadores.\n");
        return 1;
      }
      
      me->reset_quests_done();
      tell_object(me, "Ok, registro de todas las quests anteriores borrado.\n");
      return 1;
    }
    
    // different options

    if ((sscanf(str, "%s %d %s", what, which, partner) < 2) || 
      ((what != "info") && (what != "borrar") && (what != "finalizar") && (what != "compartir")) )
    {
      notify_fail("¿Qué? Prueba '"+me->query_verb()+"' sin parámetros.\n");
      return 0;
    }

    which--;
    which *= 2;

    if ((which < 0) || (which > sizeof(quests) - 1))
    {
      notify_fail("Ese número no está en la lista.\n");
      return 0;
    }

    quest = load_object(quests[which]);

    switch(what)
    {

// --------------------------------------------------------
//    quests borrar <num>        
// --------------------------------------------------------

      case "borrar":
        me->remove_quest(quests[which]);
        tell_object(me, "Ok, trabajo eliminado.\n");
        return 1;

// --------------------------------------------------------
//    quests info <num>        
// --------------------------------------------------------

      case "info":
        if (!quest)
        {
          notify_fail("Este trabajo tiene un problema, por ahora no puedes consultarlo.\n");
          return 0;
        }

        text = "Información sobre '"+quest->query_quest_title()+"':\n\n";
        text += sprintf("  %-=*s\n\n", me->query_cols(), quest->query_quest_description()); 

        switch (quest->query_quest_category())
        {
          case QUEST_GUILD:
            {
              object guild = load_object(quest->query_provider_path());
              if (guild)
                text += "%^GREEN%^Es una misión del gremio '"+guild->query_short()+"'.%^RESET%^\n";          
            }
            break;
          case QUEST_JOB:
            {
              object job = load_object(quest->query_quest_job());
              if (job)
                text += "%^GREEN%^Es una misión del oficio '"+job->query_short()+"'.%^RESET%^\n";          
            }
            break;
          case QUEST_EPIC:
            text += "Es una misión épica.\n";
            break;
          case QUEST_HISTORY:
            text += "Es una misión de historia.\n";
            break;
          default:
          case QUEST_BASIC:
            break;
        }

        switch(quest->query_quest_type())
        {
          case QUEST_TYPE_EXPLORE:
            text += "Es una misión de exploración.\n"; 
            break;
          case QUEST_TYPE_BEAT:
            text += "Es una misión de enfrentamiento.\n"; 
            break;
          case QUEST_TYPE_KILL:
          case QUEST_TYPE_GET:
            text += "Llevas conseguidos " + quests[which+1] + " de " + quest->query_how_many_to_achieve() + ".\n"; 
            break;
          default:
            break;
        }

        tell_object(me, text);

        break;

// --------------------------------------------------------
//    quests finalizar <num>        
// --------------------------------------------------------

      case "finalizar":

        if (!me->has_completed_quest(quests[which]) )
        {
          notify_fail("Aún no has terminado ese trabajo. Prueba un número válido.\n");
          return 0;
        }

        if (!quest)
        {
          notify_fail("Este trabajo tiene un problema, por ahora no puedes consultarlo.\n");
          return 0;
        }

        provider = load_object(quest->query_provider_path());

        // Quests from a provider (questman or book)
        if (quest->query_provider_needed())
        {
          if (!provider)
          {
            notify_fail("El objeto que proporcionó este trabajo tiene un problema, por ahora no podrás terminarlo.\n");
            return 0;
          }

          if (!provider->query_quest_provider())
          {
            notify_fail("El objeto que te proporcionó este trabajo no debió haberlo hecho. Avisa a un administrador.\n");
            return 0;
          }

          if (provider->query_questman() && (environment(provider) != environment(me)))
          {
            notify_fail("Esta misión te la ha proporcionado alguien, debes volver al lugar "
              "donde se encontraba esa persona para poder finalizarla.\n");
            return 0;
          }

          if (provider->query_book())
          {
            object book = present(provider->query_name(), me);
            
            // Tenemos el libro en el inventario
            if (book && (base_name(book) == quest->query_provider_path()))
            {
              if ( !provider->check_valid_finish(me, quests[which]) )    
              {
                notify_fail("No puedes terminar esa misión por alguna razón.\n");
                return 0;      
              }          
              
              me->finish_quest(quests[which]);
              return 1;
            }
            else
            {
              notify_fail("Necesitas el libro con el que empezaste esta misión para poder "
                "terminarla. ¿Lo has perdido?\n");
              return 0;          
            }
          }

          // Other categories (guild quests, etc)
          if (quest->query_quest_category() == QUEST_GUILD)
          {
            object guild = me->query_guild_ob();
            int points, max_points;
            
            if (guild)
            {
              points = me->query_xp();
              max_points = guild->query_next_level_xp(me);
              if (points < max_points)
              {
                notify_fail("Además de terminar esta quest, debes conseguir toda la experiencia "
                  "necesaria para subir de nivel. Aún no tienes suficiente. Utiliza el comando "
                  "'coste' para más información.\n");
                return 0;                              
              }
            }
                
            notify_fail("Esta es una misión de gremio, necesitas volver a la habitación principal del gremio "
                "y escribir 'avanzar' para poder terminarla y subir de nivel.\n");
            return 0;                    
          }
        }
        // provider not needed, we can finish the quest whereever we are
        else
        {
          if ( !provider->check_valid_finish(me, quests[which]) )    
          {
            notify_fail("No puedes terminar esa misión por alguna razón.\n");
            return 0;      
          }          
          
          me->finish_quest(quests[which]);
          return 1;
        }

        notify_fail("No puedes terminar esta misión así, aún debe faltarte algo por hacer.\n");
        return 0;  

        break;

// --------------------------------------------------------
//    quests compartir <num> <quien>
// --------------------------------------------------------

      case "compartir":
        
        partner = me->expand_nickname(partner);

        destination = present(partner, environment(me));

        if (!destination)
        {
          notify_fail("No puedo encontrar ningún '"+partner+"' por aquí.\n");
          return 0;      
        }

        if (!destination->query_player())
        {
          notify_fail("Sólo puedes hacer eso con otros jugadores.\n");
          return 0;      
        }

        if (!destination->query_consent(QUESTS_CONSENT))
        {
          notify_fail(destination->query_cap_name() + " no permite que se compartan trabajos.\n");
          return 0;      
        }

        provider = load_object(quest->query_provider_path());

        if (!provider)
        {
          notify_fail("El objeto que proporcionó este trabajo tiene un problema, por ahora no podrás compartirlo.\n");
          return 0;
        }

        if (destination->is_doing_quest(quests[which]))
        {
          notify_fail(destination->query_cap_name() + " ya está realizando ese trabajo.\n");
          return 0;      
        }

        if (!provider->check_valid_player(destination, quests[which]))
        {
          notify_fail(destination->query_cap_name() + " no cumple los requisitos para aceptar ese trabajo.\n");
          return 0;      
        }

        if (destination->add_quest(quests[which]))
        {
          tell_object(destination, me->query_cap_name() + " ha compartido la misión '"+quest->query_quest_title()+"' contigo.\n");        
          tell_object(me, "Ok, compartes el trabajo '"+quest->query_quest_title()+"' con "+destination->query_cap_name()+".\n");        
        }
        else
        {
          tell_object(me, "No has podido compartir el trabajo '"+quest->query_quest_title()+"' con "+destination->query_cap_name()+".\n"
                "Quizá ya esté realizando demasiados trabajos. Espera a que finalice algunos y vuelve a intentarlo.\n");
        }


    } // end switch quests <verb>
  } // end if quests <something>
    
  me->set_trivial_action();
  return 1;
}

