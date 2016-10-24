
int really_quit()
{
  object *ob, money;
  object frog, frog2;
  int i;
  int secure;
  string aux;

  aux = "";
  secure = 0;

  // quit_destination added by neverbot, 03/09  
  while (environment(this_object()) &&
        (aux = environment(this_object())->query_quit_destination()) && 
        (secure < 10))
  {
    if (load_object(aux))
      this_object()->move(aux);
    secure++;
  }
  
  if (secure != 0) 
  {
    tell_object(this_object(), "En tu localización previa no es posible salir, "+
        "has sido movido a la más cercana (vuelve a intentarlo aquí si aún "+
        "deseas desconectar).\n");
    this_object()->do_look();
    this_object()->save_me();
    return 1;
  }

  traverse_timed_properties();
  last_log_on = time();
  
  this_object()->adjust_online_time(time() - ontime);

  // TODO timekeeper
  /* This should send the time the player was on to an object which keeps
   * track of total hours played (non-immortal) Anirudh - March 17, 1996 */
  // if (!this_object()->query_coder())
  //   TIMEKEEPER->add_time(time() - ontime);

  if (this_object()->query_name() != DEF_NAME)
  {
    if (this_object()->query_property(GUEST_PROP))
      log_file("enter", sprintf("Exit  : %-15s %s (guest) [%s]\n",
                                name, ctime(time(),4),
                                query_ip_number(this_object())+" ("+query_ip_name(this_object())+")"));
    else
      log_file("enter", sprintf("Exit  : %-15s %s [%s]\n",
                                name, ctime(time(),4), 
                                query_ip_number(this_object())+" ("+query_ip_name(this_object())+")"));
  }

  // TODO editor
  // catch(editor_check_do_quit());

  // TODO social
  // if (query_race_ob())
  //   catch(query_race_ob()->player_quit(this_object()));
  // if (query_guild_ob())
  //   catch(query_guild_ob()->player_quit(this_object()));
  // if (query_class_ob())
  //   catch(query_class_ob()->player_quit(this_object()));
  // if (query_deity_ob())
  //   catch(query_deity_ob()->player_quit(this_object()));
  // if (query_group_ob())
  //   catch(query_group_ob()->player_quit(this_object()));
  // if (query_race_group_ob())
  //   catch(query_race_group_ob()->player_quit(this_object()));
  // if (query_job_ob())
  //   catch(query_job_ob()->player_quit(this_object()));
  // if (query_city_ob())
  //   catch(query_city_ob()->player_quit(this_object()));

  // curses_quit();

  // Antes del save_me, movemos todos los objetos que no vamos a grabar a
  // la room del jugador
  if (environment(this_object()))
  {
    ob = deep_inventory(this_object());
    for(i = 0; i < sizeof(ob); i++)
    {
      if (ob[i]->query_no_save_object())
      {
        ob[i]->move(environment(this_object()));
        tell_object(this_object(), "Dejas tu "+ob[i]->short()+".\n");
      }
    }
  }

  // write("Thanks for playing see you next time.\n");
  tell_player(this_object(), "Gracias por jugar, ¡hasta la próxima!\n");

  if (query_name() != DEF_NAME)
  {
    if ( !this_object()->query_hidden() )
      tell_room( environment(this_object()), this_object()->query_cap_name() + 
        " sale de "+mud_name()+".\n");

    if ( this_object()->query_coder() )
      event(users(), "inform", this_object()->query_cap_name() +
                               " sale de " + mud_name(), "logon-coders",
                               all_inventory(environment(this_object())));
    else
      event(users(), "inform", this_object()->query_cap_name() + 
                               " sale de " + mud_name(), "logon",
                               all_inventory(environment(this_object())));
  }

  ob = ({ });
  i = 0;

  this_object()->save_me();
  
  // TODO mounts
  // mounts, neverbot 07/05
  // save_mount();
  // if (query_mount())
  //   query_mount()->owner_quit();

  /* get rid of the money.... we dont want them taking it twice now do we? */
  if ((money = present(MONEY_NAME, this_object())))
      money->dest_me();

  frog = first_inventory(this_object());
  while (frog) 
  {
    frog2 = next_inventory(frog);

    // if (frog)

    // hack para los objetos unicos... si los destruimos con la funcion
    // dest_me el handler no los almacenara como grabados en la ficha
    // de un player, sino como destruidos.
    if (frog->query_unique_object())
    {
      tell_object(find_living("folken"), "Dest unique del objeto "+frog->short()+".\n");
      frog->dest_unique();
    }
    else if (frog->query_auto_load() ||
             frog->query_static_auto_load())
    {
      frog->dest_me();
    }

    frog = frog2;
  }

  transfer_all_to(environment());
  ob = all_inventory(this_object());
  
  for (i = 0; i < sizeof(ob); i++)
    ob[i]->dest_me();

  // TODO safety here?
  find_object(USER_HANDLER)->remove_user(this_object());
  find_object(LIVING_HANDLER)->remove_living(this_object());

  this_object()->dest_me();

  return 1;
} /* really quit() */

void continue_quit(int a, object here)
{
  if (this_object()->query_is_fighting()) 
  {
    tell_object(this_object(), "Debes terminar primero tus combates.\n "+
        "Escribe '%^BOLD%^detener combates%^RESET%^' para terminarlos lo antes posible.\n");
    return;
  }

  if (environment(this_object()) != here) 
  {
    tell_object(this_object(),"Si no paras de moverte será imposible buscar un buen lugar "+
        "para salir del juego.\n");
    return;
  }

  if (a <= 0)
  {
    this_object()->really_quit();
    return;
  }

  call_out("continue_quit", 0, a-1, environment(this_object()));
}

int quit()
{
  if (query_loading() || query_property(LOADING_PROP))
  {
    notify_fail("Todavía se esta cargando tu equipo, espera a que termine el proceso.\n");
    return 0;
  }

  tell_object(this_object(), "Saliendo...\n");
  
  if (this_object()->query_coder()) 
  {
    this_object()->really_quit();
    return 1;
  }
  
  tell_room(environment(this_object()), this_object()->query_cap_name()+
      " busca un lugar cómodo para salir del juego.\n", ({this_object()}));

  if (this_object()->query_is_fighting())
  {
    tell_object(this_object(),"Debes terminar primero tus combates.\n"+
        "Escribe '%^BOLD%^detener combates%^RESET%^' para terminarlos lo antes posible.\n");
    this_object()->save_me();
  }
  else
    call_out("continue_quit", 0, 5, environment(this_object()));
  
  return 1;
} 
