// job entrances. `me` is the player joining or leaving, `job` the job object

#define _LANG_JOB_ENTRANCE_CMDS_JOIN ({ "join" })
#define _LANG_JOB_ENTRANCE_CMDS_LEAVE ({ "leave" })
#define _LANG_JOB_ENTRANCE_YES ({ "y", "yes" })

#define _LANG_JOB_ENTRANCE_SIGN_LONG "A sign hangs on the wall, by the entrance.\n"
#define _LANG_JOB_ENTRANCE_SIGN_NO_JOB "No job is taught here right now."
#define _LANG_JOB_ENTRANCE_SIGN_TITLE "Job: "
#define _LANG_JOB_ENTRANCE_SIGN_COMMANDS \
        "  join     take up this job\n" + \
        "  leave    give up the job you have\n\n" + \
        "Only one job may be held at a time. Whoever comes back to a job they " + \
        "left gets back the level they had, but not the experience they had " + \
        "towards the next one."

#define _LANG_JOB_ENTRANCE_NO_JOB "No job is taught here right now.\n"
#define _LANG_JOB_ENTRANCE_ALREADY_MEMBER "You already have this job.\n"
#define _LANG_JOB_ENTRANCE_OTHER_JOB "You already have a job: you will have to " + \
        "give it up before you take up another.\n"
#define _LANG_JOB_ENTRANCE_NOT_LEGAL "You do not meet the conditions to take up " + \
        "this job.\n"
#define _LANG_JOB_ENTRANCE_JOINED "You join: from today your job is " + \
        lower_case((string)job->query_short()) + ".\n"
#define _LANG_JOB_ENTRANCE_JOINED_OTHERS me->query_cap_name() + " joins to " + \
        "learn the job.\n"
#define _LANG_JOB_ENTRANCE_NO_JOB_TO_LEAVE "You have no job.\n"
#define _LANG_JOB_ENTRANCE_CANNOT_LEAVE "For some reason you are not allowed to " + \
        "give up your job.\n"
#define _LANG_JOB_ENTRANCE_LEAVE_CONFIRM "Do you really want to give up your " + \
        "job? You will keep your level, but lose the job experience you have " + \
        "gathered. (y/n) "
#define _LANG_JOB_ENTRANCE_LEAVE_CANCELLED "All right, you keep your job.\n"
#define _LANG_JOB_ENTRANCE_LEFT "You give up your job: from now on you have " + \
        "none.\n"
