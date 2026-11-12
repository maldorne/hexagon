
// unarmed_combat table
#define _LANG_UNARMED_DEFAULT_STYLE "fighting"

#define _LANG_UNARMED_INITIAL_ATT_MSGS ({ "you hit $defname $local", \
                                         "you punch $defname $local", \
                                         "you bruise $defname $local", \
                                         })
#define _LANG_UNARMED_INITIAL_DEF_MSGS ({ "$attname hits you $local", \
                                         "$attname punches you $local", \
                                         "$attname bruises you $local", \
                                         })
#define _LANG_UNARMED_INITIAL_ROOM_MSGS ({ "$attname hits $defname $local", \
                                         "$attname punches $defname $local", \
                                         "$attname bruises $defname $local", \
                                         })

#define _LANG_UNARMED_MEDIUM_ATT_MSGS ({ "you hit $defname $local", \
                                         "you punch $defname $local", \
                                         "you bruise $defname $local", \
                                         "you hit $defname hard $local", \
                                         })
#define _LANG_UNARMED_MEDIUM_DEF_MSGS ({ "$attname hits you $local", \
                                         "$attname punches you $local", \
                                         "$attname bruises you $local", \
                                         "$attname hits you hard $local", \
                                         })
#define _LANG_UNARMED_MEDIUM_ROOM_MSGS ({ "$attname hits $defname $local", \
                                         "$attname punches $defname $local", \
                                         "$attname bruises $defname $local", \
                                         "$attname hits $defname hard $local", \
                                         })

#define _LANG_UNARMED_EXPERT_ATT_MSGS ({ "you hit $defname $local", \
                                         "you punch $defname $local", \
                                         "you bruise $defname $local", \
                                         "you hit $defname hard $local", \
                                         "you hit $defname with fury $local", \
                                         })
#define _LANG_UNARMED_EXPERT_DEF_MSGS ({ "$attname hits you $local", \
                                         "$attname punches you $local", \
                                         "$attname bruises you $local", \
                                         "$attname hits you hard $local", \
                                         "$attname hits you with fury $local", \
                                         })
#define _LANG_UNARMED_EXPERT_ROOM_MSGS ({ "$attname hits $defname $local", \
                                         "$attname punches $defname $local", \
                                         "$attname bruises $defname $local", \
                                         "$attname hits $defname hard $local", \
                                         "$attanme hits $defname with fury $local", \
                                         })

// configurations

#define _LANG_CONFIG_INTERFACE ({ "interface", "terminal", "term" })
#define _LANG_CONFIG_INFO ({ "information", "info" })
#define _LANG_CONFIG_COMMUNICATION ({ "communication" })
#define _LANG_CONFIG_CHARACTER ({ "character", "player" })
#define _LANG_CONFIG_CONSENTS ({ "consents" })

#define _LANG_CONFIG_TOPIC_TERM "type"
#define _LANG_CONFIG_TOPIC_COLS "columns"
#define _LANG_CONFIG_TOPIC_STATUS "status bar"
#define _LANG_CONFIG_TOPIC_SIDEBAR "sidebar"
#define _LANG_CONFIG_TOPIC_SIDEBAR_WIDTH "sidebar width"
#define _LANG_CONFIG_TOPIC_BOTTOM_LINES "reserved bottom lines"
#define _LANG_CONFIG_TOPIC_SIDEBAR_MAP "sidebar map"
#define _LANG_CONFIG_TOPIC_SIDEBAR_MAP_STYLE "map style"
#define _LANG_CONFIG_TOPIC_ROWS "rows"
#define _LANG_CONFIG_TOPIC_VERBOSE "verbose"
#define _LANG_CONFIG_TOPIC_EARMUFFS "earmuffs"
#define _LANG_CONFIG_TOPIC_EDITOR "editor"
#define _LANG_CONFIG_TOPIC_POV "pov"
#define _LANG_CONFIG_TOPIC_PROMPT "prompt"
#define _LANG_CONFIG_TOPIC_COMBAT "combat"
#define _LANG_CONFIG_TOPIC_ATTITUDE "stance"
#define _LANG_CONFIG_TOPIC_WIMPY "wimpy"
#define _LANG_CONFIG_TOPIC_AUTO_EQUIP "autoequip"
#define _LANG_CONFIG_TOPIC_LANGUAGE "language"

