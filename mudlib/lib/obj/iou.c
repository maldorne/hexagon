
inherit "/lib/item.c";
inherit "/lib/core/basic/auto_load.c";

mapping auto_load_info;
string file; // Raskolnikov

string stat();

void setup()
{
  set_name("iou");
  set_short("IOU");
  set_main_plural("IOUs");
  add_plural("ious");
  set_long("This is an IOU for an item that went missing for some unknown reason, " +
    "probably something obscure to do with gods meddling with things that man is " +
    "not meant to know of (The buggers have been messing with the fabric of reality again). " +
    "Hang onto it, and you never know, it might be reclaimed.\n" +
    "      \"reclaim\" to try to reclaim on the IOU.\n" +
    "      \"inspect iou\" to try to read the fine print on the IOU.\n");
}

string stat()
{
  string * path;

  if (undefinedp(auto_load_info)) 
    return "Nothing, totally useless, throw it away";

  path = explode(file, "/");

  if (sizeof(path) < 3) 
    return "Something wierd wat mortal man was not meant to mess with.\n";
  
  switch (path[0])
  {
    case "home":
      call_out("dest_me", 2, 0);
      return "Some gods toy, which aught not be in the hands of mortals.\n";
    case "games":
      return "It's for an item from the game "+path[1]+" and it's called "+path[sizeof(path)-1]+".\n";
    default:
      return "It's for some generic object know as "+path[sizeof(path)-1]+".\n";
  }

  return "Damned if I know really.\n";
}

void init()
{
  add_action("try_loading", "reclaim");
  add_action("inspect", "inspect");
}

mixed add_auto_load_info(string f, mixed str)
{
  file = f;
  auto_load_info = str;
  return auto_load_info;
}

mixed add_object(object ob)
{
  file = base_name(ob);
  auto_load_info = create_auto_load( ({ ob }) );
  return auto_load_info;
}

int inspect(string str)
{
  if (str == "iou" || str == "IOU") 
    write(stat());

  notify_fail("O.K. you've looked at everything closely now.\n");
  return 0;
}

int try_loading(string str)
{
  object * olist;

  if (!undefinedp(auto_load_info))
  {
    olist = load_auto_load(auto_load_info, this_player());
    
    if (sizeof(olist))
      write("A small demon hands you something and runs off with the IOU\n");
    else
      write("Nothing seems to happen.\n");
  }

  // if this did not work, a new iou would have been created, so
  // we destroy the current one 
  dest_me();
  return 1;
}

// By Radix
int query_iou_object() { return 1; }

mapping query_auto_load_attributes()
{
  return ([ 
      "::" : ::query_auto_load_attributes(),
      "auto load info" : auto_load_info
    ]);
}

void init_auto_load_attributes(mapping attribute_map)
{
  if (!undefinedp(attribute_map["auto load info"]))
    auto_load_info = attribute_map["auto load info"];
  if (!undefinedp(attribute_map["::"]))
    ::init_auto_load_attributes(attribute_map["::"]);
} 
