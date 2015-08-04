
#ifndef __MUDOS_CONFIG
#define __MUDOS_CONFIG

#define is_mudlib(f)  (strlen(f) >= 5 && (f)[0..4] == "/lib/")

#define mudlib_privileges() (is_mudlib(previous_program()) && \
                             previous_function() != "call_other")


#endif
