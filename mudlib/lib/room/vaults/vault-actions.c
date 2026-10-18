/*
  March 13, 1995
  Vaults - Originally designed to contain items for players
     to save'em through reboots.  Too much work, changed to
     group/guild/clan/house access instead of an object file
     for each individual player.
  For FR III, guilds and any other group willing to purchase
  these for 5000 platinum coins will be allowed to have access.
  Main purpose was for newbie equipment but other vaults can be
  purchased for individual Guild masters and even senior members
  of a group. Keep in mind, the fewer with access, charge
  them more. 
  Vault restrictions NOT to be changed: 
  Nothing but weapons, armours, scrolls, and wands, and definitely
  no unsaveable items.

  How to make your own in the game:
  Simple, inherit this file and describe your room. Make sure to
  copy or design your own add_sign()   *grin*   
  However, before putting it into the game, must be approved by
  a Demi-God beforehand and /d/apriors/doms/VAULTS should be 
  updated.
  Have fun!

                       Radix : Thane of Hoerk
  Added logging with player readable abilities for monitoring
  by Guild leader and immortals.
  Use set_admins( ({"name1", "name2",... }) ); or "all" for everyone.
  Radix - December 4, 1995
 
  Changed to use /global/auto_load as Taniwha showed me how  (:
  New driver allows larger buffer, save object file can be huge,
  won't break BUT, the restriction is set to 30k for a reason.
  Radix - February 10, 1996
 
  Missing item bug regretfully fixed. Calling save_object
  when someone else simultaneously uses the vault as well
  caused it. Radix - Jan 4, 1996

  Translated for CcMud, neverbot 08/07/04 
  Listings by item category
  Translated again for Hexagon, neverbot 01/2021
  Split from vault-room.c so rooms and locations share it, 2026
*/ 

// The vault's behaviour, shared by the vault room (/lib/room/vaults/vault-room.c)
// and the vault location component (/lib/location/components/vault.c). Whoever
// inherits it says where the vault stands (query_vault_place), where it keeps
// its contents (set_vault_storage) and puts up its sign (create_vault_sign).

#include <basic/move.h>
#include <room/storage.h>
#include <language.h>
#include <translations/language.h>
#include <translations/common.h>

#define FILE_SIZE 30000 // change, you die

// the help document the vault sign shows, read every time the sign is put up
#define VAULT_SIGN_DOC "help/world/vaults"

string save_file;
string vault_log;
string * vault_admins;
static object vault_sign;

// Optional restrictions, all empty by default (everyone may use the vault, it
// takes whatever it takes, no limit, the shared sign):
//   vault_guilds        guild object paths; only their members (and coders) use it
//   vault_accept_prop   the property an object must carry to be deposited...
//   vault_accept_value  ...and the value (compared as text) it must have
//   vault_accept_name   what is accepted, in words, for the refusal and the sign
//   vault_max_items     most objects the vault holds
//   vault_sign_text     own sign text instead of the vault help document
//   vault_sign_doc      own help document (name under docs/<lang>/) instead of it
string * vault_guilds;
string vault_accept_prop;
string vault_accept_value;
string vault_accept_name;
int vault_max_items;
string vault_sign_text;
string vault_sign_doc;

string query_save_file() { return save_file; }
string query_vault_log() { return vault_log; }

void set_admins(string *admin) { if (sizeof(admin)) vault_admins = admin; }
string *query_vault_admins() { return vault_admins; }

string * query_vault_guilds() { return vault_guilds; }
void set_vault_guilds(string * guilds) { vault_guilds = sizeof(guilds) ? guilds : ({ }); }

string query_vault_accept_prop() { return vault_accept_prop; }
string query_vault_accept_value() { return vault_accept_value; }
string query_vault_accept_name() { return vault_accept_name; }
void set_vault_accept(string prop, string value, varargs string name)
{
  vault_accept_prop = prop;
  vault_accept_value = value;
  vault_accept_name = stringp(name) ? name : "";
}

