
#include <kfun.h>

#include <type.h>
#include <limits.h>

static nomask int intp(mixed what)
{
  if (typeof(what) == T_INT)
    return 1;

  return 0;
}

static nomask int charp(mixed what) 
{
  return (intp(what) && 
         (CHAR_MIN <= what) && 
         (what <= CHAR_MAX));
}

static nomask int arrayp(mixed what)
{
  if (typeof(what) == T_ARRAY)
    return 1;

  return 0;
}

static nomask int pointerp(mixed what)
{
  return arrayp(what);
}

static nomask int mappingp(mixed what)
{
  if (typeof(what) == T_MAPPING)
    return 1;

  return 0;
}

// static nomask int mapp(mixed what)
// {
//   return mappingp(what);
// }

static nomask int objectp(mixed what)
{
  if (typeof(what) == T_OBJECT)
    return 1;

  return 0;
}

// defined in object.c
// static nomask int clonep(varargs mixed what)

static nomask int stringp(mixed what)
{
  if (typeof(what) == T_STRING)
    return 1;

  return 0;
}

static nomask int floatp(mixed what)
{
  if (typeof(what) == T_FLOAT)
    return 1;

  return 0;
}

static nomask int undefinedp(mixed what)
{
  if (what == nil)
    return 1;

  return 0;
}

// this define will only be present in kfun.h if DGD was compiled with the 
// -DCLOSURES flag, allowing the use of function pointers
#ifdef KF_NEW_FUNCTION  

int functionp(mixed what)
{
  if (typeof(what) == T_OBJECT && what<-"/builtin/function")
    return 1;

  return 0;
}

#endif 

// int userp(mixed what)
// defined in efuns/user.c
