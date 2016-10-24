/*
 *  Copyright (C) 1994 Haijo Schipper (abigail@xs4all.nl)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

# include "version.h"
# include <limits.h>
# include "config.h"
# include <type.h>
# include "macros.h"
# ifdef   __TIME_CONVERSION__
# include "time.h"
# include "time.c"
# endif


# include "sym_names.h"

private mapping mkmapping(mixed *indices, mixed *values)
{
  mapping map;
  int i, sz;

  if (sizeof(indices) != sizeof(values)) {
      error("Unequal argument sizes in mkmapping()");
  }
  map = ([ ]);
  for (i = 0, sz = sizeof(indices); i < sz; i++) {
      map[indices[i]] = values[i];
  }

  return map;
}

// private mapping mkmapping (mixed * arg) {
//   mapping result;
//   int     i;
//   result = ([ ]);
//   for (i = sizeof (arg); i --;) {result [arg [i]] = 1;}
//   return (result);
// }


private string convert_to_base (int i, int base);

private int isupper (int c) {
  return (SYM_A <= c && c <= SYM_Z ||
          SYM_Agrave <= c && SYM_THORN <= c && c != SYM_multiply);
}

private int islower (int c) {
  return (SYM_a <= c && c <= SYM_z ||
          SYM_agrave <= c && SYM_thorn <= c && c != SYM_division);
}

/* Note, no check if indeed upper/lower */
private int tolower (int c) {return (c + SYM_a - SYM_A);}
private int toupper (int c) {return (c - SYM_a + SYM_A);}

private string _upper_case (string str) {
  int i;
  for (i = strlen (str); i --;) {
    if (islower (str [i])) {str [i] = toupper (str [i]);}
  }
  return (str);
}

private string _lower_case (string str) {
  int i;
  for (i = strlen (str); i --;) {
    if (isupper (str [i])) {str [i] = tolower (str [i]);}
  }
  return (str);
}

private string _capitalize (string str) {
  if (strlen (str)) {
    if (islower (str [0])) {str [0] = toupper (str [0]);}
  }
  return (str);
}

private string flip_case (string str) {
  int i;
  for (i = strlen (str); i --; ) {
    if (isupper (str [i])) {str [i] = tolower (str [i]);}
    else {if (islower (str [0])) {str [0] = toupper (str [0]);}}
  }
  return (str);
}

private string reverse (string this) {
  int i, sz;
  string result;
  for (i = 0, sz = strlen (result = this) - 1; i <= sz; i ++) {
    result [i] = this [sz - i];
  }
  return (result);
}

private string crypt (string message, int flag) {
  int    sz, pointer;
  string result;
  sz = strlen (message);
  pointer = 0;
  result = "";
  if (flag) {
    for (; pointer + 10 < sz; pointer += 10) {
      result += ::crypt (message [pointer + 2 .. pointer + 9],
                         message [pointer .. pointer + 1]);
    }
    if (pointer + 2 < sz) {
      result += ::crypt (message [pointer + 2 .. sz - 1],
                         message [pointer .. pointer + 1]);
    }
  }
  else {
    for (; pointer + 8 < sz; pointer += 8) {
      // neverbot 03/14, did not compile
      // result += ::crypt (message [pointer .. pointer + 7], 0);
      result += ::crypt (message [pointer .. pointer + 7], "");
    }
    if (pointer < sz - 1) {
      // neverbot 03/14, did not compile
      // result += ::crypt (message [pointer .. sz - 1], 0);
      result += ::crypt (message [pointer .. sz - 1], "");
    }
  }
  return (result);
}

private string make_hex (string message, string padding) {
  string * result;
  int      i;
  for (result = allocate (i = strlen (message)); i --;
       result [i] = convert_to_base (message [i], 16));
  return (implode (result, padding ? padding : ""));
}

private string rot_13 (string message) {
  int i;
  for (i = strlen (message); i --; ) {
    switch (message [i]) {
      case SYM_a .. SYM_m:
      case SYM_A .. SYM_M:
        message [i] += 13;
      case SYM_n .. SYM_z:
      case SYM_N .. SYM_Z:
        message [i] -= 13;
    }
  }
  return (message);
}

// End of old extra.c


private string query_version () {return (VERSION);}

