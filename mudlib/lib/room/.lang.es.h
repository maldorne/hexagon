
// sign

#define _LANG_DEFAULT_SIGN_NAME "cartel"

// exits

#define _LANG_NO_EXITS "sin salidas"

// water and coast (see water.c, coast.c)

#define _LANG_WATER_SWIM_MESS "$N nada hacia $T."
#define _LANG_WATER_ARRIVE_MESS "$N llega nadando."
#define _LANG_WATER_SWEPT_MESS "La corriente arrastra a $N hacia $T."
#define _LANG_WATER_SWEPT_ME "No consigues avanzar: tragas agua y la corriente te arrastra.\n"
#define _LANG_WATER_SWIM_FAILED "No consigues avanzar y tragas agua.\n"
#define _LANG_WATER_DRIFT_ME "Te dejas llevar y la corriente te arrastra.\n"
#define _LANG_WATER_HURT_ME "La corriente te golpea contra las rocas y te arrastra bajo el agua.\n"
#define _LANG_WATER_HURT_ROOM "La corriente golpea a $N contra las rocas y lo arrastra bajo el agua."
#define _LANG_WATER_OBJECT_LOST "La corriente se lleva $O y lo pierde bajo el agua."
#define _LANG_WATER_OBJECT_SHORE_FROM "La corriente arrastra $O hasta la orilla, hacia $T."
#define _LANG_WATER_OBJECT_SHORE_TO "La corriente arrastra $O hasta la orilla."
#define _LANG_WATER_OBJECT_DRIFT_FROM "La corriente arrastra $O hacia $T."
#define _LANG_WATER_OBJECT_DRIFT_TO "La corriente trae $O."
#define _LANG_WATER_SHORE "Puedes ver la orilla hacia "
#define _LANG_WATER_NO_SHORE "No ves la orilla por ninguna parte.\n"
#define _LANG_COAST_SWIM_MESS "$N se mete en el agua y nada hacia $T."
#define _LANG_COAST_LOOK "Estás en la orilla. Escribe 'nadar' para meterte en el agua.\n"
#define _LANG_COAST_GLANCE "Estás en la orilla.\n"
#define _LANG_WATER_LOOK "Estás en el agua. Escribe 'nadar <dirección>' para avanzar.\n"
#define _LANG_WATER_SWIM_ME "Consigues nadar hacia " + ROOM_HAND->query_exit_dir(dir) + ".\n"
#define _LANG_COAST_SWIM_ME "Te metes en el agua y nadas hacia " + ROOM_HAND->query_exit_dir(dir) + ".\n"

// bridge cells (see bridge.c)

#define _LANG_BRIDGE_LOOK_ON "Estás sobre el puente; el agua corre por debajo.\n"
#define _LANG_BRIDGE_GLANCE_ON "Estás sobre el puente.\n"
#define _LANG_BRIDGE_LOOK_WATER "Estás en el agua, bajo el puente. Escribe 'nadar <dirección>' para avanzar.\n"
#define _LANG_BRIDGE_LOOK_WATER_REACH "Estás en el agua, bajo el puente. Escribe 'nadar <dirección>' para avanzar o 'trepar' para subir al puente.\n"
#define _LANG_BRIDGE_ON_BRIDGE_NO_SWIM "Estás sobre el puente; si quieres ir al agua, salta.\n"
#define _LANG_BRIDGE_FROM_WATER "No puedes subir al puente desde el agua.\n"
#define _LANG_BRIDGE_JUMP_ME "Saltas desde el puente al agua.\n"
#define _LANG_BRIDGE_JUMP_ROOM "$N salta desde el puente al agua."
#define _LANG_BRIDGE_ALREADY_IN_WATER "Ya estás en el agua.\n"
#define _LANG_BRIDGE_FALL_ME "Caes al agua.\n"
#define _LANG_BRIDGE_FALL_ROOM "$N cae al agua."
#define _LANG_BRIDGE_TOO_HIGH "El puente está demasiado alto.\n"
#define _LANG_BRIDGE_ALREADY_ON "Ya estás sobre el puente.\n"
#define _LANG_BRIDGE_CLIMB_ME "Te agarras a los pilares y consigues trepar de nuevo al puente.\n"
#define _LANG_BRIDGE_CLIMB_ROOM "$N trepa desde el agua hasta el puente."
#define _LANG_BRIDGE_CLIMB_FAIL "Resbalas, tragas agua y vuelves a caer.\n"
#define _LANG_BRIDGE_OUT_OF_REACH_FROM_BRIDGE "Desde el puente no alcanzas a quien está en el agua.\n"
#define _LANG_BRIDGE_OUT_OF_REACH_FROM_WATER "Desde el agua no alcanzas a quien está sobre el puente.\n"
#define _LANG_BRIDGE_HINT_CLIMB "Estás pasando bajo un puente, prueba a 'trepar'."
