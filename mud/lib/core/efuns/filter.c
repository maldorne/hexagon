
// filter - select certain elements

// mixed filter(mixed x, string fun, object ob, mixed extra, ...);

// mixed filter(mixed x, function f, mixed extra, ...);

// The (ob, fun) syntax works as if (: call_other, ob, fun :) had been passed as
// f.  Filter returns a new structure containing only the elements of x for which
// the function returns nonzero.  Currently, it can be used on arrays and
// mappings.  In the case of mappings, both the key and the value are passed
// to the function.  extra and all the following arguments are passed to the
// function after the element.  For example, filter(arr, fun, 2, 3) will
// first call fun(arr[0], 2, 3) then fun(arr[1], 2, 3) etc.


static nomask mapping filter_mapping(mapping map, string func, mixed ob, varargs mixed extra...)
{
  mixed * bing;
  mapping ret;
  int i;

  ret = ([ ]);
  bing = keys(map);

  for (i = 0; i < sizeof(bing); i++) 
  {
    if (call_other(ob, func, map[bing[i]], extra...))
      ret[bing[i]] = map[bing[i]];
  }

  return ret;
}

static nomask mixed * filter_array(mixed * list, string func, varargs mixed ob, mixed extra...)
{
  int i;
  mixed * ret;

  if (!ob)
    ob = this_object();

  ret = ({ });

  for (i = 0; i < sizeof(list); i++) 
  {
    if (call_other(ob, func, list[i], extra...))
      ret += ({ list[i] });

    // ret += call_other(list[i], func, ob, extra);
  }

  return ret;
}

static nomask mixed filter(mixed list, string func, varargs mixed ob, mixed extra)
{
  if (!ob)
    ob = this_object();
  
  if (arrayp(list))
    return filter_array(list, func, ob, extra);
  if (mappingp(list))
    return filter_mapping(list, func, ob, extra);
}
