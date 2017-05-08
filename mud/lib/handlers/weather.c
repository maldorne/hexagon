/*
 * weather.c, neverbot@CcMud 22/10/03
 * Version 3.1
 *
 * Tras mucho probar y retocar el sistema de clima antiguo, lo mejor es
 * hacer uno completo desde cero.
 * 
 * Añadida luz por las lunas durante la noche, neverbot 20/02/04
 * Añadida la nieve, solo habia lluvia y granizo :( neverbot 20/02/04
 */
 
inherit "/lib/core/object.c";

#include <areas/weather.h>

// Constantes de duracion del tiempo-mud
// Un tick dura 60 segundos (un minuto)
// Un dia dura 60 ticks (una hora)
// Un año son 366 dias (algo mas de dos semanas)
#define TICKS 60
#define DIA 60
#define ANYO 366

int ticks,         // [0..60]
    hora_del_dia,   // [0..23]
    dia_de_semana, // [0..6]
    dia_del_mes,   // [0..num_dias_del_mes - 1]
    dia_del_anyo,   // [0..365]
    mes,           // [0..11]
    estacion,       // [0..3]
    anyo;           // x

int luna_uno,    // Fase lunar en la que esta cada luna.
    luna_dos,
    luna_tres;

int * luna_uno_oculta;  // Arrays con la hora del dia a la que la luna
int * luna_dos_oculta;  // se oculta y comienza a ser visible.
int * luna_tres_oculta;

// Lista de zonas con el estado climatico de cada
// una de ellas
mapping zones;    

// Lista de objetos a los que hemos de notificar
// sobre los eventos del clima.
static private mapping my_obs;  

// Prototipos de las funciones
void check_zones();
string month_string();
int month_days();
void update_weather(int flag);
int query_day();
int query_dawn_time();
int query_nightfall_time();
void check_season(int flag);

void create(){

  ticks = 0;
  hora_del_dia = 0;
  dia_de_semana = 0;
  dia_del_mes = 0;
  dia_del_anyo = 0;
  mes = 0;
  estacion = 0;
  anyo = 0;
  
  luna_uno = 0;
  luna_dos = 0;
  luna_tres = 0;
  
  // Hora a la que se oculta la luna y vuelve a salir
  luna_uno_oculta = ({ 6, 21 });
  luna_dos_oculta = ({ 6, 21 });
  luna_tres_oculta = ({ 6, 21 });
  
  my_obs = ([ ]);
  zones = ([ ]);

  restore_object(SAVE_FILE, 1);

  // Comprobamos las zonas climaticas del mud, por si hay alguna nueva...
  check_zones();
  
  ::create();
}

void setup() 
{
  // Anticloning del obj/shut, neverbot 6/03
  if (file_name(this_object()) != "/lib/handlers/weather") 
  {
    write("Este objeto no puede ser clonado.\n");
    dest_me();
    return;
  }
  
  reset_get();
  set_name("tiempo");
  set_short("Controlador extraordinario del tiempo");
  set_long("¡Éste es el controlador extraordinario del tiempo!\n");
  add_alias("controlador");
  
  call_out("update_low", TICKS);
  // Move esta redefinida, siempre mueve el objeto a HOME
  move("bing");
}

/* have this move here to get it to my weather room ;) (Sleaze) */
int move(mixed dest, varargs mixed messin, mixed messout) {
  ::move(HOME, messin, messout);
}

void save_weather() { 
//tell_object(find_living("folken"), "(WEATHER) Salvando clima...\n");
  save_object(SAVE_FILE, 1); 
}

void dest_me() {
  save_object(SAVE_FILE, 1);
  ::dest_me();
}

// ********************************************************************
//  Funciones de notificacion de eventos del clima a objetos, mismo
//  sistema que el original
// ********************************************************************

mapping query_my_obs() { return my_obs; }

// The object asks to be told about weather events
void notify_me(object ob) 
{
  my_obs[ob]++;
}

// The object tells us it is really bored by these damn stupid weather events
void unnotify_me(object ob) 
{
  if (!my_obs[ob]) return;
  if (--my_obs[ob] <= 0)
    my_obs = m_delete(my_obs, ob);
}