int query_vault_max_items() { return vault_max_items; }
void set_vault_max_items(int max) { vault_max_items = max > 0 ? max : 0; }

string query_vault_sign_text() { return vault_sign_text; }
void set_vault_sign_text(string text) { vault_sign_text = text; }
string query_vault_sign_doc() { return vault_sign_doc; }
void set_vault_sign_doc(string name) { vault_sign_doc = name; }

int query_vault_room() { return 1; }

// The room or location the vault stands in: where items are moved and the
// messages are heard.
object query_vault_place() { return this_object(); }

void create()
{
  save_file = "";
  vault_log = "";
  vault_admins = ({ });
  vault_guilds = ({ });
  vault_accept_prop = "";
  vault_accept_value = "";
  vault_accept_name = "";
  vault_sign_text = "";
  vault_sign_doc = "";
}

// The name of what the vault accepts, for the messages and the sign.
string query_vault_accept_label()
{
  if (strlen(vault_accept_name))
    return vault_accept_name;
  return vault_accept_prop + " " + vault_accept_value;
}

// The guilds that may use the vault, by name.
string guild_names()
{
  string * names;
  object guild;
  int i;

  names = ({ });
  for (i = 0; i < sizeof(vault_guilds); i++)
  {
    guild = nil;
    catch(guild = load_object(vault_guilds[i]));
    names += ({ guild ? guild->query_name() : vault_guilds[i] });
  }
  return implode(names, ", ");
}

// Whether the object may be deposited: any object when the vault names no
// property to accept.
int accepts_object(object ob)
{
  mixed found;

  if (!strlen(vault_accept_prop))
    return 1;

  found = ob->query_property(vault_accept_prop);
  if (found == nil)
    return 0;
  return ("" + found) == vault_accept_value;
}

// Whether this_player() may use the vault: anybody when no guild is named,
// else coders and the members of the guilds.
int may_use_vault()
{
  string own;

  if (!sizeof(vault_guilds) || this_player()->query_coder())
    return 1;

  // guilds are named by the full path of their object, .c included
  own = this_player()->query_guild_ob();
  return stringp(own) && member_array(own, vault_guilds) != -1;
}

// The lines that state the restrictions, added to the shared sign.
string restrictions_text()
{
  string text;

  text = "";
  if (sizeof(vault_guilds))
    text += _LANG_VAULTS_SIGN_GUILDS;
  if (strlen(vault_accept_prop))
    text += _LANG_VAULTS_SIGN_ACCEPT;
  if (vault_max_items)
    text += _LANG_VAULTS_SIGN_MAX;
  return text;
}

// Where the contents and the log are kept, from the file the vault is written
// in (a room's file, or the room a location was converted from): every game's
// vaults are saved inside that game's save directory.
void set_vault_storage(string source)
{
  string * dirs;
  string name;

  if (!stringp(source) || !strlen(source))
    return;

  if (source[strlen(source) - 2 ..] == ".c" || source[strlen(source) - 2 ..] == ".o")
    source = source[.. strlen(source) - 3];

  dirs = explode(source, "/");
  name = implode(dirs[2 ..], "-");

  save_file = game_save_dir(query_vault_place()) + "vaults/" + name + "-save";
  vault_log = game_save_dir(query_vault_place()) + "vaults/" + name + "-log";
}

// Put up the sign explaining the vault. Its text is its own one if it has it,
// else the vault help document (read now, so changing the document changes
// every vault sign) with the restrictions stated after it.
object create_vault_sign(object where)
{
  string text;

  if (vault_sign)
    vault_sign->dest_me();

  // an own text says whatever it wants; the shared document, or one named,
  // gets the restrictions added to it
  if (strlen(vault_sign_text))
    text = vault_sign_text;
  else
    text = handler("help")->query_document_text(doc(strlen(vault_sign_doc) ?
             vault_sign_doc : VAULT_SIGN_DOC)) + restrictions_text();

