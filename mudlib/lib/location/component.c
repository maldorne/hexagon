// basic component all location components will inherit from

inherit "/lib/core/object.c";

private string type;        // type of component
private mapping properties; // component properties

void create()
{
  ::create();
  type = "base";
  properties = ([ ]);
}

// basic component functions
string query_type() { return type; }
void set_type(string t) { type = t; }

string * actions_provided()
{
  return ({ });
}

mapping query_auto_load_attributes() 
{
  return ::query_auto_load_attributes() +
         ([ "type" : type, ]);
}

void init_auto_load_attributes(mapping args) 
{
  if (!undefinedp(args["type"]))
    type = args["type"];

  ::init_auto_load_attributes(args);
}