
#define _LANG_NPCS_SNAKE_NAME "snake"
#define _LANG_NPCS_SNAKE_LONG "A big and aggressive snake.\n"
#define _LANG_NPCS_SNAKE_JOIN_FIGHT "The snake shows its fangs and attacks.\n"

#define _LANG_NPCS_PILGRIM_NAME "pilgrim"
#define _LANG_NPCS_PILGRIM_LONG "He is a man from the roads, going in pilgrimage to some temple. " + \
    "He seems to be harmless.\n" 
#define _LANG_NPCS_PILGRIM_CHAT ({ "'If you want to visit the Temple of Lummen go north.", \
                                   ":smiles." })

#define _LANG_NPCS_HEALER_NAME "thran"
#define _LANG_NPCS_HEALER_SHORT "Thran, the Priest"
#define _LANG_NPCS_HEALER_ALIASES ({ "Thran", "priest" })
#define _LANG_NPCS_HEALER_PLURAL "Priests"
#define _LANG_NPCS_HEALER_PLURAL_ALIASES ({ "priests" })
#define _LANG_NPCS_HEALER_LONG "Is a human priest, believer in Lummen. He is gifted with the power " + \
    "to return the souls to their original bodies. He looks mysterious and magical. " + \
    "Write 'info' to see what he can help you with.\n"
#define _LANG_NPCS_HEALER_RAISE_COMMANDS ({ "raise" })
#define _LANG_NPCS_HEALER_INFO_COMMANDS ({ "info", "information" })
#define _LANG_NPCS_HEALER_INFO "Uhmm, if your devotion to Lummen is pure " + \
     "I will be able to raise your body. Write 'raise' and you will find peace.\n"
#define _LANG_NPCS_HEALER_CHAT ({ "'if you want to know how to return to your mortal form write 'info'.", \
    ":praises Lummen.", \
    "'you must be a good devotee." })
#define _LANG_NPCS_HEALER_RAISE_NO "Why do you want me to raise you if you don't need it?\n"
#define _LANG_NPCS_HEALER_RAISE_TOO_SOON "You were raised recently, you will have to wait a bit more.\n"
#define _LANG_NPCS_HEALER_RAISE_YES_ME "Thran raises his hands calling the powers of the Gods, and you appear in your mortal form.\n"
#define _LANG_NPCS_HEALER_RAISE_YES_ROOM this_player()->query_cap_name()+" recovers "+this_player()->query_possessive()+" mortal form.\n"

#define _LANG_NPCS_FARMER_NAME "farmer"
#define _LANG_NPCS_FARMER_LONG "A farmer from the area. Seems a nice man.\n" 
#define _LANG_NPCS_FARMER_CHAT ({ ":greets you cordially.", \
                                  ":makes comments about this year's corn crop." })
