
// member_array - returns index of an occurence of a given item in an array or string
// int member_array( mixed item, mixed arr);
// int member_array( mixed item, mixed arr, int start);

// Returns the index of the first occurence of `item' in the array  or string
// `arr', or the first occurence at or after 'start'.
// If the item is not found, then -1 is returned.

int member_array(mixed element, mixed * arr)
{
  int i;
  
  if (!element || !arr) 
   return -1;
  
  for (i = 0; i < sizeof(arr); i++)
    if (arr[i] == element)
      return i; 

  return -1;
}
