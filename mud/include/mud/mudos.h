
#ifndef __MUDOS_CONFIG
#define __MUDOS_CONFIG

#define is_mudlib(f) (strlen(f) >= 5 && (f)[0..4] == "/lib/")

#define mudlib_privileges() (nullp(previous_object()) || \
                             (is_mudlib(object_name(previous_object())) && \
                              previous_function() != "call_other"))


#define MAX_HB_DEPTH 100
#define MAX_HB_TICKS 100000

#endif
