
#include "arrays/member_array.c"

static nomask int index(mixed element, mixed * arr) 
{
  return member_array(element, arr);
}

static nomask mixed * delete(mixed * arr, int start, int len) 
{
  if (start < 0)
    return arr;
  
  if (start + len >= sizeof(arr))
    return arr[..start - 1];

  return arr[..start - 1] + arr[start + len..];
} 

static nomask mixed * slice_array(mixed * arr, int start, int fin) 
{
  return arr[start..fin];
} 

static nomask mixed * insert(mixed * arr, mixed el, int pos) 
{
  if (!sizeof(arr))
    return ({ el });

  if (pos >= sizeof(arr))
    return arr + ({ el });

  if (pos <= 0)
    return ({ el }) + arr;

  return arr[..pos-1] + ({ el }) + arr[pos..];
} 

static nomask mixed * shift_left(mixed * arr)
{
	return arr[1..sizeof(arr)-1] + ({ });
}

static nomask mixed * shift_right(mixed * arr)
{
	return ({ }) + arr[0..sizeof(arr)-2];
}

static nomask mixed * exclude_array(mixed *arr, int from, varargs int to) 
{
  mixed *bottom, *top;

  bottom = ({});
  top = ({});

  if (!to) 
    to = from;
  if (from > 0) 
    bottom = arr[0..from -1];
  if (to < sizeof(arr)-1) 
    top = arr[to+1..sizeof(arr)-1];

  return bottom + top;
}

// /secure/simul_efun/uniq_array.c
// from the RotD Mudlib
// takes any array and returns an array with no repeat members
// created by Descartes of Borg 930822

static nomask mixed * uniq_array(mixed *arr) 
{
  mapping borg;
  int i;

  i = sizeof(arr);
  borg = ([]);
  
  while(i--) 
    borg[arr[i]] = 1;
  return map_indices(borg);
}

static nomask int atoi(string str) 
{
  int x;

  if (!stringp(str)) 
    return 0;
  else 
    sscanf(str, "%d", x);
  return x;
}

static nomask string itoa(int i)
{
  if (!intp(i))
    return "";
  return ""+i;
}

// by Dank Mar 11 93.  max may be a reserved word, so maxi is safer
// int maxi(int *arr) {
//   int i, j;
//   if (!sizeof(arr)) {
//     notify_fail("Array must contain one or more ints.\n");
//     return 0;
//   }
//   j = arr[0];
//   for (i=1; i<sizeof(arr); i++)
//     if (arr[i] > j)
//       j = arr[i];
//   return j;
// }

// int mini(int *arr) {
//   int i, j;
//   if (!sizeof(arr)) {
//     notify_fail("Array must contain one or more ints.\n");
//     return 0;
//   }
//   j = arr[0];
//   for (i=1; i<sizeof(arr); i++)
//     if (arr[i] < j)
//       j = arr[i];
//   return j;
// }


static nomask mixed * array_copy(mixed * what) 
{
  mixed result;

  if (typeof(what) != T_ARRAY)
    return ({ });

  result = what[..];

  return result;
}


// map elements in an array 
static nomask mixed * map_array(mixed *arr, string func, mixed ob, varargs mixed args...) 
{
  int i;
  mixed * result;

  result = allocate(sizeof(arr));

  for (i= 0; i < sizeof(arr); i++)
    result[i] = (call_other(ob, func, arr[i], args...));
  
  return result;
}



// sort_array

// this bubblesort implementation and the idea of the pre-defined sort-array types
// has been taken from the MudOSAlike4DGD project

private int sort_string_asc(string str1, string str2) { return strcmp(str1, str2); }
private int sort_string_des(string str1, string str2) { return strcmp(str2, str1); }
private int sort_int_asc(int int1, int int2) { return int1-int2; }
private int sort_int_des(int int1, int int2) { return int2-int1; }
private int sort_float_asc(float f1, float f2) { return (int)(f1-f2); }
private int sort_float_des(float f1, float f2) { return (int)(f2-f1); }

static nomask mixed * sort_array(mixed * arr, varargs string fun, mixed ob, int dir) 
{
  int i, e, sz;
  mixed * result, a, b, a2;

  if (!fun) 
  {
    if (sizeof(arr) == 0)
      return array_copy(arr);

    // check for pre-defined sort_array types 
    switch (typeof(arr[0])) 
    {
      case T_STRING:
        fun = "sort_string_asc";
        ob = this_object();
        break;
      case T_INT:
        fun = "sort_int_asc";
        ob = this_object();
        break;
      case T_FLOAT:
        fun = "sort_float_asc";
        ob = this_object();
        break;
      default:
        return array_copy(arr);
    }
  }

  result = array_copy(arr);
  i = sizeof(result);
  a = result[sz = --i];

  // ascending
  if (dir >= 0) 
  {
    while (--i >= 0) 
    {
      b = result[i];
      if (call_other(ob, fun, a, b) >= 0) 
      {
        // continue
        a = b;
      }
      else 
      {
        result[e=i] = a;
        while (++e < sz && call_other(ob, fun, a2=result[e+1], b) < 0) 
        {
          // swap backwards until we got it right
          result[e] = a2;
        }
        result[e] = b;
      }
    }
  }
  // descending
  else 
  {
    while (--i >= 0) 
    {
      b = result[i];
      if (call_other(ob, fun, a, b) > 0) 
      {
        result[e=i] = a;
        while (++e < sz && call_other(ob, fun, a2=result[e+1], b) > 0) 
        {
          // swap backwards until we got it right
          result[e] = a2;
        }
        result[e] = b;
      }
      else 
      {
        // continue
        a = b;
      }
    }
  }

  return result;
}


