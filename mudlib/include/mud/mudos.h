
#ifndef __MUDOS_CONFIG
#define __MUDOS_CONFIG

#include <kernel.h>

#define is_mudlib(f) (strlen(f) >= 5 && (f)[0..4] == "/lib/")

#define mudlib_privileges() (nullp(initial_object()) || \
                             (is_mudlib(object_name(initial_object()))))

#define is_auto_object()    (previous_program() == AUTO && \
                             previous_function() != "call_other")

// rlimits of every heart_beat in any object
#define MAX_HB_DEPTH 100
#define MAX_HB_TICKS 1000000

// for the list of pending call_outs
#define CALL_OUT_STACK_OB     0
#define CALL_OUT_STACK_HANDLE 1
#define CALL_OUT_STACK_FUNC   2
#define CALL_OUT_STACK_TIME   3
#define CALL_OUT_STACK_ARG    4
#define CALL_OUT_STACK_TP     5
#define CALL_OUT_STACK_TU     6

#endif
