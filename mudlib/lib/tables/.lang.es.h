
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

#define _LANG_CONFIG_TERMINAL ({ "terminal", "term" })
#define _LANG_CONFIG_INFO ({ "información", "informacion", "info" })
#define _LANG_CONFIG_COMMUNICATION ({ "comunicación", "comunicacion" })
#define _LANG_CONFIG_CHARACTER ({ "personaje", "jugador" })
#define _LANG_CONFIG_CONSENTS ({ "consentimientos" })

#define _LANG_CONFIG_TOPIC_TERM "tipo"
#define _LANG_CONFIG_TOPIC_COLS "columnas"
#define _LANG_CONFIG_TOPIC_ROWS "filas"
#define _LANG_CONFIG_TOPIC_VERBOSE "detallada"
#define _LANG_CONFIG_TOPIC_EARMUFFS "tapones"
#define _LANG_CONFIG_TOPIC_EDITOR "editor"
#define _LANG_CONFIG_TOPIC_POV "pov"
#define _LANG_CONFIG_TOPIC_PROMPT "prompt"
#define _LANG_CONFIG_TOPIC_COMBAT "combate"
#define _LANG_CONFIG_TOPIC_ATTITUDE "actitud"
#define _LANG_CONFIG_TOPIC_WIMPY "cobardía"
#define _LANG_CONFIG_TOPIC_LANGUAGE "idioma"

#define _LANG_CONFIG_CMD_TERM "terminal"
#define _LANG_CONFIG_CMD_COLS "columnas"
#define _LANG_CONFIG_CMD_ROWS "filas"
#define _LANG_CONFIG_CMD_VERBOSE "detalle"
#define _LANG_CONFIG_CMD_INFORM "informar"
#define _LANG_CONFIG_CMD_EARMUFFS "tapones"
#define _LANG_CONFIG_CMD_EDITOR "editor"
#define _LANG_CONFIG_CMD_POV "pov"
#define _LANG_CONFIG_CMD_PROMPT "prompt"
#define _LANG_CONFIG_CMD_WIMPY "cobardía"
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

#define _LANG_UNARMED_FANGS_STYLE "colmillos"
#define _LANG_UNARMED_FANGS_ATT_MSGS "muerdes con tus colmillos"
#define _LANG_UNARMED_FANGS_DEF_MSGS "te muerde con sus colmillos"
#define _LANG_UNARMED_FANGS_ROOM_MSGS "muerde con sus colmillos"

#define _LANG_UNARMED_CLAWS_STYLE "garras"
#define _LANG_UNARMED_CLAWS_ATT_MSGS "arañas con tus garras"
#define _LANG_UNARMED_CLAWS_DEF_MSGS "te araña con sus garras"
#define _LANG_UNARMED_CLAWS_ROOM_MSGS "araña con sus garras"

#define _LANG_UNARMED_BEAK_STYLE "pico"
#define _LANG_UNARMED_BEAK_ATT_MSGS "golpeas con el pico"
#define _LANG_UNARMED_BEAK_DEF_MSGS "te golpea con el pico"
#define _LANG_UNARMED_BEAK_ROOM_MSGS "golpea con el pico"

#define _LANG_UNARMED_HORNS_STYLE "cuernos"
#define _LANG_UNARMED_HORNS_ATT_MSGS "corneas"
#define _LANG_UNARMED_HORNS_DEF_MSGS "te cornea"
#define _LANG_UNARMED_HORNS_ROOM_MSGS "cornea"

#define _LANG_UNARMED_HOOVES_STYLE "pezuñas"
#define _LANG_UNARMED_HOOVES_ATT_MSGS "golpeas con las pezuñas"
#define _LANG_UNARMED_HOOVES_DEF_MSGS "te golpea con sus pezuñas"
#define _LANG_UNARMED_HOOVES_ROOM_MSGS "golpea con sus pezuñas"

#define _LANG_UNARMED_TEETH_STYLE "dientes"
#define _LANG_UNARMED_TEETH_ATT_MSGS "muerdes"
#define _LANG_UNARMED_TEETH_DEF_MSGS "te muerde"
#define _LANG_UNARMED_TEETH_ROOM_MSGS "muerde"

#define _LANG_UNARMED_PAWS_STYLE "zarpas"
#define _LANG_UNARMED_PAWS_ATT_MSGS "das un zarpazo"
#define _LANG_UNARMED_PAWS_DEF_MSGS "te da un zarpazo"
#define _LANG_UNARMED_PAWS_ROOM_MSGS "da un zarpazo"

#define _LANG_UNARMED_TAIL_STYLE "cola"
#define _LANG_UNARMED_TAIL_ATT_MSGS "das un coletazo"
#define _LANG_UNARMED_TAIL_DEF_MSGS "te da un coletazo"
#define _LANG_UNARMED_TAIL_ROOM_MSGS "da un coletazo"

#define _LANG_UNARMED_LEGS_STYLE "patas"
#define _LANG_UNARMED_LEGS_ATT_MSGS "golpeas con tus patas"
#define _LANG_UNARMED_LEGS_DEF_MSGS "te golpea con sus patas"
#define _LANG_UNARMED_LEGS_ROOM_MSGS "golpea con sus patas"
