// basic component all location components will inherit from

inherit obj "/lib/core/object.c";

private string type;  // type of component (outside, shop, swamp, whatever)

void create()
{
  obj::create();
  type = "base";
}

// will be called after creation + init_auto_load_attributes,
// providing de parent location object
void initialize(object location)
{

}

// basic component functions
string query_type() { return type; }
void set_type(string t) { type = t; }

mapping query_auto_load_attributes() 
{
  return obj::query_auto_load_attributes() +
    ([ "type" : type, ]);
}

void init_auto_load_attributes(mapping args) 
{
  if (!undefinedp(args["type"]))
    type = args["type"];

  obj::init_auto_load_attributes(args);
}

mixed * stats()
{
  // for the components stats, give just the essential info
  // return obj::stats() +
  return ({
    ({ "Component Type", type, }),
         });
}