// Funcion que informa a todos los objetos necesarios del mud
//  de cualquier cambio relevante en el clima (cambios climaticos,
//  amanecer/anochecer, etc).
// flag: 1 = amanecer
//       2 = anochecer
//       3 = cambio climatico
// zone: nombre de la zona climatica (nombre, no el path!!)
// values: lista con 6 int que son: 
//         lluvia, viento y temperatura antes del cambio seguidos de
//         lluvia, viento y temperatura nuevos
void weather_inform(int flag, varargs string zone, int * values)
{
  object * obs; // lista de objetos a los que informar de cambios en el clima
  int i;

  // de los cambios dia/noche informamos a todos los objetos
  if ((flag == FLAG_ANOCHECER) || (flag == FLAG_AMANECER))
  {
    obs = m_indices(my_obs);
    for (i = 0; i < sizeof(obs); i++)
      if (objectp(obs[i]))
        event(obs[i], "weather", flag);
      // neverbot, 7/03 (por si hay algun fallo, se elimina lo que sea que hay en la lista)
      else
        my_obs = m_delete(my_obs, obs[i]);
    return;
  }
  
  // De este flag solo se informa a npcs !!!
  if (flag == FLAG_CHECK){
    obs = m_indices(my_obs);
  for (i = 0; i < sizeof(obs); i++)
    if (objectp(obs[i]))
    {
      if (obs[i]->query_npc() && obs[i]->query_timed_npc())
                event(obs[i], "weather", flag);
    }
    // neverbot, 7/03 (por si hay algun fallo, se elimina lo que sea que hay en la lista)
    else 
      my_obs = m_delete(my_obs, obs[i]);
    return;
  }
  
  obs = m_indices(my_obs);

  // De este flag solo se informa a rooms y players!!
  // (aunque los players nunca deberian estar en la lista de my_obs, y
  //  tanto players como npcs que esten en esas rooms si recibiran el inform)
  for (i = 0; i < sizeof(obs); i++)
  {
    if (!objectp(obs[i]))
    {
      my_obs = m_delete(my_obs, obs[i]);
      break;
    }

    // Si el objeto no tiene environment (es una room)
    if (!environment(obs[i]))
    {
      if (zone == table(WEATHER_TABLE)->query_zone(file_name(obs[i])))
        event(obs[i], "weather", flag, values);
    }

    // Esto no deberia suceder nunca :(
    if (obs[i]->query_player())
    {
      if ((environment(obs[i])->query_outside()) &&
          (zone == table(WEATHER_TABLE)->query_zone(file_name(environment(obs[i])))))
        event(obs[i], "weather", flag, values);
    }
  }
}

// ********************************************************************

