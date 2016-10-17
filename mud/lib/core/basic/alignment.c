/*
 *  Nuevo sistema de alineamiento para Ciudad Capital, Folken 27/Abril/03
 *
 *  Para eliminar los funcionamientos por grupos (como entrar a una ciudad o
 *  como saber si eres bueno o malo simplemente por raza, grupo o gremio), vamos
 *  a tener un doble sistema de alineamiento: alineamiento real y alineamiento
 *  externo:
 *   - real: El verdadero alineamiento del personaje, como es y como actua
 *           en el mundo. Cambiara segun ataquemos o matemos a otras personas (hacia
 *           malo) o cuando ayudemos a otras (adorando a un dios del bien... 
 *           quien sabe).
 *   - externo: Como ven al personaje las demas personas. Independientemente de que
 *           sea bueno o malo, la gente siempre vera a un drow o goblin como malo. Esto
 *           se puede variar  teniendo una raza u otra, un gremio u otro, o llevando
 *           puestos determinado tipo de ropajes (un humano quiza pareciera bueno, pero
 *           llevando una armadura del caos siempre le veran como malo). Tambien cambiara
 *           con el tiempo si el personaje comienza a ser conocido (un asesino famoso
 *           acabara siendo reconocido siempre como malo, auque sea de una raza buena).
 *           Lo mediremos mediante dos variables:
 *             * externo_fijo: raza, ser o no famoso, gremio, etc.
 *             * externo_temporal: ropas que lleves, armas empuñadas, hechizos, etc.
 */

#include <basic/alignment.h>

// Definidas como private, no quiero que nadie ande tocando el
//  alineamiento de los personajes.
private int real_align;
private int ext_align;
private int tmp_ext_align;


// -----------------------------------
// Funciones para el alineamiento real
// -----------------------------------
void set_real_align(int i)
{
  if (i > MAX_ALIGN_INT) 
    i = MAX_ALIGN_INT;
  if (i < -MAX_ALIGN_INT) 
    i = -MAX_ALIGN_INT;
  real_align = i;
}
int adjust_real_align(int i)
{
  // Llamando a la funcion set no nos olvidamos de la comprobacion
  //  de maximo y minimo
  set_real_align(real_align + i);
  return (real_align);
}
int query_real_align()
{
  return real_align;
}

// --------------------------------------
// Funciones para el alineamiento externo
// --------------------------------------
void set_ext_align(int i)
{
  if (i > MAX_ALIGN_INT) 
    i = MAX_ALIGN_INT;
  if (i < -MAX_ALIGN_INT) 
    i = -MAX_ALIGN_INT;
  ext_align = i;
}
int adjust_ext_align(int i)
{
  // Llamando a la funcion set no nos olvidamos de la comprobacion
  //  de maximo y minimo
  set_ext_align(ext_align + i);
  return (ext_align);
}
// Con el alineamiento externo devolvemos tb el temporal!!!
int query_ext_align()
{
  return ext_align + tmp_ext_align;
}

// -----------------------------------------------
// Funciones para el alineamiento externo temporal
// -----------------------------------------------
int adjust_tmp_ext_align(int i)
{
  if ((i + tmp_ext_align) > MAX_ALIGN_INT) 
  {
    tmp_ext_align = MAX_ALIGN_INT;
    return (tmp_ext_align);
  }

  if ((i + tmp_ext_align) < -MAX_ALIGN_INT) 
  {
    tmp_ext_align = -MAX_ALIGN_INT;
    return (tmp_ext_align);
  }

  tmp_ext_align += i;
  return (tmp_ext_align);
}

int query_tmp_ext_align() { return tmp_ext_align; }

// Añadida funcion create :)
void create()
{
  real_align = 0;
  ext_align = 0;
  tmp_ext_align = 0;
}

// Stats añadidos, Folken 4/03
mixed stats()
{
  return ({ 
    ({ "Real Align", real_align }),
    ({ "Ext Align", ext_align }),
    ({ "Tmp Ext Align (nosave)", tmp_ext_align }),
    });
}

// Funciones de la antigua /table/alignment_table.c
// Esta funcion cambiada por Folken, 6/03
private string query_aux_align_string(int i)
{
  // El MAX es 10k
  switch(i) 
  {
    case -MAX_ALIGN_INT..-5001: return ("demoníaco"); break;
    case -5000..-2001:          return ("diabólico"); break;
    case -2000..-1001:          return ("maléfico");  break;
    case -1000..-501:           return ("malvado");   break;
    case -500..-1:              return ("malo");      break;
    case 0..100:                return ("neutral");   break;
    case 101..500:              return ("bondadoso"); break;
    case 501..1000:             return ("bueno");     break;
    case 1001..2000:            return ("muy bueno"); break;
    case 2001..5000:            return ("extremadamente bueno"); break;
    case 5001..MAX_ALIGN_INT:   return ("angélico");  break;
    
    default:                    return ("neutral");   break;            
  }
  return ("?????????");
}

string query_align_string()
{
  string ret;

  ret = "Alineamiento externo (pareces):\t";

  ret += capitalize(query_aux_align_string(query_ext_align()));
  ret += "\nAlineamiento interno (eres):\t";
  ret += capitalize(query_aux_align_string(query_real_align()));

  return ret + "\n";
}
