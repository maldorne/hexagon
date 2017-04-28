// Nuevo sistema climatico para CcMud, Folken 22/10/03

#define BASE "ciudad capital"
#define CIUDAD_CAPITAL "ciudad capital"
#define ROMPESPINA "colinas de rompespina"
#define GODEL "bosque de godel"
#define ESTEPA "estepas nubladas"
#define DARGAAD "dargaad ciudad"
#define MONT_NUBLADAS "montañas nubladas"
#define SQARGELD "sqargeld"
#define TEMAIL "valle de temail"
#define MORKHAINE "montañas morkhaine"
#define DELKHAINE "del-khaine"
#define UMROTH "umroth"
#define NEUBURG "neuburg"
#define MONGUL "mongul"
#define ANDUNN "andunn"


/* 
* Zonas que estan incluidas en este archivo:
*
* ([ zona : ({ lluvia, viento, temperatura, 
*        lluvia_media, viento_medio, temperatura_media, salidas }) ])
* salidas es un array con los nombres de las zonas adyacentes a esta.
*
* 0 es mejor tiempo, 100 es peor tiempo
*
* Los valores definidos aqui para lluvia, viento y temp deben ser los mismos que los
* valores medios (seran los iniciales y seran modificados en /lib/handlers/weather.c)
*
* IMPORTANTE: Añadiendo una nueva zona el clima se actualiza automaticamente.
*             Si lo que hacemos es modificar una YA existente (eliminarla, cambiar el 
*             grafo de zonas, o los valores medios), debemos actualizar el 
*             sistema climatico, llamando a la funcion reset_zones() de 
*             /lib/handlers/weather.c
* exec "/lib/handlers/weather.c"->reset_zones()
*
*/             
private mapping zonas;

void create()
{
  zonas = ([ 
        BASE : ({ 30, 30, 30, 30, 30, 30, 
          ({ ROMPESPINA, TEMAIL, }), 
          }),
        ROMPESPINA : ({ 40, 40, 40, 40, 40, 40, 
          ({ BASE, GODEL, }), 
          }),
        GODEL : ({ 40, 40, 40, 40, 40, 40, 
          ({ ROMPESPINA, ESTEPA, }),
          }),
        ESTEPA : ({ 60, 60, 60, 60, 60, 60, 
          ({ GODEL, MONT_NUBLADAS, SQARGELD, }),
          }),
        MONT_NUBLADAS : ({ 90, 90, 90, 90, 90, 90, 
          ({ ESTEPA, DARGAAD, }),
          }),
        DARGAAD : ({ 70, 70, 70, 70, 70, 70, 
          ({ MONT_NUBLADAS, }),
          }),
        SQARGELD : ({ 60, 60, 60, 60, 60, 60, 
          ({ ESTEPA, }),
          }),
        TEMAIL : ({ 30, 30, 30, 30, 30, 30, 
          ({ BASE, DELKHAINE, }), 
          }),
        DELKHAINE : ({ 50, 50, 50, 50, 50, 50, 
          ({ TEMAIL, MORKHAINE, }), 
          }),  
        MORKHAINE : ({ 70, 70, 70, 70, 70, 70, 
          ({ DELKHAINE, }), 
          }),  
        UMROTH : ({ 30, 30, 30, 30, 30, 30, 
          ({ MORKHAINE, NEUBURG, }), 
          }),
        NEUBURG : ({ 30, 30, 30, 30, 30, 30, 
          ({ UMROTH, MONGUL, ANDUNN, }), 
          }),
        MONGUL : ({ 70, 70, 70, 70, 70, 70, 
          ({ NEUBURG, }), 
          }),  
        ANDUNN : ({ 40, 40, 40, 40, 40, 40, 
          ({ NEUBURG, }), 
          }),  
        ]);
}