// Funcion que se llama recursivamente proporcionando el funcionamiento
//  del sistema climatico
void update_low() 
{

  ticks++;
  
  // Se da un tick del clima a cada minuto de tiempo real
  // Hacemos la correspondencia entre el numero de ticks de un dia
  // en tiempo-mud y las 24 horas que deberia tener, para saber cuando
  // tenemos que incrementar las horas del mundo de juego.  
  if ((ticks * 24)/TICKS > hora_del_dia)
  {
    // Incrementamos la hora del dia
    hora_del_dia++;
    if (hora_del_dia == query_dawn_time())
      weather_inform(FLAG_AMANECER);
    else if (hora_del_dia == query_nightfall_time())
      weather_inform(FLAG_ANOCHECER);
    // inform especial para npcs, se manda cada hora (ver weather_inform)
    else
      weather_inform(FLAG_CHECK);

//tell_object(find_living("folken"), "(WEATHER) Pasa una hora (ticks = "+ticks+" horas = "+hora_del_dia+").\n");    

    // Si el dia ha terminado
    if (hora_del_dia >= 24)
    {
      hora_del_dia = 0;
      dia_del_mes++;
      dia_de_semana++;
      dia_del_anyo++;
      
      // Cada dia avanzamos la luna_uno
      luna_uno++;
      // Cada 4 dias avanzamos la luna_dos
      if ((luna_uno % 4) == 0) 
        luna_dos++;
      // Cada semana (7 dias) avanzamos la luna_tres
      if ((luna_uno % 7) == 0) 
        luna_tres++;
      
      // Los ciclos lunares se repiten cada 10 pasos
      luna_uno = luna_uno % 10;
      luna_dos = luna_dos % 10;
      luna_tres = luna_tres % 10;
      
      // El dia 22 (por ejemplo) de diciembre, marzo, junio y septiembre
      // cambiamos la estacion
      if (dia_del_mes == 21)
      {
        // 0 == enero .. 11 == diciembre
        if (mes == 11)
        {
           estacion = 0; // comienza el invierno
           check_season(0);
        }
        if (mes == 2)
        {
           estacion = 1; // comienza la primavera
           check_season(0);
        }
        if (mes == 5)
        {
           estacion = 2; // comienza el verano
           check_season(0);
        }
        if (mes == 8)
        {
           estacion = 3; // comienza el otoño
           check_season(0);
        }
      }
  
      // Cada 7 dias comienza una nueva semana
      if (dia_de_semana >= 7)
      {
        dia_de_semana = 0;
      }
      // Cada vez que termina el mes
       if (dia_del_mes >= month_days())
      {
        dia_del_mes = 0;
        mes++;
        if (mes >= 12)
        {
          mes = 0;
          dia_del_anyo = 0;
          anyo++;
        }
       }
    }

    // Aqui terminamos la actualizacion del tiempo-mud
  
    // Cada 4 horas actualizamos el clima
    // Las 4 horas es un valor escogido aleatoriamente para no actualizar
    // el clima demasiado a menudo, puede ser cambiado.
    if ((hora_del_dia % 4) == 0)
    {
      // flag == 0 para que no salve, ya lo hacemos en esta funcion
      update_weather(0);
    }
  }

  save_weather();  

  // ticks nunca pasa de 60
  ticks = ticks % TICKS;

  call_out("update_low", TICKS);
}

// Para el funcionamiento de los timed_npcs, etc...
int * query_date_data()
{
  return ({ hora_del_dia, dia_del_anyo + 1, mes + 1, estacion + 1, anyo, dia_del_mes, });
} 

// Funciones para la fecha (de tiempo de mud), neverbot 6/03
string date_string()
{
  int * data;
  string ret;

  data = query_date_data();

  if (data[0] == 1)
    ret = "Es la una ";
  else
    ret = "Son las " + data[0] + " ";//horas ";
  
  ret += "del " + capitalize(handler(CALENDAR_HANDLER)->query_week_day_string()) + " " + (dia_del_mes + 1) + " de "+
        // month_string()+" del año "+data[4]+". Temporada de "+capitalize(season_string());
        capitalize(month_string())+" del año "+ handler(CALENDAR_HANDLER)->query_year_name(data[4]);
  
  return ret;
}

// Funciones para la fecha (de tiempo de mud), neverbot 6/03
int month_days()
{
  switch(mes){
    case 0:   return 31;  // 31 dias enero
    case 1:   return 29;  // 29 dias febrero
    case 2:   return 31;  // 31 marzo
    case 3:   return 30;  // 30 abril
    case 4:   return 31;  // 31 ...
    case 5:   return 30;  // 30
    case 6:   return 31;  // 31
    case 7:   return 31;  // 31
    case 8:   return 30;  // 30
    case 9:   return 31;  // 31
    case 10:  return 30;  // 30
    case 11:  return 31;  // 31 diciembre
    default:  return 30;
  }
}

// Funciones para la fecha (de tiempo de mud), neverbot 6/03
string month_string()
{
  switch(mes){
    case 0:  return "enero";
    case 1:  return "febrero";
    case 2:  return "marzo";
    case 3:  return "abril";
    case 4:  return "mayo";
    case 5:  return "junio";
    case 6:  return "julio";
    case 7:  return "agosto";
    case 8:  return "septiembre";
    case 9:  return "octubre";
    case 10: return "noviembre";
    default:
    case 11: return "diciembre";
  }
}

