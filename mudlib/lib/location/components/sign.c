/*
 * Sign component.
 *
 * A sign is deliberately a real item (a clone of /lib/item.c), not a
 * prop: it lists on its own line in the room contents, which gives it
 * the visual weight a sign deserves, and it carries the `read` / `leer`
 * verb through /lib/core/basic/read_desc.c.
 *
 * The item itself is transient — its read data is static, so it never
 * survives the location's .o. This component is the durable owner: it
 * keeps the sign's definition in the location's autoload channel and,
 * on every load, re-materialises the item by calling the location's
 * inherited add_sign() (from /lib/room/sign.c).
 *
 * Only one sign per location is supported, which matches how signs are
 * used in the world (an entrance, a crossroads, a single posted board).
 *
 * add_sign signature (from /lib/room/sign.c):
 *   add_sign(string long, string read_mess,
 *            varargs string name, string short,
 *            string frame_style, string lang)
 */

#include <room/location.h>

inherit component "/lib/location/component.c";

private string sign_long;    // the sign's description (its long)
private string sign_text;    // what is written on it (the read message)
private string sign_name;    // parser id; nil => add_sign's default
private string sign_short;   // contents-listing short; nil => capitalized name
private string sign_frame;   // frame style for the read rendering; nil => default
private string sign_lang;    // written language of the text; nil => standard

void create()
{
  component::create();
  set_type(LOCATION_COMPONENT_SIGN);
}

// Re-materialise the sign item on the parent location. Runs once per
// load (init_components) and once when the component is first attached
// (add_component). Needs at least a description or some written text to
// be worth a sign.
void initialize(object loc)
{
  component::initialize(loc);

  if (!loc)
    return;
  if (!sign_long && !sign_text)
    return;

  loc->add_sign(sign_long, sign_text, sign_name, sign_short,
                sign_frame, sign_lang);
}

// ************************************************************
//  Definition — set by the conversion path / builder
// ************************************************************

void set_sign(string long, string text, varargs string name,
              string short, string frame, string lang)
{
  sign_long  = long;
  sign_text  = text;
  sign_name  = name;
  sign_short = short;
  sign_frame = frame;
  sign_lang  = lang;
}

string query_sign_long()  { return sign_long; }
string query_sign_text()  { return sign_text; }
string query_sign_name()  { return sign_name; }
string query_sign_short() { return sign_short; }
string query_sign_frame() { return sign_frame; }
string query_sign_lang()  { return sign_lang; }

// ************************************************************
//  Persistence
// ************************************************************

mapping query_auto_load_attributes()
{
  return component::query_auto_load_attributes() +
         ([
           "sign_long":  sign_long,
           "sign_text":  sign_text,
           "sign_name":  sign_name,
           "sign_short": sign_short,
           "sign_frame": sign_frame,
           "sign_lang":  sign_lang,
         ]);
}

void init_auto_load_attributes(mapping args)
{
  component::init_auto_load_attributes(args);

  if (!undefinedp(args["sign_long"]))  sign_long  = args["sign_long"];
  if (!undefinedp(args["sign_text"]))  sign_text  = args["sign_text"];
  if (!undefinedp(args["sign_name"]))  sign_name  = args["sign_name"];
  if (!undefinedp(args["sign_short"])) sign_short = args["sign_short"];
  if (!undefinedp(args["sign_frame"])) sign_frame = args["sign_frame"];
  if (!undefinedp(args["sign_lang"]))  sign_lang  = args["sign_lang"];
}

// ************************************************************
//  Stats
// ************************************************************

mixed * stats()
{
  return component::stats() +
         ({
           ({ "Sign long",  sign_long,  }),
           ({ "Sign text",  sign_text,  }),
           ({ "Sign name",  sign_name,  }),
           ({ "Sign short", sign_short, }),
           ({ "Sign frame", sign_frame, }),
           ({ "Sign lang",  sign_lang,  }),
         });
}
