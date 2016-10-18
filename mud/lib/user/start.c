
#include <mud/secure.h>
#include <areas/common.h>
#include <files/log.h>
#include <living/consents.h>
#include <living/death.h>

// prototypes
void do_first_look();
void start_player();
void start_money();


void move_player_to_start(varargs int going_invis, int is_new_player) 
{
  object tmp;
  // object obb = find_object(OMIQ_H);
  mapping mail_stat;

  if (!SECURE->valid_progname(1, "/lib/core/login"))
  {
    notify_fail("No tienes los suficientes permisos para hacer eso.");
    return ;
  }

  /* some stupid test to make sure that the previous object is /secure/login. */
  // seteuid("Root");
  // set_name(bong);

  if (query_property(GUEST_PROP))
    log_file("enter", sprintf("Enter  : %-15s %s (guest) [%s]\n",
                              name, ctime(time(),4),
                              query_ip_number(this_object())+" ("+query_ip_name(this_object())+")"));
  else
    log_file("enter", sprintf("Enter  : %-15s %s [%s]\n",
                              name, ctime(time(),4), 
                              query_ip_number(this_object())+" ("+query_ip_name(this_object())+")"));

  /* Nos olvidamos de omiqs, neverbot 4/2003
  if (OMIQ_HAND->flag_in_progress() &&
  (file_size("/save/playertmp/"+name+".o") > 0)) {
  tell_object(this_object(), "Restaurando tu ficha temporal de flag omiq...\n");
  restore_object("/save/playertmp/"+name,1);
  }
  else
  */

  // restore_object("/save/players/"+name[0..0]+"/"+name,1);

  /* Hamlet added this.  For logging on invis */
  if (going_invis != -1)
    set_invis(going_invis);

  // Aragorn fix
  // TODO cuando este global/events, fix_string tirara de esta variable en
  //  vez de consultar siempre el term_handler?
  // colour_map = 0;
  set_short(capitalize(name));

  if (!query_cols())
    set_cols(79);

  if (last_on_from && query_coder())
    tell_object(this_object(), "Tu última conexión fue desde '"+last_on_from+"'.\n");

  last_on_from = query_ip_name(this_object())+" ("+
                 query_ip_number(this_object())+")";

  if (time_on < -5000*24*60*60)
    time_on += time();
  if (time_on > 0)
    time_on = 0;

  time_on += time();
  start_player();

  if (!msgin || msgin[0] != '@')
    msgin = msgout = mmsgin = mmsgout = "";
  if (!msgin)
    msgin = "@$N llega desde $F.";
  if (!msgout)
    msgout = "@$N se va hacia $T.";
  if (!mmsgin)
    mmsgin = "@$N aparece de la nada.";
  if (!mmsgout)
    mmsgout = "@$N desaparece en una nube de humo.";

  // TODO channels
  // CHAT_HANDLER->init_player_channels(query_property(CHANNELS_PROPERTY));
  // channel_init();

  if (!last_pos || 
      catch(call_other(last_pos, "??")))
  {
    if (query_level() >= 5)
    {
      last_pos = MULTIEXITS_ROOM;
      move(last_pos);
    }
    else
    {
      last_pos = START_POS;
      move(last_pos);
    }
  }
  else
  {
    move(last_pos);
  }

  if (query_coder())
  {
    event(users(), "inform", query_cap_name()+" "+query_gtitle()+
                             " entra en "+mud_name(), "logon-coders", this_object());
  }
  else
  {
    event(users(), "inform", query_cap_name()+" entra"+
                            (query_property(GUEST_PROP)?(" como invitad"+G_CHAR):"")+ 
                            " en " + mud_name()+
                            (is_new_player ? " (%^GREEN%^BOLD%^Nuev"+G_CHAR+" jugador"+
                              (query_gender()==2?"a":"")+
                            "%^RESET%^)":""), "logon", this_object());
  }

  if (query_invis() < 2)
    event(environment(this_object()), "login", this_object());

  call_out("do_first_look", 0);
  
  // last_pos->enter(this_object());

  if (query_property(PASSED_OUT_PROP))
    call_out("remove_property", 10+random(30), PASSED_OUT_PROP);
  
  // curses_start();

  // TODO mail
  /*
  mail_stat = (mapping)POSTAL_D->mail_status(query_name());
  if (mail_stat["unread"]) 
  {
    if (mail_stat["total"] == 1)
        write("\n\t >>> ¡Tu único correo está por leer! <<<\n");
    else 
        write("\n\t >>> ¡"+mail_stat["unread"]+" de tus "+
            mail_stat["total"]+" correos están por leer! <<<\n");
  }
  */

  if (query_dead()) 
  {
    tmp = clone_object(DEATH_SHADOW);
    if (tmp)
      tmp->setup_shadow(this_object());
  }

  if (query_old_property(NORAISE_PROP))
  //    DEATH_CHAR->person_died(query_name());
    set_dead(1);

  // Esto de ejecutar cosas al entrar mejor lo quitamos, neverbot 4/2003
  // exec_alias("login","");

  last_log_on = time();
  ontime = time();

  if (query_coder())
    if (file_size("/home/" + name + "/" + PLAYER_ERROR_LOG) > 0)
      write("\nTienes informes de errores en /home/"+name+"/"+PLAYER_ERROR_LOG+"\n");

  // ident stuff.  -- Hamlet 
  // if (!catch(load_object("/net/identd")))
  //  IDENTD->do_ident(this_object(), this_object());
} /* move_player_to_start() */