// Actualiza los valores medios climaticos segun la estacion en la que nos 
// encontremos.
// flag == 0: cambio natural de estacion (orden logico), se cambian los valores siempre
// flag == 1: se resetean las zonas, solo se cambian si estamos en invierno o verano
void check_season(int flag)
{
  string * zone_names;
  int i;
  
  zone_names = keys(zones);

  for (i = 0; i < sizeof(zone_names); i++)
  {
    switch(estacion)
    {
      case 0: // invierno
        zones[zone_names[i]][3] += 20;
        zones[zone_names[i]][4] += 20;
        zones[zone_names[i]][5] += 20;
        break;
      case 1: // primavera
        // solo cambiamos las medias si venimos del invierno (los valores
        // deben volver a los normales)
        if (!flag){
         zones[zone_names[i]][3] -= 20;
         zones[zone_names[i]][4] -= 20;
         zones[zone_names[i]][5] -= 20;
        }
        break;
      case 2: // verano
        zones[zone_names[i]][3] -= 20;
        zones[zone_names[i]][4] -= 20;
        zones[zone_names[i]][5] -= 20;
        break;        
      case 3: // otoño
        // solo cambiamos las medias si venimos del verano (los valores
        // deben volver a los normales)
        if (!flag)
        {
          zones[zone_names[i]][3] += 20;
          zones[zone_names[i]][4] += 20;
          zones[zone_names[i]][5] += 20;
        }
        break;    
    }

    // Comprobamos que no nos hemos salido de los limites [0..100]
    if (zones[zone_names[i]][3] < 0)
       zones[zone_names[i]][3] = 0;
    if (zones[zone_names[i]][3] > 100)
       zones[zone_names[i]][3] = 100;
    if (zones[zone_names[i]][4] < 0)
       zones[zone_names[i]][4] = 0;
    if (zones[zone_names[i]][4] > 100)
       zones[zone_names[i]][4] = 100;
    if (zones[zone_names[i]][5] < 0)
       zones[zone_names[i]][5] = 0;
    if (zones[zone_names[i]][5] > 100)
       zones[zone_names[i]][5] = 100;
  }
}

// Reseteamos todas las zonas climaticas a su valor original
// Habra que utilizarla si modificamos las zonas climaticas en la tabla
// (cambiamos el grafo de salidas o los valores medios)
// NOTA: El map_mapping parece que no funciona como deberia, por lo que siempre estamos
//   utilizando el mismo mapping tanto en TABLE como aqui... siempre
//   que llamemos a esta funcion deberiamos hacer un discard/update de TABLE antes,
//   para asegurar que el mapping que copiemos sea el original
void reset_zones()
{
  log_file(LOG_FILE, "Reseteamos las zonas climáticas " + ctime(time(), 4) + "\n");  
  
  // El map_copy nos sirve para copiar el mapping completo (no solo
  // su direccion). De otro modo estaremos siempre modificando el mapping
  // cargado en memoria de TABLE).
  zones = map_copy(table(WEATHER_TABLE)->query_zones());

  // Actualizamos las medias segun la estacion en la que nos encontremos
  // (en verano hara mas calor y en invierno mas frio)  
  check_season(1);
  save_weather();
}

// Comprobamos todas las zonas climaticas definidas en la tabla, por si hemos añadido
//  alguna nueva
void check_zones()
{
  mapping table_zones;
  string * table_names;
  string * names;
  int i;
  int changed;

  changed = 0;
  table_zones = map_copy(table(WEATHER_TABLE)->query_zones());
  table_names = keys(table_zones);
  names = keys(zones);
  
  for (i = 0; i < sizeof(table_names); i++)
  {
    // Si la zona de la tabla no esta en nuestra lista
    // de zonas, la añadimos
    if (member_array(table_names[i], names) == -1)
    {
      zones[table_names[i]] = table_zones[table_names[i]];
      log_file(LOG_FILE, "Weather zone added: "+table_names[i] + 
        " " +ctime(time(), 4) + "\n");
      changed = 1;
    }
  }
  
  if (changed)
    save_weather();
}

