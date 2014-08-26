

// Non-dgd efuns
// neverbot, 03/2014

#include <type.h>

int intp(mixed what)
{
  if (typeof(what) == T_INT)
    return TRUE;

  return FALSE;
}

int arrayp(mixed what)
{
  if (typeof(what) == T_ARRAY)
    return TRUE;

  return FALSE;
}

int pointerp(mixed what)
{
  return arrayp(what);
}

int mappingp(mixed what)
{
  if (typeof(what) == T_MAPPING)
    return TRUE;

  return FALSE;
}

int objectp(mixed what)
{
  if (typeof(what) == T_OBJECT)
    return TRUE;

  return FALSE;
}

int stringp(mixed what)
{
  if (typeof(what) == T_STRING)
    return TRUE;

  return FALSE;
}

int floatp(mixed what)
{
  if (typeof(what) == T_FLOAT)
    return TRUE;

  return FALSE;
}

int undefinedp(mixed what)
{
  if (what == nil)
    return TRUE;

  return FALSE;
}

// int userp(mixed what)
// defined in efuns/user.c

