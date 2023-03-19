
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
#define _LANG_CONFIG_PLAYER ({ "player" })

#define _LANG_CONFIG_TOPIC_TERM "type"
#define _LANG_CONFIG_TOPIC_COLS "columns"
#define _LANG_CONFIG_TOPIC_ROWS "rows"
#define _LANG_CONFIG_TOPIC_HEALTH "health"
#define _LANG_CONFIG_TOPIC_VERBOSE "verbose"
#define _LANG_CONFIG_TOPIC_LOGON "logon"
#define _LANG_CONFIG_TOPIC_HUD "hud"
#define _LANG_CONFIG_TOPIC_STUN "stun"

#define _LANG_CONFIG_CMD_TERM "terminal"
#define _LANG_CONFIG_CMD_COLS "columns"
#define _LANG_CONFIG_CMD_ROWS "rows"
#define _LANG_CONFIG_CMD_MONITOR "monitor"
#define _LANG_CONFIG_CMD_VERBOSE "verbose"
#define _LANG_CONFIG_CMD_INFORM "inform"
#define _LANG_CONFIG_CMD_HUD "hud"
#define _LANG_CONFIG_CMD_STUN "combat"