// Cada vez que modifiquemos los valores climaticos de una zona,
// intentaremos balancear el grafo de zonas, para no encontrarnos con
// cambios bruscos al cambiar de una a otra...
// Lo que haremos sera modificar levemente las zonas adyacentes a las 
// modificadas (las llevaremos a la media entre ambas).
void balance_weather(string * modified_zones)
{
  string * zones_to_modify;
  int i, j;
  mixed * old_zone;
  mixed * new_zone;
  mapping zones_to_inform;

  zones_to_modify = ({ });
  old_zone = ({ });
  new_zone = ({ });
  zones_to_inform = ([ ]);

  for (i = 0; i < sizeof(modified_zones); i++)
  {
    // Zona de la que hemos modificado el clima
    old_zone = zones[modified_zones[i]];

//tell_object(find_living("folken"), "(WEATHER) Rebalanceando adyacentes de "+modified_zones[i]+"\n");

    // Lista de adyacentes
    zones_to_modify = old_zone[6];
    // Eliminamos de la lista de adyacentes los que hemos modificado
    // (los balanceados no cuentan, es posible que balanceemos algunas
    //  zonas varias veces si varios de sus climas adyacentes han variado)
    zones_to_modify -= modified_zones;
    
    for( j = 0; j < sizeof(zones_to_modify); j++)
    {
      // Zona adyacente que vamos a balancear

//tell_object(find_living("folken"), "(WEATHER) Rebalanceamos "+zones_to_modify[j]+"\n");

      new_zone = zones[zones_to_modify[j]];
      zones_to_inform[zones_to_modify[j]] = ({ new_zone[0], new_zone[1], new_zone[2], });
      new_zone[0] -= (new_zone[0] - old_zone[0])/2;
      new_zone[1] -= (new_zone[1] - old_zone[1])/2;
      new_zone[2] -= (new_zone[2] - old_zone[2])/2;
      // Comprobamos los limites
      if (new_zone[0] < 0) new_zone[0] = 0;
      if (new_zone[1] < 0) new_zone[1] = 0;
      if (new_zone[2] < 0) new_zone[2] = 0;
      if (new_zone[0] > 100) new_zone[0] = 100;
      if (new_zone[1] > 100) new_zone[1] = 100;
      if (new_zone[2] > 100) new_zone[2] = 100;
      zones_to_inform[zones_to_modify[j]] += ({ new_zone[0], new_zone[1], new_zone[2], });
    }
  }
  // Informamos de los eventos de cambio de clima
  // Reutilizamos la variable zones_to_modify
  zones_to_modify = keys(zones_to_inform);
  for( i = 0; i < sizeof(zones_to_modify); i++)
  {
    weather_inform(FLAG_CLIMA, zones_to_modify[i], zones_to_inform[zones_to_modify[i]]);
  }
}

