
// Non-dgd efuns
// neverbot, 03/2014

#include <type.h>

int intp(mixed what)
{
  if (typeof(what) == T_INT)
    return 1;

  return 0;
}

int arrayp(mixed what)
{
  if (typeof(what) == T_ARRAY)
    return 1;

  return 0;
}

int pointerp(mixed what)
{
  return arrayp(what);
}

int mappingp(mixed what)
{
  if (typeof(what) == T_MAPPING)
    return 1;

  return 0;
}

int mapp(mixed what)
{
  return mappingp(what);
}

int objectp(mixed what)
{
  if (typeof(what) == T_OBJECT)
    return 1;

  return 0;
}

int stringp(mixed what)
{
  if (typeof(what) == T_STRING)
    return 1;

  return 0;
}

int floatp(mixed what)
{
  if (typeof(what) == T_FLOAT)
    return 1;

  return 0;
}

int undefinedp(mixed what)
{
  if (what == nil)
    return 1;

  return 0;
}

int nullp(mixed what)
{
  return undefinedp(what);
}

// int userp(mixed what)
// defined in efuns/user.c

