
#include <mud/time.h>
#include <language.h>

int really_quit()
{
  object *ob, money;
  object frog, frog2;
  int i;
  int secure;
  string aux;
  object user;

  aux = "";
  secure = 0;

  user = this_object()->user();

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
    write(_LANG_QUIT_BEEN_MOVED);
    this_object()->do_look();
    this_object()->save_me();
    return 1;
  }

  traverse_timed_properties();
  last_log_on = time();

  this_object()->adjust_online_time(time() - ontime);

  // This should send the time the player was on to an object which keeps
  // track of total hours played Anirudh - March 17, 1996
  TIMEKEEPER->add_time(time() - ontime);

  if (this_object()->query_name() != DEF_NAME)
  {
    if (this_object()->query_property(GUEST_PROP))
      log_file("enter", sprintf("Exit  : %-15s %s (guest) [%s]\n",
                                name, ctime(time(), 4),
                                query_ip_number(user)+
                                  " ("+query_ip_name(user)+")"));
    else
      log_file("enter", sprintf("Exit  : %-15s %s [%s]\n",
                                name, ctime(time(), 4),
                                query_ip_number(user)+
                                  " ("+query_ip_name(user)+")"));
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

  // before save_me, move every item we are not going to save to
  // the player's room
  if (environment(this_object()))
  {
    ob = deep_inventory(this_object());
    for(i = 0; i < sizeof(ob); i++)
    {
      if (ob[i]->query_no_save_object())
      {
        ob[i]->move(environment(this_object()));
        write(_LANG_QUIT_DROP_ITEM);
      }
    }
  }

  tell_player(this_object(), _LANG_QUIT_THANKS);

  if (query_name() != DEF_NAME)
  {
    if ( !this_object()->query_hidden() )
      tell_room( environment(this_object()), _LANG_QUIT_ENV_QUIT, ({ this_object() }));

    if ( this_object()->query_coder() )
      event(users(), "inform", _LANG_QUIT_INFORM_CODERS, "logon-coders",
                               all_inventory(environment(this_object())));
    else
      event(users(), "inform", _LANG_QUIT_INFORM_PLAYERS, "logon",
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

    // hack for unique items... if we destroy them with the dest_me function
    // the handler won't save them as stored in the players inventory, but as
    // destroyed, and we don't want that
    if (frog->query_unique_object())
    {
      // tell_object(find_living("neverbot"), "Dest unique of item "+frog->short()+".\n");
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
  // this remove will be done at the end of the user->dest_me()
  // find_object(USER_HANDLER)->remove_user(user);
  find_object(LIVING_HANDLER)->remove_living(this_object());

  // the player object will be destroyed from the user->dest_me()
  // so beyond this line, no code will be safe
  user->dest_me();
  // this_object()->dest_me();

  return 1;
} /* really_quit() */

void continue_quit(int a, object here)
{
  if (this_object()->query_is_fighting())
  {
    write(_LANG_QUIT_STOP_COMBAT_FIRST);
    return;
  }

  if (environment(this_object()) != here)
  {
    write(_LANG_QUIT_STOP_MOVING_FIRST);
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
    notify_fail(_LANG_QUIT_LOADING);
    return 0;
  }

  write(_LANG_QUIT_EXITING);

  if (this_object()->query_coder())
  {
    this_object()->really_quit();
    return 1;
  }

  tell_room(environment(this_object()), _LANG_QUIT_ENV_MSG, ({ this_object() }));

  if (this_object()->query_is_fighting())
  {
    write(_LANG_QUIT_STOP_COMBAT_FIRST);
    this_object()->save_me();
  }
  else
    call_out("continue_quit", 0, 5, environment(this_object()));

  return 1;
}
