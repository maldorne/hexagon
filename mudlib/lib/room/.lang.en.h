
// sign

#define _LANG_DEFAULT_SIGN_NAME "sign"

// exits

#define _LANG_NO_EXITS "no exits"

// water and coast (see water.c, coast.c)

#define _LANG_WATER_SWIM_MESS "$N swims towards $T."
#define _LANG_WATER_ARRIVE_MESS "$N arrives swimming."
#define _LANG_WATER_SWEPT_MESS "The current sweeps $N away towards $T."
#define _LANG_WATER_SWEPT_ME "You get nowhere: you swallow water and the current sweeps you away.\n"
#define _LANG_WATER_SWIM_FAILED "You get nowhere and swallow water.\n"
#define _LANG_WATER_DRIFT_ME "You let yourself go and the current sweeps you away.\n"
#define _LANG_WATER_HURT_ME "The current batters you against the rocks and drags you under.\n"
#define _LANG_WATER_HURT_ROOM "The current batters $N against the rocks and drags them under."
#define _LANG_WATER_OBJECT_LOST "The current carries $O off and it is lost under the water."
#define _LANG_WATER_OBJECT_SHORE_FROM "The current carries $O to the shore, towards $T."
#define _LANG_WATER_OBJECT_SHORE_TO "The current carries $O to the shore."
#define _LANG_WATER_OBJECT_DRIFT_FROM "The current carries $O away towards $T."
#define _LANG_WATER_OBJECT_DRIFT_TO "The current brings $O along."
#define _LANG_WATER_SHORE "You can see the shore towards "
#define _LANG_WATER_NO_SHORE "You cannot see the shore anywhere.\n"
#define _LANG_WATER_SWIM_WAYS "You can swim towards "
#define _LANG_COAST_SWIM_MESS "$N wades into the water and swims towards $T."
#define _LANG_COAST_LOOK "You are on the shore. Type 'swim' to get into the water.\n"
#define _LANG_COAST_GLANCE "You are on the shore.\n"
#define _LANG_WATER_LOOK "You are in the water. Type 'swim <direction>' to move on.\n"
#define _LANG_WATER_SWIM_ME "You manage to swim towards " + ROOM_HAND->query_exit_dir(dir) + ".\n"
#define _LANG_COAST_SWIM_ME "You wade into the water and swim towards " + ROOM_HAND->query_exit_dir(dir) + ".\n"

// bridge cells (see bridge.c)

#define _LANG_BRIDGE_LOOK_ON "You are on the bridge; the water runs below.\n"
#define _LANG_BRIDGE_GLANCE_ON "You are on the bridge.\n"
#define _LANG_BRIDGE_LOOK_WATER "You are in the water, under the bridge. Type 'swim <direction>' to move on.\n"
#define _LANG_BRIDGE_LOOK_WATER_REACH "You are in the water, under the bridge. Type 'swim <direction>' to move on or 'climb' to get up onto the bridge.\n"
#define _LANG_BRIDGE_ON_BRIDGE_NO_SWIM "You are on the bridge; if you want to get into the water, jump.\n"
#define _LANG_BRIDGE_FROM_WATER "You cannot get up onto the bridge from the water.\n"
#define _LANG_BRIDGE_JUMP_ME "You jump from the bridge into the water.\n"
#define _LANG_BRIDGE_JUMP_ROOM "$N jumps from the bridge into the water."
#define _LANG_BRIDGE_ALREADY_IN_WATER "You are already in the water.\n"
#define _LANG_BRIDGE_FALL_ME "You fall into the water.\n"
#define _LANG_BRIDGE_FALL_ROOM "$N falls into the water."
#define _LANG_BRIDGE_TOO_HIGH "The bridge is too high.\n"
#define _LANG_BRIDGE_ALREADY_ON "You are already on the bridge.\n"
#define _LANG_BRIDGE_CLIMB_ME "You grab the piers and manage to climb back onto the bridge.\n"
#define _LANG_BRIDGE_CLIMB_ROOM "$N climbs from the water up to the bridge."
#define _LANG_BRIDGE_CLIMB_FAIL "You slip, swallow water and fall back.\n"
#define _LANG_BRIDGE_OUT_OF_REACH_FROM_BRIDGE "You cannot reach whoever is in the water from the bridge.\n"
#define _LANG_BRIDGE_OUT_OF_REACH_FROM_WATER "You cannot reach whoever is on the bridge from the water.\n"
#define _LANG_BRIDGE_HINT_CLIMB "You are passing under a bridge, try to 'climb'."
