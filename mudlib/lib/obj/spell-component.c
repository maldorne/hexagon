// A component of a spell: an item the caster must carry (or may carry, to make
// the spell stronger) and that the spell uses up when it works. The spell
// asks for it by file or by id (see set_components and
// set_optional_components in /lib/effects/base_effect.c) and the match is
// always by the id set here, never by the name of the item.

inherit "/lib/item.c";

private string component_id;

/**
 * The id the spells that ask for this component match it by.
 * @param  id  a plain word, e.g. "unicorn-rune"
 */
void set_spell_component(string id) { component_id = id; }

/**
 * @return the id of this component, nil when it is not set
 */
string query_spell_component() { return component_id; }
