
// The social objects a living belongs to. The first eight name a blueprint --
// a race, a guild, a citizenship -- written by hand, one file per kind, and a
// living stores the path of that file. The ninth is different and deliberately
// so: a family is made at run time, there is no end of them and they die out,
// so no file can be written for one. It stores the surname, which the families
// handler resolves. Nothing walks this list generically, so the two kinds of
// entry never meet.
#define NUM_SOCIAL_OBJECTS 9

#define RACE_OB 0
#define GUILD_OB 1
#define RACEG_OB 2
#define GROUP_OB 3
#define JOB_OB 4
#define DEITY_OB 5
#define CITY_OB 6
#define CLASS_OB 7
#define FAMILY_OB 8

#define NULL_SOCIAL_VALUE "none"

#define LOG_ADJUST_LEVEL "adjust_level"
#define LOG_ADJUST_GUILD_LEVEL "adjust_guild_level"

// old jobs.h
// where a job object may live: the one every game shares
#define DEFAULT_JOB_DIR "/lib/obj/jobs/"
#define JOB_XP_LIMIT 15000
#define LOGFILE "job_busted"
#define LOG_JOB_XP "job_xp"
#define LOG_ADJUST_JOB_LEVEL "adjust_job_level"
