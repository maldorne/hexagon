
// old cute_look.c, now contents.c, neverbot 06/03

// #include <alignment.h>

#define HUD "/table/hud_table.c"

string number_as_place(int num);

/* returns an array of the form:
 * ({
 *   ({
 *     "foo",
 *     "bing",
 *   }),
 *   ({
 *     ({ <list of living objects with short "foo"> }),
 *     ({ <ditto for short "bing"> }),
     }),
 *   ({
 *     "s1",
 *     "s2",
 *   }),
 *   ({
 *     ({ <list of pointers to objects that have a short of "s1"> }),
 *     ({ <ditto for objects with a short of "s2"> }),
 *   }),
 *   ({ <pointers to invisible objects> }),
 * })
 * The first two arrays contain living objects, the second two contain
 * the rest, the last contains the pointers to invisible objects.
 * NB: this_player() is _not_ included in the array if (s)he is present.
 */

mixed query_inventory(object *obs) 
{
  mixed inv;
  object ob;
  mixed desc;
  int i,j,z;
  object me;

  me = real_this_player();

  inv = ({ ({ }), ({ }), ({ }), ({ }), ({ }), });
  if (!obs)
    obs = all_inventory(this_object());
  for (z = sizeof(obs)-1; z >= 0;z--) {
    ob = obs[z];
    if (!(desc = (string)ob->pretty_short()) || (desc == "")) {
      inv[4] += ({ ob });
      continue;
    }
    /* Remove comments when ready for hidden players - Radix
    if(interactive(ob) && (int)ob->query_hidden())
    {
       inv[4] += ({ ob });
       continue;
    }
    */
    if (pointerp(desc)) {
      if (living(ob)) {
        if (ob == me) 
            continue;
        for (j = 0; j < sizeof(desc); j++)
          if ((i = member_array(desc[j], inv[0])) >=0)
            inv[1][i] += ({ ob });
          else {
            inv[0] += ({ desc[j] });
            inv[1] += ({ ({ ob }), });
          }
        continue;
      }
      for (j = 0; j < sizeof(desc); j++)
        if ((i = member_array(desc[j], inv[2])) >= 0)
          inv[3][i] += ({ ob });
        else {
          inv[2] += ({ desc[j] });
          inv[3] += ({ ({ ob }) });
        }
      continue;
    }
    if (living(ob)) {
      if (ob == me) continue;
      if ((i = member_array(desc, inv[0])) >= 0) {
        inv[1][i] += ({ ob });
        continue;
      }
      inv[0] += ({ desc });
      inv[1] += ({ ({ ob }), });
      continue;
    } 
    if ((i = member_array(desc, inv[2])) >= 0) {
      inv[3][i] += ({ ob });
      continue;
    }
    inv[2] += ({ desc });
    inv[3] += ({ ({ ob }), });
  }
  return inv;
}

// Cambio mas importante de esta funcion: hacemos iguales 
//  a los npcs y a los jugadores, ambos salen en la misma linea
//  y a ambos se les aplican colores si son 'enemigos'
// Cambio: a los npcs no les sale nunca el "subtitulo" de la raza
//  ej: Ciudadano de Ciudad Capital el humano está aquí. :P
// Se sabe si son enemigos comparando 'mi' alineamiento real
//  con 'su' alineamiento externo (atencion!!! si yo soy un drow bueno (p.ej.)
//  quiza no vea a un elfo como enemigo, pero el SI me vera a mi como enemigo

string query_contents(string str, varargs object *obs) 
{
  int i, j, drunk;
  mixed inv;
  string ret, aux;
  int count, howmany;
  object me;

  ret = "";
  aux = "";
  drunk = 0;

  // en ocasiones, si la accion la ha iniciado un objeto
  // (por ejemplo al salir del mar no hay que escribir ningun
  // comando para salir, la accion la inicia la propia room)
  // el driver no sabra resolver this_player ni this_player(1)
  me = real_this_player();
  
  inv = query_inventory(obs);
  drunk = me->query_volume(0);

  if (!sizeof(inv[0]) && !sizeof(inv[2])) 
     return ret;
  
  if (str) ret += str;
  
  howmany = count = sizeof(inv[0]);

  // Recorremos la lista de objetos living de la room
  for (i = 0; i < sizeof(inv[0]); i++) 
  {
    if (inv[0][i][0] < '0' && inv[0][i][0] > '9')
      j = sizeof(inv[1][i]) * ((random(drunk)/1000)+1);
    else
      j = sizeof(inv[1][i]);

    // al_him = (int)inv[1][i][0]->query_ext_align();

    // Nuevo metodo:
    ret += (string)HUD->query_colored_name(me, inv[1][i][0], inv[0][i], j);

    // Un living menos a mostrar
    count--;
    if (count > 1)
       ret += ", ";
    if (count == 1)
       ret += " y ";
  }

  // Si hay varios tipos (howmany) o varios del mismo tipo (j)
  if ((howmany > 1) || (j > 1))
     ret += " están aquí.\n"; 	
  if ((howmany == 1) && (j <= 1))
     ret += " está aquí.\n"; 	
    	
  // Objetos  
  for (i = sizeof(inv[2]) - 1; i >= 0; i--) {
    j = sizeof(inv[3][i])*((random(drunk)/1000)+1);
    if (j > 1) {
      ret += capitalize(query_num(j, 20)) + " " +
             capitalize((string)inv[3][i][0]->pretty_plural(inv[2][i])) + ".\n";
      continue;
    }
    ret += capitalize(inv[2][i]) + ".\n";
  }  

  return ret;
}

string number_as_place(int n) {
  /*
  if (!n) return "zero";
  return n+"o";
  if (n>9 && n<20) return n+"th";
  return n+((n%10 == 1)?"st":(n%10 == 2)?"nd":(n%10 == 3)?"rd":"th");
  */
  // Parche rapido por Folken, mejor esto que en ingles
  return (""+(string)n+"º");
}

void list_contents(string str) {
  write(query_contents(str));
}
