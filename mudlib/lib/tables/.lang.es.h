
// unarmed_combat table
#define _LANG_UNARMED_DEFAULT_STYLE "pelea"

#define _LANG_UNARMED_INITIAL_ATT_MSGS ({ "golpeas a $defname $local", \
                                         "das un puñetazo a $defname $local", \
                                         "magullas a $defname $local", \
                                         })
#define _LANG_UNARMED_INITIAL_DEF_MSGS ({ "$attname te golpea $local", \
                                         "$attname te da un puñetazo $local", \
                                         "$attname te magulla $local", \
                                         })
#define _LANG_UNARMED_INITIAL_ROOM_MSGS ({ "$attname golpea a $defname $local", \
                                         "$attname da un puñetazo a $defname $local", \
                                         "$attname magulla a $defname $local", \
                                         })

#define _LANG_UNARMED_MEDIUM_ATT_MSGS ({ "golpeas a $defname $local", \
                                         "das un puñetazo a $defname $local", \
                                         "magullas a $defname $local", \
                                         "golpeas a $defname con fuerza $local", \
                                         })
#define _LANG_UNARMED_MEDIUM_DEF_MSGS ({ "$attname te golpea $local", \
                                         "$attname te da un puñetazo $local", \
                                         "$attname te magulla $local", \
                                         "$attname te golpea con fuerza $local", \
                                         })
#define _LANG_UNARMED_MEDIUM_ROOM_MSGS ({ "$attname golpea a $defname $local", \
                                         "$attname da un puñetazo a $defname $local", \
                                         "$attname magulla a $defname $local", \
                                         "$attname golpea a $defname con fuerza $local", \
                                         })

#define _LANG_UNARMED_EXPERT_ATT_MSGS ({ "golpeas a $defname $local", \
                                         "das un puñetazo a $defname $local", \
                                         "magullas a $defname $local", \
                                         "golpeas a $defname con fuerza $local", \
                                         "golpeas a $defname con furia y rabia $local", \
                                         })
#define _LANG_UNARMED_EXPERT_DEF_MSGS ({ "$attname te golpea $local", \
                                         "$attname te da un puñetazo $local", \
                                         "$attname te magulla $local", \
                                         "$attname te golpea con fuerza $local", \
                                         "$attname te golpea con furia y rabia $local", \
                                         })
#define _LANG_UNARMED_EXPERT_ROOM_MSGS ({ "$attname golpea a $defname $local", \
                                         "$attname da un puñetazo a $defname $local", \
                                         "$attname magulla a $defname $local", \
                                         "$attname golpea a $defname con fuerza $local", \
                                         "$attanme golpea a $defname con furia y rabia $local", \
                                         })

// configurations

#define _LANG_CONFIG_INTERFACE ({ "interfaz", "interface", "terminal", "term" })
#define _LANG_CONFIG_INFO ({ "información", "informacion", "info" })
#define _LANG_CONFIG_COMMUNICATION ({ "comunicación", "comunicacion" })
#define _LANG_CONFIG_CHARACTER ({ "personaje", "jugador" })
#define _LANG_CONFIG_CONSENTS ({ "consentimientos" })

#define _LANG_CONFIG_TOPIC_TERM "tipo"
#define _LANG_CONFIG_TOPIC_COLS "columnas"
#define _LANG_CONFIG_TOPIC_STATUS "barra de estado"
#define _LANG_CONFIG_TOPIC_SIDEBAR "barra lateral"
#define _LANG_CONFIG_TOPIC_SIDEBAR_WIDTH "ancho lateral"
#define _LANG_CONFIG_TOPIC_BOTTOM_LINES "líneas reservadas abajo"
#define _LANG_CONFIG_TOPIC_SIDEBAR_MAP "mapa lateral"
#define _LANG_CONFIG_TOPIC_SIDEBAR_MAP_STYLE "estilo del mapa"
#define _LANG_CONFIG_TOPIC_ROWS "filas"
#define _LANG_CONFIG_TOPIC_VERBOSE "detallada"
#define _LANG_CONFIG_TOPIC_EARMUFFS "tapones"
#define _LANG_CONFIG_TOPIC_EDITOR "editor"
#define _LANG_CONFIG_TOPIC_POV "pov"
#define _LANG_CONFIG_TOPIC_PROMPT "prompt"
#define _LANG_CONFIG_TOPIC_COMBAT "combate"
#define _LANG_CONFIG_TOPIC_ATTITUDE "actitud"
#define _LANG_CONFIG_TOPIC_WIMPY "cobardía"
#define _LANG_CONFIG_TOPIC_AUTO_EQUIP "autoequiparse"
#define _LANG_CONFIG_TOPIC_LANGUAGE "idioma"

