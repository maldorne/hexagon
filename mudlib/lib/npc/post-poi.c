// post-poi.c -- reading the point of interest of the post an NPC works at, for
// the NPC components that act on the exits of a town entrance (guard,
// gatekeeper).
//
// A post is a location the NPC's job names; its town_entrance point of
// interest says which exits lead into town. Components read it here instead of
// storing the exits, so the post stays the one place that names them.

#include <areas/poi.h>
#include <room/location.h>

/**
 * The point of interest of the post an NPC works at.
 *
 * @param npc the NPC
 * @return the POI mapping, nil when the NPC has no post or the post marks none
 */
mapping query_post_poi(object npc)
{
  object post;
  mixed work;

  work = npc ? npc->query_work() : nil;
  if (!stringp(work) || !strlen(work))
    return nil;

  post = (object)load_object(LOCATION_HANDLER)->load_location(work);
  if (!post || !post->query_area())
    return nil;

  return ((mapping)post->query_area()->query_pois())[work];
}

/**
 * The exits an NPC watches: its own, when it has them, otherwise the ones the
 * town_entrance POI of its post names. Empty at a square or at a post that is
 * not an entrance.
 *
 * @param npc the NPC
 * @param own the exits the component was given, comma-separated, or nil
 * @return the exits, one per element
 */
string * query_post_directions(object npc, string own)
{
  mapping poi;
  string dirs;

  if (own && strlen(own))
    dirs = own;
  else
  {
    poi = query_post_poi(npc);
    if (!poi || poi[POI_FIELD_KIND] != POI_KIND_TOWN_ENTRANCE ||
        !stringp(poi[POI_FIELD_GUARD_DIR]))
      return ({ });
    dirs = poi[POI_FIELD_GUARD_DIR];
  }

  return explode(dirs, ",") - ({ "" });
}
