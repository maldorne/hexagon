
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

static nomask mixed * unique_array(mixed *arr)
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
static int maxi(int *arr)
{
  int i, j;

  if (!sizeof(arr))
    return 0;

  j = arr[0];

  for (i = 1; i < sizeof(arr); i++)
    if (arr[i] > j)
      j = arr[i];

  return j;
}

static int mini(int *arr)
{
  int i, j;

  if (!sizeof(arr))
    return 0;

  j = arr[0];

  for (i = 1; i<sizeof(arr); i++)
    if (arr[i] < j)
      j = arr[i];

  return j;
}

static nomask mixed * array_copy(mixed * what)
{
  if (typeof(what) != T_ARRAY)
    return ({ });

  return what[..];
}

// map elements in an array
static nomask mixed * map_array(mixed *arr, string func, mixed ob, varargs mixed args...)
{
  int i;
  mixed * result;

  result = allocate(sizeof(arr));

  for (i = 0; i < sizeof(arr); i++)
    result[i] = (call_other(ob, func, arr[i], args...));

  return result;
}

// sort_array - sort an array
// array sort_array( array arr, string fun, object ob );
// array sort_array( array arr, function f );
// array sort_array( array arr, int direction );

// The (ob, fun) syntax behaves the same as if (: call_other, ob, fun :)
// was passed as f.

// In the first two forms, the returned array is sorted with respect to
// the comparison function given.  The function takes two elements as
// arguments, and returns -1 if if first argument is less than the second,
// 0 if they are the same, or 1 if the first argument is greater than the
// second.

// The third form returns an array with the same elements as 'arr', but
// quicksorted using built-in sort routines.  A 'direction' of 1 or 0 will
// quicksort in ascending order, while a 'direction' of -1 will
// quicksort in descending order.  A limitation of the built-in
// sort routines is that the array must be homogeneous, composed entirely
// of a single type, where that type is string, int, or float.
// Arrays of arrays are sorted by sorting based on the first element,
// making database sorts possible.

// test:
// exec return sort_array(({ "a", "c", "b" })) ascending order
// exec return sort_array(({ "a", "c", "b" }), nil, nil, -1) descending order

static nomask mixed * sort_array(mixed * arr,
  varargs string fun, mixed ob, int dir)
{
  return "/lib/handlers/sort"->bubblesort(arr, fun, ob, dir);
  // return "/lib/handlers/sort"->quicksort(arr, fun, ob, dir);
}


