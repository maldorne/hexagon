
/*
 * props — coder/builder cmd to manage prop instances on the current
 * location. Design-time tool. See the Nottario doc dev/props.md for
 * the design.
 *
 * Subverbs:
 *   props list                        — print attached instances
 *   props types                       — registry catalogue
 *   props add <type> [<id>] [k=v ...] — append an instance
 *   props remove <id|type>            — drop an instance
 *   props set <id|type> <path> <val>  — write override or initial state
 *   props state <id|type> [reset]     — inspect or reset runtime state
 *
 * `k=v` and `<path>` accept dotted paths: top-level token routes
 * `overrides` writes through set_override (depth-2 supported), and
 * `state` writes through set_state.
 */

#include <mud/cmd.h>
#include <room/location.h>
#include <room/prop.h>

inherit CMD_BASE;

void setup()
{
  set_aliases(({ "props" }));
  set_usage("props list\n" +
            "props types\n" +
            "props add <type> [<id>] [overrides.k=v ...]\n" +
            "props remove <id|type>\n" +
            "props set <id|type> <dotted.path> <value>\n" +
            "props state <id|type> [reset]");
  set_help("Manage prop instances on the current location.\n" +
           "\n" +
           "  list    Print every instance attached here: type, id,\n" +
           "          overrides digest, current runtime state.\n" +
           "  types   List every type the registry knows about.\n" +
           "  add     Append a new instance of <type>. <id> is\n" +
           "          optional; if omitted, one is generated as\n" +
           "          <type>_<ordinal>. Extra k=v pairs accept dotted\n" +
           "          paths (overrides.material=metal,\n" +
           "          overrides.props.nailed=1, state.tipped=1).\n" +
           "  remove  Drop the first instance whose id or type\n" +
           "          matches.\n" +
           "  set     Write an override or initial state. Path is\n" +
           "          dotted: overrides.material,\n" +
           "          overrides.props.X, overrides.removed_actions.X,\n" +
           "          state.X. Value parsed as int when it looks\n" +
           "          like a number, nil for the literal 'nil',\n" +
           "          otherwise string.\n" +
           "  state   Print current runtime state of the matching\n" +
           "          instance. With 'reset' as the second token,\n" +
           "          re-applies the type's default_state (useful to\n" +
           "          recover from a mid-test mutation).\n" +
           "\n" +
           "The props component is lazily attached to the location on\n" +
           "the first `props add`; locations without props pay no cost.\n");
}

// ************************************************************
//  Helpers
// ************************************************************

// Parse a value string into the right LPC type:
//   "nil"   -> nil
//   "123"   -> int
//   "-7"    -> int
//   "foo"   -> "foo" (string)
//   "\"a b\"" -> "a b" (strips surrounding quotes for spaces)
static mixed _parse_value(string raw)
{
  int n;

  if (!raw) return nil;
  if (raw == "nil" || raw == "0") return raw == "0" ? 0 : nil;

  // quoted string
  if (strlen(raw) >= 2 && raw[0] == '"' && raw[strlen(raw)-1] == '"')
    return raw[1..strlen(raw)-2];

  // try int — round-trip check via the "" + scalar idiom so the parse
  // accepts "42" / "-7" but rejects "42x" or "0042".
  if (sscanf(raw, "%d", n) == 1 && ("" + n) == raw)
    return n;

  return raw;
}

// Forward protos for the recursive value/mapping/array formatters
// used by `props list` and `props state`. Hand-rolled instead of
// leaning on `to_string` (which is a debug-only pretty-printer that
// quotes strings and emits multi-line layout).
static string _fmt_value(mixed v);
static string _fmt_array(mixed * arr);
static string _fmt_map(mapping m);

static string _fmt_value(mixed v)
{
  if (v == nil)        return "nil";
  if (mappingp(v))     return _fmt_map(v);
  if (pointerp(v))     return _fmt_array(v);
  if (stringp(v))      return "\"" + v + "\"";
  return "" + v;       // int / float scalar via concat
}