  vault_sign = where->add_sign(_LANG_VAULTS_SIGN_LONG, text,
    nil, nil, nil, STD_LANG);
  return vault_sign;
}

// Take the sign down again, when the vault leaves its place.
void take_down_vault_sign()
{
  if (vault_sign)
    vault_sign->dest_me();
  vault_sign = nil;
}

void init()
{
  add_action("do_deposit", _LANG_VAULTS_CMDS_DEPOSIT);
  add_action("do_retrieve", _LANG_VAULTS_CMDS_RETRIEVE);
  add_action("do_list", _LANG_VAULTS_CMDS_LIST);
  add_action("view_vault_log", _LANG_VAULTS_CMDS_LOGS);
}

int view_vault_log(string str)
{
  string name;
  string tmp;

  if (!may_use_vault())
  {
    notify_fail(_LANG_VAULTS_NOT_ALLOWED);
    return 0;
  }
 
  if (strlen(str)) 
  {
    notify_fail(_LANG_SYNTAX + ": 'logs'\n");
    return 0;
  }

  name = this_player()->query_name();
  
  // only if you are a coder, admin, or if all are allowed
  if (this_player()->query_coder() ||
    member_array(name,vault_admins) != -1 ||
    !sizeof(vault_admins) || 
    (vault_admins[0] == "all") )
  {
    tmp = read_file(query_vault_log());

    if (!tmp || !strlen(tmp))
    {
      tell_object(this_player(), _LANG_VAULTS_LOG_EMPTY);
      return 1;
    }

    this_user()->more_string(tmp);
    return 1;
  }
  
  notify_fail(_LANG_VAULTS_LOG_PERMISSION);
  return 0;
}

int do_deposit(string str)
{
  object vault;
  object * all;
  // object * checked;
  int i;
  int ret;

  if (!may_use_vault())
  {
    notify_fail(_LANG_VAULTS_NOT_ALLOWED);
    return 0;
  }

  if (!strlen(str))
  {
    notify_fail(_LANG_VAULT_ROOM_DEPOSIT_WHAT);
    return 0;
  }
 
  vault = clone_object(VAULT_FILES_PATH + "vault-obj.c");
  /* Added by Timion, 06 NOV 97
    To prevent deposits in vault during CTF
  if ("/global/omiq.c"->flag_in_progress())
  {
    notify_fail("Sorry, you may not deposit items during a Flag Game.\n");
    vault->dest_me();
    return 0;
  }
  */
 
  if (query_vault_place()->query_property(VAULT_USE_PROP))
  {
    notify_fail(_LANG_VAULT_IN_USE);
    vault->dest_me();
    return 0;
  }

  all = find_match(str, this_player());
  // checked = all;

  if (sizeof(all))
  {
    vault->move(query_vault_place());
    vault->set_save_file(save_file);
    
    if (file_size(query_save_file() + ".o") > FILE_SIZE)
    {
      notify_fail(_LANG_VAULT_ROOM_DEPOSIT_FULL);
      vault->dest_me();
      return 0;
    }
    
    for (i = 0; i < sizeof(all); i++)
    {
      // how is it possible this wasn't here already???
      if ((int)all[i]->query_in_use())
      {
        tell_object(this_player(), _LANG_VAULT_ROOM_DEPOSIT_EQUIPPED);
        // checked -= ({ all[i] });
        continue;
      }

      if (!accepts_object(all[i]))
      {
        tell_object(this_player(), _LANG_VAULTS_NOT_ACCEPTED);
        continue;
      }

      if (vault_max_items && sizeof(all_inventory(vault)) >= vault_max_items)
      {
        tell_object(this_player(), _LANG_VAULTS_TOO_MANY);
        continue;
      }

      if (all[i]->avoid_auto_load())
      {
        tell_object(this_player(), _LANG_VAULT_ROOM_DEPOSIT_AVOID);
          
        // checked -= ({ all[i] });
        all[i]->dest_me();
        continue;
      }

      if (all[i]->query_no_save_object())
      {
        tell_object(this_player(), _LANG_VAULT_ROOM_DEPOSIT_NO_SAVE);
        // checked -= ({ all[i] });
        continue;
      }
  
      if (all[i]->is_money())
      {
        tell_object(this_player(), _LANG_VAULT_ROOM_DEPOSIT_MONEY);
        // checked -= ({ all[i] });
        continue;
      }

      // This is here for a REASON, if you change it you had better
      // pray I don't find it - Radix
      /*
      if (!all[i]->query_weapon() && !all[i]->query_armour() &&
        !all[i]->query_shield() )
        // !all[i]->query_scroll() && !all[i]->query_wand())
      {
      */

      // a vault that names what it accepts takes it whatever it is worth
      if (!strlen(vault_accept_prop) && !all[i]->query_value())
      {
        tell_object(this_player(), _LANG_VAULT_ROOM_DEPOSIT_NO_VALUE);
        // checked -= ({ all[i] });
        continue;
      }

      ret = all[i]->move(vault);

      if (ret == MOVE_OK)
      {
        string log;
        log = "";
        tell_object(this_player(), _LANG_VAULT_ROOM_DEPOSIT_ME);
        tell_room(query_vault_place(), _LANG_VAULT_ROOM_DEPOSIT_THEM, ({ this_player() }));

        log = this_player()->query_cap_name()+
           " " + _LANG_VAULT_ROOM_DEPOSIT_LOG + ": " + 
           all[i]->query_name() + " (" + ctime(time(), 4) + ")\n";

        write_file(query_vault_log(), log);
      }
      else
      {
        tell_object(this_player(), _LANG_VAULT_ROOM_DEPOSIT_FAILED);
        // vault->dest_me();
        // return 0;
        continue;
      }
    }
    vault->dest_me();
    return 1;
  }
  
  vault->dest_me();
  notify_fail(_LANG_VAULT_ROOM_DEPOSIT_NOT_FOUND);
  return 0;
}

