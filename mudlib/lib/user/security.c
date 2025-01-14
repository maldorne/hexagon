
#include <mud/secure.h>
#include <areas/calendar.h>
#include <language.h>

string password;
static string _tmppassword;

void create()
{
}

void set_password(string str)
{
  password = str;
  this_object()->save_me();
}

// string query_password() { return password; }

void security_commands()
{
  add_action("change_password", _LANG_COMM_PASSWORD);
}

nomask int valid_password(string pass)
{
  if (!strlen(pass))
    return 0;

  if (undefinedp(password) || !strlen(password))
  {
    write(_LANG_ERROR_IN_PASSWORD);
    return 0;
  }

  pass = crypt(pass, password);

  if (pass != password)
    return 0;

  return 1;
}

// prototypes
int change_password2(mixed pass);
int change_password3(string pass);
int change_password4(string pass);

int change_password(string str)
{
  write(_LANG_OLD_PASSWORD);
  input_to("change_password2", 1);
  return 1;
}

int change_password2(mixed pass)
{
  string str;

  if (!strlen(pass))
  {
    write(_LANG_CANCELLING);
    return 1;
  }

  if (password)
  {
    if (!valid_password(pass))
    {
      write(_LANG_WRONG_PASSWORD + " " + _LANG_CANCELLING);
      return 1;
    }
  }
  else
  {
    write(_LANG_UNKNOWN_PASSWORD_ERROR);
    return 1;
  }

  write(_LANG_NEW_PASSWORD);
  input_to("change_password3",1);
  return 1;
}

int change_password3(string pass)
{
  if (strlen(pass) < 6)
  {
    write(_LANG_PASSWORD_CONDITIONS + " " + _LANG_CANCELLING);
    return 1;
  }

  _tmppassword = pass;
  write(_LANG_REPEAT_PASSWORD);
  input_to("change_password4",1);
  return 1;
}

int change_password4(string pass)
{
  if (_tmppassword != pass)
  {
    write(_LANG_WRONG_PASSWORD + " " + _LANG_CANCELLING);
    return 1;
  }

  password = crypt(pass, "");
  this_object()->save_me();

  write("Ok.\n");
  return 1;
}

mixed * stats()
{
  return ({ 
    // ({ "Password", password, }),
  });
}