#define _LANG_CONFIG_CMD_TERM "terminal"
#define _LANG_CONFIG_CMD_COLS "columnas"
#define _LANG_CONFIG_CMD_STATUS "barra"
#define _LANG_CONFIG_CMD_SIDEBAR "lateral"
#define _LANG_CONFIG_CMD_ROWS "filas"
#define _LANG_CONFIG_CMD_VERBOSE "detalle"
#define _LANG_CONFIG_CMD_INFORM "informar"
#define _LANG_CONFIG_CMD_EARMUFFS "tapones"
#define _LANG_CONFIG_CMD_EDITOR "editor"
#define _LANG_CONFIG_CMD_POV "pov"
#define _LANG_CONFIG_CMD_PROMPT "prompt"
#define _LANG_CONFIG_CMD_WIMPY "cobardía"
#define _LANG_CONFIG_CMD_AUTO_EQUIP "equipar auto"
#define _LANG_CONFIG_CMD_ATTITUDE "actitud"
#define _LANG_CONFIG_CMD_COMBAT "combate"
#define _LANG_CONFIG_CMD_SPEAK "hablar"
#define _LANG_CONFIG_CMD_CONSENT "consentir"

// calendar table

// nombres de año y género de cada nombre: 1 masculino, 2 femenino,
// 3 femenino con artículo masculino ("el águila")
#define _LANG_CALENDAR_YEAR_NAMES ({ \
  "rata", 2, "dragón", 1, "caballo", 1, "cerdo", 1, \
  "grulla", 2, "fénix", 1, "tigre", 1, "león", 1, \
  "zorro", 1, "naga", 2, "lagarto", 1, "serpiente", 2, \
  "gato", 1, "perro", 1, "águila", 3, "buitre", 1, \
  "unicornio", 1, "gorrión", 1, "grajo", 1, "buey", 1, \
  "araña", 2, "ratón", 1, "vaca", 2, "paloma", 2, \
  "cobra", 2, "cuervo", 1, "halcón", 1, "lobo", 1, \
  "oso", 1, "jabalí", 1, "ciervo", 1, "carnero", 1, \
  "cabra", 2, "liebre", 2, "lince", 1, "nutria", 2, \
  "tejón", 1, "erizo", 1, "topo", 1, "murciélago", 1, \
  "golondrina", 2, "garza", 2, "cisne", 1, "lechuza", 2, \
  "urraca", 2, "escorpión", 1, "avispa", 2, "abeja", 2, \
  "salmón", 1, "anguila", 2, "pulpo", 1, "ballena", 2, \
  "delfín", 1, "grifo", 1, \
  })

// adjetivos, en pares masculino y femenino
#define _LANG_CALENDAR_YEAR_ADJECTIVES ({ \
  "cornudo", "cornuda", "llameante", "llameante", \
  "encabritado", "encabritada", "orgulloso", "orgullosa", \
  "inmortal", "inmortal", "naciente", "naciente", \
  "imperial", "imperial", "callejero", "callejera", \
  "durmiente", "durmiente", "sigiloso", "sigilosa", \
  "mentiroso", "mentirosa", "ardiente", "ardiente", \
  "infectado", "infectada", "real", "real", \
  "oriental", "oriental", "occidental", "occidental", \
  "rojo", "roja", "negro", "negra", \
  "pestilente", "pestilente", "furioso", "furiosa", \
  "aullador", "aulladora", "traidor", "traidora", \
  "dorado", "dorada", "plateado", "plateada", \
  "sombrío", "sombría", "errante", "errante", \
  "silencioso", "silenciosa", "hambriento", "hambrienta", \
  "valiente", "valiente", "sabio", "sabia", \
  "ciego", "ciega", "herido", "herida", \
  "sediento", "sedienta", "helado", "helada", \
  "tormentoso", "tormentosa", "solitario", "solitaria", \
  "vengativo", "vengativa", "paciente", "paciente", \
  "insomne", "insomne", "altivo", "altiva", \
  "cautivo", "cautiva", "marchito", "marchita", \
  "luminoso", "luminosa", "doliente", "doliente", \
  })

#define _LANG_CALENDAR_WEEK_DAYS ({ "lunes", "martes", "miércoles", \
                                    "jueves", "viernes", "sábado", \
                                    "domingo" })

