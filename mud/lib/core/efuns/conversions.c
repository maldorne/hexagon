
#include "conversions/chr.c"
#include "conversions/ord.c"
#include "conversions/to_string.c"
#include "conversions/print_object.c"
#include "conversions/number_as_string.c"

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
          " "+bity[i+1][0]->pretty_plural()) });
      else
        str += ({ (query_num(j,20)+
          " "+bity[i+1][0]->query_plural()) });
    }
  }

  if (!sizeof(str))
    return "";

  if (sizeof(str) == 1)
    return str[0];

  return implode(str[0..sizeof(str)-2],", ")+" "+_LANG_AND+" "+str[sizeof(str)-1];
} /* query_multiple_short() */

