/*
 *  new alignment system for ccmud, neverbot 27/Abril/03
 *
 *  to avoid checking social/group belongings (like not allowing
 *  entrance to a city just because of you race, guild, etc), we are going
 *  to use a double alignment: the real one (inner) and the perceived
 *  one (extern):
 *  - real: the true alignment of the character/npc/item, how they act.
 *          Will change when attacking or killing other characters
 *          (towards evil) or helping others (towards good).
 *  - extern: how the character is perceived by the others. Regardless
 *          they are good or evil, others will see a drow or goblin
 *          as an evil character. This will change if the character
 *          is really known or famous, with a costume, etc. Will be
 *          measured with two variables:
 *           * fixed extern: race, fame, guild, etc.
 *           * temporal extern: costumes, weapons wieldad, spells, etc.
 */

#include <basic/alignment.h>

private int real_align;
private int ext_align;
private int tmp_ext_align;

void create()
{
  real_align = 0;
  ext_align = 0;
  tmp_ext_align = 0;
}

// -----------------------------------
//  real alignment functions
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
  set_real_align(real_align + i);
  return (real_align);
}
int query_real_align()
{
  return real_align;
}

// --------------------------------------
//  perceived alignment functions
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
  set_ext_align(ext_align + i);
  return (ext_align);
}

int query_ext_align()
{
  return ext_align + tmp_ext_align;
}

// -----------------------------------------------
//  tmp perceived alignment functions
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

// old /table/alignment_table.c, neverbot, 6/03
private string query_aux_align_string(int i)
{
  // MAX is 10k
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

// stats added, neverbot 4/03
mixed stats()
{
  return ({
    ({ "Real Align", real_align }),
    ({ "Ext Align", ext_align }),
    ({ "Tmp Ext Align (nosave)", tmp_ext_align }),
    });
}