// the styles a creature fights with

#define _LANG_NATURAL_ATTACK_FANGS_STYLE "colmillos"
#define _LANG_NATURAL_ATTACK_FANGS_ATT_MSGS "muerdes a $defname con tus colmillos $local"
#define _LANG_NATURAL_ATTACK_FANGS_DEF_MSGS "$attname te muerde con sus colmillos $local"
#define _LANG_NATURAL_ATTACK_FANGS_ROOM_MSGS "$attname muerde a $defname con sus colmillos $local"

#define _LANG_NATURAL_ATTACK_CLAWS_STYLE "garras"
#define _LANG_NATURAL_ATTACK_CLAWS_ATT_MSGS "arañas a $defname con tus garras $local"
#define _LANG_NATURAL_ATTACK_CLAWS_DEF_MSGS "$attname te araña con sus garras $local"
#define _LANG_NATURAL_ATTACK_CLAWS_ROOM_MSGS "$attname araña a $defname con sus garras $local"

#define _LANG_NATURAL_ATTACK_BEAK_STYLE "pico"
#define _LANG_NATURAL_ATTACK_BEAK_ATT_MSGS "golpeas a $defname con el pico $local"
#define _LANG_NATURAL_ATTACK_BEAK_DEF_MSGS "$attname te golpea con el pico $local"
#define _LANG_NATURAL_ATTACK_BEAK_ROOM_MSGS "$attname golpea a $defname con el pico $local"

#define _LANG_NATURAL_ATTACK_HORNS_STYLE "cuernos"
#define _LANG_NATURAL_ATTACK_HORNS_ATT_MSGS "corneas a $defname $local"
#define _LANG_NATURAL_ATTACK_HORNS_DEF_MSGS "$attname te cornea $local"
#define _LANG_NATURAL_ATTACK_HORNS_ROOM_MSGS "$attname cornea a $defname $local"

#define _LANG_NATURAL_ATTACK_HOOVES_STYLE "pezuñas"
#define _LANG_NATURAL_ATTACK_HOOVES_ATT_MSGS "golpeas a $defname con las pezuñas $local"
#define _LANG_NATURAL_ATTACK_HOOVES_DEF_MSGS "$attname te golpea con sus pezuñas $local"
#define _LANG_NATURAL_ATTACK_HOOVES_ROOM_MSGS "$attname golpea a $defname con sus pezuñas $local"

#define _LANG_NATURAL_ATTACK_TEETH_STYLE "dientes"
#define _LANG_NATURAL_ATTACK_TEETH_ATT_MSGS "muerdes a $defname $local"
#define _LANG_NATURAL_ATTACK_TEETH_DEF_MSGS "$attname te muerde $local"
#define _LANG_NATURAL_ATTACK_TEETH_ROOM_MSGS "$attname muerde a $defname $local"

#define _LANG_NATURAL_ATTACK_PAWS_STYLE "zarpas"
#define _LANG_NATURAL_ATTACK_PAWS_ATT_MSGS "das un zarpazo a $defname $local"
#define _LANG_NATURAL_ATTACK_PAWS_DEF_MSGS "$attname te da un zarpazo $local"
#define _LANG_NATURAL_ATTACK_PAWS_ROOM_MSGS "$attname da un zarpazo a $defname $local"

#define _LANG_NATURAL_ATTACK_TAIL_STYLE "cola"
#define _LANG_NATURAL_ATTACK_TAIL_ATT_MSGS "das un coletazo a $defname $local"
#define _LANG_NATURAL_ATTACK_TAIL_DEF_MSGS "$attname te da un coletazo $local"
#define _LANG_NATURAL_ATTACK_TAIL_ROOM_MSGS "$attname da un coletazo a $defname $local"

#define _LANG_NATURAL_ATTACK_LEGS_STYLE "patas"
#define _LANG_NATURAL_ATTACK_LEGS_ATT_MSGS "golpeas a $defname con tus patas $local"
#define _LANG_NATURAL_ATTACK_LEGS_DEF_MSGS "$attname te golpea con sus patas $local"
#define _LANG_NATURAL_ATTACK_LEGS_ROOM_MSGS "$attname golpea a $defname con sus patas $local"

// mounts.c

