
#include <mud/secure.h>
#include <areas/calendar.h>
#include <language.h>

void create()
{
}

void account_commands()
{
  // add_action("set_email", "email");
  add_action("chfn", _LANG_ACCOUNT_DETAILS_VERBS);
}

void show_commands()
{
  write(_LANG_ACCOUNT_COMMANDS);
}

int chfn(string str)
{
  write(_LANG_ACCOUNT_DETAILS_HEADER);
  write(_LANG_ACCOUNT_ASK_REAL_NAME);
  input_to("real_name");
  return 1;
}

int real_name(string str)
{
  if (strlen(str))
  {
    if (member_array(str, _LANG_ACCOUNT_NONE_WORDS) != -1)
    {
      this_object()->set_real_name("");
      write(_LANG_ACCOUNT_REAL_NAME_CLEARED);
    }
    else
    {
      this_object()->set_real_name(str);
      write(_LANG_ACCOUNT_REAL_NAME_SET);
    }
  }
  else
  {
    write(_LANG_ACCOUNT_REAL_NAME_KEPT);
  }

  write(_LANG_ACCOUNT_ASK_LOCATION);
  input_to("get_where");
  return 1;
} /* real_name() */

int get_where(string str)
{
  if (strlen(str))
  {
    if (member_array(str, _LANG_ACCOUNT_NONE_WORDS) != -1)
    {
      this_object()->set_location("");
      write(_LANG_ACCOUNT_LOCATION_CLEARED);
    }
    else
    {
      this_object()->set_location(str);
      write(_LANG_ACCOUNT_LOCATION_SET);
    }
  }
  else
  {
    write(_LANG_ACCOUNT_LOCATION_KEPT);
  }

  write(_LANG_ACCOUNT_ASK_BIRTHDAY);
  input_to("birthday");
  return 1;
} /* get_where() */

void birthday(string str)
{
  if (strlen(str))
  {
    if (member_array(str, _LANG_ACCOUNT_NONE_WORDS) != -1)
    {
      this_object()->set_birthday("");
      write(_LANG_ACCOUNT_BIRTHDAY_CLEARED);
    }
    else
    {
      if (!handler("calendar")->valid_birthday(str))
      {
        write(_LANG_ACCOUNT_BIRTHDAY_INVALID);
        input_to("birthday",1);
        return;
      }
      this_object()->set_birthday(str);
      write(_LANG_ACCOUNT_BIRTHDAY_SET);
    }
  }
  else
  {
    write(_LANG_ACCOUNT_BIRTHDAY_KEPT);
  }

  this_object()->save_me();

  return;

  // remove email change for now, it's set with
  // the account and cannot be changed

  // write("\nIntroduce tu dirección de email ["+this_object()->query_email()+"]\n"+
  //   "('ninguna' para borrarla): ");

  // write("\nPuedes anteponer el símbolo ':' delante, y así sólo los administradores "+
  //     "podrán verlo.\n");
  // input_to("set_email");

} /* birthday() */

/*
int set_email(string str)
{
  if (strlen(str))
  {
    if ((str == "none") || (str == "ninguno") || (str == "ninguna"))
    {
      this_object()->set_email("");
      write("Ok, dirección de email borrada.\n");
    }
    else
    {
      // hack para evitar un error comun del cliente ZMud
      if (str[1..1] == ":")
        str = str[1..strlen(str)-1];

      if (!SECURE->valid_email(str))
      {
        write("Dirección de email inválida, se deja sin modificar.\n");
        return 1;
      }

      if (str[0..0] == ":")
        write("Tu dirección de email sólo será visible por administradores.\n");

      this_object()->set_email(str);
      write("Ok, dirección de email establecida como "+str+".\n");
    }
  }
  else
  {
    write("Ok, dirección de email sin modificar.\n");
  }
  return 1;
}
*/

mixed * stats()
{
  return ({ });
}
