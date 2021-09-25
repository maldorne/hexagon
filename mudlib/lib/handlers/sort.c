
#include <type.h>

// these bubblesort and quicksort implementations and the idea of the 
// pre-defined sort functions are based in the implementation included
// with the MudOSAlike4DGD project

// the code has been reviewed and has been changed, but in the end this
// is just an implementation of quicksort and bubblesort... 
// so I prefer to think there is no license or copyright 
// infringment as this really... cannot be done very differently and in
// the end are just implementations of public domain algorithms

static int sort_string_asc(string str1, string str2) { return strcmp(str1, str2); }
static int sort_string_des(string str1, string str2) { return strcmp(str2, str1); }
static int sort_int_asc(int int1, int int2) { return int1-int2; }
static int sort_int_des(int int1, int int2) { return int2-int1; }
static int sort_float_asc(float f1, float f2) { return (int)(f1-f2); }
static int sort_float_des(float f1, float f2) { return (int)(f2-f1); }

string choose_default_sort_function(mixed type)
{
  switch (typeof(type)) 
  {
    case T_STRING:
      return "sort_string_asc";
    case T_INT:
      return "sort_int_asc";
    case T_FLOAT:
      return "sort_float_asc";
    default:
      return "";
  }  
}

// example of direction:
// using "sort_int_asc" with dir == -1 would be the same as 
// using "sort_int_des" with dir >= 0

mixed * bubblesort(mixed * arr, varargs string fun, mixed ob, int dir) 
{
  int i, j, sz;
  mixed * result, a, b, pivot;

  result = ({ });

  if (sizeof(arr) <= 1)
    return result;

  result = array_copy(arr);

  if (!fun)
  {
    // check for pre-defined sort_array types 
    fun = choose_default_sort_function(arr[0]);
    if (!strlen(fun))
      return result;
    ob = this_object();
  }
  else
    if (!ob)
      ob = this_object();

  i = sizeof(result);
  a = result[sz = --i];

  // descending
  if (!undefinedp(dir) && (dir < 0))
  {
    while (--i >= 0) 
    {
      mixed ret;
      b = result[i];

      if (undefinedp(ret = call_other(ob, fun, a, b)) || (ret >= 0)) 
      {
        // continue
        a = b;
      }
      else 
      {
        result[j = i] = a;
        while (++j < sz && 
          (intp(ret = call_other(ob, fun, pivot = result[j+1], b)) && 
          (ret < 0))) 
        {
          // swap backwards until we got it right
          result[j] = pivot;
        }
        result[j] = b;
      }
    }
  }
  // ascending
  else 
  {
    while (--i >= 0) 
    {
      b = result[i];
      if (call_other(ob, fun, a, b) > 0) 
      {
        result[j = i] = a;
        while (++j < sz && call_other(ob, fun, pivot = result[j+1], b) > 0) 
        {
          // swap backwards until we got it right
          result[j] = pivot;
        }
        result[j] = b;
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

mixed * quicksort(mixed *arr, varargs mixed fun, mixed ob, int dir) 
{
  int stack_size, min, max, low, high, pivot;
  mixed * result, stack, a;

  result = ({ });

  if (sizeof(arr) <= 1)
    return result;

  result = array_copy(arr);

  if (!fun)
  {
    // check for pre-defined sort_array types 
    fun = choose_default_sort_function(arr[0]);
    if (!strlen(fun))
      return result;
    ob = this_object();
  }
  else
    if (!ob)
      ob = this_object();

  stack = ({ 0, sizeof(result)-1 });
  stack_size = 2;

  // descending
  if (!undefinedp(dir) && (dir < 0))
  {
    while (stack_size >= 2) 
    {
      // pop ({ <low>, <high> }) from stack
      min = low = stack[stack_size-2];
      max = high = stack[stack_size-1];
      
      if (stack_size > 2) 
        stack = stack[..stack_size-3];
      else 
        stack = ({ });
      
      stack_size -= 2;

      // sort <pivot> towards all elements in the span [min..max]
      a = result[pivot = low];
      while (low < high) 
      {
        while (call_other(ob, fun, a, result[high]) < 0 && --high > low);
          result[low] = result[high];
        while (++low < high && call_other(ob, fun, a, result[low]) > 0);
          result[high] = result[low];
      }

      // next <pivot> at <high>
      result[pivot = high] = a;

      // <max> greater than <pivot>: sort [pivot+1..max] (push)
      if (max > pivot) 
      {
        stack += ({ pivot+1, max });
        stack_size += 2;
      }

      // <min> lesser than <pivot>: sort [min..pivot-1] (push)
      if (min < pivot) 
      {
        stack += ({ min, pivot-1 });
        stack_size += 2;
      }
    }
  }
  // ascending
  else 
  {
    while (stack_size >= 2) 
    {
      // pop ({ <low>, <high> }) from stack
      min = low = stack[stack_size-2];
      max = high = stack[stack_size-1];

      if (stack_size > 2) 
        stack = stack[..stack_size-3];
      else 
        stack = ({ });

      stack_size -= 2;

      // sort <pivot> towards all elements in the span [min..max]
      a = result[pivot = low];
      while (low < high) 
      {
        while (call_other(ob, fun, a, result[high]) > 0 && --high > low);
          result[low] = result[high];
        while (++low < high && call_other(ob, fun, a, result[low]) < 0);
         result[high] = result[low];
      }

      // next <pivot> at <high>
      result[pivot = high] = a;

      // <max> greater than <pivot>: sort [pivot+1..max] (push)
      if (max > pivot) 
      {
        stack += ({ pivot+1, max });
        stack_size += 2;
      }

      // <min> lesser than <pivot>: sort [min..pivot-1] (push)
      if (min < pivot) 
      {
        stack += ({ min, pivot-1 });
        stack_size += 2;
      }
    }
  }

  return result;
}