// Si flag es 1 salvamos al final, si es 0 no
// (sera 1 cuando llamemos desde algun lugar de control del clima,
//  0 cuando se llame desde este archivo (update_low))
void update_weather(int flag)
{
  
  string * names;
  string * names_to_modify;
  int azar, i, cuanto;
  mixed * zone_data;
  int * inform;

  names_to_modify = ({ });
  zone_data = ({ });
  inform = ({ });

//tell_object(find_living("folken"), "(WEATHER) Actualizando clima...\n");
  
  names = keys(zones);
  for (i = 0; i < MODIFY; i++)
  {
    azar = random(sizeof(names));
    names_to_modify += ({ names[azar] });
    // Eliminamos el nombre de la lista para seleccionar siempre 
    // zonas distintas
    names -= ({ names[azar] });
    // Con el delete no funciona... :(
    // delete(names, azar, 1);
  }

  // En names_to_modify tenemos la lista de zonas a modificar...
  // Para cada una de ellas incrementaremos o reduciremos los valores
  // climaticos de lluvia, viento y temperatura (frio) al azar:
  
  for (i = 0; i < sizeof(names_to_modify); i++)
  {
    inform = ({ });
    azar = random(2); // 0 == reducimos, 1 == aumentamos

    zone_data = zones[names_to_modify[i]];
    // Almacenamos los valores previos a los cambios
    inform += ({ zone_data[0], zone_data[1], zone_data[2], });

    if (azar)
    {
//tell_object(find_living("folken"), "(WEATHER) Aumentamos los valores de "+names_to_modify[i]+"\n");
      // Lluvia
      cuanto = zone_data[0] - zone_data[3];
      if (cuanto < 0) { cuanto *= -1; }
      if (cuanto < 10) { cuanto = DEF_INC; }
      zone_data[0] += random(cuanto);
      // Viento
      cuanto = zone_data[1] - zone_data[4];
      if (cuanto < 0) { cuanto *= -1; }
      if (cuanto < 10) { cuanto = DEF_INC; }
      zone_data[1] += random(cuanto);
      // Temperatura
      cuanto = zone_data[2] - zone_data[5];
      if (cuanto < 0) { cuanto *= -1; }
      if (cuanto < 10) { cuanto = DEF_INC; }
      zone_data[2] += random(cuanto);
    }
    else
    {
//tell_object(find_living("folken"), "(WEATHER) Reducimos los valores de "+names_to_modify[i]+"\n");

      // Lluvia
      cuanto = zone_data[0] - zone_data[3];
      if (cuanto < 0) { cuanto *= -1; }
      if (cuanto < 10) { cuanto = DEF_INC; }
      zone_data[0] -= random(cuanto);
      // Viento
      cuanto = zone_data[1] - zone_data[4];
      if (cuanto < 0) { cuanto *= -1; }
      if (cuanto < 10) { cuanto = DEF_INC; }
      zone_data[1] -= random(cuanto);
      // Temperatura
      cuanto = zone_data[2] - zone_data[5];
      if (cuanto < 0) { cuanto *= -1; }
      if (cuanto < 10) { cuanto = DEF_INC; }
      zone_data[2] -= random(cuanto);
    }

    // Comprobamos los limites
    if (zone_data[0] < 0) zone_data[0] = 0;
    if (zone_data[1] < 0) zone_data[1] = 0;
    if (zone_data[2] < 0) zone_data[2] = 0;
    if (zone_data[0] > 100) zone_data[0] = 100;
    if (zone_data[1] > 100) zone_data[1] = 100;
    if (zone_data[2] > 100) zone_data[2] = 100;

    // Almacenamos tambien los nuevos valores
    inform += ({ zone_data[0], zone_data[1], zone_data[2], });
    
    // Informamos del evento de cambio climatico
    weather_inform(FLAG_CLIMA, names_to_modify[i], inform);
  }
  
  // Ahora que hemos actualizado algunas zonas, conviene hacer un
  // 'rebalanceo' del grafo de zonas climaticas, para no encontrarnos
  // con cambios muy bruscos al movernos de una zona a otra...
  // Espero que de un resultado mas o menos aparente... 
  balance_weather(names_to_modify);
  
  if(flag)
    save_weather();
}

// ***********************************************************************
// Funciones para compatibilidad con el anterior sistema (string que se piden desde
// el comando mirar o desde las rooms outside, etc...
// ***********************************************************************