static string _fmt_array(mixed * arr)
{
  string * pieces;
  int i;

  pieces = ({ });
  for (i = 0; i < sizeof(arr); i++)
    pieces += ({ _fmt_value(arr[i]) });
  return "[" + implode(pieces, ", ") + "]";
}

static string _fmt_map(mapping m)
{
  string * ks;
  string * pieces;
  int i;

  if (!m || !m_sizeof(m)) return "{}";
  ks = map_indices(m);
  pieces = ({ });
  for (i = 0; i < sizeof(ks); i++)
    pieces += ({ ks[i] + "=" + _fmt_value(m[ks[i]]) });
  return "{" + implode(pieces, ", ") + "}";
}

// Generate <type>_<ordinal> by counting existing instances of <type>.
static string _autogen_id(object comp, string type)
{
  mapping * existing;

  existing = comp->query_instances_by_type(type);
  return type + "_" + (sizeof(existing) + 1);
}

// Apply one dotted-path write. Returns 1 on success, 0 on failure.
// Top-level token routes to overrides or state setters on the component.
static int _apply_path(object comp, string handle, string dotted, mixed value)
{
  string * parts;
  string top;

  if (!dotted || !strlen(dotted)) return 0;
  parts = explode(dotted, ".");
  if (!sizeof(parts)) return 0;

  top = parts[0];

  if (top == "overrides")
  {
    if (sizeof(parts) < 2) return 0;
    return comp->set_override(handle,
                              implode(parts[1..], "."),
                              value);
  }

  if (top == "state")
  {
    if (sizeof(parts) < 2) return 0;
    if (sizeof(parts) > 2) return 0;     // depth 1 only for state
    return comp->set_state(handle, parts[1], value);
  }

  // No top route: assume overrides.<path>
  return comp->set_override(handle, dotted, value);
}

// Return environment(me) if it's a location, else nil + notify.
static object _here(object me)
{
  object env;

  env = environment(me);
  if (!env)
  {
    notify_fail("You are nowhere.\n");
    return nil;
  }
  if (!env->query_location())
  {
    notify_fail("You are not in a location.\n");
    return nil;
  }
  return env;
}

// Get or lazily attach the props component on the given location.
static object _ensure_props_component(object loc)
{
  object comp;

  comp = loc->query_component_by_type(LOCATION_COMPONENT_PROPS);
  if (comp) return comp;

  loc->add_component(LOCATION_COMPONENT_PROPS,
                     ([ "props_instances": ({ }), ]));
  return loc->query_component_by_type(LOCATION_COMPONENT_PROPS);
}

// ************************************************************
//  Subverbs
// ************************************************************

static int do_list(object me)
{
  object loc;
  object comp;
  mapping * instances;
  string out;
  int i;

  if (!(loc = _here(me))) return 0;

  comp = loc->query_component_by_type(LOCATION_COMPONENT_PROPS);
  if (!comp)
  {
    write("No props component attached to this location.\n");
    return 1;
  }

  instances = comp->query_prop_instances();
  if (!sizeof(instances))
  {
    write("No prop instances attached.\n");
    return 1;
  }

  out = "Prop instances (" + sizeof(instances) + "):\n";
  for (i = 0; i < sizeof(instances); i++)
  {
    mapping inst;
    inst = instances[i];
    out += sprintf("  [%d] type=%s id=%s\n      overrides=%s\n      state=%s\n      actions=%s\n",
                   i,
                   inst[PROP_FIELD_TYPE],
                   inst[PROP_FIELD_ID] ? inst[PROP_FIELD_ID] : "(none)",
                   _fmt_map(inst[PROP_FIELD_OVERRIDES]),
                   _fmt_map(inst[PROP_FIELD_STATE]),
                   comp->query_action_verbs(inst));
  }
  write(out);
  return 1;
}