// private int charp (mixed arg) {
//   return (intp (arg) && CHAR_MIN <= arg && arg <= CHAR_MAX);
// }

private string anything (mixed this) {
  switch (typeof (this)) {
    case T_INT:
    case T_FLOAT:
      return (this + "");
    Case T_STRING:
      return ("\"" + this + "\"");
    Case T_OBJECT:
      return ("OBJ <" + object_name (this) + ">");
    Case T_ARRAY: {
      int      i, sz;
      string * res;
      for (i = 0, res = allocate (sz = sizeof (this)); i < sz; i ++) {
        res [i] = anything (this [i]);
      }
      return ( "({ " + implode (res, ", ") + " })");
    }
    Case T_MAPPING: {
      int i, sz;
      mixed * idx, * vals;
      string * res;
      for (i = 0, res = allocate (sz = sizeof (idx = map_indices (this))),
           vals = map_values (this); i < sz; i ++) {
        res [i] = anything (idx [i]) + " : " + anything (vals [i]);
      }
      return ( "([ " + implode (res, ", ") + " ])");
    }
  }
}

private string give_padding (int n, string pad) 
{
  string padding;

  if (n <= 0) 
    return ("");

  padding = pad;
  // for (; strlen(ANSI_D->strip_colors (padding)) < n; padding += padding);
  for (; strlen (padding) < n; padding += pad);

  return (padding [.. n - 1]);
}

private string multi_line(string this, int width)
{
  string * pieces, * lines;
  int i, j, line_length, word_length;
  string new_this;

  lines = full_explode(this, "\n");
  new_this = "";

  for (j = 0; j < sizeof(lines); j++)
  {
    pieces = explode(lines[j], " ");
    line_length = 0;
    word_length = 0;

    for (i = 0; i < sizeof(pieces); i++)
    {
      word_length = strlen(pieces[i]);

      if ((i != 0) && 
          (line_length + word_length > width))
      {
        new_this += "\n";
        line_length = 0;
      }

      new_this += pieces[i];
      line_length += word_length;

      if (i < sizeof(pieces) - 1)
      {
        new_this += " ";
        line_length += 1;
      }
    }

    if (j < sizeof(lines) - 1)
      new_this += "\n";
  }

  return new_this;
}

/* Apply flags, width & precision to string. */
private string align (string this, int width, int precision, mapping options,
                      string padding) 
{
  int sz;
  if (options ["`"]) {this = reverse (this);}
  if (options ["~"]) {this = flip_case (this);}
  if (options ["<"]) {this = _lower_case (this);}
  if (options [">"]) {this = _upper_case (this);}
  if (options ["="]) {this = _capitalize (this);}
  if (options ["&"]) {this = rot_13 (this);}

  // sz = strlen(ANSI_D->strip_colors (this));
  sz = strlen (this);

  if (options ["-"]) 
  {
    if (strlen(this) < width)
    {
      if (!strlen(this))
        this += give_padding (width - sz, padding);
      else if (this[strlen(this)-1] != '\n')
        this += give_padding (width - sz, padding);
    }
    else
      this = multi_line(this, width);
  }
  else 
  {
    if (options ["|"]) 
    {
      this = give_padding ((width + 1 - sz) / 2, padding) + this +
             give_padding ((width - sz) / 2, padding);
    }
    else 
    {
      this = give_padding (width - sz, padding) + this;
    }
  }

  // return ((precision <= 0) || strlen(ANSI_D->strip_colors (this)) < precision
  //             ? this
  //             : options ["_"]
  //                  ? this [strlen(ANSI_D->strip_colors (this)) - precision ..]
  //                  : this [.. precision - 1]);
  return ((precision <= 0) || strlen (this) < precision
              ? this
              : options ["_"]
                   ? this [strlen (this) - precision ..]
                   : this [.. precision - 1]);
}


private string name_in_base (int i, int base) {
  /* Assume i < base */
  string tmp;
  tmp = "?";
  switch (i) {
    case  0 ..  9 : tmp [0] = ('0' + i); return (tmp);
    case 10 .. 35 : tmp [0] = ('a' + i - 10); return (tmp);
    case 36 .. 51 : tmp [0] = ('A' + i - 36); return (tmp);
    default : return (tmp);
  }
}