// Utilizada desde las rooms outside, para saber el nivel de luz en la room
// Devuelve un porcentaje: 100 == luz total, 0 == oscuridad total
int query_darkness(object room)
{
  int cuanto;
  string zona;

  zona = table(WEATHER_TABLE)->query_zone(file_name(room));
  
  if (query_day())
  {
    cuanto = 100;
    // Reducimos la visibilidad por la lluvia y el viento
    cuanto -= zones[zona][0]/4;
    cuanto -= zones[zona][1]/5;
    // Primeras dos horas del amanecer, menos luz
    if ((hora_del_dia > query_dawn_time()) && 
        (hora_del_dia < query_dawn_time() + 2) )
        cuanto -= 10;
    // Ultimas horas del dia, menos luz
    if ((hora_del_dia < query_nightfall_time()) && 
        (hora_del_dia > query_nightfall_time() - 2) )
        cuanto -= 10;
  }
  else
  {
    cuanto = 25;
    // Comprobamos el estado de cada luna para añadir algo de luz a la noche
    // la luna uno proporciona mas luz, la dos menos y la tres aun menos
    if (luna_uno == 0) // llena
      cuanto += 10;
    // estados cercanos a llena
    else if (((luna_uno >= 1) && (luna_uno <= 3)) || 
         ((luna_uno >= 7) && (luna_uno <= 9))) 
      cuanto += 5;
    
    if (luna_dos == 0) // llena
      cuanto += 8;
    // estados cercanos a llena
    else if (((luna_dos >= 1) && (luna_dos <= 2)) || 
         ((luna_dos >= 8) && (luna_dos <= 9))) 
      cuanto += 4;
    
    if (luna_tres == 0) // llena
      cuanto += 6;
    // estados cercanos a llena
    else if ((luna_tres == 1) || (luna_tres == 9)) 
      cuanto += 3;

    /* Orden de las lunas para comprender lo anterior:
     ({ "llena",
            "tres cuartos menguante",
            "media menguante",
            "menguante",
            "menguante",
            "nueva",
            "creciente",
            "creciente",
            "media creciente",
            "tres cuartos creciente" })
            */
  }

  if (cuanto < 0)
    return 0;
  if (cuanto > 100)
    return 100;
  return cuanto;
}

// Utilizada para tonterias varias... quiza pueda ser de mayor utilidad en el futuro
int query_raining(object room)
{
  string zona;
  zona = table(WEATHER_TABLE)->query_zone(file_name(room));
  return (zones[zona][0] >= 50);
}

// Comprobada desde weather_heart_beat, para saber si debemos hacer daño por frio, etc
int * query_actual_data(object room)
{
  string zona;
  zona = table(WEATHER_TABLE)->query_zone(file_name(room));
  return ({ zones[zona][0], zones[zona][1], zones[zona][2], });
}

// Hora a la que amanece
int query_dawn_time()
{
  // invierno, primavera, verano, otoño
  switch(estacion)
  {
    case 0: return 9;
    case 1: return 8;
    case 2: return 7;
    default:
    case 3: return 8;
  }
}

// Hora a la que anochece
int query_nightfall_time()
{
  // invierno, primavera, verano, otoño
  switch(estacion)
  {
    case 0: return 20;
    case 1: return 21;
    case 2: return 22;
    default:
    case 3: return 21;
  }
}  

// 1 si es de dia, 0 en otro caso
int query_day()
{
  if ((hora_del_dia >= query_dawn_time()) && (hora_del_dia < query_nightfall_time()))
    return 1;
  else
    return 0;
}

// Valores de la temperatura en cada zona:
// [0..9]  calor abrasador
// [10..19] mucho calor
// [20..29] caluroso
// [30..39] clima cálido
// [40..59] buena temperatura
// [60..69] clima frío
// [70..79] frío
// [80..89] mucho frío
// [>=90]   frío polar
string temperature_string(string zona)
{
  int cuanto;
  string ret;

  cuanto = zones[zona][2];
  ret = "";

  switch(cuanto)
  {
    case 0..9:
      ret = "un calor abrasador";
      break;
    case 10..19:
      ret = "mucho calor";
      break;
    case 20..29:
      ret = "un clima caluroso";
      break;
    case 30..39:
      ret = "un clima cálido";
      break;
    case 40..59:
      ret = "buena temperatura";
      break;
    case 60..69:
      ret = "un clima frío";
      break;
    case 70..79:
      ret = "frío";
      break;
    case 80..89:
      ret = "mucho frío";
      break;
    case 90..100:
      ret = "un frío polar";
      break;
  }
  return ret;
}

// Valores del viento en cada zona:
// [0..59]  no devolvemos nada
// [60..75] viento fuerte
// [76..90] viento muy fuerte
// [91..98] ventisca
// [>=99]   viento huracanado
string wind_string(string zona)
{
  int cuanto;
  string ret;

  cuanto = zones[zona][1];
  ret = "";

  switch(cuanto)
  {
    case 0..59:
      ret = "";
      break;
    case 60..75:
      ret = "viento fuerte";
      break;
    case 76..90:
      ret = "viento muy fuerte";
      break;
    case 91..98:
      ret = "ventisca";
      break;
    case 99..100:
      ret = "viento huracanado";
      break;
  }
  return ret;
}

