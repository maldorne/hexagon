
#include <mud/secure.h>
#include <areas/common.h>
#include <living/consents.h>
#include <living/death.h>
#include <basic/communicate.h>
#include <translations/races.h>
#include <translations/inform.h>
#include <language.h>

// prototypes
nomask void start_player();
nomask void check_mandatory_inventory();

nomask void start(varargs int going_invis, int is_new_player, int reconnected, object destination)
{
  object tmp;
  mapping mail_stat;
  object last;

  if (!SECURE->valid_progname("/lib/core/login"))
    return;

  if (!reconnected)
  {
    if (query_property(GUEST_PROP))
      log_file("enter", sprintf("Enter : %-15s %s (guest) [%s]\n",
                          name, ctime(time(), 4),
                          query_ip_number(this_user())+" ("+query_ip_name(this_user())+")"));
    else
      log_file("enter", sprintf("Enter : %-15s %s [%s]\n",
                          name, ctime(time(), 4),
                          query_ip_number(this_user())+" ("+query_ip_name(this_user())+")"));
  }

  set_short(capitalize(name));

  if (time_on < -5000*24*60*60)
    time_on += time();
  if (time_on > 0)
    time_on = 0;

  time_on += time();
  start_player();

  if (!msgin || msgin[0] != '@')
    msgin = msgout = mmsgin = mmsgout = "";
  if (!msgin)
    msgin = _LANG_RACES_MSG_IN_STD;
  if (!msgout)
    msgout = _LANG_RACES_MSG_OUT_STD;
  if (!mmsgin)
    mmsgin = _LANG_RACES_MMSG_IN_STD;
  if (!mmsgout)
    mmsgout = _LANG_RACES_MMSG_OUT_STD;

  CHAT_HANDLER->init_player_channels(query_property(CHANNELS_PROPERTY), this_object());
  // channel_init();

  // death stuff
  if (query_dead())
  {
    tmp = clone_object(DEATH_SHADOW);
    if (tmp)
      tmp->setup_shadow(this_object());
  }

  if (query_old_property(NORAISE_PROP))
  //    DEATH_CHAR->person_died(query_name());
    set_dead(1);

  // move the player to its initial destination
  if (reconnected)
  {
    move_living("X", destination);
  }
  else
  {
    if (!strlen(last_pos) ||
        catch(last = load_object(last_pos)) ||
        !last)
    {
      // TODO: MULTIEXITS_ROOM does not exist 
      // if (query_level() >= 5)
      // {
      //   last_pos = MULTIEXITS_ROOM;
      //   move_living("X", last_pos);
      // }
      // else
      // {
        move_living("X", START_POS);
      // }
    }
    else
    {
      move_living("X", last_pos);
    }

    if (query_coder())
      event(users(), "inform", _LANG_START_INFORM_CODER, 
            INFORM_EVENT_LOGON_CODERS, this_object()->user());
    else
      event(users(), "inform", _LANG_START_INFORM_PLAYER, 
            INFORM_EVENT_LOGON, this_object()->user());

    if (user()->query_invis() < 2)
      event(environment(this_object()), "login", this_object());

    if (query_property(PASSED_OUT_PROP))
      call_out("remove_property", 20, PASSED_OUT_PROP);
  }

  // check items we have lost from our inventory
  call_out("check_mandatory_inventory", 1);

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

  // removed these things about executing scripts on login, neverbot 4/2003
  // exec_alias("login","");

  last_log_on = time();
  ontime = time();

  // ident stuff.  -- Hamlet
  // if (!catch(load_object("/net/identd")))
  //  IDENTD->do_ident(this_object(), this_object());
} /* start() */

nomask void check_mandatory_inventory()
{
  int i, j;
  int * has;
  object * obs;
  object ob;

  has = allocate_int(sizeof(MUST_HAVE));

  // if we are creating the character, his level is zero, we must not do this
  if (query_level() >= 1)
  {
    // check the items every player must always have
    // (they can be lost by some error in saving or loading the character)
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
          this_object()->user()->add_notification("inventory", _LANG_NEW_MANDATORY_ITEM);
        }
      }
  }
}

nomask void start_player()
{
  int lockout;

  reset_get();

  // player migrations
  if (this_object()->update_player())
    tell_player(this_object(), "Tu ficha ha sido actualizada.\n");

  living::start_player();

  reset_all();

  call_out("do_load_auto", 0);

  add_timed_property(LOADING_PROP, 1, 50);

  // Helpless (relatively) for a while after you log in, you can run...
  // but you can't hide.
  // Added to make it very undesirable to log out
  if (query_property(PACIFY_PROP))
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