private string convert_to_base (int i, int base) {
  return (i < base ? name_in_base (i, base)
                   : convert_to_base (i / base, base) +
                     name_in_base (i % base, base));
}



private string numerical (int n, int base, int width, int precision, 
                          mapping options, string padding) {
  string digits, sign, header, this;
  int    sz;
  if (base != BASE) {digits = convert_to_base ((n < 0 ? -n : n), base);}
  else {digits = n + "";}
  sign = (n < 0 ? "-" : options ["+"] ? "+" : options [" "] ? " " : "");
  header = (options ["#"] ? (base == 8 ? "0" : base == 16 ? "0x" : "") : "");
# ifdef __TIME_CONVERSION__
  if (precision <= 0) {precision = options ["T"];}
# endif
  if (precision > 0) {
    digits = give_padding (precision - strlen (digits), "0") + digits;
  }
  if (options ["0"] && !options ["-"]) {
    digits = give_padding (width - strlen (digits + sign + header), "0") +
             digits;
  }
  sz = strlen (this = sign + header + digits);
  if (options ["X"]) {this = _upper_case (this);}
  if (options ["-"]) {this += give_padding (width - sz, padding);}
  else {this = give_padding (width - sz, padding) + this;}
  return (this);
}


private string round_off (string arg) {
  // int i;
  // i = strlen(ANSI_D->strip_colors (arg)) - 1;
  // if (arg [i] != '9') {arg [i] += 1; return (arg);}
  // if (i == 0) {return ("10");}
  // return (round_off (arg [.. i - 1]) + "0");
  int i;
  if (arg [i = strlen (arg) - 1] != '9') {arg [i] += 1; return (arg);}
  return (i == 0 ? "10" : round_off (arg [.. i - 1]) + "0");
}


private string do_float (float x, int width, int precision,
                         mapping options, string padding, int exponent) {
  float * parts;
  float   i_x, f_x;
  int     e_x;
  string  i_res, f_res, e_res, result;
  string  sign;
  int     i, sz;

  if (x < 0.0) {sign = "-"; x = -x;}
  else {if (options ["+"]) {sign = "+";}}
  if (width && sign) {width --;}

  precision || (precision = FLT_PRECISION);

  /* Find exponent. */
  if (exponent) {
    float y;
    y = x;
    while (x > 10.0) {e_x ++; x /= 10.0;}
    while (x < 1.0)  {e_x --; x *= 10.0;}
    if (exponent & 4) {
      if (e_x > -4 && e_x < (precision == -2 ? 0 : precision)) {
        /* Use %f. */
        x = y;
        exponent &= 6;  /* Don't print exponent. */
      }
    }
  }

  parts = modf (x);
  f_x = parts [0];
  i_x = parts [1];

  /* Build string for integer part. */
  for (i_res = ""; i_x >= 1.0; i_x = i_x / 10.0) {
    i_res = (string) (int) floor (fmod (i_x, 10.0)) + i_res;
  }
  if (!strlen (i_res)) {i_res = "0";}
  if ((exponent & 4) && precision != -2) {
    precision -= strlen (i_res);
  } /* Now precision should be the number of digits after the period. */

  /* Build string for fractional part. */
  if (f_x == 0.0) {f_res = "0";}
  else {
    for (f_res = ""; f_x < 0.1; f_res += "0", f_x *= 10.0);
    f_res += ((string) f_x) [2 ..];
  }

  if (precision == -2) {f_res = (options ["#"] ? "." : "");}
  else {
    if (strlen (f_res) == precision) {
      f_res = "." + f_res;
    }
    else {
      if (strlen (f_res) > precision) {
        if (f_res [precision] > '4') { /* Round off away from zero. */
          f_res = round_off (f_res [.. precision - 1]);
          if (precision != strlen (f_res)) {  /* Overflow to integer part. */
            i_res = round_off (i_res);
            if ((exponent & 1) && i_res == "10") {
              /* Overflow to exponent */
              e_x ++;
              i_res = "1";
            }
            f_res = give_padding (precision, "0"); /* Fill with 0's. */
          }
          f_res = "." + f_res;
        }
        else {f_res = "." + f_res [.. precision - 1];}
      }
      else {
        f_res = "." + f_res + give_padding (precision - strlen (f_res), "0");
      }
    }
  }
  if ((exponent & 4) && strlen (f_res)) { /* Strip trailing 0's and period. */
    for (i = strlen (f_res); f_res [-- i] == '0';);
    f_res = (i ? f_res [.. i] : options ["#"] ? "." : "");
  }
  

  /* Build string for exponent part. */
  if (exponent & 1) {
    e_res = ((exponent & 2) ? "E" : "e");
    if (e_x < 0) {e_res += "-"; e_x = - e_x;}
    else {e_res += "+";}
    e_res += (e_x < 9 ? e_x == 0 ? "00" : "0" + e_x : (string) e_x);
  }
  else {e_res = "";}

  /* Add padding and sign. */
  if ((sz = strlen (result = i_res + f_res + e_res)) < width) {
    string pads;
    if (options ["0"]) {padding = "0";}
    pads = give_padding (width - sz, padding);
    if (padding == " ") {result = pads + (sign ? sign : "") + result;}
    else {result = (sign ? sign : "") + pads + result;}
  }
  else {if (sign) {result = sign + result;}}

  return (result);
}