static int do_types(object me)
{
  string * known;
  string out;
  int i;

  known = handler("props")->query_known_types();
  if (!sizeof(known))
  {
    write("The registry knows no prop types yet.\n");
    return 1;
  }

  out = "Known prop types (" + sizeof(known) + "):\n";
  for (i = 0; i < sizeof(known); i++)
  {
    mapping summary;
    summary = handler("props")->query_type_summary(known[i]);
    out += sprintf("  %-16s short=%s  custom=%d\n",
                   known[i],
                   summary && summary["short"] ? summary["short"] : "(none)",
                   summary && summary["custom"] ? 1 : 0);
  }
  write(out);
  return 1;
}

static int do_add(string args, object me)
{
  object loc;
  object comp;
  string * tokens;
  string type, id;
  mapping overrides;
  int i;

  if (!args || !strlen(args))
  {
    notify_fail("Usage: props add <type> [<id>] [overrides.k=v ...]\n");
    return 0;
  }
  if (!(loc = _here(me))) return 0;

  tokens = explode(args, " ");
  type = tokens[0];

  // verify the type is known
  if (member_array(type, handler("props")->query_known_types()) == -1)
  {
    notify_fail("Unknown prop type '" + type + "'. " +
                "Use `props types` to see the catalogue.\n");
    return 0;
  }

  // optional id is the next token if it has no `=`
  id = nil;
  i = 1;
  if (sizeof(tokens) > 1 && strsrch(tokens[1], "=") == -1)
  {
    id = tokens[1];
    i = 2;
  }

  comp = _ensure_props_component(loc);
  if (!comp)
  {
    notify_fail("Failed to attach the props component.\n");
    return 0;
  }

  if (!id) id = _autogen_id(comp, type);

  overrides = ([ ]);
  comp->add_prop_instance(type, id, overrides);

  // apply any k=v pairs after the type/id
  for ( ; i < sizeof(tokens); i++)
  {
    string path, raw_val;

    if (sscanf(tokens[i], "%s=%s", path, raw_val) != 2)
    {
      write("Skipping malformed pair: " + tokens[i] + "\n");
      continue;
    }
    if (!_apply_path(comp, id, path, _parse_value(raw_val)))
      write("Skipping unrecognised path: " + path + "\n");
  }

  loc->save_me();
  comp->refresh_actions();
  write("Added prop '" + type + "' (id=" + id + ").\n");
  return 1;
}

static int do_remove(string handle, object me)
{
  object loc;
  object comp;

  if (!handle || !strlen(handle))
  {
    notify_fail("Usage: props remove <id|type>\n");
    return 0;
  }
  if (!(loc = _here(me))) return 0;

  comp = loc->query_component_by_type(LOCATION_COMPONENT_PROPS);
  if (!comp)
  {
    notify_fail("No props component attached here.\n");
    return 0;
  }

  if (!comp->remove_prop_instance(handle))
  {
    notify_fail("No instance with id/type '" + handle + "'.\n");
    return 0;
  }

  loc->save_me();
  comp->refresh_actions();
  write("Removed.\n");
  return 1;
}

static int do_set(string args, object me)
{
  object loc;
  object comp;
  string * tokens;
  string handle, path, raw_val;

  if (!args || !strlen(args))
  {
    notify_fail("Usage: props set <id|type> <dotted.path> <value>\n");
    return 0;
  }
  if (!(loc = _here(me))) return 0;

  tokens = explode(args, " ");
  if (sizeof(tokens) < 3)
  {
    notify_fail("Usage: props set <id|type> <dotted.path> <value>\n");
    return 0;
  }

  handle = tokens[0];
  path = tokens[1];
  raw_val = implode(tokens[2..], " ");

  comp = loc->query_component_by_type(LOCATION_COMPONENT_PROPS);
  if (!comp)
  {
    notify_fail("No props component attached here.\n");
    return 0;
  }

  if (!_apply_path(comp, handle, path, _parse_value(raw_val)))
  {
    notify_fail("Failed to apply " + path + "; verify path and target id.\n");
    return 0;
  }

  loc->save_me();
  // refresh in case the set touched actions.<verb> (verb set changed).
  // Cheap for non-action paths too; idempotent.
  comp->refresh_actions();
  write("Set " + path + " on " + handle + ".\n");
  return 1;
}

