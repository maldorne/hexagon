/*
 * Config stuff for the action queue, and error codes
 *
 * By Wonderflug, 1996
 */

#ifndef _ACTION_QUEUE_H
#define _ACTION_QUEUE_H

/* This determines the maximum size of the action queue, and should be
 * kept fairly small since every living object gets one.  (note that
 * currently it isn't always this size, usually it will be 0 sized)
 */
#define AQ_MAX_ACTIONS 50

/* This is an upper bound on the number of actions that can be performed
 * in any given hb.  Not a strict upper bound; it only counts trivial actions;
 * presumably for regular actions the limitations imposed by the action
 * counter will prevent them getting too high.  Mostly, this is to prevent
 * abuse such as recursive aliases.
 */
// #define MAXIMUM_COMMANDS_PER_HB 5
#define MAXIMUM_COMMANDS_PER_HB 2

#define ACT_TIME_CONTROL 0

/* return codes */
#define AQ_OK    0
/* errors */
#define AQ_FULL  -1
#define AQ_EMPTY -2
#define AQ_ERROR -3

/* Busy conditions; used for prompting */
#define NON_INTERRUPTABLE_BUSY 2
#define INTERRUPTABLE_BUSY     1
#define NOT_BUSY               0

/* Some common aliases */
#define EXPANSION ([     \
     "n" : "norte",      \
     "s" : "sur",      \
     "e" : "este",       \
     "ne" : "noreste", \
     "no" : "noroeste", \
     "se" : "sudeste", \
     "so" : "sudoeste", \
     "ar"  : "arriba",         \
     "ab" : "abajo",       \
     "o" : "oeste",       \
     "de" : "dentro",       \
     "fu" : "fuera",       \
     "es" : "escaleras",       \
  ])

#endif

