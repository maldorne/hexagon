
inherit component  "/lib/location/component.c";

inherit sign       "/lib/ventures/shop-sign.c";
inherit inventory  "/lib/ventures/shop-inventory.c";
inherit actions    "/lib/ventures/shop-actions.c";
inherit messages   "/lib/ventures/shop-messages.c";
inherit attendable "/lib/ventures/attendable.c";
inherit prices     "/lib/ventures/prices.c";

// allow items in inventory, a vault_obj will be moved here
// when operating with the shop contents
int test_add(object ob, int flag) { return 1; }
int test_remove(object ob, int flag) { return 1; }

void create()
{
  inventory::create();
  attendable::create();
  messages::create();
  component::create();

  set_type("shop");
}

void init() 
{
  // component::init();
  actions::init();
}

void dest_me() 
{
  inventory::dest_me();
}

// will be called after creation + init_auto_load_attributes,
// providing the parent location object
void initialize(object location)
{
  component::initialize(location);

  // add a sign with available commands
  create_sign(location);

  // force create needed directories if they do not exist
  this_object()->check_directories();

  // include the shop in the ventures handler
  handler("ventures")->include_shop(location->query_file_name());
}

mapping query_auto_load_attributes() 
{
  return component::query_auto_load_attributes() +
    ([ "permanent_goods" : permanent_goods, ]);
}

void init_auto_load_attributes(mapping args) 
{
  if (!undefinedp(args["permanent_goods"]))
    permanent_goods = args["permanent_goods"];

  component::init_auto_load_attributes(args);
}

mixed * stats()
{
  return component::stats() +
        inventory::stats() +
        attendable::stats();
}

// *******************************************************
//  specific functions for the shop component
// *******************************************************

// allows vault objects to be cloned in this object
int query_shop(){ return 1; }

string query_save_file_name()
{
  string file_name, game;

  file_name = query_my_location()->query_file_name();
  game = game_from_path(file_name);

  file_name = replace_string(file_name, "/save/games/" + game + "/locations", "");
  file_name = replace_string(file_name, ".o", "");

  return "/save/games/" + game + "/ventures/" + 
    implode(explode(file_name, "/"), "-") + "-save.o";
}

string query_log_file_name()
{
  string file_name, game;

  file_name = query_my_location()->query_file_name();
  game = game_from_path(file_name);

  file_name = replace_string(file_name, "/save/games/" + game + "/locations", "");
  file_name = replace_string(file_name, ".o", "");

  return "/save/games/" + game + "/logs/ventures/" + 
    implode(explode(file_name, "/"), "-") + "-log";
}

void check_directories()
{
  string * dirs;
  string * pieces;
  int i;

  dirs = ({ query_save_file_name(), query_log_file_name() });

  for (i = 0; i < sizeof(dirs); i++)
  {
    pieces = explode(dirs[i], "/");
    // remove last part
    dirs[i] = "/" + implode(pieces[0..sizeof(pieces) - 2], "/");
    mkdir(dirs[i]);
  }
}

// returns 1 if the object can be sold to the shop, 0 if can not
int check_sell(object ob) { return 1; }
