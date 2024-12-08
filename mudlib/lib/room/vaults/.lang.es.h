
#define _LANG_VAULTS_ROOM_SHORT "Habitación del baúl"
#define _LANG_VAULTS_ROOM_LONG "Esta es la habitación del baúl. Aquí se pueden almacenar los objetos " + \
         "hasta que llegue el momento de darles uso.\n"
#define _LANG_VAULTS_SIGN_LONG "Es un pequeño cartel de madera colgado en una pared.\n"

#define _LANG_VAULTS_CMDS_DEPOSIT ({ "depositar" })
#define _LANG_VAULTS_CMDS_RETRIEVE ({ "recuperar" })
#define _LANG_VAULTS_CMDS_LIST ({ "listar" })
#define _LANG_VAULTS_CMDS_LOGS ({ "logs" })

#define _LANG_VAULTS_LOG_EMPTY "El registro de actividades está vacío.\n"
#define _LANG_VAULTS_LOG_PERMISSION "No tienes permiso para ver el registro de actividades.\n"
#define _LANG_VAULT_IN_USE "El baúl está siendo usado, inténtalo de nuevo en unos segundos.\n"

#define _LANG_VAULT_ROOM_DEPOSIT_WHAT "¿Depositar qué?\n"
#define _LANG_VAULT_ROOM_DEPOSIT_FULL "El bául está lleno, ya no puedes depositar más objetos en él.\n"
#define _LANG_VAULT_ROOM_DEPOSIT_EQUIPPED "No puedes depositar en un baúl tu " + all[i]->query_short() + \
                ", lo llevas puesto.\n"
#define _LANG_VAULT_ROOM_DEPOSIT_AVOID "Al intentar depositar tu " + all[i]->query_short() + " " + \
          "en el baúl, desaparece dejando una nube de humo en su lugar.\n"
#define _LANG_VAULT_ROOM_DEPOSIT_NO_SAVE "No puedes depositar en un baúl tu " + all[i]->query_short() + ", " + \
                "estos objetos no se pueden salvar.\n"
#define _LANG_VAULT_ROOM_DEPOSIT_MONEY "Busca un banco si quieres almacenar dinero.\n"
#define _LANG_VAULT_ROOM_DEPOSIT_NO_VALUE "No puedes depositar en un baúl tu " + all[i]->query_short() + ", estos " + \
                "objetos no están permitidos en baules.\n"
#define _LANG_VAULT_ROOM_DEPOSIT_ME "Depositas tu " + all[i]->query_short()+".\n"
#define _LANG_VAULT_ROOM_DEPOSIT_THEM this_player()->query_cap_name()+" deposita "+all[i]->query_numeral()+" "+ \
           all[i]->query_short()+" en el baúl.\n"
#define _LANG_VAULT_ROOM_DEPOSIT_FAILED "No has podido depositar en un baúl tu " + all[i]->query_short() + ".\n"
#define _LANG_VAULT_ROOM_DEPOSIT_NOT_FOUND "No puedes encontrar nada por el nombre de '"+str+"'.\n"
#define _LANG_VAULT_ROOM_DEPOSIT_LOG "depositó"

#define _LANG_VAULT_ROOM_RETRIEVE_WHAT "¿Recuperar qué?\n"
#define _LANG_VAULT_ROOM_RETRIEVE_ME "Recuperas "+all[i]->query_numeral()+" "+all[i]->query_short()+" del baúl.\n"
#define _LANG_VAULT_ROOM_RETRIEVE_THEM (string)this_player()->query_cap_name()+" recupera "+all[i]->query_numeral()+" "+ \
          all[i]->query_short()+" del baúl.\n"
#define _LANG_VAULT_ROOM_RETRIEVE_LOG "recuperó"
#define _LANG_VAULT_ROOM_RETRIEVE_WEIGHT "No podrías cargar con todo el peso de "+all[i]->query_numeral()+" " + \
                 all[i]->query_short()+".\n"
#define _LANG_VAULT_ROOM_RETRIEVE_CANNOT "No puedes recuperar '"+str+"'.\n"

#define _LANG_VAULTS_LIST_ARMOURS "armaduras"
#define _LANG_VAULTS_LIST_WEAPONS "armas"
#define _LANG_VAULTS_LIST_SHIELDS "escudos"
#define _LANG_VAULTS_LIST_HERBS "hierbas"
#define _LANG_VAULTS_LIST_SKINS "pieles"
#define _LANG_VAULTS_LIST_OTHER "otros"
#define _LANG_VAULTS_LIST_HEADER "  Hay " + number_as_string(sizeof(all)) + " objeto"+((sizeof(all)>1)?"s":"")+ " en el baúl.\n"
#define _LANG_VAULTS_LIST_USING "  Se está utilizando un "+capacity+"% de la capacidad del baúl.\n"
#define _LANG_VAULTS_LIST_EMPTY "El baúl está vacío.\n"
