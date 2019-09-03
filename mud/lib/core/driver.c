
/*
 *  Interface between the mudlib and DGD
 */

#include <kernel.h>
#include <status.h>
#include <trace.h>

#include <user/user.h>
#include <living/living.h>
#include <user/terminal.h>
#include <mud/secure.h>
#include <mud/config.h>
#include <mud/translations.h>


// ************************************************************
//  Function prototypes
// ************************************************************

nomask void log_driver(string str);
static nomask void inform_user(string str, int message_type);
// own version, different from the one in the auto object
static nomask void write(string str);


// ************************************************************
//  efuns needed in the driver object
// ************************************************************

// to avoid the error:
//     /lib/core/driver.c: too many functions declared
// do not include every efun in the driver object, just the ones used,
// as the driver cannot inherit other programs

// #include "/lib/core/efuns.c"

#include "/lib/core/efuns/types.c"
#include "/lib/core/efuns/output/stderr.c"
#include "/lib/core/efuns/users/this_player.c"
#include "/lib/core/efuns/functions/previous_objects.c"
#include "/lib/core/efuns/functions/initial_object.c"
#include "/lib/core/efuns/objects/compile_object.c"
#include "/lib/core/efuns/objects/load_object.c"
#include "/lib/core/efuns/output/fix_string.c"
#include "/lib/core/efuns/arrays/member_array.c"
#include "/lib/core/efuns/paths/resolve_path.c"
#include "/lib/core/efuns/livings/living.c"
#include "/lib/core/efuns/users/interactive.c"
#include "/lib/core/efuns/objects/file_name.c"
#include "/lib/core/efuns/conversions/to_string.c"
#include "/lib/core/efuns/conversions/print_object.c"
#include "/lib/core/efuns/paths/path.c"
// #include "/lib/core/efuns/singletons.c"


static object user_h;
static object living_h;
static object error_h;
static object object_h;

static object mudos;
static object secure;
static object debugger;

static nomask void initialize()
{
  string date;

  date = ::ctime(time())[4 .. 18];

  log_driver("[" + date + "] ** " + status()[ST_VERSION] + "\n");
  log_driver("Initializing...\n");
#ifdef __NETWORK_EXTENSIONS__
  log_driver("Network extensions in use.\n");
#endif

  load_object(AUTO);

  ::call_other(error_h  = load_object(ERROR_HANDLER), "???"); // obviously, must be the first
  ::call_other(debugger = load_object(DEBUGGER_OB), "???");

  // global object in charge of heart_beats, init calls, etc
  ::call_other(mudos    = load_object(MUDOS_PATH), "???");
  ::call_other(secure   = load_object(SECURE_OB), "???");
  ::call_other(user_h   = load_object(USER_HANDLER), "???");
  ::call_other(living_h = load_object(LIVING_HANDLER), "???");
  ::call_other(object_h = load_object(OBJECT_HANDLER), "???");
  ::call_other(load_object(TERM_HANDLER), "???");

  load_object(LOGIN_OB);

  date = ::ctime(time())[4 .. 18];

  log_driver("\n[" + date + "] ** " + "Initialization complete.\n\n");
}

// Called from stderr efun (so it shouldn't be static)
nomask void _stderr(string str)
{
  if (!str)
    return;

  send_message(str);
}

// return the mudos global object
nomask object mudos() { return mudos; }
// return the secure global object
nomask object secure() { return secure; }
// return the debugger global object
nomask object debugger() { return debugger; }
// get a new login object
nomask object login()
{
  object login;

  login = clone_object(find_object(LOGIN_OB));

  return login;
}

nomask void log_driver(string str)
{
  _stderr(str);
}

static nomask void write(string str)
{
  object user;

  if (nullp(str))
    return;

  user = this_user();

  if (!user)
    user = this_player();

  if (!user)
    return;

  str = fix_string(str);

  user->catch_tell(str);
}

static nomask void inform_user(string str, int message_type)
{
  if (this_player()->query_coder())
    write(str);

  switch(message_type)
  {
    case DRIVER_RUNTIME_ERROR:

      write("\nSe ha producido un error.\n");

      if (strlen(mudos->query_current_command()))
        write(_LANG_DIDNT_WORK);
      this_player()->show_prompt("\n");

      break;

    case DRIVER_COMPILE_ERROR:
      break;

    case DRIVER_ATOMIC_ERROR:
      break;
  }
}

