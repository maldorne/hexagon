
#include "/lib/core/efuns/strings/full_explode.c"
#include "/lib/core/efuns/strings/visible_strlen.c"
#include "/lib/core/efuns/strings/sprintf/sprintf.c"

// prototypes

// defined in /lib/core/efuns/conversions.c
static string chr(int c);


static void printf(string format, mixed args...)
{
  write(sprintf(format, args));
}

// To simulate the member_array(character, string)
static int at(int character, string str)
{
  int i;

  if (!character || !str)
   return -1;

  for (i = 0; i < strlen(str); i++)
    if (character == str[i])
      return i;

  return -1;
}

static string replace_string(string str, string foo, string bar)
{
  return implode(full_explode(str, foo), bar);

  // string a, b, result;
  // int i;

  // a = "";
  // b = "";

  // i = sscanf(str, "%s" + foo + "%s", a, b);
  // result = a + bar + b;

  // if (i <= 0)
  //   return str;

  // while (i > 0)
  // {
  //   i = sscanf(result, "%s"+foo+"%s", a, b);
  //   result = a + bar + b;
  // }

  // return result;
}

static string replace(string str, mixed bing, varargs string rep)
{
  int i;

  if (pointerp(bing))
  {
    for (i = 0; i < sizeof(bing); i+=2)
      str = replace_string(str, bing[i], bing[i+1]);
    return str;
  }
  return replace_string(str, bing, rep);
}

static string extract(string str, int start, varargs int end)
{
  if (end)
    return str[start..end];
  else
    return str[start..];
}

static string trim(string str)
{
  if (strlen(str) == 0)
    return str;

  if ((str[strlen(str)-1] != 32 && str[strlen(str)-1] != 9) &&
    (str[0] != 32 && str[0] != 9))
    return str;

  while (str[strlen(str)-1] == 32 || str[strlen(str)-1] == 9)
  {
    if (strlen(str) == 1)
      return str;
    str = str[0..strlen(str)-2];
  }

  while (str[0] == 32 || str[0] == 9)
  {
    if (strlen(str) == 1)
      return str;
    str = str[1..strlen(str)-1];
  }

  return str;
}

static int strcmp(string a, string b)
{
  int i, aa, bb, len_a, len_b;

  len_a = strlen(a);
  len_b = strlen(b);
  i = 0;

  while ((i < len_a) && (i < len_b))
  {
    aa = a[i];
    bb = b[i];

    if (aa > bb)
      return -1;
    else if (aa < bb)
      return 1;

    i++;
  }

  // one is substring of the other
  if (len_a < len_b)
    return -1;
  else if (len_a > len_b)
    return 1;

  // same strings
  return 0;
}

// strsrch - search for substrings in a string
// int strsrch( string str, string substr | int char, int flag );

// strsrch() searches for the first occurance of the string 'substr' in the
// string 'str'.  The last occurance of 'substr' can be found by passing '-1'
// as the 3rd argument (which is optional).  If the second argument is an
// integer, that character is found (like C's strchr()/strrchr().)  The empty
// string or null value cannot be searched for.

// The integer offset of the first (last) match is returned.  -1 is returned
// if there was no match, or an error occurred (bad args, etc).

static int strsrch( string str, mixed substr, varargs int flag )
{
  string tmp1, tmp2;

  // if the substring is a char
  if (intp(substr))
    substr = chr(substr);
  else if (!stringp(substr))
    return -1;

  if (flag >= 0)
  {
    int substr_len, i;

    substr_len = strlen(substr);
    i = -substr_len;

    do
    {
      if (sscanf(str, "%s" + substr + "%s", tmp1, str) >= 1)
      {
        i += strlen(tmp1) + substr_len;
        if (!str)
          str = "";
      }
      else
        return -1;

      flag--;
    }
    while(flag > 0);

    return i;
  }
  else if (flag == -1)
  {
    string * arr;

    arr = full_explode(str, substr);

    return strlen(implode(arr[..sizeof(arr)-2], substr));
  }

  if (sscanf(str, "%s" + substr + "%s", tmp1, tmp2) >= 1)
    return strlen(tmp1) + strlen(substr);

  return -1;
}

// /secure/simul_efun/wrap.c
// from the RotD Mudlib
// wraps text automatically
// created by Pallando@Nightmare

static string wrap(string str, varargs int width)
{
  return (width ? sprintf("%-=" + width + "s", str + "\n") :
    sprintf("%-=75s", str + "\n"));
}

