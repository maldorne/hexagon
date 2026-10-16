// Names of the body parts (<living/body-parts.h>), by part id.

#define _LANG_BODY_PART_NAMES ([ \
    "skin" : "piel", "plumage" : "plumaje", "scales" : "escamas", \
    "head" : "cabeza", "hand" : "mano", "heart" : "corazón", \
    "teeth" : "dientes", "fangs" : "colmillos", "claws" : "garras", \
    "horn" : "cuerno", "tail" : "cola", "mandibles" : "mandíbulas" ])
#define _LANG_BODY_PART_PLURALS ([ \
    "skin" : "pieles", "plumage" : "plumajes", "scales" : "escamas", \
    "head" : "cabezas", "hand" : "manos", "heart" : "corazones", \
    "teeth" : "dientes", "fangs" : "colmillos", "claws" : "garras", \
    "horn" : "cuernos", "tail" : "colas", "mandibles" : "mandíbulas" ])
// other words a player may use for them, plain-ASCII variants included
#define _LANG_BODY_PART_ALIASES ([ \
    "skin" : ({ "pellejo" }), "plumage" : ({ "plumas" }), \
    "scales" : ({ "escama" }), "heart" : ({ "corazon" }), \
    "teeth" : ({ "diente", "dentadura" }), "fangs" : ({ "colmillo" }), \
    "claws" : ({ "garra", "zarpas" }), \
    "mandibles" : ({ "mandibulas", "mandíbula", "mandibula" }) ])
#define _LANG_BODY_PART_FEMININE ({ "skin", "scales", "head", "hand", "claws", \
    "tail", "mandibles" })
#define _LANG_BODY_PART_PLURAL_NOUNS ({ "scales", "teeth", "fangs", "claws", "mandibles" })
