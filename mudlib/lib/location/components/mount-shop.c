// Mount shop component: the location sells mounts (horses, mounts, vehicles).
//
// Its only attribute is `stock`, a list of what is on sale, each entry
//
//     ([ "template" : "<template id>", "price" : <value> ])
//     ([ "file" : "/games/<game>/<path>.c", "price" : <value> ])
//
// a mount made from a bestiary template or cloned from a file, and the price in
// the base unit of the money. The label of an entry is the short of the mount.
// Whoever buys one is its owner: the mount is made in the location, tied to the
// buyer, saved in a file of theirs by the riding handler (/lib/handlers/riding.c),
// which also keeps the cap of mounts a player may own.
//
// The verbs are `list`, `examine <mount>` and `buy <mount>` (a mount is named by
// what it answers to, or by its number in the list). A sign in the location
// tells so.

#include <room/location.h>
#include <living/riding.h>
#include <living/persisted.h>
#include <basic/money.h>
#include <language.h>

inherit component "/lib/location/component.c";

// what is on sale, as the attribute `stock` gives it
private mapping * stock;

// the sign this component put up, so it can take it down again
private static object sign_item;

void create()
{
  component::create();
  stock = ({ });
  set_type(LOCATION_COMPONENT_MOUNT_SHOP);
}

void init() {}
void dest_me() {}

void initialize(object location)
{
  component::initialize(location);

  if (!location)
    return;

  sign_item = location->add_sign(_LANG_MOUNT_SHOP_SIGN_DESC,
                                 _LANG_MOUNT_SHOP_SIGN_TEXT);
}

/**
 * Takes the sign out of the location. The location destructs a component it
 * drops without calling it, so whoever removes this one calls this first.
 */
void take_down()
{
  if (sign_item)
    sign_item->dest_me();
  sign_item = nil;
}

/**
 * What is on sale, as it was given.
 */
mapping * query_stock() { return stock; }

/**
 * Puts a mount on sale and saves the location.
 * @param entry  ([ "template" : id, "price" : value ]) or
 *               ([ "file" : path, "price" : value ])
 */
void add_stock(mapping entry)
{
  stock += ({ entry });

  if (query_entity())
    query_entity()->save_me();
}

string query_info()
{
  return sizeof(stock) + " mounts";
}

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() + ([ "stock" : stock ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);

  if (pointerp(args["stock"]))
    stock = args["stock"];
}

mixed * stats()
{
  return component::stats() + ({ ({ "Stock", stock }) });
}

/**
 * The verbs of the shop.
 */
mapping query_component_actions()
{
  mapping out;
  string * verbs;
  int i;

  out = ([ ]);

  verbs = _LANG_MOUNT_SHOP_LIST_VERBS;
  for (i = 0; i < sizeof(verbs); i++)
    out[verbs[i]] = "do_list";

  verbs = _LANG_MOUNT_SHOP_EXAMINE_VERBS;
  for (i = 0; i < sizeof(verbs); i++)
    out[verbs[i]] = "do_examine";

  verbs = _LANG_MOUNT_SHOP_BUY_VERBS;
  for (i = 0; i < sizeof(verbs); i++)
    out[verbs[i]] = "do_buy";

  return out;
}

// A text of a template, which may be given per gender: the first one found.
private string text_of(mixed value)
{
  string * keys;

  if (stringp(value))
    return value;

  if (mappingp(value))
  {
    if (stringp(value["1"]))
      return value["1"];

    keys = map_indices(value);
    if (sizeof(keys) && stringp(value[keys[0]]))
      return value[keys[0]];
  }

  return "";
}

// Every name a mount answers to, in lower case.
private string * names_of(mixed name, mixed aliases, string label)
{
  string * names;
  int i;

  names = ({ lower_case(label) });

  if (strlen(text_of(name)))
    names += ({ lower_case(text_of(name)) });

  if (mappingp(aliases))
    aliases = aliases["1"];
  if (pointerp(aliases))
    for (i = 0; i < sizeof(aliases); i++)
      if (stringp(aliases[i]))
        names += ({ lower_case(aliases[i]) });

  return names;
}

// What is for sale, each ([ "entry", "label", "long", "names", "price" ]): the
// stock entries whose mount can be read.
private mapping * for_sale()
{
  mapping * goods, t, info;
  object ob;
  string label, game;
  int i;

  goods = ({ });
  game = game_name(query_entity());

  for (i = 0; i < sizeof(stock); i++)
  {
    info = nil;

    if (stringp(stock[i][MOUNT_TEMPLATE]))
    {
      t = BESTIARY_HANDLER->query_template(game, stock[i][MOUNT_TEMPLATE]);
      label = t ? text_of(t["short"]) : "";
      if (strlen(label))
        info = ([ "label" : label, "long" : text_of(t["long"]),
                  "names" : names_of(t["name"], t["aliases"], label) ]);
    }
    else if (stringp(stock[i][MOUNT_FILE]))
    {
      ob = nil;
      catch(ob = load_object(stock[i][MOUNT_FILE]));
      label = ob ? (string)ob->query_short() : "";
      if (strlen(label))
        info = ([ "label" : label, "long" : (string)ob->query_long(),
                  "names" : names_of((string)ob->query_name(),
                                     ob->query_alias(), label) ]);
    }

    if (!info)
      continue;

    info["entry"] = stock[i];
    info["price"] = (int)stock[i]["price"];
    goods += ({ info });
  }

  return goods;
}

