
// Lista de las ultimas localizaciones visitadas, neverbot 08/2010
// Util para seguir, rastrear, sigilar, etc, etc.

#define MAX_VISITED 20

#define VISITED_POS_FILE_NAME  0
#define VISITED_POS_DIRECTION  1
#define VISITED_POS_STEALTH    2
#define VISITED_POS_MOUNTED    3
#define VISITED_POS_MOUNT_NAME 4

// Cada elemento de la lista visited es una lista de la forma:
//    ({ file_name de la room, salida_tomada, sigilando (1/0), montado (1/0), nombre_montura })
private mixed * visited_list;

void create()
{
  visited_list = ({ }); // allocate(20);
}

mixed query_visited_list() { return visited_list; }

// Devuelve en que posicion de la lista de visitados esta una room concreta
// -1 si no se ha visitado nunca
private int member_visited_list(string room_name) 
{
  int i;

  for (i = 0; i < sizeof(visited_list); i++)
  {
    if (visited_list[i][VISITED_POS_FILE_NAME] == room_name)
      return i;
  }
  
  return -1;
}

void add_visited(string file_name, string direction, int is_stealth, 
        int is_mounted, string mount_name)
{
  // int position;

  // Eliminamos esto, puede dar lugar a "aprovecharse" por parte de los jugadores
  // al volver sobre su propio camino y borrar parte del historico de movimientos
  
  // Si ya hemos estado en esta room, limpiamos el resto de la lista
  /*
  if ((position = member_visited_list(file_name) ) != -1)
  {
    visited_list = slice_array(visited_list, position + 1, MAX_VISITED);
  }
  else 
  */

  // Eliminamos un elemento de la lista si tenemos demasiados almacenados
  if (sizeof(visited_list) >= MAX_VISITED)
  {
    visited_list = shift_right(visited_list);
  }

  visited_list = ({ ({ file_name, direction, is_stealth, is_mounted, mount_name }) }) + visited_list;
}

string query_visited_direction(string room_name)
{
  int i;

  for (i = 0; i < sizeof(visited_list); i++)
  {
    if (visited_list[i][VISITED_POS_FILE_NAME] == room_name)
      return visited_list[i][VISITED_POS_DIRECTION];
  }
  
  return "";
}

mixed * stats() 
{
  return ({ 
    ({"Visited List (nosave)", visited_list, }),
       });
}