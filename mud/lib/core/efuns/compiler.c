
#include <status.h>

// #define eval_cost() status()[ST_TICKS]

// eval_cost - returns the evaluation cost remaining
// void eval_cost()
// eval_cost() returns the number of instructions that can be executed
// before the driver decides it is in an infinite loop.

static nomask int eval_cost()
{
  return status()[ST_TICKS];
}

// reset_eval_cost - resets the evaluation cost remaining
// void reset_eval_cost();
// reset_eval_cost() resets the evaluation cost remaining to the maximum
// evaluation cost.

static nomask void reset_eval_cost()
{

}

// set_eval_limit - set the maximum evaluation cost
// void set_eval_limit( int );
// set_eval_limit(), with a nonzero argument, sets the maximum evaluation
// cost that is allowed for any one thread before a runtime error occurs.
// With a zero argument, it sets the current evaluation counter to zero,
// and the maximum cost is returned.  set_eval_limit(-1) returns the
// remaining evaluation cost.

static nomask int set_eval_limit(int limit)
{
  /*
  if (limit == -1)
  {
    // just info
    return eval_cost();
  }
  // set to the max
  else if (limit == 0)
  {
    // something here...
    return eval_cost();
  }
  */

  // something here
  return eval_cost();
}

// idea taken from the DGD mailing list
// https://mail.dworkin.nl/pipermail/dgd/2003-March/003279.html

#define kb_memory(mem,used) sprintf("%2.2f Mb (%2.2f%% used)", \
                                    (float)mem / 1048576.0, \
                                    ((float)used/(float)mem) * 100.0)

static nomask string memory_status()
{
  mixed *status;

  status = status();
  return "MEMORY STATUS: static: " + 
         kb_memory(status[ST_SMEMSIZE], status[ST_SMEMUSED]) +
         " dynamic: " + 
         kb_memory(status[ST_DMEMSIZE], status[ST_DMEMUSED]) +
         "\n";
}

// memory_info - obtain info on object/overall memory usage
// varargs int memory_info( object ob );
// If optional argument `ob' is given, memory_info() returns the approximate
// amount of memory that `ob' is using.  If no argument is given, memory_info()
// returns the approximate amount of memory that the entire mud is using.  Note
// that the amount of memory the mud is using does not necessarily correspond
// to the amount of memory actually allocated by the mud from the system, and
// that total memory used by all the objects is not additive due to sharing of
// certain structures.

static nomask int memory_info(varargs object ob)
{
  mixed *status;

  if (ob)
  {
    status = status(ob);
    return status[O_PROGSIZE] + status[O_DATASIZE];
  }

  status = status();  
  return status[ST_SMEMSIZE] + status[ST_DMEMSIZE];
}
