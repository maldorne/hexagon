
// Non-dgd efuns
// neverbot, 03/2014

int member_array(mixed element, mixed *arr)
{
  int i;
  
  if (!element || !arr) 
   return -1;
  
  for (i = 0; i < sizeof(arr); i++)
    if (arr[i] == element)
      return i; 

  return -1;
}

mixed * delete(mixed * arr, int start, int len) 
{
  return arr[0..start-1]+arr[start+len..sizeof(arr)];
} 

mixed * slice_array(mixed * arr, int start, int fin) 
{
  return arr[start..fin];
} 

mixed * insert(mixed * arr, mixed el, int pos) 
{
  return arr[0..pos-1]+({ el })+arr[pos..sizeof(arr)];
} 

mixed * shift_left(mixed * arr)
{
	return arr[1..sizeof(arr)-1] + ({ });
}

mixed * shift_right(mixed * arr)
{
	return ({ }) + arr[0..sizeof(arr)-2];
}

mixed * exclude_array(mixed *arr, int from, varargs int to) 
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

mixed * uniq_array(mixed *arr) 
{
  mapping borg;
  int i;

  i = sizeof(arr);
  borg = ([]);
  
  while(i--) 
    borg[arr[i]] = 1;
  return keys(borg);
}

int atoi(string str) 
{
  int x;

  if(!stringp(str)) return 0;
  else sscanf(str, "%d", x);
  return x;
}

string itoa(int i)
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


mixed * array_copy(mixed * what) 
{
  mixed result;

  if (typeof(what) != T_ARRAY)
    return ({ });

  result = what[..];

  return result;
}
