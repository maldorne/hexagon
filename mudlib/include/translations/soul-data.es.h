// soul_data.c Translated for RlMud by Elannon (1999)

// Removed most of the souls in spanish, fixed some mistakes,
// removed a lot of parameters, neverbot 02/2006

#include <translations/souls.h>

#define GLOBAL_ADJ ",lentamente,felizmente,tristemente,nerviosamente," + \
     "haciendo una mueca,ampliamente,torcidamente,estúpidamente," + \
     "condescendientemente,irónicamente," + \
     "pacientemente,alegremente,ligeramente," + \
     "excitadamente,extrañamente," + \
     "cuidadosamente,sádicamente,de lado," + \
     "levemente,enfadado," + \
     "inocentemente," + \
     "débilmente,humildemente,paradójicamente," + \
     "con dulzura," + \
     "cariñosamente,soñador,radiantemente,entusiasmadamente," + \
     "insolente,maniáticamente,completamente,inconscientemente," + \
     "borracho,como un loco,con gracia,con malicia," + \
     "de pasada,secamente," + \
     "increíblemente,tranquilamente," + \
     "de modo diferente,protestando,triunfalmente,seductor," + \
     "suavemente,positivamente," + \
     "demoníacamente,afectivo,con frialdad,negativamente," + \
     "perezoso,sereno," + \
     "desilusionado,con furia," + \
     "tontamente," + \
     "espasmódicamente," + \
     "como un cafre,con locura," + \
     "con irreverencia," + \
     "locuazmente,verbalmente,"

// old ping:
/* "$mcname$ te hace '¡¡¡PING!!!'$force#alias ping_alias $mname$#2$", */

