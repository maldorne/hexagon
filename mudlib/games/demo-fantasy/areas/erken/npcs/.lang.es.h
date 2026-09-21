
#define _LANG_NPCS_GOBLIN_NAME "goblin"
#define _LANG_NPCS_GOBLIN_CHAT ({ "'¡Erken es nuestro!", \
                                  ":sonríe maliciosamente." })
#define _LANG_NPCS_GOBLIN_A_CHAT ({ "'¡¡¡Grrrrrrrr!!!", \
                                    "'¡te voy a morder una oreja!" })

#define _LANG_NPCS_GNOLL_NAME "gnoll"
#define _LANG_NPCS_GNOLL_LONG "Es una especie de Goblin pero de forma mas estilizada. Tiene " + \
    "un morro alargado característico de su raza. Es un bicho repugnante.\n"
#define _LANG_NPCS_GNOLL_A_CHAT ({ "'¡¡¡Grrrrrrrr!!!", \
                                   "'¡Urrrrl muere!", })

#define _LANG_NPCS_ORC_NAME "orco"
#define _LANG_NPCS_ORC_LONG "Un enorme humanoide de piel verdosa, fornido y amenazante.\n"
#define _LANG_NPCS_ORC_CHAT ({ ":gruñe.", \
                               ":escupe en el suelo.", \
                               ":maldice el nombre de $lname$.", \
                               ":te mira maliciosamente con sus ojos inyectados en sangre.", })


#define _LANG_NPCS_BARMAN_NAME "arak"
// "barman" will be use by set_attender in the pub, do not change!
#define _LANG_NPCS_BARMAN_ALIAS ({ "arak", "barman" })
#define _LANG_NPCS_BARMAN_PLURAL "arak"
#define _LANG_NPCS_BARMAN_PLURALS ({ "arak" })
#define _LANG_NPCS_BARMAN_LONG "Para ser un goblin tiene una estatura bastante notable. Sus " + \
      "efluvios corporales le destacan entre la multitud.\n"
#define _LANG_NPCS_BARMAN_CHAT ({ ":te mira.", \
                                  ":te escupe a los pies.", })
#define _LANG_NPCS_BARMAN_A_CHAT ({ "'¡Arrggghhh!" })

#define _LANG_NPCS_ASSASSIN_NAME "kael"
#define _LANG_NPCS_ASSASSIN_ALIAS ({ "Kael", "asesino" })
#define _LANG_NPCS_ASSASSIN_MAIN_PLURAL "Asesinos"
#define _LANG_NPCS_ASSASSIN_PLURALS ({ "asesinos" })
#define _LANG_NPCS_ASSASSIN_SHORT "Kael, el Asesino"
#define _LANG_NPCS_ASSASSIN_LONG "Kael es un hombre oscuro, sombrío, vestido completamente de negro. " + \
                     "Se mueve silenciosamente y parece estar observando todos tus movimientos.\n"

#define _LANG_NPCS_SHOPKEEPER_NAME "grukk"
// "shopkeeper" will be used by set_attender in the shop, do not change!
#define _LANG_NPCS_SHOPKEEPER_ALIAS ({ "grukk", "shopkeeper", "tendero" })
#define _LANG_NPCS_SHOPKEEPER_PLURAL "grukk"
#define _LANG_NPCS_SHOPKEEPER_PLURALS ({ "grukk" })
#define _LANG_NPCS_SHOPKEEPER_LONG "Un goblin escuálido, sentado sobre un montón de cosas " + \
      "que no hacen juego entre sí. Muchas llevan todavía las iniciales de otro dueño y " + \
      "él no piensa hablar de eso.\n"

#define _LANG_NPCS_SHOPKEEPER_CHAT ({ "'Barato, barato. No preguntes de dónde.", \
                                      ":cuenta sus monedas otra vez, por si acaso.", \
                                      "'Aquí nadie devuelve nada." })
#define _LANG_NPCS_SHOPKEEPER_A_CHAT ({ "'¡Mío! ¡Todo mío!", \
                                        "'¡Arak! ¡Ayuda!" })

// races this village does not tolerate, by the name the race answers to
#define _LANG_NPCS_HATED_RACES ({ "humano", "elfo" })
