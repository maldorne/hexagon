
// Base class every custom prop blueprint inherits from.
//
// Inheritance is only required for the handful of types whose action
// logic cannot be expressed declaratively in /lib/tables/props_table.c
// (a fountain that heals, a statue that consults a quest handler, …).
// Decorative types — chair, table, statue, tapestry — live entirely as
// table entries and never need a `.c` file under /lib/location/props/.
//
// PER-INSTANCE STATE LIVES ELSEWHERE
// ---------------------------------
// This blueprint is stateless. The state and overrides of each instance
// live on the location component (/lib/location/components/props.c) as
// data inside the location's component_info. The blueprint receives
// them by reference on every call and may mutate `state` directly;
// LPC mappings are reference types, so mutations persist with the next
// save_me() on the location.
//
// DISPATCH CONTRACT
// -----------------
// The component owns the action dispatch. When the registry hands it
// a custom plan, the component calls:
//
//   blueprint->handle(verb, args, state, overrides, player, location)
//
// `handle` is the single entry point; subclasses switch on `verb`.
// Returning 0 means "I do not satisfy this action", and the component
// falls through to a global cmd with the same verb (if any). Returning
// 1 means the action was consumed.
//
// OPTIONAL OVERRIDE HOOKS
// -----------------------
// Subclasses MAY redefine any of the query_* helpers below to tailor
// rendering for state-aware descriptions ("It lies on its side." when
// state.tipped is set, "The fountain is dry." when state.dried_up).
// Returning nil from any of these tells the registry to fall back to
// the table-driven default. The base implementations all return nil so
// a custom blueprint can opt in piecewise.
//
// CONTRACT SUMMARY
// ----------------
// Required (subclass must redefine):
//   string  query_type_id();
//   int     handle(string verb, string args, mapping state,
//                  mapping overrides, object player, object location);
//
// Optional (return nil to fall back to table defaults):
//   string  query_long(mapping state, mapping overrides,
//                      string instance_id);
//   string  query_render_line(mapping state, mapping overrides);
//   string  query_short(mapping state, mapping overrides);
//   string * query_id_list();
//   string * query_supported_verbs(mapping overrides);

#include <room/prop.h>

// Marker that lets the registry distinguish a prop blueprint from any
// other random object that happens to expose query_type_id().
nomask int query_location_prop() { return 1; }

// ------------------------------------------------------------
//  Required surface — base stubs that subclasses MUST override.
// ------------------------------------------------------------

// Returns the type id used by the registry to register this blueprint
// as the `custom` entry for that type. Must match exactly the key the
// table uses for the same type.
string query_type_id() { return nil; }

// Single dispatch entry point. The component routes verbs marked as
// PROP_PLAN_CUSTOM in the type spec to this method. Subclass switches
// on `verb`; default returns 0 so an unconfigured blueprint never
// silently consumes an action.
int handle(string verb, string args, mapping state, mapping overrides,
           object player, object location)
{
  return 0;
}

// ------------------------------------------------------------
//  Optional surface — base returns nil; the registry then falls
//  back to whatever the table declares for this type.
// ------------------------------------------------------------

// Full description shown on `look <prop>`. Override to make it
// state-aware (e.g. a "dry" fountain returns different text when
// state["dried_up"] is set). `instance_id` is the per-location id used
// when multiple instances of the same type coexist.
string query_long(mapping state, mapping overrides, string instance_id)
{
  return nil;
}

// Single-line entry composed inside the props section in look output.
// Override to apply state-driven modifiers the table cannot express
// (e.g. composing material + texture into one phrase).
string query_render_line(mapping state, mapping overrides)
{
  return nil;
}

// Short form, mostly for nominal references in messages. Rarely
// needed; override only if state changes the noun phrase ("a lit
// torch" vs "an unlit torch").
string query_short(mapping state, mapping overrides)
{
  return nil;
}

// Parser id list. Override when the type accepts ids that depend on
// instance shape (rare). Default behavior: registry combines the
// table's id_list with the instance override id.
string * query_id_list()
{
  return nil;
}

// Verbs this type supports for the dispatcher's add_action walk in
// init(). Override when the available verbs depend on overrides in a
// way the table cannot express. Default behavior: registry computes
// the supported set from the table's action keys minus any nilled out
// in `overrides[PROP_OVERRIDE_ACTIONS]`.
string * query_supported_verbs(mapping overrides)
{
  return nil;
}
