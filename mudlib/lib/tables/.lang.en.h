
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

#define _LANG_CONFIG_TERMINAL ({ "terminal", "term" })
#define _LANG_CONFIG_INFO ({ "information", "info" })
#define _LANG_CONFIG_COMMUNICATION ({ "communication" })
#define _LANG_CONFIG_CHARACTER ({ "character", "player" })
#define _LANG_CONFIG_CONSENTS ({ "consents" })

#define _LANG_CONFIG_TOPIC_TERM "type"
#define _LANG_CONFIG_TOPIC_COLS "columns"
#define _LANG_CONFIG_TOPIC_ROWS "rows"
#define _LANG_CONFIG_TOPIC_VERBOSE "verbose"
#define _LANG_CONFIG_TOPIC_EARMUFFS "earmuffs"
#define _LANG_CONFIG_TOPIC_EDITOR "editor"
#define _LANG_CONFIG_TOPIC_POV "pov"
#define _LANG_CONFIG_TOPIC_PROMPT "prompt"
#define _LANG_CONFIG_TOPIC_COMBAT "combat"
#define _LANG_CONFIG_TOPIC_ATTITUDE "stance"
#define _LANG_CONFIG_TOPIC_WIMPY "wimpy"
#define _LANG_CONFIG_TOPIC_LANGUAGE "language"

#define _LANG_CONFIG_CMD_TERM "terminal"
#define _LANG_CONFIG_CMD_COLS "columns"
#define _LANG_CONFIG_CMD_ROWS "rows"
#define _LANG_CONFIG_CMD_VERBOSE "verbose"
#define _LANG_CONFIG_CMD_INFORM "inform"
#define _LANG_CONFIG_CMD_EARMUFFS "earmuffs"
#define _LANG_CONFIG_CMD_EDITOR "editor"
#define _LANG_CONFIG_CMD_POV "pov"
#define _LANG_CONFIG_CMD_PROMPT "prompt"
#define _LANG_CONFIG_CMD_WIMPY "wimpy"
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