// The driver object cannot clone objects
nomask object clone_object(mixed what, varargs string uid)
{
  log_driver("Attempt to clone object from driver, avoided\n");
  return nil;
}

// nomask object compile_object(string path)
// {
//   log_driver("Attempt to compile object from driver, avoided\n");
//   return nil;
// }

#ifndef __NETWORK_EXTENSIONS__
static object telnet_connect(int port)
{
  object user;

  log_driver(" ~ telnet_connect: " + port + "\n");

  user = user_h->new_connection();

  return user;
}
#endif

static string object_type(string file, string type)
{
  string str;

  str = resolve_path(file);

  log_driver(" ~ object_type: " + file + " = " + str + "\n");

  return str;

  // return normalize_path(type, file + "/..", creator(file));

  // TODO check this is working
  // return resolve_path(file);
}

static int compile_rlimits(string objname)
{
  // unlimited resource usage for kernel objects
  // return sscanf(objname, "/lib/core/%*s");
  return TRUE;
}

static object inherit_program(string from, string path, int priv)
{
  int i;
  object ob;
  string err;

  if ((i = strlen(path)) >= 2 && path[i - 2 ..] == ".c")
    path = path[0 .. i - 3];

  ob = ::find_object(path);

  if (ob)
    return ob;

  log_driver(" + inherit_program: " + path + " from " + from + "\n");

  log_driver(" - compile_object: " + path + " (from driver)\n");
  err = catch(ob = ::compile_object(path));

  if (err)
  {
    log_driver(" + inherit_program error: " + err + "\n");
    inform_user(" + inherit_program error\n   compile_object returned: " + err + "\n", DRIVER_COMPILE_ERROR);
  }

  return ob;
}

static mixed include_file(string includer, string include)
{
  // multilanguage options, when we try to include the general language header file
  // with an #include <language.h>,
  // we will include a file from our current directory: .lang.en, .lang.es, etc
  if (include == "/include/language.h")
  {
    include = path(includer) + ".lang." + GLOBAL_COMPILE_LANG;
  }
  else
  {
    if (include[0] != '/')
      include = resolve_path(includer + "/../" + include);
  }

  return include;
}

// Error handling

static void runtime_error(string error, int caught, int ticks)
{
  string ret;

  if (!error_h)
    log_driver(" - runtime_error: " + error + "\n");
  else
    ret = error_h->runtime_error(error, caught, ticks);

  if (!ret || !strlen(ret))
    return;

  log_driver(ret);
  inform_user(ret, DRIVER_RUNTIME_ERROR);
}

static void compile_error(string file, int line, string error)
{
  string ret;

  if (!error_h)
    log_driver(" - compile_error in file " + file + ", line " + line + "\n" +
               "   Error: " + error + "\n");
  else
    ret = error_h->compile_error(file, line, error);

  if (!ret || !strlen(ret))
    return;

  log_driver(ret);
  inform_user(ret, DRIVER_COMPILE_ERROR);
}

static string atomic_error(string error, int atom, mixed **trace)
{
  string ret;
  ret = error_h->atomic_error(error, atom, trace);

  if (!ret || !strlen(ret))
    return "";

  log_driver(ret);
  inform_user(ret, DRIVER_ATOMIC_ERROR);

  return ret;
}

// An object which has been marked by call_touch() is about to have the
// given function called in it.  A non-zero return value indicates that the
// object's "untouched" status should be preserved through the following
// call.
static int touch(object obj, string func)
{
  log_driver(" - touch object " + object_name(obj) + ", function " + func + "\n");

  return FALSE;
}

// get an object for call_other's first (string) argument
static object call_object(string path)
{
  object ob;

  if (ob = find_object(path))
    return ob;

  return compile_object(path);
}

// The last reference to the given program has been removed.
static void remove_program(string path, int timestamp, int index)
{
  log_driver(" + remove_program: " + path + "\n");
}