#define _LANG_MOUNTS_VERBS_BOARD ({ "montar", "cabalgar" })
#define _LANG_MOUNTS_VERBS_DISMOUNT ({ "desmontar" })
#define _LANG_MOUNTS_PREPOSITIONS ({ "a", "al", "en", "sobre", "el", "la", "un", "una" })
#define _LANG_MOUNTS_BOARD_ME "Te subes a la montura.\n"
#define _LANG_MOUNTS_BOARD_ROOM "$N se sube a la montura.\n"
#define _LANG_MOUNTS_DISMOUNT_ME "Te bajas de la montura.\n"
#define _LANG_MOUNTS_DISMOUNT_ROOM "$N se baja de la montura.\n"
#define _LANG_MOUNTS_LEAVE "@$N se va hacia $T montado."
#define _LANG_MOUNTS_ARRIVE "@$N llega desde $F montado."
#define _LANG_MOUNTS_LISTING "(montado)"
#define _LANG_MOUNTS_LOOK "$N va montado.\n"
#define _LANG_MOUNTS_FALL_ME "¡Te caes de la montura!\n"
#define _LANG_MOUNTS_FALL_ROOM "¡$N se cae de la montura!\n"
#define _LANG_MOUNTS_REFUSE_DEAD "En tu estado no puedes hacer eso.\n"
#define _LANG_MOUNTS_REFUSE_PASSED_OUT "En tu estado no puedes hacer eso.\n"
#define _LANG_MOUNTS_REFUSE_ALREADY "Ya vas montado.\n"
#define _LANG_MOUNTS_REFUSE_FULL "No queda sitio en ella.\n"
#define _LANG_MOUNTS_REFUSE_SKILL "No sabes cómo manejarla.\n"
#define _LANG_MOUNTS_REFUSE_BODY_SIZE "No cabes en ella.\n"
#define _LANG_MOUNTS_REFUSE_PASSENGER "Solo eres un pasajero.\n"
#define _LANG_MOUNTS_REFUSE_NOT_OWNER "No es tuya: solo su dueño puede llevarla.\n"
#define _LANG_MOUNTS_REFUSE_EXIT "No puede ir por ahí.\n"
#define _LANG_MOUNTS_REFUSE_EXIT_SIZE "No cabe por ahí.\n"
#define _LANG_MOUNTS_REFUSE_TERRAIN "No puede ir por ahí.\n"
#define _LANG_MOUNTS_REFUSE_INDOORS "No puede entrar en un edificio.\n"
#define _LANG_MOUNTS_REFUSE_UNDERGROUND "No puede ir bajo tierra.\n"
#define _LANG_MOUNTS_REFUSE_WATER "No puede cruzar el agua.\n"
#define _LANG_MOUNTS_REFUSE_GROUND "No puede ir por tierra.\n"
#define _LANG_MOUNTS_REFUSE_ATTACK "¿Piensas atacar a tu montura?\n"
#define _LANG_MOUNTS_PROTECT_ROOM "¡La montura se interpone para defender a $N!\n"
#define _LANG_MOUNTS_FLEE_ME "¡Tu montura sale huyendo contigo encima!\n"
#define _LANG_MOUNTS_FLEE_ROOM "¡La montura de $N sale huyendo con su jinete!\n"
#define _LANG_MOUNTS_SUMMON_ME "Llamas a tu $S, que acude a tu lado.\n"
#define _LANG_MOUNTS_SUMMON_ROOM "$N llama, y un $S acude a su lado.\n"
#define _LANG_MOUNTS_DISMISS_ME "Despides a tu $S.\n"
#define _LANG_MOUNTS_DISMISS_ROOM "El $S de $N se marcha.\n"
#define _LANG_MOUNTS_TRAVEL_START "Empiezas a viajar hacia $T.\nVuelve a escribir el comando, sin dirección, para detenerte.\n"
#define _LANG_MOUNTS_TRAVEL_FOLLOW "Sigues el camino hacia $T.\n"
#define _LANG_MOUNTS_TRAVEL_STOP "Te detienes.\n"
#define _LANG_MOUNTS_TRAVEL_LOST "Tu $S no sabe hacia dónde seguir.\n"
#define _LANG_MOUNTS_TRAVEL_BLOCKED "Algo te impide seguir.\n"
#define _LANG_MOUNTS_TRAVEL_FIGHT "La pelea te obliga a detenerte.\n"
#define _LANG_MOUNTS_REFUSE_SUMMON "No se la puede llamar desde lejos.\n"
#define _LANG_MOUNTS_REFUSE_SUMMON_INDOORS "Tu $S no puede venir aquí: necesita espacio abierto.\n"
#define _LANG_MOUNTS_REFUSE_SUMMON_WATER "Tu $S no puede venir aquí: no puede cruzar el agua.\n"
