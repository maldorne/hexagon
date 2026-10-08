
#include <language.h>

inherit "/lib/item.c";
inherit "/lib/core/basic/auto_load.c";

mapping auto_load_info;
string file; // Raskolnikov

string stat();

void setup()
{
  set_name(_LANG_IOU_NAME);
  set_short(_LANG_IOU_SHORT);
  add_alias(_LANG_IOU_ALIASES);
  set_main_plural(_LANG_IOU_PLURAL);
  add_plural(_LANG_IOU_PLURALS);
  set_long(_LANG_IOU_LONG);
}

// what can be done with it, apart from the description
string long(varargs string s, int dark)
{
  return ::long(s, dark) + _LANG_IOU_USAGE;
}

string stat()
{
  string * path;

  if (undefinedp(auto_load_info)) 
    return _LANG_IOU_STAT_NOTHING;

  path = explode(file, "/");

  if (sizeof(path) < 3) 
    return _LANG_IOU_STAT_WEIRD;
  
  switch (path[0])
  {
    case "home":
      call_out("dest_me", 2, 0);
      return _LANG_IOU_STAT_HOME;
    case "games":
      return _LANG_IOU_STAT_GAME;
    default:
      return _LANG_IOU_STAT_GENERIC;
  }

  return _LANG_IOU_STAT_UNKNOWN;
}

void init()
{
  add_action("try_loading", _LANG_IOU_RECLAIM_VERBS);
  add_action("inspect", _LANG_IOU_INSPECT_VERBS);
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
  if (!str || !id(lower_case(str)))
  {
    notify_fail(_LANG_IOU_INSPECT_WHAT);
    return 0;
  }

  write(stat());
  return 1;
}

int try_loading(string str)
{
  object * olist;
  string * files;
  int i;

  if (!undefinedp(auto_load_info))
  {
    // older IOUs kept one attribute mapping per file instead of a list of them
    files = map_indices(auto_load_info);

    for (i = 0; i < sizeof(files); i++)
      if (mappingp(auto_load_info[files[i]]))
        auto_load_info[files[i]] = ({ auto_load_info[files[i]] });

    olist = load_auto_load(auto_load_info, this_player());
    
    if (sizeof(olist))
      write(_LANG_IOU_RECLAIMED);
    else
      write(_LANG_IOU_NOTHING_HAPPENS);
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
