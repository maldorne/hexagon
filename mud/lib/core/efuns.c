
// tmp prototype so we could debug from everywhere, for debugging
static string to_string(mixed ob, varargs int offset, string pre);
static string print_object(mixed ob);
static nomask void write(string str);
static nomask void stderr(string str);


#include "/lib/core/efuns/mud.c"

// neverbot, 03/14
#include "/lib/core/efuns/types.c"
#include "/lib/core/efuns/string.c"
#include "/lib/core/efuns/mapping.c"
#include "/lib/core/efuns/array.c"
#include "/lib/core/efuns/function.c"
#include "/lib/core/efuns/object.c"
#include "/lib/core/efuns/euid.c"
#include "/lib/core/efuns/shadow.c"
#include "/lib/core/efuns/calls.c"
#include "/lib/core/efuns/filter.c"
#include "/lib/core/efuns/compiler.c"

#include "/lib/core/efuns/living.c"
#include "/lib/core/efuns/user.c"
#include "/lib/core/efuns/init.c"

#include "/lib/core/efuns/move.c"
#include "/lib/core/efuns/actions.c"

#include "/lib/core/efuns/event.c"

#include "/lib/core/efuns/output.c"
#include "/lib/core/efuns/file.c"
#include "/lib/core/efuns/input.c"

#include "/lib/core/efuns/heart_beat.c"
#include "/lib/core/efuns/conversions.c"

// included for compatibility, neverbot 17/01/03
#include "/lib/core/efuns/process_string.c"

// users, children, etc
#include "/lib/core/efuns/collections.c"
#include "/lib/core/efuns/singletons.c"

// neverbot 21/4/2003 
#include "/lib/core/efuns/time.c"
#include "/lib/core/efuns/log.c"

#include "/lib/core/efuns/find_match.c"

#include "/lib/core/efuns/snoop.c"

// Hamlet patch
#include "/lib/core/efuns/exec.c"

#include "/lib/core/efuns/domains.c"
#include "/lib/core/efuns/path.c"
#include "/lib/core/efuns/math.c"
#include "/lib/core/efuns/net.c"
