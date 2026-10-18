// Hit point and gp regeneration (see heart_beat in /lib/living/living.c): a
// living regenerates one point on a heart_beat when its counter, plus its
// constitution (its main stat for gp), reaches REGEN_THRESHOLD plus a random
// number below REGEN_RANDOM. The counter restarts after each point.

#define REGEN_THRESHOLD 22
#define REGEN_RANDOM    10