#define SOUL_DATA_A ([  \
  SOUL_CMD_ACK : ({ ({ 0, "%s", 0, "a %s" }), \
    ({ "Respodes ack", \
      "$mcname$ responde ack", \
      "Respondes ack a $hcname$", \
      "$mcname$ te responde ack", \
      "$mcname$ responde ack a $hcname$", }) }), \
  SOUL_CMD_ACCUSE : ({ ({ 0, "%s", 2, "%s de %s", 2, "%s %s", 3, "%s" }), ({ \
     "$ifarg:Acusas a $arg:#$$else$¡El mayordomo fue el culpable!~$",  \
     "$mcname$ $ifarg:acusa a $lastarg$$else$acusa al mayordomo~$", \
     "Acusas a $hcname$$ifarg: de $arg:#$~$", \
     "$mcname$ te acusa$ifarg: de $lastarg$~$", \
     "$mcname$ acusa a $hcname$$ifarg: de $lastarg$~$" }) }), \
  SOUL_CMD_ADMIRE : ({ ({ 0, "%s", 2, "%s %s", 3, "%s" }), ({ \
     "$ifarg:Admiras a $arg:#$$else$¡Admiras "+mud_name()+"!~$", \
     "$mcname$ $ifarg:admira $lastarg$$else$admira "+mud_name()+"~$", \
     "Admiras a $hcname$$ifarg: por ser $arg:#$~$", \
     "$mcname$ te admira$ifarg: por ser $lastarg$~$", \
     "$mcname$ admira $hcname$$ifarg: por ser $lastarg$~$" }) }), \
  SOUL_CMD_HALLUCINATE : ({ ({ 0, "%s" }), ({ \
      "Alucinas con lo que sucede", \
      "$mcname$ alucina con lo que sucede" }) }), \
  SOUL_CMD_APPLAUDE : ({ /* puede necesitar un $ifarg: $arg:...,...$~$ */ \
    ({ 0, "%s", 1, "%s %s", 2, "%s %s", 3, "%s" }), ({ \
      "Aplaudes $arg:totalmente,ruidosamente,ligeramente,vagamente,brevemente$", \
      "$mcname$ aplaude $lastarg$", \
      "Aplaudes a $hcname$ $arg:totalmente,ruidosamente,ligeramente,vagamente,brevemente$", \
      "$mcname$ te aplaude $lastarg$", \
      "$mcname$ aplaude a $hcname$ $lastarg$" }) }), \
  SOUL_CMD_AMAZE : ({ ({ 0, "%s" }), ({ \
      "¡Estás asombrado!", \
      "¡$mcname$ está asombrado!", \
      "Asombras a $hcname$$ifarg: con tu $arg:estupidez,inteligencia"+ \
      ",sabiduría$~$!", \
      "$mcname$ te asombra$ifarg: con $mposs$ $lastarg$~$!", \
      "$mcname$ asombra a $hcname$$ifarg: con $mposs$ $lastarg$~$" }) }), \
  SOUL_CMD_CUDDLE : ({ ({ 0, "%s", 2, "%s %s", }), ({  \
      "Acaricias a $hcname$$ifarg: $arg:dulcemente,con pasión y locura,"+ \
      "firmemente,seductoramente,sugestivamente,levemente,sensualmente,"+ \
      "educadamente,amorosamente,vigorosamente,con ansia$~$", \
      "$mcname$ te acaricia$ifarg: $lastarg$~$", \
      "$mcname$ acaricia a $hcname$$ifarg: $lastarg$~$", }) }), \
  SOUL_CMD_SHAKE : ({ ({ 3, "%s" }), ({ \
      "Agitas tus $arg:orejas,brazos,piernas,manos,pies$", \
      "$mcname$ agita sus $arg:orejas,brazos,piernas,manos,pies$" }) }), \
  SOUL_CMD_HUG : ({ ({ 0, "%s", 1, "%s %s", 2, "%s %s" }), ({ \
      "Abrazas a $hcname$$ifarg: $arg:fuerte,cariñosamente,amigablemente,"+ \
      "muy fuerte,vigorosamente,desesperadamente,amorosamente,alegremente,como un oso,"+ \
      "protectoramente,sensualmente,eróticamente$~$", \
      "$mcname$ te abraza $ifarg: $lastarg$~$", \
      "$mcname$ abraza a $hcname$ $ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_AGREE : ({ ({ \
      0, "%s", 1, "%s a %s", 1, "%s %s", 2, "a %s %s", \
      2, "%s %s", 3, "%s" }), ({ \
      "Asientes con la cabeza$ifarg: $arg:solemnemente,alegremente,secamente,comprensivamente,"+ \
      "pacientemente,lentamente,cansadamente,sarcásticamente,sabiamente,con aprobación,"+ \
      "con conocimiento,vigorosamente,en acuerdo,en desacuerdo$~$", \
      "$mcname$ asiente con la cabeza$ifarg: $lastarg$~$", \
      "Asientes con la cabeza$ifarg: $arg:solemnemente,alegremente,secamente,comprensivamente,"+ \
      "pacientemente,lentamente,cansadamente,sarcásticamente,sabiamente,con aprobación,"+ \
      "con conocimiento,vigorosamente,en acuerdo,en desacuerdo$~$"+ \
      " a $hcname$", \
      "$mcname$ te asiente con la cabeza$ifarg: $lastarg$~$", \
      "$mcname$ le asiente con la cabeza$ifarg: $lastarg$~$ a $hcname$" }) }), \
  SOUL_CMD_SCARE : ({ ({ 0, "%s" }), ({ \
      "Le das un susto de muerte a $hcname$", \
      "$mcname$ te da un susto de muerte$force#miedo $mname$#2$", \
      "$mcname$ le da un susto de muerte a $hcname$" }) }), \
  SOUL_CMD_STROKE : ({ ({ 0, "%s", 1, "%s %s", 2, "%s %s" }), ({ \
      "Achuchas a $hcname$ $arg:cariñosamente,gentilmente,muy fuerte$", \
      "$mcname$ te achucha $lastarg$", \
      "$mcname$ achucha $lastarg$ a $hcname$" }) }), \
  SOUL_CMD_THANK : ({ ({ 0, "%s", 2, "%s %s" }), ({ \
      "Le agradeces a $hcname$$ifarg: $arg:profusamente,un montón,levemente,"+ \
      "con reservas,solemnemente,de todo corazón$~$", \
      "$mcname$ te lo agradece$ifarg: $lastarg$~$", \
      "$mcname$ se lo agradece a $hcname$$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_WORSHIP : ({ ({ 0, "%s", 3, "%s" }), ({ \
      "Adoras $ifarg:$arg:#$$else$"+mud_name()+"~$", \
      "$mcname$ adora $ifarg:$lastarg$$else$"+mud_name()+"~$", \
      "Adoras a $hcname$", \
      "$mcname$ te adora", "$mcname$ adora a $hcname$" }) }), \
])

