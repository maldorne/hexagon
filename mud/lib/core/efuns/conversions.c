
#include "conversions/chr.c"
#include "conversions/ord.c"
#include "conversions/to_string.c"
#include "conversions/print_object.c"

private string number_as_string_aux(int n);
static string number_as_string(int n);

static string query_num(int n, int limit)
{
  if (limit && n>limit)
    return "montones de"; /* this is a little pointless ... */
  return number_as_string(n);
}

static string number_as_string(int n)
{
  string ret;
  int i;

  if (n < 0)
    return "miles de"; /* fo wraps... */
  if (n > 99999)
    return "miles de"; /* sorry 'bout the hard limit */

  ret = nil;

  if ((i = n/1000))
  {
    n = n%1000;
    if (i==1)
      ret = "mil";
    else
      ret = number_as_string_aux(i) + " mil";
    if (!n)
      return ret;
  }

  if ((i = n/100))
  {
    n = n%100;
    if (!ret)
      ret="";
    else
      ret += " ";

    switch (i)
    {
      case 1: if (!n) ret += "cien"; else ret += "ciento"; break;
      case 2: ret += "doscientos"; break;
      case 3: ret += "trescientos"; break;
      case 4: ret += "cuatrocientos"; break;
      case 5: ret += "quinientos"; break;
      case 6: ret += "seiscientos"; break;
      case 7: ret += "setecientos"; break;
      case 8: ret += "ochocientos"; break;
      case 9: ret += "novecientos"; break;
    }

    if (!n)
      return ret;
  }

  if (ret)
    return ret + " " + number_as_string_aux(n);

  return number_as_string_aux(n);
} /* query_num() */

private string number_as_string_aux(int n)
{
  string ret;

  if (!n)
    return "cero";

  if (n > 99)
    return "muchos";

  if (n<30 && n>9)
    return ({ "diez", "once", "doce", "trece",
              "catorce", "quince", "dieciséis", "diecisiete",
              "dieciocho", "diecinueve", "veinte",
              "veintiún", "veintidós", "veintitrés",
              "veinticuatro", "veinticinco", "veintiséis",
              "veintisiete", "veintiocho", "veintinueve" })[n-10];

  ret = ({ "", "", "", "treinta", "cuarenta", "cincuenta", "sesenta",
           "setenta", "ochenta", "noventa"})[n/10];

  if ((n = n%10)&&(ret!=""))
    ret += " y ";

  return ret + ({ "", "un", "dos", "tres", "cuatro", "cinco", "seis",
                  "siete", "ocho", "nueve"})[n];
} /* number_as_string() */


// flag = use pretty_short() if is a list of objects instead of just short()
static string query_multiple_short(mixed *obs, varargs int flag)
{
  int i, j, k;
  string * str;
  mixed * bity;

  bity = ({ });
  for (i = 0; i < sizeof(obs); i++)
  {
    if (!stringp(obs[i]))
    {
      if (!pointerp(obs[i]))
      // like a string. First bit is the string.
      // second the object
      {
        if (objectp(obs[i]))
        {
          if (flag)
            str = ({ (string)obs[i]->pretty_short() });
          else
            str = ({ (string)obs[i]->short() });
        }
      }
      else
      {
        str = obs[i][0];
        obs[i] = obs[i][1];
      }
    }
    else
      str = ({ obs[i] });

    if (!str) // ignore invis objects
      continue;

    if (stringp(str))
      str = ({ str });

    for (j=0;j<sizeof(str);j++)
      if ((k=member_array(str[j], bity)) == -1)
        bity += ({ str[j], ({ obs[i] }) });
      else
        bity[k+1] = ({ obs[i] })+bity[k+1];
  }

  str = ({ });

  for (i = 0; i < sizeof(bity); i+=2)
  {
    j = sizeof(bity[i+1]);
    if (stringp(bity[i+1][0]))
      if (j==1)
        str += ({ bity[i] });
      else
        str += ({ (query_num(j,20)+
          " "+pluralize(bity[i])) });
    else if (j == 1)
    // determinate property removed
    // if (bity[i+1][0]->query_property("determinate"))
    //   str += ({ bity[i+1][0]->query_property("determinate")+bity[i] });
    // else
      // str += ({ add_a(bity[i]) });
      str += ({ bity[i] });
    else
    {
      if (flag)
        str += ({ (query_num(j,20)+
          " "+bity[i+1][0]->pretty_plural(bity[i])) });
      else
        str += ({ (query_num(j,20)+
          " "+bity[i+1][0]->query_plural(bity[i])) });
    }
  }

  if (!sizeof(str))
    return "";

  if (sizeof(str) == 1)
    return str[0];

  return implode(str[0..sizeof(str)-2],", ")+" "+_LANG_AND+" "+str[sizeof(str)-1];
} /* query_multiple_short() */

