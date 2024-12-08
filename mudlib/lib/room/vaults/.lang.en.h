
#define _LANG_VAULTS_ROOM_SHORT "Vault Room"
#define _LANG_VAULTS_ROOM_LONG "This is the vault room. Here your can store items until the time " + \
         "you need them again.\n"
#define _LANG_VAULTS_SIGN_LONG "A small wooden sign, hanging on the wall.\n"

#define _LANG_VAULTS_CMDS_DEPOSIT ({ "deposit" })
#define _LANG_VAULTS_CMDS_RETRIEVE ({ "retrieve" })
#define _LANG_VAULTS_CMDS_LIST ({ "list" })
#define _LANG_VAULTS_CMDS_LOGS ({ "logs" })

#define _LANG_VAULTS_LOG_EMPTY "The log is empty.\n"
#define _LANG_VAULTS_LOG_PERMISSION "You don't have permission to view the log.\n"
#define _LANG_VAULT_IN_USE "The vault is in use, please try again in a few seconds.\n"

#define _LANG_VAULT_ROOM_DEPOSIT_WHAT "Deposit what?\n"
#define _LANG_VAULT_ROOM_DEPOSIT_FULL "The vault is full, you cannot deposit more items in it.\n"
#define _LANG_VAULT_ROOM_DEPOSIT_EQUIPPED "You cannot deposit in a vault your " + all[i]->query_short() + \
                ", you are wearing it.\n"
#define _LANG_VAULT_ROOM_DEPOSIT_AVOID "When trying to deposit your " + all[i]->query_short() + " " + \
          "in the vault, it disappears leaving a puff of smoke in its place.\n"
#define _LANG_VAULT_ROOM_DEPOSIT_NO_SAVE "You cannot deposit in a vault your " + all[i]->query_short() + ", " + \
                "these objects cannot be saved.\n"
#define _LANG_VAULT_ROOM_DEPOSIT_MONEY "Look for a bank if you want to store money.\n"
#define _LANG_VAULT_ROOM_DEPOSIT_NO_VALUE "You cannot deposit in a vault your " + all[i]->query_short() + ", these " + \
                "objects are not allowed in vaults.\n"
#define _LANG_VAULT_ROOM_DEPOSIT_ME "You deposit your " + all[i]->query_short()+".\n"
#define _LANG_VAULT_ROOM_DEPOSIT_THEM this_player()->query_cap_name()+" deposits "+all[i]->query_numeral()+" "+ \
            all[i]->query_short()+" in the vault.\n"
#define _LANG_VAULT_ROOM_DEPOSIT_FAILED "You could not deposit in a vault your " + all[i]->query_short() + ".\n"
#define _LANG_VAULT_ROOM_DEPOSIT_NOT_FOUND "You cannot find anything by the name of '"+str+"'.\n"
#define _LANG_VAULT_ROOM_DEPOSIT_LOG "deposited"

#define _LANG_VAULT_ROOM_RETRIEVE_WHAT "Retrieve what?\n"
#define _LANG_VAULT_ROOM_RETRIEVE_ME "You retrieve "+all[i]->query_numeral()+" "+all[i]->query_short()+" from the vault.\n"
#define _LANG_VAULT_ROOM_RETRIEVE_THEM (string)this_player()->query_cap_name()+" retrieves "+all[i]->query_numeral()+" "+ \
          all[i]->query_short()+" from the vault.\n"
#define _LANG_VAULT_ROOM_RETRIEVE_LOG "retrieved"
#define _LANG_VAULT_ROOM_RETRIEVE_WEIGHT "You couldn't carry all the weight of "+all[i]->query_numeral()+" " + \
                 all[i]->query_short()+".\n"
#define _LANG_VAULT_ROOM_RETRIEVE_CANNOT "You cannot retrieve '"+str+"'.\n"

#define _LANG_VAULTS_LIST_ARMOURS "armours"
#define _LANG_VAULTS_LIST_WEAPONS "weapons"
#define _LANG_VAULTS_LIST_SHIELDS "shields"
#define _LANG_VAULTS_LIST_HERBS "herbs"
#define _LANG_VAULTS_LIST_SKINS "skins"
#define _LANG_VAULTS_LIST_OTHER "other"
#define _LANG_VAULTS_LIST_HEADER "There are " + number_as_string(sizeof(all)) + " item"+((sizeof(all)>1)?"s":"")+ " in the vault.\n"
#define _LANG_VAULTS_LIST_USING "A "+capacity+"% of the vault capacity is being used.\n"
#define _LANG_VAULTS_LIST_EMPTY "The vault is empty.\n"