#define SOUL_DATA_B ([  \
  SOUL_CMD_HOWL : ({ 0, ({ 
      "Berreas hasta quedarte afónico", \
      "$mcname$ berrea hasta quedarse afónico" }) }), \
  SOUL_CMD_DANCE : ({ ({ 0, "%s", 0, "con %s" }), ({ \
      "Te marcas un baile... ¿genial verdad?", \
      "$mcname$ se marca un baile", \
      "Deslizas a $hcname$ por la pista de baile", \
      "$mcname$ te desliza por la pista de baile", \
      "$mcname$ desliza a $hcname$ por la pista de baile" }) }), \
  SOUL_CMD_DROOL : ({ ({ 0, "%s", 0, "a %s" }), ({ \
      "Babear como si estuvieras en celo", \
      "$mcname$ babea", \
      "Babeas por $hcname$", \
      "$mcname$ babea por ti", \
      "$mcname$ babea por $hcname$" }) }), \
  SOUL_CMD_KISS : ({ ({ 0, "%s", 1, "%s %s", 2, "%s %s" }), ({ \
      "Besas a $hcname$$ifarg: $arg:apasionadamente,profundamente,con ternura,suavemente,"+ \
      "largo y tendido,con dulzura,sensualmente$~$", \
      "$mcname$ te besa$ifarg: $lastarg$~$", \
      "$mcname$ besa a $hcname$$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_YAWN : ({ ({ 3, "%s" }), ({ \
      "Bostezas $arg:cansadamente,de aburrimiento,de sueño$", \
      "$mcname$ bosteza $lastarg$" }) }) , \
])

#define SOUL_DATA_C ([  \
  SOUL_CMD_UNDERSTAND: ({ ({ 0,"%s", 1, "%s %s", 1, "%s a %s", 2, "%s %s", 2, "%s a %s"}), ({ \
     "Comprendes a $hcname$$ifarg: $arg:sinceramente,totalmente,gentilmente,"+ \
     "a medias,vagamente,completamente,"+ \
     "con tristeza,compartiendo tu dolor,"+ \
     "amistosamente$~$", \
     "$mcname$ te comprende$ifarg: $lastarg$~$", \
     "$mcname$ comprende a $hcname$$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_COMFORT : ({ ({ 0, "%s", 1, "%s %s", 2, "%s %s" }), ({ \
      "Consuelas a $hcname$$ifarg: $arg:sinceramente,totalmente,maliciosamente,"+ \
      "a medias,vagamente,animosamente,completamente,amorosamente,con falsedad,gentilmente,"+ \
      "delicadamente,tristemente,compartiendo tu dolor,"+ \
      "ansiosamente,con cuidado,delicadamente,sensualmente$~$", \
      "$mcname$ te consuela$ifarg: $lastarg$~$", \
      "$mcname$ consuela a $hcname$$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_CONFESS : ({ ({ 0, "%s" }), ({ \
      "Confiesas ser $mnumeral$ adict$mvocal$ a "+mud_name(), \
      "$mcname$ confiesa ser $mnumeral$ adict$mvocal$ a "+mud_name(),  \
      }), }), \
  SOUL_CMD_CROSS : ({ ({ 3, "%s", 3, "sus %s" }), ({ \
      "Cruzas tus $arg:brazos,dedos,piernas,ojos$", \
      "$mcname$ cruza sus $lastarg$", }) }), \
  SOUL_CMD_WALK : ({ ({ 0, "%s", 3, "%s", 1, "%s %s", 2, "%s %s" }), ({ \
      "Caminas de un lado a otro$ifarg: $arg:nerviosamente,"+ \
      "pensando,ansiosamente,con impaciencia,lentamente,rápidamente,"+ \
      "enfadado,esperando una respuesta,pacientemente,"+ \
      "ponderando la cuestión,irritantemente,"+ \
      "aburrido,a la pata coja$~$", \
      "$mcname$ camina de un lado a otro$ifarg: $lastarg$~$", \
      "Caminas $ifarg:$lastarg$~$alrededor de $hcname$ "+ \
      "$arg:nerviosamente,pensando,ansiosamente,con impaciencia,lentamente,rápidamente,"+ \
      "enfadado,esperando una respuesta,pacientemente,"+ \
      "ponderando la cuestión,irritantemente,"+ \
      "aburrido,a la pata coja$", \
      "$mcname$ camina$ifarg: $lastarg$~$ a tu alrededor", \
      "$mcname$ camina$ifarg: $lastarg$~$ alrededor de $hcname$", \
       }) }), \
  SOUL_CMD_SCREAM : ({ ({ 3, "%s", 3, "de %s" }), ({ \
      "$ifarg:Chillas $arg:de miedo,de pánico,de terror,de horror,de dolor,de frustración,de rabia,"+ \
      "de desesperación,de alegría$$else$¡¡¡¡Arrggghhhhh!!!!~$", \
      "$mcname$ chilla$ifarg: $lastarg$$else$ a pleno pulmón~$" }) }), \
])

#define SOUL_DATA_D ([  \
  SOUL_CMD_APOLOGIZE : ({ \
    ({ 3, "%s", 1, "%s de %s", 1, "%s %s", 2, "%s %s", }), ({ \
      "Te disculpas $arg:felizmente,tristemente,reservadamente,"+ \
      "lentamente,levemente,secamente,"+ \
      "sin necesidad,espontáneamente,por partida doble,completamente,humildemente$", \
      "$mcname$ se disculpa $lastarg$", \
      "Te disculpas $arg:felizmente,tristemente,reservadamente,"+ \
      "lentamente,levemente,secamente,"+ \
      "sin necesidad,espontáneamente,por partida doble,completamente,humildemente$ con $hcname$", \
      "$mcname$ se disculpa $lastarg$ contigo", \
      "$mcname$ pide disculpas $lastarg$ a $hcname$" }) }), \
  SOUL_CMD_ROLL : ({ ({ 0, "%s" }), ({ \
      "Te descojonas de $hcname$ hasta que se te saltan las lágrimas", \
      "$mcname$ se descojona de ti hasta que se le saltan las lágrimas", \
      "$mcname$ se descojona de $hcname$ hasta que se le saltan las lágrimas" }) }), \
  SOUL_CMD_WAVE : ({ ({ \
      0, "%s", 0, "a %s", 0, "a %s", 2, "a %s %s", 1, "%s %s",  \
      1, "%s a %s", 2, "a %s %s", 0, "a %s", 1, "%s a %s", \
      2, "%s %s", 3, "%s" }), ({ \
      "Dices adiós$ifarg: $arg:cansadamente,somnolientamente,perezosamente,tristemente,"+ \
      "alegremente$~$", \
      "$mcname$ dice adiós$ifarg: $lastarg$~$", \
      "Dices adiós$ifarg: $arg:cansadamente,somnolientamente,perezosamente,tristemente,"+ \
      "alegremente,amorosamente,y buenas noches$~$ a $hcname$", \
      "$mcname$ te dice adiós$ifarg: $lastarg$~$", \
      "$mcname$ dice adiós$ifarg: $lastarg$~$ a $hcname$" }) }), \
  SOUL_CMD_SNIFF : ({ ({ 0, "%s" }), ({ \
      "Miras con desprecio a $hcname$, como el ser vil y rastrero que es", \
      "$mcname$ te mira con desprecio, como la criatura vil y rastrera que eres", \
      "$mcname$ mira con desprecio a $hcname$" }) }), \
  SOUL_CMD_RAVE : ({ ({ 0, "%s" }), ({ \
      "Desvarías como un loco", \
      "$mcname$ desvaría como un loco" }) \
])

#define SOUL_DATA_E ([  \
  SOUL_CMD_BURP : ({ ({ 3, "%s" }), ({ \
      "Eructas$ifarg: $arg:estruendosamente,brutalmente,dando asco$~$", \
      "$mcname$ eructa$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_SNEEZE : ({ ({ 0, "a %s", 2, "%s a %s", 3, "%s" }), ({ \
      "Estornudas $arg:violentamente,ruidosamente,silenciosamente,de repente,"+ \
      "inesperadamente$", \
      "$mcname$ estornuda $lastarg$", \
      "Estornudas $arg:violentamente,ruidosamente,silenciosamente,de repente,"+ \
      "inesperadamente$ salpicando a $hcname$", \
      "$mcname$ estornuda $lastarg$ salpicándote", \
      "$mcname$ estornuda $lastarg$ salpicando a $hcname$" }) }), \
  SOUL_CMD_SPIT : ({ ({ 0, "%s", 0,"a %s", 3, "%s" }), ({ \
      "Escupes$ifarg: $arg:habilidosamente,dando asco$~$", \
      "$mcname$ escupe$ifarg: $lastarg$~$", \
      "Le escupes a $mhcname$ en la cara", \
      "$mcname$ te escupe en la cara", \
      "$mcname$ le escupe a $mhcname$ en la cara" }) }), \
  SOUL_CMD_WAIT : ({ ({ \
      0, "%s", 0, "a %s", 0, "a %s %s", 1, "%s %s", \
      0, "por %s", 1, "%s por %s", \
      2, "%s %s", 3, "%s" }), ({ \
      "Esperas$ifarg: $arg:pacientemente,impacientemente,medio dormido,con tristeza,"+ \
      "alegremente,cansadamente,solitariamente,exciatadamente,con ternura$~$", \
      "$mcname$ espera$ifarg: $lastarg$~$", \
      "Esperas$ifarg: $arg:arg:pacientemente,impacientemente,medio dormido,con tristeza,"+ \
      "alegremente,cansadamente,solitariamente,excitadamente,con ternura$~$ por $hcname$", \
      "$mcname$ te espera$ifarg: $lastarg$~$", \
      "$mcname$ espera $ifarg: $lastarg$~$ a $hcname$" }) }), \
])

#define SOUL_DATA_F ([  \
  SOUL_CMD_ANNOY : ({ ({ 0, "%s", }), ({ \
      "Fastidias $hcname$ hasta hacer que desee tirarse por un barranco", \
      "$mcname$ te fastidia hasta el punto en que desearías tirarte por un barranco", \
      "$mcname$ fastidia a $hcname$ hasta que desea morirse", }) }), \
  SOUL_CMD_FLIRT : ({ ({ 0, "%s", 1, "%s con %s", 1, "%s %s", 2, "%s %s" }), ({ \
      "Flirteas$ifarg: $arg:seductoramente,desvergonzadamente,"+ \
      "adorablemente,amorosamente$~$ con $hcname$", \
      "$mcname$ flirtea$ifarg: $lastarg$~$ contigo", \
      "$mcname$ flirtea$ifarg: $lastarg$~$ con $hcname$" }) }), \
])

#define SOUL_DATA_G ([  \
  SOUL_CMD_GESTICULATE : ({ 0, ({ \
      "Gesticulas exageradamente", \
      "$mcname$ gesticula exageradamente" }) }), \
  SOUL_CMD_MOAN : ({ ({ 3, "%s" }), ({ \
      "Gimes$ifarg: $arg:ruidosamente,resignadamente,de placer,"+ \
      "de dolor,quejumbrosamente,con voz de ultratumba,suavemente,profundamente,"+ \
      "sensualmente,en agonía,de terror,por gemir$~$", \
      "$mcname$ gime$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_SNARL : ({ ({ \
      0, "%s", 0, "a %s", 1, "%s %s", 1, "%s a %s", \
      2, "a %s %s", 2, "%s %s", 3, "%s" }), ({ \
      "Gruñes $arg:amenazadoramente,demoníacamente,ruidosamente,inocentemente$", \
      "$mcname$ gruñe $lastarg$", \
      "Gruñes $arg:amenazadoramente,demoníacamente,ruidosamente,inocentemente$ a $hcname$", \
      "$mcname$ te gruñe $lastarg$", \
      "$mcname$ gruñe $lastarg$ a $hcname$" }) }), \
  SOUL_CMD_WINK : ({ ({ 0, "%s", 0, "a %s", 1, "%s a %s", 1, "%s %s", \
      2, "a %s %s", 2, "%s %s", 3, "%s" }), ({ \
      "Guiñas un ojo$ifarg: $arg:sugestivamente,ampliamente,lentamente,ruidosamente,"+ \
      "sospechosamente,seductoramente,con ganas de cachondeo$~$", \
      "$mcname$ guiña un ojo$ifarg: $lastarg$~$", \
      "Le guiñas el ojo$ifarg: $arg:sugestivamente,ampliamente,lentamente,ruidosamente,gruñonamente,"+ \
      "sospechosamente,seductoramente,con ganas de cachondeo$~$ a $hcname$", \
      "$mcname$ te guiña el ojo$ifarg: $lastarg$~$", \
      "$mcname$ le guiña el ojo$ifarg: $lastarg$~$ a $hcname$" }) }), \
])

#define SOUL_DATA_H ([  \
  SOUL_CMD_HICCUP : ({ 0, ({ /* exciting stuff */ \
      "Te da hipo", \
      "A $mcname$ le da hipo" }) }), \  
])

#define SOUL_DATA_I ([ ]) 

#define SOUL_DATA_J ([ ]) 
  SOUL_CMD_OATH : ({ ({ 0, "%s" }), ({ \
      "Juras tu eterno amor por $hcname$", \
      "$mcname$ te jura amor eterno", \
      "$mcname$ le jura su eterno amor a $hcname$" }) }), \
])

#define SOUL_DATA_K ([ ])

#define SOUL_DATA_L ([ \
  SOUL_CMD_BARK : ({ ({ 3, "%s" }), ({ \
      "Ladras como $arg:un perro,un lobo,un perro de caza,"+ \
      "un idiota$", \
      "$mcname$ ladra como $lastarg$", }) }), \
  SOUL_CMD_LICK : ({ ({ 0, "%s", 3, "%s", 1, "%s %s", 2, "%s %s" }), ({ \
      "Te lames los labios$ifarg: $arg:seductoramente,sensualmente,eróticamente$~$", \
      "$mcname$ se lame los labios$ifarg: $lastarg$~$", \
      "Lames a $hcname$$ifarg: $arg:suavemente,cariñosamente,como un perro,"+ \
      "por entero,sensualmente,de forma seductora,amorosamente,"+ \
      "eróticamente,suavemente$~$", \
      "$mcname$ te lame$ifarg: $lastarg$~$", \
      "$mcname$ lame a $hcname$$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_CRY : ({ ({ 3, "%s" }), ({ \
      "Lloras$ifarg: $arg:dolorosamente,asustadamente,"+ \
      "triunfalmente$~$", \
      "$mcname$ llora$ifarg: $lastarg$~$", }), }), \
])

#define SOUL_DATA_M ([ \
  SOUL_CMD_PAMPER : ({ ({ 0, "%s", 1, "%s %s", 2, "%s %s" }), ({ \
      "Le haces mimos a $hcname$$ifarg: $arg:alegremente,bromeando,"+ \
      "amorosamente,sensualmente,seductoramente,"+ \
      "con intenciones de hacer algo mas que mimos$~$", \
      "$mcname$ te hace mimos$ifarg: $lastarg$~$", \
      "$mcname$ le hace mimos a $hcname$$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_FEAT : ({ ({ 0, "%s" }), ({ \
      "Gritas aterradamente a $hcname$", \
      "$mcname$ te grita aterradamente", \
      "$mcname$ le grita a $hcname$ aterradamente" }) }), \
  SOUL_CMD_MEOW : ({ ({ 3, "%s", 3, "como %s" }), ({ \
      "Maúllas como $arg:un gato,un gatito,una gatita,"+ \
      "una gatita en celo$", \
      "$mcname$ maúlla como $lastarg$", }) }), \
])

#define SOUL_DATA_N ([ \
  SOUL_CMD_DENY : ({ ({ 0, "%s" }), ({ \
      "Lo niegas todo, todo, todo", \
      "$mcname$ lo niega absolutamente todo", \
      "Niegas todo lo que $hcname$ ha dicho", \
      "$mcname$ niega todo lo que has dicho", \
      "$mcname$ niega todo lo que $hcname$ ha dicho" }) }), \

#define SOUL_DATA_O ([ ])

#define SOUL_DATA_P ([ \
  SOUL_CMD_BLINK : ({ ({ 0, "%s", 3, "%s" }), ({ \
      "Parpadeas$ifarg: $arg:lentamente,cuidadosamente,alucinadamente,"+ \
      "somnoliento,estupefacto$~$", \
      "$mcname$ parpadea$ifarg: $lastarg$~$", \
      "Parpadeas ante $hcname$", \
      "$mcname$ parpadea ante ti", \
      "$mcname$ parpadea ante $hcname$" }) }), \
  SOUL_CMD_FORGIVE : ({ ({ 0, "%s", }), ({ \
      "Perdonas a $hcname$", \
      "$mcname$ te perdona", \
      "$mcname$ perdona a $hcname$", }), }), \
  SOUL_CMD_PROTEST : ({ ({ 3, "%s", }), ({ \
      "Protestas $arg:agriamente,amenazadoramente,sin ganas,"+ \
      "seductoramente,sugestivamente,escandalosamente,irritantemente,"+ \
      "de forma devastadora,impotentemente,débilmente,acaloradamente$", \
      "$mcname$ protesta $lastarg$", }) }), \
  SOUL_CMD_PING : ({ ({ 0, "%s" }), ({ \
      "Le haces '¡¡¡PING!!!' a $hcname$",  \
      "$mcname$ te hace '¡¡¡PING!!!'$force#ack $mname$#2$", \
      "$mcname$ le hace '¡¡¡PING!!!' a $hcname$" })  }), \
  SOUL_CMD_PONDER : ({ 0, ({ \
      "Ponderas la situación", \
      "$mcname$ pondera la situación" }) }), \
  SOUL_CMD_PROMISE : ({ ({ 0, "%s" }), ({ \
      "Le haces una promesa a $hcname$", \
      "$mcname$ te hace una promesa", \
      "$mcname$ le hace una promesa a $hcname$" }) }), \
  SOUL_CMD_THINK : ({ ({ 3, "%s", }), ({ \
      "Piensas $ifarg:$arg:#$$else$con cuidado para no hacerte daño~$", \
      "$mcname$ piensa $ifarg:$lastarg$$else$con cuidado para no hacerse daño~$" }) }), \
  SOUL_CMD_WONDER : ({ ({ 3, "%s" }), ({ \
      "Te preguntas $ifarg:$arg:#$$else$sobre la realidad y "+ \
      "lo aburrida que puede ser~$", \
      "$mcname$ se pregunta $ifarg:$arg:#$$else$sobre la realidad y "+ \
      "lo aburrida que puede ser~$", }) }), \
])

#define SOUL_DATA_Q ([ ])

#define SOUL_DATA_R ([ \
  SOUL_CMD_ACKNOWLEDGE : ({ ({ 0, "%s" }), ({ \
      "Reconoces a $hcname$", \
      "$mcname$ te reconoce", \
      "$mcname$ reconoce a $hcname$" }) }), \
  SOUL_CMD_BREATHE : ({ ({ 3, "%s", }), ({ \
      "Respiras $arg:rápidamente,sensualmente,dolorosamente,entrecortadamente,lento,una vez,"+ \
      "pesadamente,acaloradamente$", \
      "Respiras $lastarg$", }) }), \
  SOUL_CMD_BOW : ({ ({ 0, "%s", 0, "a %s", 1, "%s a %s", 1, "%s %s", 3, "%s" }), ({ \
      "Haces una reverencia$ifarg: $arg:solemne,profunda,formal,rápida,leve,"+ \
      "respetuosa,insolente,torpe,grácil,ágil,colorida,"+ \
      "sensual,conspiratoria,irrespetuosa,elegante$~$", \
      "$mcname$ hace una reverencia$ifarg: $lastarg$~$", \
      "Haces una reverencia$ifarg: $arg:solemne,profunda,formal,rápida,leve,"+ \
      "respetuosa,insolente,torpe,grácil,ágil,colorida"+ \
      "sensual,conspiratoria,irrespetuosa,elegante$~$ a $hcname$", \
      "$mcname$ te hace una reverencia$ifarg: $lastarg$~$", \
      "$mcname$ hace una reverencia$ifarg: $lastarg$~$ a $hcname$" }) }), \
  SOUL_CMD_LAUGH : ({ ({ \
      0, "de %s", 0, "%s", 1, "%s %s", 1, "%s de %s", \
      2, "de %s %s", 3, "%s"}), ({ \
      "$ifarg:Te ríes $arg:malignamente,malvadamente,tranquilamente,incontroladamente,educadamente,"+ \
      "de felicidad,extasiado,demoníacamente,infernalmente,"+ \
      "discretamente$"+ \
      "$else$Te ríes~$", \
      "$ifarg:$mcname$ se ríe $lastarg$$else$$mcname$ se ríe~$", \
      "Te ríes$ifarg: $arg:malignamente,malvadamente,tranquilamente,incontroladamente,educadamente,"+ \
      "de felicidad,extasiado,demoníacamente,infernalmente,"+ \
      "discretamente,$~$ de $hcname$", \
      "$mcname$ se ríe$ifarg: $lastarg$~$ de ti", \
      "$mcname$ se ríe$ifarg: $lastarg$~$ de $hcname$" }) }), \
  SOUL_CMD_SNORE : ({ ({ 0, "%s", 3, "%s" }), ({ \
      "$ifarg:Roncas $arg:ruidosamente,descuidadamente$"+ \
      "$else$Roncas con tranquilidad~$", \
      "$mcname$ ronca$ifarg: $lastarg$~$" }) }), \
])

#define SOUL_DATA_S ([ \
  SOUL_CMD_BLUSH : ({ ({ 3, "%s", 0, "%s", 1, "%s %s", 2, "%s %s" }), ({ \
      "Te sonrojas$ifarg: $arg:de verguenza,levemente,"+ \
      "modestamente,cual doncella virginal,acaloradamente$~$", \
      "$mcname$ se sonroja$ifarg: $lastarg$~$", \
      "Te sonrojas$ifarg: $arg:de verguenza,levemente,"+ \
      "modestamente,cual doncella virginal,acaloradamente$~$ por culpa de $hcname$", \
      "$mcname$ se sonroja$ifarg: $lastarg$~$ por ti", \
      "$mcname$ se sonroja$ifarg: $lastarg$~$ por $hcname$" }) }), \
  SOUL_CMD_GREET : ({ ({ 0, "%s" }), ({ \
      "Saludas a $hcname$ $arg:con la mano,alegremente,con los brazos abiertos,"+ \
      "cortesmente,histéricamente,"+ \
      "reservadamente,calurosamente,fríamente,insolentemente,secamente,"+ \
      "a lo loco,dando saltos de alegría,amorosamente,cariñosamente$", \
      "$mcname$ te saluda $lastarg$", \
      "$mcname$ saluda a $hcname$ $lastarg$", }) }), \
  SOUL_CMD_SMILE : ({ ({ 0, "%s", 0, "a %s", 1, "%s a %s", 2, "a %s %s", \
      1, "%s %s", 2, "%s %s", 3, "%s" }), ({ \
      "Sonríes$ifarg: $arg:malignamente,tramando algo,"+ \
      "de oreja a oreja,"+ \
      "desvergonzadamente,como si fueras idiota$~$", \
      "$mcname$ sonríe$ifarg: $lastarg$~$", \
      "Sonríes$ifarg: $arg:malignamente,tramando algo,"+ \
      "de oreja a oreja,"+ \
      "desvergonzadamente,como si fuera $mnumeral$ idiota$~$ a $hcname$", \
      "$mcname$ te sonríe$ifarg: $lastarg$~$", \
      "$mcname$ le sonríe$ifarg: $lastarg$~$ a $hcname$" }) }), \
  SOUL_CMD_LUCK : ({ ({ 0, "%s" }), ({ \
      "Le deseas mucha suerte a $hcname$", \
      "$mcname$ te desea mucha suerte", \
      "$mcname$ desea a $hcname$ que tenga mucha suerte" }) }), \
  SOUL_CMD_POINT : ({ ({ 0, "%s", 0, "a %s", 1, "%s a %s", 2, "a %s %s" }), ({ \
      "Señalas con el dedo$ifarg: $arg:acusando,índice$~$ a $hcname$", \
      "$mcname$ te señala con el dedo$ifarg: $lastarg$~$", \
      "$mcname$ señala con el dedo$ifarg: $lastarg$~$ a $hcname$" }) }), \
  SOUL_CMD_SEDUCE : ({ ({ 0, "%s", 1, "%s %s", 2, "%s %s" }), ({ \
      "Seduces a $hcname$$ifarg: $arg:atrevidamente,desvergonzadamente,"+ \
      "profesionalmente,con dulzura,sensualmente,adorablemente$~$", \
      "$mcname$ te seduce$ifarg: $lastarg$~$", \
      "$mcname$ seduce a $hcname$$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_SIGH : ({ ({ 3, "%s", 2, "%s %s", 1, "%s %s" }), ({ \
      "Suspiras$ifarg: $arg:profundamente,silenciosamente,desesperadamente,cansadamente,lentamente,"+ \
      "soñador,suavemente,de felicidad,de amor,pesadamente,"+ \
      "sarcásticamente,de alivio,con tristeza,de aburrimiento$~$", \
      "$mcname$ suspira$ifarg: $lastarg$~$", \
      "Suspiras$ifarg: $arg:profundamente,silenciosamente,desesperadamente,cansadamente,lentamente,"+ \
      "soñador,suavemente,de felicidad,de amor,pesadamente,"+ \
      "sarcásticamente,de alivio,con tristeza,de aburrimiento$~$ ante $hcname$", \
      "$mcname$ suspira$ifarg: $lastarg$~$ ante ti", \
      "$mcname$ suspira$ifarg: $lastarg$~$ ante $hcname$" }) }), \
  SOUL_CMD_WHISTLE : ({ ({ 0, "%s", 1, "%s %s", 2, "%s %s", 3, "%s" }), ({ \
      "Silbas $arg:inocentemente,muy alto,musicalmente,de forma agradable,"+ \
      "discordantemente$", \
      "$mcname$ silba $lastarg$", \
      "Le silbas $arg:inocentemente,muy alto,musicalmente,de forma agradable,"+ \
      "discordantemente$ a $hcname$", \
      "$mcname$ te silba $lastarg$", \
      "$mcname$ le silba $lastarg$ a $hcname$" }) }), \
])

#define SOUL_DATA_T ([ \
  SOUL_CMD_COUGH : ({ 0, ({ "Toses", "$mcname$ tose" }) }), \
  SOUL_CMD_TREMBLE : ({ 0, ({ \
      "Tiemblas$ifarg: de $arg:miedo,verguenza,frío,fiebre$~$", \
      "$mcname$ tiembla$ifarg: de $lastarg$~$" }) }), \
  SOUL_CMD_STAGGER : ({ ({ 3, "%s" }), ({ \
      "te tambaleas$ifarg: $arg:#$~$", \
      "$mcname$ se tambalea$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_HUM : ({ ({ 3, "%s" }), ({ \
      "Tarareas $arg:una alegre canción,muy mal,irritantemente,distraídamente,"+ \
      "silenciosamente,ruidosamente$", \
      "$mcname$ tararea $lastarg$" }), }), \
  SOUL_CMD_SHIVER : ({ ({ 3, "con %s", 3, "de %s" }), ({ \
      "Tiritas$ifarg: $arg:de miedo,de frío,ligeramente,#$~$", \
      "$mcname$ tirita$ifarg: $lastarg$~$" }) }), \
  SOUL_CMD_TEMPT : ({ ({ 0, "%s", 2, "%s con %s" }), ({ \
      "Tientas a $hcname$$ifarg: con $arg:dinero,joyas,"+ \
      "fresas,galletas$~$", \
      "$mcname$ te tienta$ifarg: con $lastarg$~$", \
      "$mcname$ tienta a $hcname$$ifarg: con $lastarg$~$" }) }), \
  SOUL_CMD_TRIP : ({ ({ 0, "%s" }), ({ \
      "Tropiezas", \
      "$mcname$ tropieza", \
      "Tropiezas con $hcname$", \
      "$mcname$ tropieza contigo", \
      "$mcname$ tropieza con $hcname$" }) }), \
])

#define SOUL_DATA_U ([ ])

#define SOUL_DATA_V ([ \
  SOUL_CMD_PUKE : ({ ({ 0, "en %s", 0, "%s", 0, "sobre %s"  }), ({ \
      "Iluminas el lugar con una vomitona de colores", \
      "$mcname$ ilumina el lugar con una vomitona de colores", \
      "Vomitas sobre $hcname$", "$mcname$ vomita sobre ti", \
      "$mcname$ vomita sobre $hcname$" }) }), \
])

#define SOUL_DATA_W ([ ])

#define SOUL_DATA_X ([ ])

#define SOUL_DATA_Y ([ ])

#define SOUL_DATA_Z ([ ])

