

// coder_object

#define _LANG_CODER_OBJECT_DONT_EXIST "No existe tal objeto.\n"
#define _LANG_CODER_OBJECT_NO_OBJECTS "No hay archivos/objetos coincidentes.\n"
#define _LANG_CODER_OBJECT_HERE ({ "here", "aqui", "aquí" })
#define _LANG_CODER_OBJECT_CANNOT_LOAD "No se pudo cargar " + str + ".\n"
#define _LANG_CODER_OBJECT_NO_RSV "¡La habitación en el vacío ha desaparecido!\n"
#define _LANG_CODER_OBJECT_DO_NOT_UPDATE "¡Tu NO updateas eso! " + \
                "(no se pueden updatear objetos interactive()).\n"
#define _LANG_CODER_OBJECT_DO_NOT_UPDATE_CORE "¡Tu NO updateas eso! " + \
                "(no se pueden updatear objetos core.\n"
#define _LANG_CODER_OBJECT_NAME_BROKEN "Ups. Tu "+itemname+" acaba de romperse.\n"
#define _LANG_CODER_OBJECT_BROKEN "Ups. Algo que tenías acaba de romperse.\n"
#define _LANG_CODER_OBJECT_UPDATED "Actualizado: "
#define _LANG_CODER_DESTROYS (string)this_player()->query_cap_name() + " destruye " + \
          (shrt ? shrt : "algo") + ".\n"
#define _LANG_CODER_TRANS_THEY "\nEres transferido a algún lugar.\n\n"

// coder_path

#define _LANG_CURRENT_DIR "Directorio actual"
#define _LANG_WRONG_DIR "Directorio erróneo"
#define _LANG_AMBIGUOUS_DIRECTORY "Directorio ambiguo.\n"
#define _LANG_NONEXISTANT_DIRECTORY "Directorio inexistente.\n"

#define _LANG_CD_ERROR_WIZ "Error: call a wiz_present erróneo, cd imposible.\n"

#define _LANG_NO_HOMEDIR "No tienes homedir. Usa 'homedir <directorio>' para establecerlo.\n"
#define _LANG_HOMEDIR_SET "Homedir establecido en '"+home_dir+"'.\n"

// login_role

#define _LANG_LOGIN_ROLE_CLEAR_ACTIONS ({ "clear", "cls" })
#define _LANG_LOGIN_ROLE_SAVE_ACTIONS ({ "salvar", "grabar" })
#define _LANG_LOGIN_ROLE_QUIT_ACTIONS ({ "salir" })