// /adm/simul_efun/arrange_string.c
// from the Nightmare mudlib
// makes a string a certain length
// created by Descartes of Borg 23 december 1992

static string arrange_string(string str, int x)
{
    int i, y;

    if (!str || str == "")
      return "";
    if (!x)
      return "";
    if (intp(str))
      str = str+"";

    y = strlen(str);

    if (y>x)
      return str[0..(x-1)];

    for (i=y; i<x; i++)
      str += " ";

    return str;
}

// what the hell would need this???
// static int vowel(int i)
// {
//   return (i == 'a' || i == 'e' || i == 'i' || i == 'o' || i == 'u');
// }


// spanish characters, neverbot 4/03

static string lower_case(string str)
{
  int first_char;

  if (!strlen(str))
    return str;

  first_char = str[0];

  switch(first_char)
  {
    case 'Á': str[0] = 'á'; break;
    case 'É': str[0] = 'é'; break;
    case 'Í': str[0] = 'í'; break;
    case 'Ó': str[0] = 'ó'; break;
    case 'Ú': str[0] = 'ú'; break;
    case 'Ü': str[0] = 'ü'; break;
    case 'Ñ': str[0] = 'ñ'; break;
    default:
      if ((first_char >= 65) && (first_char <= 90))
        str[0] = first_char + 32;
  }

  return str;
}

static string capitalize(string str)
{
  int first_char;

  if (!strlen(str))
    return str;

  first_char = str[0];

  switch(first_char)
  {
    case 'á': str[0] = 'Á'; break;
    case 'é': str[0] = 'É'; break;
    case 'í': str[0] = 'Í'; break;
    case 'ó': str[0] = 'Ó'; break;
    case 'ú': str[0] = 'Ú'; break;
    case 'ü': str[0] = 'Ü'; break;
    case 'ñ': str[0] = 'Ñ'; break;
    default:
      if ((first_char >= 97) && (first_char <= 122))
        str[0] = first_char - 32;
  }

  return str;
}

static string upper_case(string str)
{
  return capitalize(str);
}

// Taniwha 1995, since the efun one doesn't
static string pluralize(string str)
{
  if (!str || str == "")
    return "objects";
  else return (str + "s");
}

// needed in a spanish-language mud
// it could change only the accents or both the accents and the ñ

static string replace_spanish_characters(string str, varargs int flag_enye)
{
	int i;
	string ret;
	ret = "";

	for (i = 0; i < strlen(str); i++)
	{
		switch(str[i..i])
		{
			case "á": ret += "a"; break;
			case "é": ret += "e"; break;
			case "í": ret += "i"; break;
			case "ó": ret += "o"; break;
			case "ú": ret += "u"; break;
			case "Á": ret += "A"; break;
			case "É": ret += "E"; break;
			case "Í": ret += "I"; break;
			case "Ó": ret += "O"; break;
			case "Ú": ret += "U"; break;
			case "ñ": ret += (flag_enye?"ny":"ñ"); break;
			case "Ñ": ret += (flag_enye?"NY":"N"); break;
			default: ret += str[i..i];
		}
	}
	return ret;
}

// format_page()
// Tweaked by Radix for who command : MArch 4, 1997
static string format_page(string *items, int columns)
{
  int width, i, j, x;
  string ret;

  if (!columns)
    columns = 2;

  ret = "";

  if (!this_user())
    width = 75/columns;
  else
    width = this_user()->query_cols() / columns;

  for (i = 0, x = sizeof(items); i < x; i += columns)
  {
    for (j = 0; j < columns; j++)
    {
      if(i + j >= x)
        break;
      // ret += arrange_string(items[i+j], width);
      // changed to improve presentation, neverbot 02/05
      ret += arrange_string("  " +capitalize(items[i+j]), width);
    }
    ret += "\n";
  }
  return ret;
}

// string pad(string str, int len, int padchar)
// Left or right justify str to len chars. If padchar is not specified,
// defaults to ' '. Default is left justify. To specify right justification,
// make len negative. str can also be a number.

static nomask string pad(string str, int size, varargs string char)
{
  int i, len;
  string padding;

  if (size == 0)
    return str;

  padding = (char ? char[0..0] : " ");
  len = strlen(str);

  if (size >= 0)
  {
    if (len >= size)
      return str[0..size-1];

    for (i = 0; i < size-len; i++)
      str += padding;
  }
  else
  {
    if (len >= abs(size))
      return str[len-abs(size)..len-1];

    for (i = 0; i < abs(size)-len; i++)
      str = padding + str;
  }

  return str;
}
