/* 
 * Sistema generico para añadir guardianes a una salida, de modo
 * que impidan a npcs y jugadores salir por esa salida a menos que
 * cumplan una condicion (o el npc se mueva o desaparezca).
 * 
 *   Folken 15/04/2009
 */

// files[i] = ({ file_name, cuantos, })
static mixed * guard_files;
// Lista de objetos ya clonados
static mixed * guards;

void create()
{
  guard_files = ({ });
  guards = ({ });
}

mixed * query_guards() { return guards; }
mixed * query_files() { return guard_files; }

int add_guard(string file_name, int howmany, string direction)
{
  int i;
  object * list;

  list = ({ });
  
  if (!howmany) 
    return 0;

  if ( !stringp(file_name) || !load_object(file_name) )
  {
    log_file("room.log", "(add_guard) bad guard file: " + file_name
        + ", " + file_name(this_object())
        + " [" + ctime(time()) + "]\n");
    return 0;
  }

  /*
  // Si incluimos esta comprobacion, obligamos a hacer los add_exit antes que los add_guard
  if (!this_object()->query_exit(direction))
        return 0;
    */
  
  if (!guard_files)
    guard_files = ({ });
  if (!guards)
    guards = ({ });

  // Cada elemento de guard_files es una tuple ({ archivo, cuantos, direccion })
  guard_files += ({ ({ file_name, howmany, direction, }) });
  
  // Dejamos espacio para los nuevos guardias
  for (i = 0; i < howmany; i++)
    list += ({ 0, direction, });

  // guards[i] es la lista de objetos que se corresponden con el filename guard_files[i][0]
  guards += ({ list });
  
  return howmany;
}

void reset()
{
  int i, j;
    
  // Recorremos todos los archivos de guardias
  for (i = 0; i < sizeof(guard_files); i++)
  {
    // Cada uno de los guardias (pueden ser varios del mismo tipo)
    for (j = 0; j < guard_files[i][1]; j++)
    {
      if (!guards[i][j*2])
      {
        guards[i][j*2] = clone_object(guard_files[i][0]);
        guards[i][j*2]->move(this_object());
      }
    }
  }
}

void dest_me()
{
  int i, j;

  for (i = 0; i < sizeof(guards); i++)
  {
    for (j = 0; j < sizeof(guards[i]); j+=2)
    {
      // Si quedan guardianes se destruyen
      if (guards[i][j] && guards[i][j]->query_npc()) // && 
        // (environment(guards[i]) == this_object()))
        guards[i][j]->dest_me();
    }
  }
  
  guards = ({ });
  guard_files = ({ });
}

mixed * stats() 
{
  return ({
        ({ "Guards", guards, }),
        ({ "Guard files", guard_files, }),
          }); 
} 