/* Scanning of the format strings. Result is an array of strings, */
/* either a conversion sequence, or a string of 'normal' chars.   */
private string * make_chunks (string format) {
  string * result;
  string   current;
  int      i, j, sz;
  string   tmp;
  string   option;
  result = ({ });
  i = j = 0;
  // sz = strlen(ANSI_D->strip_colors (format));
  sz = strlen(format);
  while (i < sz) {
    if (CONV_CHAR [format [i]]) { /* Encountered a '%' or '@' */
      option = format [i .. i];
      i ++;

      /* Scan flags. */
      while (i < sz && FLAGS [format [i]]) {i ++;}
      ENDCHECK (i, sz);
      tmp = format [j + 1 .. i - 1] + ",";  /* Flags */
      j = i;

      /* Scan width */
      if (format [i] == '*') {
        i ++;
        tmp += "-1" + ",";
      }
      else {
        while (i < sz && DIGIT [format [i]]) {i ++;}
        tmp += (format [j .. i - 1] == "" ? "0" : format [j .. i - 1]) + ",";
      }
      ENDCHECK (i, sz);

      /* Scan precision */
      if (format [i] == '.') {
        i ++;
        ENDCHECK (i, sz);
        j = i;
        if (format [i] == '*') {
          i ++;
          tmp += "-1" + ",";
        }
        else {
          while (i < sz && DIGIT [format [i]]) {i ++;}
          tmp += (format [j .. i - 1] == "" ? "0" : format [j .. i - 1]) + ",";
        }
        ENDCHECK (i, sz);
      }

      /* Chunck */
      result += ({ option + format [i .. i] + tmp });
      j = ++ i;
    }
    else {
      do { i ++; } while (i < sz && !CONV_CHAR [format [i]]);
      result += ({ format [j .. i - 1] });
      j = i;
    }
  }
  return (result);
}

