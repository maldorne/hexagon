
// Player account
//   neverbot 12/2010

#include <user/user.h>
#include <mud/secure.h>

string account_name;
static object _account;

void create()
{
  account_name = "";
}

object query_account() { return _account; }
string query_account_name() { return account_name; }

int set_account(string acc_name)
{
  // for safety reasons, we allow set_account only to be called from /lib/core/login
  // the login progname should be 1 steps ago in the call trace
  if (!SECURE->valid_progname(1, "/lib/core/login"))
    return 0;

  // if (!interactive(player))
  //   return 0;

  _account = clone_object(ACCOUNT_OB);
  account_name = acc_name;

  // try to restore the account
  if (!_account->restore_me(acc_name))
    return 0;

  return 1;
}

int set_account_ob(object account)
{
  // for safety reasons, we allow set_account only to be called from /lib/core/login
  // the login progname should be 1 steps ago in the call trace
  if (!SECURE->valid_progname(1, "/lib/core/login"))
    return 0;

  _account = account;
  
  return 1;
}

string query_account_password() { return _account->query_password(); }
void set_account_password(string pass)
{
  _account->set_password(pass);
  _account->update_last_connection();
}

string query_account_email() { return _account->query_email(); }
void set_account_email(string str)
{
  _account->set_email(str);
  _account->update_last_connection();
}

string query_account_real_name() { return _account->query_real_name(); }
void set_account_real_name(string str)
{
  _account->set_real_name(str);
  _account->update_last_connection();
}

string query_account_location() { return _account->query_location(); }
void set_account_location(string str)
{
  _account->set_location(str);
  _account->update_last_connection();
}

string query_account_birthday() { return _account->query_birthday(); }
void set_account_birthday(string str)
{
  _account->set_birthday(str);
  _account->update_last_connection();
}

mixed * stats() 
{
  return (_account ? _account->stats() : ({ }));
}
