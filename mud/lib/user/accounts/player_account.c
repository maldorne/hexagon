/* ******************************************************
 *
 *     User account
 *
 *     New login system and multiple player characters,
 *        neverbot 12/2010
 *
 * ****************************************************** */

inherit "/lib/core/object";

#define SAVE_DIR "/save/accounts/"

string account_name;
string password;
static string tmp_password;

// string email;
string real_name;
string birthday; 
string location;
string * player_list;

int last_connected;

void create()
{
  ::create();
  player_list = ({ });
  account_name = "";
}

int restore_me(string name)
{
  if (restore_object(SAVE_DIR + name[0..0] + "/"  + name + ".o", 1))
    return 1;
  return 0;
}

void save_me()
{
  if (!account_name || (account_name == ""))
    return;
  save_object(SAVE_DIR + account_name[0..0] + "/" + account_name + ".o", 1);
}

void load_me()
{
  if (!account_name || (account_name == ""))
    return;
  restore_object(SAVE_DIR + account_name[0..0] + "/"  + account_name + ".o", 1);
}

void set_account_name(string str) 
{ 
  if (account_name != "")
    return; // no permitimos el cambio del nombre de cuenta
  account_name = str; 
  save_me();
}

string query_account_name() { return account_name; }
void set_password(string str) 
{ 
  password = str; 
  save_me();
}

string query_password() { return password; }
string query_email() { return account_name; }
void set_real_name(string str) { real_name = str; }
string query_real_name() { return real_name; }
void set_birthday(string str) { birthday = str; }
string query_birthday() { return birthday; }
void set_location(string str) { location = str; }
string query_location() { return location; }

string * query_player_list() { return player_list; }

void add_player(string name) 
{ 
  if (member_array(name, player_list) != -1)
    return;
  player_list += ({ name }); 
  save_me();
}

void remove_player(string name)
{
  player_list -= ({ name });
  save_me();
}

int query_last_connection() { return last_connected; }
void update_last_connection() 
{ 
  last_connected = time(); 
  save_me();
}

mixed * stats() 
{
  return ({ 
        ({ "(account) Account Name", account_name }),
          });
}
