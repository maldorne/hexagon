
// begin.c

#define _LANG_START_SHORT "%^BOLD%^GREEN%^Emerald %^WHITE%^Grove%^RESET%^"
#define _LANG_START_LONG "You step into a shimmering portal and find yourself in " + \
          "a vibrant and fantastical world filled with the sweet scent of blooming " + \
          "flowers and the distant sound of chirping birds. Before you stretches " + \
          "a vast landscape of rolling hills, towering mountains, and lush " + \
          "vegetation, inhabited by colorful creatures you've never seen before. " + \
          "As you take in the sights and sounds of this new world, " + \
          "a sense of excitement and anticipation builds inside you, " + \
          "eager to embark on your journey and discover what wonders and dangers lie ahead."  
#define _LANG_START_HINTS ({ "Take a look to every item here. You can only choose one of them.", \
          "Write 'choose <item>' to choose your character class." })

// begin2.c

#define _LANG_START2_SHORT "%^BOLD%^GREEN%^Emerald %^WHITE%^Canopy%^RESET%^"
#define _LANG_START2_LONG "As you walk a little further, you find yourself at " + \
          "the edge of a dense forest that looms ahead, its emerald canopy " + \
          "towering high above your head. The chirping of birds grows louder, " + \
          "accompanied by the rustling of leaves and the distant sound of " + \
          "trickling water. The air is cooler here, and the sweet scent of " + \
          "blooming flowers is replaced by the earthy aroma of damp soil and " + \
          "fresh greenery. You feel a sense of intrigue and trepidation as " + \
          "you gaze into the heart of the forest, wondering what secrets " + \
          "and challenges lie within. Around you, you see " + \
          number_as_string(num_races) + " %^BOLD%^chest" + ((num_races == 1) ? "" : "s") + \
          "%^RESET%^, placed " + \
          "as if you were the center of all existance."
#define _LANG_START2_CHESTS_NAME "chest"
#define _LANG_START2_CHESTS ({ "chests", "chest" })
#define _LANG_START2_CHESTS_DESC "You can see around you " + number_as_string(num_races) + " small wooden chests with portraits " + \
          "of the diverse races of " + game_pretty_name(this_object()) + ". Specifically, " + \
          "you can see the portraits of " + query_multiple_short(keys(races)) + ". " + \
          "%^BOLD%^You can look at each one of them separately " + \
          "to observe them (i.e. <look chest " + keys(races)[random(num_races)] + ">).%^RESET%^\n"
#define _LANG_START2_HINTS ({ "Take a look to every chest here. You can only open one of them.", \
          "Write 'open chest " + r + "' to choose you character race." })
#define _LANG_START2_LOOK_VERBS ({ "look" })
#define _LANG_START2_OPEN_VERBS ({ "open" })
#define _LANG_START2_CHEST_RACE "Chest with the portrait of some "+r+"."
#define _LANG_START2_CHEST_NOT_FOUND "Open what? Maybe you would like to 'open chest <race>'.\n"
#define _LANG_START2_OPEN_MSG "You open the chest with the portrait of some "+r+", noting how it " + \
    "begins to draw you in before you lose consciousness.\n"

// begin3.c

#define _LANG_START3_SHORT "%^BOLD%^Emerald %^GREEN%^Cave%^RESET%^"
#define _LANG_START3_LONG "You wake up in a strange place, in a cave with stone " + \
          "walls and a rock ceiling. In the center, you can see a small reddish " + \
          "orb at a low height from the ground. You are struck by the way " + \
          "the orb appears to be floating, as if it were a transparent " + \
          "crystal ball without any kind of support."
#define _LANG_START3_HINTS ({ "This orb is here to help you finish the creation process.", \
          "Type 'adjust' to customize your character's features." })
