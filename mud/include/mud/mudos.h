
#ifndef __MUDOS_CONFIG
#define __MUDOS_CONFIG

#define is_mudlib(f) (strlen(f) >= 5 && (f)[0..4] == "/lib/")

#define mudlib_privileges() (nullp(previous_object()) || \
                             (is_mudlib(object_name(previous_object())) && \
                              previous_function() != "call_other"))


#define MAX_HB_DEPTH 100
#define MAX_HB_TICKS 100000

// for the list of pending call_outs
#define CALL_OUT_STACK_OB     0
#define CALL_OUT_STACK_HANDLE 1
#define CALL_OUT_STACK_FUNC   2
#define CALL_OUT_STACK_TIME   3
#define CALL_OUT_STACK_ARG    4
#define CALL_OUT_STACK_TP     5

#endif
