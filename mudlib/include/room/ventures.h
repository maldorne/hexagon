// neverbot, Agosto 2008

#define VENTURES_HANDLER "/lib/handlers/ventures.c"

/* Hamlet, August 1995.  */

/* PAY_RATES is broken into pairs.  First number is minimum of each
   value-range.  Second is the fraction of the value that will be given.
   To better explain, 500 copper, sells for 1/2.  1500 copper sells for
   1/3.  Values inside the range can be determined by calculating the
   end-points of the line-segments described and then drawing a line and
   looking up your point.  if you don't understand, get graph paper and
   try it.  it's not as hard as it sounds.
   UPGRADE NOTICE:  Probably best to let me do it.  But if you must,
   ((PAY_RATES[i] / PAY_RATES[i+1]) < (PAY_RATES[i+2] / PAY_RATES[i+3]))
   This property must hold true for any 4 elements where i is even.
   If you don't know why, again, graph paper will make things clear.
   *sigh*, there's another rule too.  
   PAY_RATES[i+2] * ( (PAY_RATES[i+2] / PAY_RATES[i+3]) - 
                      (PAY_RATES[i] / PAY_RATES[i+1]) ) < INT_MAX
   TRUST me on this one.  It must hold true.
*/

#define PAY_RATES ({ 0, 1, 500, 2, 1500, 3, 4000, 4, 14000, 7, 40000, 10, \
                     104000, 13, 204000, 17, 336000, 21, 500000, 25, \
                     667000, 29, 825000, 33, 999000, 37, 1168500, 41, \
                     1350000, 45, 1519000, 49, 1696000, 53, 1881000, 57, \
                     2074000, 61, 2147483647, 2074000 \
                  })

/* 100 platinum coins... ;) */
#define MAX_AMOUNT 50000

/* max objects in buy/sell - to prevent over-evalling */
#define MAX_OBS 8

#define CHA_AD 15
#define AVG_CHA 11

// #define GOOD_RACES ({ "elf","dwarf","halfling","half-elf" })
// #define NEUTRAL_RACES ({ "human","gnome","lizard-man" })
// #define EVIL_RACES ({ "drow","duergar","orc","half-orc","goblin"})

// absurdly high number to check results
#define MAX_AMOUNT_OF_THINGS 10000