void do_first_look()
{
  int i, j;
  int * has; 
  object * obs;
  object ob;

  has = allocate_int(sizeof(MUST_HAVE)); 

  for (i = 0; i < sizeof(has); i++)
    has[i] = 0;

  // TODO sheet table  
  // Aprovechamos que esto se hace tras el login completo + carga de equipo
  // para hacer la actualizacion de la ficha
  // if (SHEET_TABLE->update_player(this_object()))
  //   tell_player(this_object(), "Tu ficha ha sido actualizada.\n");

  if (query_verbose())
    do_command("look");
  else
    do_command("glance");

  // Si estamos creando el personaje, su nivel es cero, no debemos hacer esto
  if (query_level() >= 1)
  {
    // Comprobamos los objetos que todo player debe llevar siempre
    //  (solo pueden haberse perdido por algun error del salvado o carga de la ficha)
    obs = all_inventory(this_object());
    
    for (i = 0; i < sizeof(MUST_HAVE); i++)
      for (j = 0; j < sizeof(obs); j++)
        if (base_name(obs[j]) == MUST_HAVE[i])
          has[i] = 1;

    for (i = 0; i < sizeof(has); i++)
      if (!has[i])
      {
        ob = clone_object(MUST_HAVE[i]);

        if (ob)
        {
          ob->move(this_object());
          tell_object(this_object(), "Por algún error has debido perder tu "+ob->query_name()+". "+
              "Otr"+ob->query_vocal()+" nuev"+ob->query_vocal()+" te es concedid"+ob->query_vocal()+".\n\n");                
        }
      }
  }
}

void start_player() 
{
  int lockout;

  reset_get();

  // TODO 
  // Añadido para actualizar fichas antes de añadir comandos, etc
  // if (SHEET_TABLE->pre_update_player(this_object()))
  //   tell_player("Características del personaje modificadas.\n");
 
  living::start_player();

  // TODO move this to living::start_player
  // event_commands();
  // finger_commands();
  // communicate_commands();
  // living_commands();
  // spell_commands();
  // // logging_commands(); // bug, typo, idea, etc...
  // weather_commands();
  // editor_commands();
  // alias_commands();
  // nickname_commands(); 
  // recipes_commands();

  reset_all();

  start_money();
  call_out("do_load_auto", 0);

  add_timed_property(LOADING_PROP, 1, 50);
  
  // Helpless (relatively) for a while after you log in, you can run... 
  // but you can't hide.
  // Added to make it very undesirable to log out
  if ( query_property(PACIFY_PROP) )
    lockout = LOGINLOCK * 10;
  else
    lockout = LOGINLOCK;

  // old NOCAST and NOGUILD
  add_timed_property(NOFEAT_PROP, 1, lockout);

  /* Vamos a olvidarnos de estas cosas por ahora, neverbot 4/2003
  // birthday_gifts(); // check if birthday today and give gifts
  */

  // TODO wimpy
  // if (wimpy > 100)
  //     wimpy = 25;

} /* start_player() */

void start_money()
{
  // TODO money
  // object money;

  // // Done here after encumberance is set up
  // money = clone_object(MONEY_OBJECT);
  // money->set_money_array(money_array);
  // money->move(this_object());
}

int do_load_auto() 
{
  object ranking;
  
  load_auto_load(auto_load, this_object());
  
  reset_hands();

  // mounts, neverbot 07/05
  if (this_object()->query_mount_file_name())
  {
    if (this_object()->query_administrator() && this_object()->query_invis() == 2)
      tell_object(this_object(), "Tu montura no será cargada por estar en total invisibilidad.\n");
    else
      load_mount();
  }
    
  remove_timed_property(LOADING_PROP);
  call_out("do_auto_equip",3);

  // Añadimos jugadores al ranking segun reconectan, neverbot 06/10
  // (solo los que no son invitados)
  if (!query_property(GUEST_PROP))
  {
    ranking = load_object(RANKING_OB);

    if (ranking)
      ranking->update(this_object());
  }
}

void do_auto_equip()
{
  // if(this_object()->query_property(AUTOEQUIP_PROP))
  if (this_object()->query_consent(AUTOEQUIP_CONS))
      this_object()->do_equip();
}

