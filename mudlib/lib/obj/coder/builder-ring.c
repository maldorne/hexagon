/*
 * One ring to build them all
 *
 * neverbot 06/06/24
 */

inherit "/lib/item.c";

#define BUILDER_RING_CONVERT_SYNTAX "convert < filename | dirname | here >"
#define BUILDER_RING_HELP "This ring can be used by coders to help them building areas.\n" + \
                 "Available commands:\n" + \
                 "\t" + BUILDER_RING_CONVERT_SYNTAX + "\n"

void create()
{
  ::create();
  
  set_name("builder ring");
  set_short("Builder ring");
  add_alias(({ "builder", "ring" }));

  set_main_plural("Builder rings");
  add_plural(({ "builder rings", "rings" }));
  set_long("This ring can be used by coders to help them building areas.\n");

  reset_drop();

  set_gender(2);
  set_weight(1);
}

string get_help(varargs string str) { return BUILDER_RING_HELP; }

void init()
{
  add_action("do_convert", "convert");

  ::init();
}

int do_mark(string str)
{
  if (!str || !strlen(str))
  {
    notify_fail("convert what?\n\n" + BUILDER_RING_CONVERT_SYNTAX + "\n");
    return 0;
  }

  if (!query_in_use())
  {
    notify_fail("You must equip the ring before using it.\n");
    return 0;
  }

  if (!this_player()->query_coder())
  {
    notify_fail("You won't be able to handle the power of this ring.\n");
    return 0;
  }

  tell_object(this_player(), "Something something.\n");
  return 1;
}

/*
mixed * stats()
{
  return ::stats() + ({
      ({ "Destination Path", destination_path, }),
      ({ "Destination Name", destination_name, }),
    });
}

mapping query_auto_load_attributes()
{
  return ([ "::" : ::query_auto_load_attributes() ]) + 
      ((strlen(destination_path)) ? ([ "destination path" : destination_path ]) : ([ ])) + 
      ((strlen(destination_name)) ? ([ "destination name" : destination_name ]) : ([ ]));
}

void init_auto_load_attributes(mapping attribute_map)
{
  if (!undefinedp(attribute_map["destination path"]))
    destination_path = attribute_map["destination path"];
  if (!undefinedp(attribute_map["destination name"]))
    destination_name = attribute_map["destination name"];
  if (!undefinedp(attribute_map["::"]))
    ::init_auto_load_attributes(attribute_map["::"]);
}
*/