// Valores de lluvia en cada zona:
// [0..49]  no llueve
// [50..59] lluvia suave
// [60..69] lluvia
// [70..79] tormenta 
// [80..89] tormenta fuerte
// [>=90]   tormenta pesada (la tormenta perfecta :))
string rain_string(string zona)
{
  int cuanto;
  string ret;
  int mod;

  cuanto = zones[zona][0];
  ret = "";
  mod = 0;
  
  // Si el frio esta a mas de 80 no hay lluvia, hay granizo.
  // Y si esta a mas de 85 no hay granizo, hay nieve
  if(zones[zona][2] > 80) mod = 1;
  if(zones[zona][2] > 85) mod = 2;

  switch(cuanto)
  {
    case 0..49:
      ret = "";
      break;
    case 50..59:
      ret = "lluvia suave";
      break;
    case 60..69:
      ret = "lluvia";
      break;
    case 70..79:
      if (mod == 1)
        ret = "granizo";
      if (mod == 2)
        ret = "nieve";
      else
        ret = "tormenta";
      break;
    case 80..89:
      if (mod == 1)
        ret = "fuerte granizo";
      if (mod == 2)
        ret = "una fuerte nevada";
      else
        ret = "fuerte tormenta";
      break;
    case 90..100:
      if (mod == 1)
        ret = "granizo muy fuerte";
      if (mod == 2)
        ret = "una nevada muy densa";
      else
        ret = "tormenta muy fuerte";
      break;
    default:
      ret = "";
      break;
  }
  return ret;
}

string season_string() 
{
  return ({ "invierno", /* 0 */
      "primavera", /* 1 */
      "verano", /* 2 */
      "otoño" })[estacion]; /* 3 */
}

string daynight_string()
{
  if (query_day())
    return "día";
  else
    return "noche";
}

string weather_string(object room) 
{
  string ret;
  string zona;
  string temp, wind, rain;

  ret = capitalize(daynight_string())+" de "+season_string();
  zona = table(WEATHER_TABLE)->query_zone(file_name(room));
  
  temp = temperature_string(zona);
  wind = wind_string(zona);
  rain = rain_string(zona);

  ret += " con " + temp;
  if ( wind != "" )
    ret += ", " + wind;
  if ( rain != "" )
    ret += " y " + rain;
  ret += ".\n";
  
  return ret;
}

private string query_aux_moon_string(int value)
{
  return ({ "llena",
            "tres cuartos menguante",
            "media menguante",
            "menguante",
            "menguante", // from here 
            "nueva",
            "creciente", // to here you cannot see at night... ! 
            "creciente",
            "media creciente",
            "tres cuartos creciente" })[value];
}

string moon_string() 
{
  string ret;
 
  ret =  WHITE_MOON + " " + query_aux_moon_string(luna_uno) + ".\n";
  ret += RED_MOON + " " + query_aux_moon_string(luna_dos) + ".\n";
  ret += BLACK_MOON + " " + query_aux_moon_string(luna_tres) + ".\n";
  
  if (!ret || (ret == ""))
    ret = "No puedes ver las lunas en el cielo.\n";

  return ret;
}


// ***********************************************************************
// neverbot, debugging
void query_all_data()
{
  tell_object(this_player(), "Hora del dia: " + hora_del_dia + "\n" +
                             "Dia de la semana: " + dia_de_semana + "\n" +
                             "Dia del mes: " + dia_del_mes + "\n" +
                             "Dia del año: " + dia_del_anyo + "\n" +
                             "Mes: " + mes + "\n" +
                             "Estacion: " + estacion + "\n" +
                             "Año: " + anyo + "\n" +
                             "Luna uno: " + luna_uno + "\n" +
                             "Luna dos: " + luna_dos + "\n" +
                             "Luna tres: " + luna_tres + "\n");
}

// neverbot, debugging
mapping query_zones(){ return zones; }