int do_retrieve(string str)
{   
  object vault;
  object *all;
  int i;

  if (!may_use_vault())
  {
    notify_fail(_LANG_VAULTS_NOT_ALLOWED);
    return 0;
  }

  if (!strlen(str))
  {
    notify_fail(_LANG_VAULT_ROOM_RETRIEVE_WHAT);
    return 0;
  }

  vault = clone_object(VAULT_FILES_PATH + "vault-obj.c");

  if (query_vault_place()->query_property(VAULT_USE_PROP))
  {
    notify_fail(_LANG_VAULT_IN_USE);
    vault->dest_me();
    return 0;
  }
  
  vault->move(query_vault_place());
  vault->set_save_file(save_file);
  
  all = find_match(str,vault);
  
  if (sizeof(all))
  {
    for (i = 0; i < sizeof(all); i++)
    {
      if (this_player()->query_contents_weight() + all[i]->query_weight() <
        this_player()->query_max_weight())
      {
        string log;
        
        all[i]->move(this_player());
        
        tell_room(query_vault_place(), _LANG_VAULT_ROOM_RETRIEVE_THEM, ({ this_player() }));
        tell_object(this_player(), _LANG_VAULT_ROOM_RETRIEVE_ME);

        log = this_player()->query_cap_name() +
          " " + _LANG_VAULT_ROOM_RETRIEVE_LOG + ":  " + 
          all[i]->query_name() + " (" + ctime(time(), 4) + ")\n";

#ifdef COMMON_SAVE
        if (is_in_game(query_vault_place()))
           log_file(query_vault_log(), log);
        else   
           write_file(query_vault_log(), log);
#else
        write_file(query_vault_log(), log);
#endif

      }
      else
        tell_object(this_player(), _LANG_VAULT_ROOM_RETRIEVE_WEIGHT);
    }
    vault->dest_me();
    return 1;
  }
  
  vault->dest_me();
  notify_fail(_LANG_VAULT_ROOM_RETRIEVE_CANNOT);
  return 0;
}

