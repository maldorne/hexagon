
inherit component "/lib/location/component.c";

inherit sign      "/lib/ventures/shop-sign.c";
inherit inventory "/lib/ventures/shop-inventory.c";


void create()
{
  component::create();
  inventory::create();
  set_type("shop");
}

// will be called after creation + init_auto_load_attributes,
// providing de parent location object
void initialize(object location)
{
  object sign;

  component::initialize(location);

  // add a sign with available commands
  sign = create_sign(location);
  sign->move(location);

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

mixed stats()
{
  return component::stats() +
        inventory::stats();
}