#define _LANG_CONFIG_CMD_TERM "terminal"
#define _LANG_CONFIG_CMD_COLS "columns"
#define _LANG_CONFIG_CMD_STATUS "statusbar"
#define _LANG_CONFIG_CMD_SIDEBAR "sidebar"
#define _LANG_CONFIG_CMD_ROWS "rows"
#define _LANG_CONFIG_CMD_VERBOSE "verbose"
#define _LANG_CONFIG_CMD_INFORM "inform"
#define _LANG_CONFIG_CMD_EARMUFFS "earmuffs"
#define _LANG_CONFIG_CMD_EDITOR "editor"
#define _LANG_CONFIG_CMD_POV "pov"
#define _LANG_CONFIG_CMD_PROMPT "prompt"
#define _LANG_CONFIG_CMD_WIMPY "wimpy"
#define _LANG_CONFIG_CMD_AUTO_EQUIP "equip auto"
#define _LANG_CONFIG_CMD_ATTITUDE "stance"
#define _LANG_CONFIG_CMD_COMBAT "combat"
#define _LANG_CONFIG_CMD_SPEAK "speak"
#define _LANG_CONFIG_CMD_CONSENT "consent"

// calendar table

// year names and the gender of each name, which english does not
// distinguish: the list keeps the shape the table reads
#define _LANG_CALENDAR_YEAR_NAMES ({ \
  "rat", 1, "dragon", 1, "horse", 1, "pig", 1, \
  "crane", 1, "phoenix", 1, "tiger", 1, "lion", 1, \
  "fox", 1, "naga", 1, "lizard", 1, "serpent", 1, \
  "cat", 1, "dog", 1, "eagle", 1, "vulture", 1, \
  "unicorn", 1, "sparrow", 1, "rook", 1, "ox", 1, \
  "spider", 1, "mouse", 1, "cow", 1, "dove", 1, \
  "cobra", 1, "raven", 1, "falcon", 1, "wolf", 1, \
  "bear", 1, "boar", 1, "stag", 1, "ram", 1, \
  "goat", 1, "hare", 1, "lynx", 1, "otter", 1, \
  "badger", 1, "hedgehog", 1, "mole", 1, "bat", 1, \
  "swallow", 1, "heron", 1, "swan", 1, "owl", 1, \
  "magpie", 1, "scorpion", 1, "wasp", 1, "bee", 1, \
  "salmon", 1, "eel", 1, "octopus", 1, "whale", 1, \
  "dolphin", 1, "griffin", 1, \
  })

// adjectives, in masculine and feminine pairs: both are the same word
#define _LANG_CALENDAR_YEAR_ADJECTIVES ({ \
  "horned", "horned", "flaming", "flaming", \
  "rearing", "rearing", "proud", "proud", \
  "immortal", "immortal", "rising", "rising", \
  "imperial", "imperial", "stray", "stray", \
  "sleeping", "sleeping", "stealthy", "stealthy", \
  "lying", "lying", "burning", "burning", \
  "infected", "infected", "royal", "royal", \
  "eastern", "eastern", "western", "western", \
  "red", "red", "black", "black", \
  "pestilent", "pestilent", "furious", "furious", \
  "howling", "howling", "treacherous", "treacherous", \
  "golden", "golden", "silver", "silver", \
  "shadowy", "shadowy", "wandering", "wandering", \
  "silent", "silent", "hungry", "hungry", \
  "brave", "brave", "wise", "wise", \
  "blind", "blind", "wounded", "wounded", \
  "thirsty", "thirsty", "frozen", "frozen", \
  "stormy", "stormy", "lonely", "lonely", \
  "vengeful", "vengeful", "patient", "patient", \
  "sleepless", "sleepless", "haughty", "haughty", \
  "captive", "captive", "withered", "withered", \
  "shining", "shining", "mourning", "mourning", \
  })

#define _LANG_CALENDAR_WEEK_DAYS ({ "monday", "tuesday", "wednesday", \
                                    "thursday", "friday", "saturday", \
                                    "sunday" })

// the styles a creature fights with

