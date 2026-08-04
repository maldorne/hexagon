// generated.c — a generic mob with no hard-coded content. Its data is loaded
// from an NPC data template (see /lib/handlers/bestiary and
// dev/area-npc-system.md): the bestiary snapshots a hand-authored NPC .c into
// a template once, and every spawn clones this object and restores the
// template into it. The original .c is a one-time seed, never reloaded here.
//
// Shares the /lib/monster.c inherit tree with the source NPCs so that a
// template saved from any monster restores cleanly into this object.

inherit "/lib/monster.c";

// No hard-coded setup: the template supplies name, race, class, stats, etc.
void setup()
{
}

int query_generated() { return 1; }
