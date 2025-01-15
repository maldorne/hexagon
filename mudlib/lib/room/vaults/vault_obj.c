// Vault object- Radix 1996
// Reviewed, neverbot 7/03
 
inherit container "/lib/core/basic/container";
inherit auto_load "/lib/core/basic/auto_load";

#include <room/storage.h>
#include <mud/secure.h>

mapping auto_load;
static string savefile;

void create()
{
  auto_load = ([ ]);
  savefile = "";

  ::create();
  reset_get();
}
 
// unique items, neverbot 08/07/04
int query_vault_object() { return 1; }
string query_savefile() { return savefile; }

void save_me()
{
  seteuid(ROOM_EUID);
  save_object(savefile);
  seteuid(PLAYER_EUID);  
}

void restore_me()
{
  seteuid(ROOM_EUID);
  restore_object(savefile);
  seteuid(PLAYER_EUID);
}

void set_save_file(string file) 
{
  object * olist;
  string err;

  savefile = file;
  auto_load = ([ ]);

  if (!environment() || 
     (!environment()->query_vault_room() && !environment()->query_shop()))
  {
    destruct(this_object()); // die screaming
    return;
  }
  
  restore_me();

  if (map_sizeof(auto_load))  
    olist = load_auto_load(auto_load, this_object());
  else
    olist = ({ });

  environment()->add_property(VAULT_USE_PROP, 1);
}
 
void dest_me() 
{
  object * olist;
  int i;

  olist = all_inventory(this_object());

  if (!savefile || !environment() || 
     (!environment()->query_vault_room() && !environment()->query_shop()) )
  {
    destruct(this_object()); // die screaming
    return;
  }
  
  if (sizeof(olist)) 
    auto_load = create_auto_load(olist);
  else
    auto_load = ([ ]);

  save_me();
  
  // unique items, neverbot 08/07/04
  for (i = 0; i < sizeof(olist); i++)
  {
    if (olist[i] && olist[i]->query_unique_object())
   {
      debug("uniques", "Unique object in vault_obj, destroy.\n");
      olist[i]->dest_unique();
    }
  }

  environment()->remove_property(VAULT_USE_PROP);

  ::dest_me();
}

// easier find_match, to be used in shop, inventories, etc, neverbot 04/2009
mixed simple_find_match(string str)
{
  object * list;
  string * id_list;
  string * id_list_plurals;
  object * result;
  int i;
  
  list = ({ });
  id_list = ({ });
  id_list_plurals = ({ });
  result = ({ });

  if (str == "" || !str)
    return ({ });

  list = (object *)find_inv_match(str);
  if (!sizeof(list))
    return ({ });

  // if (no_hidden)
  //   list = filter(list, "not_hidden", this_object());  

  for (i = 0; i < sizeof(list); i++)
  {
    id_list = ({ list[i]->query_name(), list[i]->query_short(), }) + 
    list[i]->query_alias();

    id_list_plurals = list[i]->query_plurals() + 
    ({ list[i]->query_main_plural(),  });

    if (member_array(str, id_list) != -1)
      return ({ list[i] });

    if (member_array(str, id_list_plurals) != -1)
      result += ({ list[i] });
  }

  return result;  
}