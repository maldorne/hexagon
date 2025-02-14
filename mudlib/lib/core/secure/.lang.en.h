
// finger

#define _LANG_FINGER_DOMAIN_NOT_FOUND "This domain has some problem.\n"
#define _LANG_FINGER_DOMAIN_HEADER "Domain of %^BOLD%^" + name + "%^RESET%^.\n"
#define _LANG_FINGER_DOMAIN_MANAGER "The manager of this domain is " + \
                    capitalize(master->query_dom_lord())+".\n"
#define _LANG_FINGER_DOMAIN_NO_MANAGER "This domain has no assigned manager.\n"
#define _LANG_FINGER_DOMAIN_ONLY_CODER "The only coder of this domain is "
#define _LANG_FINGER_DOMAIN_CODERS "The coders of this domain are: "
#define _LANG_FINGER_DOMAIN_NO_INFO "This domain has no info available.\n"

#define _LANG_FINGER_NAME "Name: "
#define _LANG_FINGER_REAL_NAME "Real name: "
#define _LANG_FINGER_BIRTHDAY "Birthday: "
#define _LANG_FINGER_EMAIL "E-mail: "
#define _LANG_FINGER_PLACE "City: "
#define _LANG_FINGER_RACE_GENDER "   %^GREEN%^"+(gender==2?"She":"He")+" is a " + \
            social_object_list[0]->query_race_gender_string(this_object(), 1)+"%^RESET%^. "
#define _LANG_FINGER_GUILD "   "+(gender==2?"She":"He")+" is a member of the '" + \
            social_object_list[1]->short()+"' guild.\n"
#define _LANG_FINGER_WRONG_GUILD "   "+(gender==2?"She":"He")+" is a member of a non existant guild.\n"
#define _LANG_FINGER_CODER "   %^GREEN%^"+(gender==2?"She":"He")+" is a "+ \
            "coder in "+mud_name()+"%^RESET%^.\n"
#define _LANG_FINGER_MANAGER (gender==2?"She":"He")+" is the manager of the " + \
            query_multiple_short(domains, 0) + " domain"+(sizeof(domains)==1?"":"s") + ".\n"
#define _LANG_FINGER_MANAGER_ONLY "   "+(gender==2?"She":"He")+" is not a member of any other domain.\n"
#define _LANG_FINGER_NO_MEMBER "   "+(gender==2?"She":"He")+" is not a member of any domain.\n"
#define _LANG_FINGER_MEMBER_OF "   "+(gender==2?"She":"He")+" is a member of the "+ \
            (string)implode(bing, ", ")+" domain"+(sizeof(bing)==1?"":"s")+".\n"
#define _LANG_FINGER_FIRST_ON "   Connected for the first time on "
#define _LANG_FINGER_TIME_ON "   %^GREEN%^"+(gender==2?"She":"He")+" has " + \
            seconds_to_time_string(time_on) + \
            " of playing time%^RESET%^.\n"
#define _LANG_FINGER_CONNECTED_SINCE "   Connected since "+ctime(last_log_on, 1)+".\n"
#define _LANG_FINGER_LAST_ON "   Last logged on " + \
            seconds_to_time_string(tmp_time) + " ago.\n"
#define _LANG_FINGER_IDLE_TIME "   %^GREEN%^Idle since "+ \
            seconds_to_time_string(ob->user()->query_idle()) + \
            " ago%^RESET%^.\n"
#define _LANG_FINGER_LAST_CONNECTION_FROM "   Last connection from: " + last_on_from + "\n"
#define _LANG_FINGER_LAST_POSITION "   Last position:\n     " + last_pos + "\n"
#define _LANG_FINGER_MAIL_NO_MAIL "   Has no mud-mails.\n"
#define _LANG_FINGER_MAIL_NUM_MAILS "   Has "+mail_stat["total"]+" mud-mail"+ \
            ((mail_stat["total"]==1)?"":"s")
#define _LANG_FINGER_MAIL_UNREAD_MAILS ", "+mail_stat["unread"]+" unread.\n"
#define _LANG_FINGER_PLAN "   Plan: "

// retire

#define _LANG_TRY_RETIRE_CODER "You are a programmer, you can't retire this way. Ask an administrator.\n"
#define _LANG_TRY_RETIRE_DELETE_WARNING "%^BOLD%^This will delete your character and all your belongings.%^RESET%^\n"
#define _LANG_TRY_RETIRE_BE_CAREFUL "Be sure of what you are doing.\nDo not enter the password if you regret.\n\n"
#define _LANG_TRY_RETIRE_INPUT_PASSWORD "Enter your password: "
#define _LANG_RETIRE_NO_PASSWORD "Without password you can't retire.\n"
#define _LANG_RETIRE_WRONG_PASSWORD "Wrong password, you can't retire.\n"
#define _LANG_RETIRE_OK "Ok, you have deleted your character, " + name + ".\n"
#define _LANG_RETIRE_GUEST_MSG "Now you are a guest.\nExit the game to delete your character definitively.\n"