#define _LANG_NATURAL_ATTACK_FANGS_STYLE "fangs"
#define _LANG_NATURAL_ATTACK_FANGS_ATT_MSGS "you bite $defname with your fangs $local"
#define _LANG_NATURAL_ATTACK_FANGS_DEF_MSGS "$attname bites you with its fangs $local"
#define _LANG_NATURAL_ATTACK_FANGS_ROOM_MSGS "$attname bites $defname with its fangs $local"

#define _LANG_NATURAL_ATTACK_CLAWS_STYLE "claws"
#define _LANG_NATURAL_ATTACK_CLAWS_ATT_MSGS "you scratch $defname with your claws $local"
#define _LANG_NATURAL_ATTACK_CLAWS_DEF_MSGS "$attname scratches you with its claws $local"
#define _LANG_NATURAL_ATTACK_CLAWS_ROOM_MSGS "$attname scratches $defname with its claws $local"

#define _LANG_NATURAL_ATTACK_BEAK_STYLE "beak"
#define _LANG_NATURAL_ATTACK_BEAK_ATT_MSGS "you strike $defname with your beak $local"
#define _LANG_NATURAL_ATTACK_BEAK_DEF_MSGS "$attname strikes you with its beak $local"
#define _LANG_NATURAL_ATTACK_BEAK_ROOM_MSGS "$attname strikes $defname with its beak $local"

#define _LANG_NATURAL_ATTACK_HORNS_STYLE "horns"
#define _LANG_NATURAL_ATTACK_HORNS_ATT_MSGS "you gore $defname $local"
#define _LANG_NATURAL_ATTACK_HORNS_DEF_MSGS "$attname gores you $local"
#define _LANG_NATURAL_ATTACK_HORNS_ROOM_MSGS "$attname gores $defname $local"

#define _LANG_NATURAL_ATTACK_HOOVES_STYLE "hooves"
#define _LANG_NATURAL_ATTACK_HOOVES_ATT_MSGS "you strike $defname with your hooves $local"
#define _LANG_NATURAL_ATTACK_HOOVES_DEF_MSGS "$attname strikes you with its hooves $local"
#define _LANG_NATURAL_ATTACK_HOOVES_ROOM_MSGS "$attname strikes $defname with its hooves $local"

#define _LANG_NATURAL_ATTACK_TEETH_STYLE "teeth"
#define _LANG_NATURAL_ATTACK_TEETH_ATT_MSGS "you bite $defname $local"
#define _LANG_NATURAL_ATTACK_TEETH_DEF_MSGS "$attname bites you $local"
#define _LANG_NATURAL_ATTACK_TEETH_ROOM_MSGS "$attname bites $defname $local"

#define _LANG_NATURAL_ATTACK_PAWS_STYLE "paws"
#define _LANG_NATURAL_ATTACK_PAWS_ATT_MSGS "you swipe a paw at $defname $local"
#define _LANG_NATURAL_ATTACK_PAWS_DEF_MSGS "$attname swipes a paw at you $local"
#define _LANG_NATURAL_ATTACK_PAWS_ROOM_MSGS "$attname swipes a paw at $defname $local"

#define _LANG_NATURAL_ATTACK_TAIL_STYLE "tail"
#define _LANG_NATURAL_ATTACK_TAIL_ATT_MSGS "you lash $defname with your tail $local"
#define _LANG_NATURAL_ATTACK_TAIL_DEF_MSGS "$attname lashes you with its tail $local"
#define _LANG_NATURAL_ATTACK_TAIL_ROOM_MSGS "$attname lashes $defname with its tail $local"

#define _LANG_NATURAL_ATTACK_LEGS_STYLE "legs"
#define _LANG_NATURAL_ATTACK_LEGS_ATT_MSGS "you strike $defname with your legs $local"
#define _LANG_NATURAL_ATTACK_LEGS_DEF_MSGS "$attname strikes you with its legs $local"
#define _LANG_NATURAL_ATTACK_LEGS_ROOM_MSGS "$attname strikes $defname with its legs $local"

// mounts.c