static int do_state(string args, object me)
{
  object loc;
  object comp;
  string * tokens;
  string handle;
  mapping inst;

  if (!args || !strlen(args))
  {
    notify_fail("Usage: props state <id|type> [reset]\n");
    return 0;
  }
  if (!(loc = _here(me))) return 0;

  tokens = explode(args, " ");
  handle = tokens[0];

  comp = loc->query_component_by_type(LOCATION_COMPONENT_PROPS);
  if (!comp)
  {
    notify_fail("No props component attached here.\n");
    return 0;
  }

  // reset path
  if (sizeof(tokens) >= 2 && tokens[1] == "reset")
  {
    if (!comp->reset_state(handle))
    {
      notify_fail("No instance with id/type '" + handle + "'.\n");
      return 0;
    }
    loc->save_me();
    write("Reset state on " + handle + ".\n");
    return 1;
  }

  // inspect path
  inst = comp->query_instance_by_id(handle);
  if (!inst)
  {
    mapping * matches;
    matches = comp->query_instances_by_type(handle);
    if (sizeof(matches)) inst = matches[0];
  }
  if (!inst)
  {
    notify_fail("No instance with id/type '" + handle + "'.\n");
    return 0;
  }

  write("State on " + handle + ":\n  " +
        _fmt_map(inst[PROP_FIELD_STATE]) + "\n");
  return 1;
}

// Print the list of verbs accepted by the matched instance. Same
// data the player-facing "You can: …" hint uses (so what the coder
// sees here is exactly what the player would discover via look).
static int do_verbs(string handle, object me)
{
  object loc;
  object comp;
  mapping inst;

  if (!handle || !strlen(handle))
  {
    notify_fail("Usage: props verbs <id|type>\n");
    return 0;
  }
  if (!(loc = _here(me))) return 0;

  comp = loc->query_component_by_type(LOCATION_COMPONENT_PROPS);
  if (!comp)
  {
    notify_fail("No props component attached here.\n");
    return 0;
  }

  inst = comp->query_instance_by_id(handle);
  if (!inst)
  {
    mapping * matches;
    matches = comp->query_instances_by_type(handle);
    if (sizeof(matches)) inst = matches[0];
  }
  if (!inst)
  {
    notify_fail("No instance with id/type '" + handle + "'.\n");
    return 0;
  }

  write("Verbs on " + handle + " (type=" + inst[PROP_FIELD_TYPE] +
        "): " + comp->query_action_verbs(inst) + "\n");
  return 1;
}

// ************************************************************
//  Dispatch
// ************************************************************

static int cmd(string str, object me, string verb)
{
  string * args;
  string subverb;
  string rest;

  if (!str || !strlen(str))
  {
    notify_fail(query_usage() + "\n");
    return 0;
  }

  args = explode(str, " ");
  subverb = args[0];
  rest = sizeof(args) > 1 ? implode(args[1..], " ") : "";

  switch (subverb)
  {
    case "list":   return do_list(me);
    case "types":  return do_types(me);
    case "add":    return do_add(rest, me);
    case "remove": return do_remove(rest, me);
    case "set":    return do_set(rest, me);
    case "state":  return do_state(rest, me);
    case "verbs":  return do_verbs(rest, me);
    default:
      notify_fail("Unknown subverb '" + subverb + "'. " +
                  "Try one of: list, types, add, remove, set, state, verbs.\n");
      return 0;
  }
}