int do_list(string str)
{   
  object vault;
  object *all;
  int i, j;
  string *list;
  string tmp;  // Radix Nov 29, 1997
  int capacity;

  // separate items by category, neverbot 08/07/04
  // every mapping entry is an array with the corresponding objects
  mapping objects;
  
  if (!may_use_vault())
  {
    notify_fail(_LANG_VAULTS_NOT_ALLOWED);
    return 0;
  }

  vault = clone_object(VAULT_FILES_PATH + "vault-obj.c");
  
  // set the order to show
  list = ({ _LANG_VAULTS_LIST_WEAPONS, _LANG_VAULTS_LIST_ARMOURS, _LANG_VAULTS_LIST_SHIELDS, 
          _LANG_VAULTS_LIST_HERBS, _LANG_VAULTS_LIST_SKINS, _LANG_VAULTS_LIST_OTHER, });

  objects = ([ ]);
  objects[_LANG_VAULTS_LIST_ARMOURS] = ({ });
  objects[_LANG_VAULTS_LIST_WEAPONS] = ({ });
  objects[_LANG_VAULTS_LIST_SHIELDS] = ({ });
  objects[_LANG_VAULTS_LIST_HERBS] = ({ });
  objects[_LANG_VAULTS_LIST_SKINS] = ({ });
  objects[_LANG_VAULTS_LIST_OTHER] = ({ });

  if (query_vault_place()->query_property(VAULT_USE_PROP))
  {
    notify_fail(_LANG_VAULT_IN_USE);
    vault->dest_me();
    return 0;
  }
  
  vault->move(query_vault_place());
  vault->set_save_file(save_file);
  
  all = all_inventory(vault);
  
  if (sizeof(all))
  {
    /* Metodo original:
    tell_object(this_player(), "Hay "+sizeof(all)+" objetos almacenados en este baúl:\n\n");
    list = (string *)vault->list_contents();
    tmp = "";
    for (i = 0; i < sizeof(list); i++)
      tmp += list[i]+"\n";
    
    this_player()->more_string(tmp);
    tell_object(this_player(), "\nCapacidad máxima:  "+FILE_SIZE+"\n"
          "Capacidad utilizada:  "+file_size(query_save_file()+".o")+
        "\n\n");
    */
    
    // Ordenamos los objetos por categorias
    /* 
    // Sistema pre-agrupar por mismo nombre
    for (i = 0; i < sizeof(all); i++)
    {
       if (all[i]->query_weapon())
          objects[_LANG_VAULTS_LIST_WEAPONS] += ({ all[i] });
       else if (all[i]->query_armour())
          objects[_LANG_VAULTS_LIST_ARMOURS] += ({ all[i] });
       else if (all[i]->query_shield())
          objects[_LANG_VAULTS_LIST_SHIELDS] += ({ all[i] });
       else 
          objects[_LANG_VAULTS_LIST_OTHER] += ({ all[i] });
    }
    */

    for (i = 0; i < sizeof(all); i++)
    {
      int position;
      position = 0;
      if (all[i]->query_weapon())
      {
        if ((position = member_array(all[i]->query_short(), objects[_LANG_VAULTS_LIST_WEAPONS])) == -1)
            objects[_LANG_VAULTS_LIST_WEAPONS] += ({ all[i]->query_short(), all[i], 1 });
        else
           objects[_LANG_VAULTS_LIST_WEAPONS][position+2] = objects[_LANG_VAULTS_LIST_WEAPONS][position+2] + 1;
      }
      else if (all[i]->query_armour())
      {
        if ((position = member_array(all[i]->query_short(), objects[_LANG_VAULTS_LIST_ARMOURS])) == -1)
            objects[_LANG_VAULTS_LIST_ARMOURS] += ({ all[i]->query_short(), all[i], 1 });
        else
           objects[_LANG_VAULTS_LIST_ARMOURS][position+2] = objects[_LANG_VAULTS_LIST_ARMOURS][position+2] + 1;   
      }
      else if (all[i]->query_shield())
      {
        if ((position = member_array(all[i]->query_short(), objects[_LANG_VAULTS_LIST_SHIELDS])) == -1)
            objects[_LANG_VAULTS_LIST_SHIELDS] += ({ all[i]->query_short(), all[i], 1 });
        else
           objects[_LANG_VAULTS_LIST_SHIELDS][position+2] = objects[_LANG_VAULTS_LIST_SHIELDS][position+2] + 1;   
      }
      else if (all[i]->query_herb())
      {
        if ((position = member_array(all[i]->query_short(), objects[_LANG_VAULTS_LIST_HERBS])) == -1)
            objects[_LANG_VAULTS_LIST_HERBS] += ({ all[i]->query_short(), all[i], 1 });
        else
           objects[_LANG_VAULTS_LIST_HERBS][position+2] = objects[_LANG_VAULTS_LIST_HERBS][position+2] + 1;   
      }
      else if (all[i]->query_skin())
      {
        if ((position = member_array(all[i]->query_short(), objects[_LANG_VAULTS_LIST_SKINS])) == -1)
            objects[_LANG_VAULTS_LIST_SKINS] += ({ all[i]->query_short(), all[i], 1 });
        else
           objects[_LANG_VAULTS_LIST_SKINS][position+2] = objects[_LANG_VAULTS_LIST_SKINS][position+2] + 1;   
      }
      else 
      {
        if ((position = member_array(all[i]->query_short(), objects[_LANG_VAULTS_LIST_OTHER])) == -1)
            objects[_LANG_VAULTS_LIST_OTHER] += ({ all[i]->query_short(), all[i], 1 });
        else
           objects[_LANG_VAULTS_LIST_OTHER][position+2] = objects[_LANG_VAULTS_LIST_OTHER][position+2] + 1;   
      }
    }
    
    // list = keys(objects);    
    
    tmp  = "-------------------------------------------------------\n";
    tmp += "  " + _LANG_VAULTS_LIST_HEADER;
    tmp += "-------------------------------------------------------\n\n";
    
    for (j = 0; j < sizeof(list); j ++)
    {
      if (sizeof(objects[list[j]]))
      {
        tmp += "  %^BOLD%^"+capitalize(list[j])+"%^RESET%^:\n";
        for (i = 0; i < sizeof(objects[list[j]]); i+=3)
        {
          if (objects[list[j]][i+2] > 1)
          {
             tmp += "   " + capitalize(number_as_string(objects[list[j]][i+2])) + " " +
                        objects[list[j]][i+1]->query_plural() + "\n";
          }
          else
          {
             tmp += "   " + capitalize(objects[list[j]][i+1]->query_numeral()) + " " +
                  objects[list[j]][i]/*->query_short()*/ + "\n";
          }
        }
        tmp += "\n";
      }
    }

    capacity = file_size(query_save_file()+".o") * 100 / FILE_SIZE;

    /*
    tmp += "-------------------------------------------------------\n";
    tmp += "  Capacidad máxima:   "+FILE_SIZE+"\n" +
         "  Capacidad utilizada: "+file_size(query_save_file()+".o")+ "\n";
    tmp += "-------------------------------------------------------\n\n";
    */

    tmp += "-------------------------------------------------------\n";
    tmp += "  " + _LANG_VAULTS_LIST_USING; 
    tmp += "-------------------------------------------------------\n\n";
    
    this_user()->more_string(tmp);
    
  }
  else
    tell_object(this_player(), _LANG_VAULTS_LIST_EMPTY);

  vault->dest_me();
  return 1;
}

mixed * stats()
{
  return ({
    ({ "Vault Save File", save_file, }),
    ({ "Vault Log", vault_log, }),
    ({ "Vault Admins", vault_admins, }),
    ({ "Vault Guilds", vault_guilds, }),
    ({ "Vault Accepts", vault_accept_prop + " = " + vault_accept_value, }),
    ({ "Vault Max Items", vault_max_items, }),
  });
}