string query_zone(string path)
{
  /*
  
  Cambio el algoritmo para que pruebe reduciendo el path desde el total
  en lugar de ampliandolo desde la raiz. De media va a ser casi la mitad de iteraciones
  para cada llamada a query_zone. Folken 10/2009
  
  int i;
  string * words = explode(path, "/");
  string zone = BASE;
  string where;

  if (sizeof(words) < 2)
  return BASE;

  where = "/" + words[0];

  for(i = 1; i < sizeof(words); i++)
  {
  where += "/" + words[i];
  */
  
  string where;
  string * words;
  string zone;

  where = path;
  words = explode(where, "/");
  zone = "";

  // Nos quedamos solo con los directorios
  where = "/" + implode(words[0..sizeof(words)-2], "/");

  while ((zone == "") && (sizeof(words = explode(where, "/")) > 1))
  {
    switch(where)
    {
      case "/game/areas/ciudad-capital":
      case "/game/areas/grupos/cazadores/bosque_de_porte":
      case "/game/areas/grupos/guardia_escarlata":
      case "/game/areas/grupos/caballeros_astrales/ciudad-capital":
      case "/game/areas/grupos/cofradia_comadrejas":
      case "/game/areas/grupos/clerigos_nhimue/nhimue_cc_oeste":
      case "/game/areas/grupos/clerigos_rael/rael_cc_oeste":
      case "/game/areas/grupos/templarios_negros/ciudad-capital":
        zone = CIUDAD_CAPITAL;
        break;
        
      case "/game/areas/ciudad-capital/rompespina":
        zone = ROMPESPINA;
        break;

      case "/game/areas/roth":
        zone = TEMAIL;
        break;
        
      case "/game/areas/morkhaine":
        zone = MORKHAINE;
        break;
      case "/game/areas/morkhaine/delkhaine":
      case "/game/areas/morkhaine/cordillera/ascenso_temail":
        zone = DELKHAINE;
        break;                  
                  
      case "/game/areas/umroth":
      case "/game/areas/grupos/cazadores/umroth":
      	zone = UMROTH;
      	break;

      case "/game/areas/neuburg":
      	zone = NEUBURG;
      	break;

      case "/game/areas/andunn":
      	zone = ANDUNN;
      	break;

      case "/game/areas/mongul":
      case "/game/areas/grupos/cazadores/mongul":
      	zone = MONGUL;
      	break;

      case "/game/areas/ciudad-capital/bosque_godel":
      case "/game/areas/ciudad-capital/colinas_pensut":
        zone = GODEL;
        break;

      case "/game/areas/dargaad":
      case "/game/areas/grupos/cazadores/bosque_de_jukingnall":
      case "/game/areas/grupos/druidas/jukingnall":
        zone = DARGAAD;
        break;

      case "/game/areas/dargaad/estepa":
      case "/game/areas/dargaad/alcazar":
      case "/game/areas/grupos/templarios_negros/hierro_negro":
      case "/game/areas/grupos/caballeros_lobo":
      case "/game/areas/dargaad/colinas_umbrias":
      case "/game/areas/dargaad/valle_nieblas":
      case "/game/areas/dargaad/nodd":
      case "/game/areas/oceano/lago_esmeralda":
        zone = ESTEPA;
        break;

      case "/game/areas/dargaad/montanyas_nubladas":
      case "/game/areas/dargaad/siempreinvierno":
      case "/game/areas/nhorrod":
        zone = MONT_NUBLADAS;
        break;

      case "/game/areas/sqargeld":
      case "/game/areas/grupos/cazadores/sqargeld":
        zone = SQARGELD;
        break;
        
    } // fin switch

    // Subimos un nivel de directorios, ej: de /game/areas/ciudad-capital/rompespina a /game/areas/ciudad-capital
    where = "/" + implode(words[0..sizeof(words)-2], "/");

  } // fin while

  if (zone != "")
    return zone;
  else
    return BASE;
}

// Para pedir los datos completos desde el handler del clima
mixed query_zones()
{ 
  call_out("dest_me", 1);
  return zonas; 
}

