

// common parts of descriptions

#define __ROAD_LONG "You are on a cobblestone road."
#define __ROAD_MOUNTAINS "On the horizon, you can see " + \
        "a mountainous landscape."
#define __ROAD_NO_TREES "This area has no trees, and you can only find bushes and weeds."
#define __ROAD_NEAR_FOREST "On the horizon you can see a beautiful wooded landscape near you."
#define __ROAD_FOREST "Around you you can see a variety of trees, in different sizes and shapes."
#define __ROAD_FAR_FOREST "In the distance, but not very far away, you can see a dense forest."

// rooms

#define _LANG_ROAD_SHORT "Road"
#define _LANG_ROAD_LONG __ROAD_LONG + " " + __ROAD_MOUNTAINS + " " + __ROAD_NO_TREES + "\n"
#define _LANG_ROAD_TEMPLE "To the east there is a building that looks like a temple.\n"
#define _LANG_ROAD_NEAR_FOREST_SHORT _LANG_ROAD_SHORT + " in the forest"
#define _LANG_ROAD_NEAR_FOREST_LONG __ROAD_LONG + " " + __ROAD_NEAR_FOREST + " " + __ROAD_NO_TREES + "\n"
#define _LANG_ROAD_FOREST_LONG __ROAD_LONG + " " + __ROAD_FOREST + "\n"
#define _LANG_ROAD_FAR_FOREST_LONG __ROAD_LONG + " " + __ROAD_FAR_FOREST + "\n"

#define _LANG_RAISEROOM_SHORT "%^CYAN%^Raise Room%^RESET%^"
#define _LANG_RAISEROOM_LONG "This place makes you remember a temple, as you can see " + \
    "a big altar, and behind it a statue representing the God Lummen. If your devotion " + \
    "is pure, you will be able to recover your mortal body.\n"
#define _LANG_RAISEROOM_STATUE ({ "statue" })
#define _LANG_RAISEROOM_STATUE_DESC "Forged in metal, it represents the God Lummen, " + \
    "carrier of the souls and benefactor of the priests.\n"
#define _LANG_RAISEROOM_ALTAR ({ "altar" })
#define _LANG_RAISEROOM_ALTAR_DESC "A modest altar of polished stone.\n"

#define _LANG_GUILD_SHORT "Adventurer's Home"
#define _LANG_GUILD_LONG "Here young apprentices from anywhere " + \
  "can share their belongings to help each other, give advice, train, " + \
  "and support their peers. At the end of the room you can see a vault " + \
  "where the adventurers can deposit their belongings, with only one condition: " + \
  "here everything has to be shared, so nothing has an official owner. If you take " + \
  "something you need, return it when you can, for the greater good. " + \
  "Here you can see a big board where adventurers can write messages.\n"

#define _LANG_VAULT_SHORT "Adventurer's Home: Vault Room"
#define _LANG_VAULT_LONG "Here young apprentices from anywhere " + \
  "can share their belongings to help each other, give advice, train, " + \
  "and support their peers. You can see a vault " + \
  "where the adventurers can deposit their belongings, with only one condition: " + \
  "here everything has to be shared, so nothing has an official owner. If you take " + \
  "something you need, return it when you can, for the greater good.\n"

#define _LANG_CROSSING_SHORT "Crossing"
#define _LANG_CROSSING_LONG "A crossing where you can take three different roads. " + \
        "To the east and southwest, the roads go through a forest " + \
        "but the northwest road goes to the mountains.\n"
#define _LANG_CROSSING_SIGN_LONG "A metal sign with some indications.\n"
#define _LANG_CROSSING_SIGN_MESS "  East      : Naduk      \n" + \
                                 "  Southwest : Erken      \n" + \
                                 "  Northwest : Mountains  \n"
#define _LANG_CROSSING_SIGN_SHORT "Metal Sign"
#define _LANG_CROSSING_SIGN_NAME "sign"