// The place in `goods` of what somebody typed (its number, or a name after the
// articles are dropped), or -1.
private int find_mount(mapping * goods, string str)
{
  string * words;
  int i, number;

  words = explode(lower_case(trim(str)), " ") - ({ "" });
  while (sizeof(words) > 1 && member_array(words[0], _LANG_MOUNT_SHOP_ARTICLES) != -1)
    words = words[1 ..];
  str = implode(words, " ");

  if (sscanf(str, "%d", number) == 1 && "" + number == str)
    return number >= 1 && number <= sizeof(goods) ? number - 1 : -1;

  for (i = 0; i < sizeof(goods); i++)
    if (member_array(str, goods[i]["names"]) != -1)
      return i;

  return -1;
}

// A price in words.
private string price_in_words(int price)
{
  return handler("money", query_entity())->money_value_string(price);
}

// The first paragraph of a description, without the line breaks.
private string first_paragraph(string text)
{
  string * paragraphs;

  paragraphs = explode(text, "\n\n");
  return sizeof(paragraphs) ? replace_string(trim(paragraphs[0]), "\n", " ") : "";
}

/**
 * list: the mounts on sale and their prices.
 */
int do_list(string str)
{
  mapping * goods;
  string text, label, price_text;
  int i, number;

  goods = for_sale();
  if (!sizeof(goods))
  {
    notify_fail(_LANG_MOUNT_SHOP_EMPTY);
    return 0;
  }

  text = _LANG_MOUNT_SHOP_LIST_HEADER;
  for (i = 0; i < sizeof(goods); i++)
  {
    number = i + 1;
    label = goods[i]["label"];
    price_text = price_in_words(goods[i]["price"]);
    text += _LANG_MOUNT_SHOP_LIST_LINE;
  }

  write(text);
  return 1;
}

/**
 * examine <mount>: its short, the first paragraph of its description and its
 * price, and whether the buyer can pay it.
 */
int do_examine(string str)
{
  mapping * goods;
  string label, description, price_text, text;
  int idx;

  if (!strlen(str))
    return 0;

  goods = for_sale();
  idx = find_mount(goods, str);
  if (idx == -1)
    return 0;

  label = goods[idx]["label"];
  description = first_paragraph(goods[idx]["long"]);
  price_text = price_in_words(goods[idx]["price"]);

  text = _LANG_MOUNT_SHOP_EXAMINE;
  if (this_player()->query_value() < goods[idx]["price"])
    text += _LANG_MOUNT_SHOP_EXAMINE_CANNOT_AFFORD;

  write(text);
  return 1;
}

/**
 * buy <mount>: the buyer pays and is given the mount, which stands in the
 * location and is theirs. Refused over the cap of mounts or without the money.
 */
int do_buy(string str)
{
  mapping * goods;
  object buyer, mount, riding;
  string label, price_text;
  int idx, max;

  if (!strlen(str))
  {
    notify_fail(_LANG_MOUNT_SHOP_BUY_WHAT);
    return 0;
  }

  buyer = this_player();
  goods = for_sale();
  idx = find_mount(goods, str);
  if (idx == -1)
  {
    notify_fail(_LANG_MOUNT_SHOP_NOT_SOLD);
    return 0;
  }

  label = goods[idx]["label"];
  price_text = price_in_words(goods[idx]["price"]);
  riding = handler("riding", query_entity());
  max = (int)riding->query_max_owned_mounts();

  if ((int)riding->query_owned_count(buyer) >= max)
  {
    notify_fail(_LANG_MOUNT_SHOP_BUY_LIMIT);
    return 0;
  }

  if ((int)buyer->query_value() < goods[idx]["price"])
  {
    notify_fail(_LANG_MOUNT_SHOP_BUY_CANNOT_AFFORD);
    return 0;
  }

  mount = riding->add_mount(buyer, goods[idx]["entry"],
                            query_entity());
  if (!mount)
  {
    notify_fail(_LANG_MOUNT_SHOP_BUY_FAILED);
    return 0;
  }

  buyer->pay_money(handler("money", query_entity())->
                   create_money_array(goods[idx]["price"]));

  tell_object(buyer, _LANG_MOUNT_SHOP_BUY_ME);
  tell_room(query_entity(), _LANG_MOUNT_SHOP_BUY_ROOM, ({ buyer }));
  return 1;
}