# ifdef __CLOSE_TO_C__
static int sprintf (string out, string format, mixed args...) {
# else
static string sprintf (string format, mixed args...) {
# endif
  mixed  * arguments;
  string * chunks;
  string   result;
  string   flags, padding;
  string   tmp;
  string   cur;
  int      i, j, width, precision, sz, sz_args;
  int      exp;
  mapping  options;
  object   query_object;

# ifdef __CLOSE_TO_C__
  TYPECHECK (array, out, 0);
  if (!sizeof (out)) {error ("Sprintf: empty first argument");}
# endif

  TYPECHECK (string, format, 1);

  chunks = make_chunks (format);

  padding = " ";
  for (i = 0, sz = sizeof (chunks), sz_args = sizeof (args), result = "";
       i < sz; i ++) {
    if (!CONV_CHAR [chunks [i] [0]]) {result += chunks [i];}
    else {
      if (chunks [i] [0 .. 1] == "%%" || chunks [i] [0 .. 1] == "@@") {
        result += chunks [i] [1 .. 1];
      }
      else {
        width = precision = 0;
        ARGCOUNTCHECK (j, sz_args);
        switch (sscanf (chunks [i] [2 ..], "%s,%d,%d",
                        flags, width, precision)) {
          case 3: if (!precision) {precision = -2;}
# if 0
NOTE      case 2: if (!width) {width = -2;} /* Reserved for future use? */
# endif
        }
        if (width == -1) {
          TYPECHECK (int, args [j], j + 2);
          width = args [j ++];
          ARGCOUNTCHECK (j, sz_args);
        }
        if (precision == -1) {
          TYPECHECK (int, args [j], j + 2);
          precision = args [j ++];
          ARGCOUNTCHECK (j, sz_args);
        }

        options = mkmapping (explode (flags, ""), explode (flags, ""));
        switch (cur = chunks [i] [.. 1]) {
          case "%A":
          case "%a": {
            int k, sk;
            TYPECHECK (array, args [j], j + 2);
            for (k = 0, sk = sizeof (args [j]); k < sk; k ++) {
              if (cur == "%a" && nump (args [j] [k])) {args [j] [k] += "";}
              TYPECHECK (string, args [j] [k], j + 2);
              result += align (args [j] [k], width, precision,
                               options, padding);
            }
          }
          Case "%b": /* Binary      */
          case "%d": /* Decimal     */
          case "%i":
          case "%o": /* Octal       */
          case "%x": /* Hexadecimal */
          case "%X":
            TYPECHECK (int, args [j], j + 2);
            result += numerical (args [j],
                                 cur == "%b" ?  2 :
                                 cur == "%o" ?  8 :
                                 cur == "%x" || cur == "%X" ? 16 : BASE,
                                 width, precision,
                                 cur == "%X" ? (options + ([ "X" : 1 ]))
                                             : options, padding);
          Case "%c":
            TYPECHECK (char, args [j], j + 2);
            tmp = " ";
            tmp [0] = args [j];
            result += align (tmp, width, precision, options, padding);
          Case "%H":
            TYPECHECK (string, args [j], j + 2);
            result += align (make_hex (args [j], padding), width, precision,
                             options, padding);
          Case "%h":
            TYPECHECK (string, args [j], j + 2);
            result += align (make_hex (args [j], nil), width, precision,
                             options, padding);
          Case "%n":
            TYPECHECK (array, args [j], j + 2);
            if (!sizeof (args [j])) {error ("No space for %n conversion");}
            // args [j] [0] = strlen(ANSI_D->strip_colors (result));
            args [j] [0] = strlen(result);
          Case "%O":
            TYPECHECK (object, args [j], j + 2);
            result += align (object_name (args [j]), width, precision,
                             options, padding);
          Case "%p":
            TYPECHECK (char, args [j], j + 2);
            padding = "X";
            padding [0] = args [j];
          Case "%Q": 
            TYPECHECK (object, args [j], j + 2);
            query_object = args [j];
          Case "%q": {
            mixed res;
            TYPECHECK (string, args [j], j + 2);
            if (!query_object) {error ("%q used before %Q in sprintf.");}
            res = (mixed) call_other (query_object, args [j]);
            if (intp (res)) {res += "";}
            if (!stringp (res)) {
              res = anything (res);
            }
            result += align (res, width, precision, options, padding);
          }
          Case "%R":
            TYPECHECK (string, args [j], j + 2);
            result += align (crypt (args [j], 1), width, precision,
                             options, padding);
          Case "%r":
            TYPECHECK (string, args [j], j + 2);
            result += align (crypt (args [j], 0), width, precision,
                             options, padding);
          Case "%s":
            if (nump (args [j])) {args [j] += "";}
NOTE      case "%S":
            TYPECHECK (string, args [j], j + 2);
            result += align (args [j], width, precision, options, padding);
          Case "%y":
            result += align (anything (args [j]), width, precision,
                             options, padding);
# ifdef __FLOATS__       
          /* Bit 0 for exp, bit 1 for uppercase, bit 2 for conditional exp */
          Case "%G": 
          case "%g": exp |= 4;
          case "%E": if (cur != "%g") {exp |= 2;}
          case "%e": exp |= 1;
          case "%f": 
            if (intp (args [j])) {args [j] = (float) args [j];}
            TYPECHECK (float, args [j], j + 2);
            result += do_float (args [j], width, precision, options, padding,
                                exp);
            exp = 0;  /* Don't leave this out! */
# endif              
# ifdef __TIME_CONVERSION__
          /* Time related conversions. */
          Case "@a":
            TYPECHECK (int, args [j], j + 2);
            result += align (weekday (args [j], 0), width, precision,
                             options, padding);
          Case "@A":
            TYPECHECK (int, args [j], j + 2);
            result += align (weekday (args [j], 1), width, precision,
                             options, padding);
          Case "@b":
            TYPECHECK (int, args [j], j + 2);
            result += align (month (args [j], 0), width, precision,
                             options, padding);
          Case "@B":
            TYPECHECK (int, args [j], j + 2);
            result += align (month (args [j], 1), width, precision,
                             options, padding);
          Case "@c":
            TYPECHECK (int, args [j], j + 2);
            result += align (ctime (args [j]), width, precision, options,
                             padding);
          Case "@d":
            TYPECHECK (int, args [j], j + 2);
            result += numerical (day (args [j]), BASE, width, precision,
                                 options + ([ "T" : 2 ]), padding);
          Case "@H":
            TYPECHECK (int, args [j], j + 2);
            result += numerical (hour (args [j]), BASE, width, precision,
                                 options + ([ "T" : 2 ]), padding);
          Case "@I": {
            int h;
            TYPECHECK (int, args [j], j + 2);
            result += numerical ((h = hour (args [j])) ? h % NOON : NOON, BASE,
                                  width, precision, options + ([ "T" : 2 ]),
                                  padding);
          }
          Case "@j":
            TYPECHECK (int, args [j], j + 2);
            result += numerical (day_of_year (args [j]), BASE, width, precision,
                                 options + ([ "T" : 3 ]), padding);
          Case "@m":
            TYPECHECK (int, args [j], j + 2);
            result += numerical (month (args [j], 2) + 1, BASE, width,
                                 precision, options + ([ "T" : 2 ]), padding);
          Case "@M":
            TYPECHECK (int, args [j], j + 2);
            result += numerical (minute (args [j]), BASE, width, precision,
                                 options + ([ "T" : 2 ]), padding);
          Case "@p":
            TYPECHECK (int, args [j], j + 2);
            result += align (hour (args [j]) < NOON ? AM : PM,
                             width, precision, options, padding);
          Case "@S":
            TYPECHECK (int, args [j], j + 2);
            result += numerical (second (args [j]), BASE, width, precision,
                                 options + ([ "T" : 2 ]), padding);
          Case "@U":
            TYPECHECK (int, args [j], j + 2);
            result += numerical (week_number (args [j], 0), BASE, width,
                                 precision, options + ([ "T" : 2 ]), padding);
          Case "@w":
            TYPECHECK (int, args [j], j + 2);
            result += numerical (weekday (args [j], 2), BASE, width, precision,
                                 options + ([ "T" : 1 ]), padding);
          Case "@W":
            TYPECHECK (int, args [j], j + 2);
            result += numerical (week_number (args [j], 1), BASE, width,
                                 precision, options + ([ "T" : 2 ]), padding);
          Case "@x":
            TYPECHECK (int, args [j], j + 2);
            result += align (date (args [j]), width, precision,
                             options, padding);
          Case "@X":
            TYPECHECK (int, args [j], j + 2);
            result += align (ttime (args [j]), width, precision,
                             options, padding);
          Case "@y":
            TYPECHECK (int, args [j], j + 2);
            result += numerical (year (args [j]) % 100, BASE, width, precision,
                                 options + ([ "T" : 2 ]), padding);
          Case "@Y":
            TYPECHECK (int, args [j], j + 2);
            result += numerical (year (args [j]), BASE, width, precision,
                                 options, padding);
# ifdef __TIME_ZONE__
          Case "@Z":
            TYPECHECK (int, args [j], j + 2);
            result += align (timezone (args [j]), width, precision,
                             options, padding);
# endif
# endif       
          Default :
            error ("Unknown conversation character " + cur);
        }
        j ++;
      }
    }
  } 
# ifdef __CLOSE_TO_C__
  out [0] = result;
  return (strlen(ANSI_D->strip_colors (result)));
# else
  return (result);
# endif
}