#define _LANG_MOUNTS_VERBS_BOARD ({ "mount", "ride" })
#define _LANG_MOUNTS_VERBS_DISMOUNT ({ "dismount" })
#define _LANG_MOUNTS_PREPOSITIONS ({ "on", "onto", "upon", "the", "a", "an" })
#define _LANG_MOUNTS_BOARD_ME "You mount the $S.\n"
#define _LANG_MOUNTS_BOARD_ROOM "$N mounts the $S.\n"
#define _LANG_MOUNTS_DISMOUNT_ME "You dismount from the $S.\n"
#define _LANG_MOUNTS_DISMOUNT_ROOM "$N dismounts from the $S.\n"
#define _LANG_MOUNTS_LEAVE "@$N rides off $T."
#define _LANG_MOUNTS_ARRIVE "@$N arrives from $F riding the $S."
#define _LANG_MOUNTS_LISTING "(riding the $S)"
#define _LANG_MOUNTS_LOOK "$N is riding the $S.\n"
#define _LANG_MOUNTS_FALL_ME "You fall from the $S!\n"
#define _LANG_MOUNTS_FALL_ROOM "$N falls from the $S!\n"
#define _LANG_MOUNTS_REFUSE_DEAD "You are in no state to do that.\n"
#define _LANG_MOUNTS_REFUSE_PASSED_OUT "You are in no state to do that.\n"
#define _LANG_MOUNTS_REFUSE_ALREADY "You are already riding.\n"
#define _LANG_MOUNTS_REFUSE_FULL "There is no room left on it.\n"
#define _LANG_MOUNTS_REFUSE_SKILL "You do not know how to handle it.\n"
#define _LANG_MOUNTS_REFUSE_BODY_SIZE "You do not fit on it.\n"
#define _LANG_MOUNTS_REFUSE_PASSENGER "You are only a passenger.\n"
#define _LANG_MOUNTS_REFUSE_NOT_OWNER "It is not yours: only its owner may drive it.\n"
#define _LANG_MOUNTS_REFUSE_EXIT "It cannot go that way.\n"
#define _LANG_MOUNTS_REFUSE_EXIT_SIZE "It does not fit through there.\n"
#define _LANG_MOUNTS_REFUSE_TERRAIN "It cannot go that way.\n"
#define _LANG_MOUNTS_REFUSE_INDOORS "It cannot go indoors.\n"
#define _LANG_MOUNTS_REFUSE_UNDERGROUND "It cannot go underground.\n"
#define _LANG_MOUNTS_REFUSE_WATER "It cannot cross water.\n"
#define _LANG_MOUNTS_REFUSE_GROUND "It cannot go over the ground.\n"
#define _LANG_MOUNTS_REFUSE_ATTACK "Do you mean to attack your own mount?\n"
#define _LANG_MOUNTS_PROTECT_ROOM "The mount steps in to defend $N!\n"
#define _LANG_MOUNTS_FLEE_ME "Your mount bolts, carrying you away!\n"
#define _LANG_MOUNTS_FLEE_ROOM "The mount of $N bolts, carrying its rider away!\n"
#define _LANG_MOUNTS_SUMMON_ME "You call your $S, which comes to your side.\n"
#define _LANG_MOUNTS_SUMMON_ROOM "$N calls, and a $S comes to $N's side.\n"
#define _LANG_MOUNTS_DISMISS_ME "You send your $S away.\n"
#define _LANG_MOUNTS_DISMISS_ROOM "The $S of $N goes away.\n"
#define _LANG_MOUNTS_TRAVEL_START "You start riding towards $T.\nType the command again, with no direction, to stop.\n"
#define _LANG_MOUNTS_TRAVEL_FOLLOW "You follow the way towards $T.\n"
#define _LANG_MOUNTS_TRAVEL_STOP "You stop.\n"
#define _LANG_MOUNTS_TRAVEL_LOST "Your $S does not know where to go.\n"
#define _LANG_MOUNTS_TRAVEL_BLOCKED "Something stops you from going on.\n"
#define _LANG_MOUNTS_TRAVEL_FIGHT "The fight makes you stop.\n"
#define _LANG_MOUNTS_REFUSE_SUMMON "It cannot be called from afar.\n"
#define _LANG_MOUNTS_REFUSE_SUMMON_INDOORS "Your $S cannot come here: it needs open ground.\n"
#define _LANG_MOUNTS_REFUSE_SUMMON_WATER "Your $S cannot come here: it cannot cross water.\n"
