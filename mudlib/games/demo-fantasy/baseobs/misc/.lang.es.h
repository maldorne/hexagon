
#define _LANG_BASEOBS_ROPE "cuerda"
#define _LANG_BASEOBS_ROPE_LONG "Es una cuerda resistente y de buena longitud, " + \
      "capaz de soportar tu peso si la usas para escalar, y útil para atar " + \
      "cualquier cosa.\n"

#define _LANG_BASEOBS_TORCH "antorcha"
#define _LANG_BASEOBS_TORCH_LONG "Una antorcha pequeña para iluminar el camino, poco " + \
      "más que un palo de madera vieja con tiras de tela enrolladas en un extremo. " + \
      "Se enciende y se apaga con \"encender antorcha\" y \"apagar antorcha\".\n"

#define _LANG_BASEOBS_TORCH_AS_NEW "Está como nueva.\n"
#define _LANG_BASEOBS_TORCH_BURNT_OUT "Está completamente quemada.\n"
#define _LANG_BASEOBS_TORCH_LEFT(percentage) ("Aún le queda un " + percentage + "% de uso.\n")
#define _LANG_BASEOBS_TORCH_LIT " (%^YELLOW%^encendida%^RESET%^)"
#define _LANG_BASEOBS_TORCH_LIT_PLURAL " (%^YELLOW%^encendidas%^RESET%^)"

#define _LANG_BASEOBS_TORCH_VERBS_LIGHT ({ "encender" })
#define _LANG_BASEOBS_TORCH_VERBS_EXTINGUISH ({ "apagar" })

#define _LANG_BASEOBS_TORCH_WHAT ("¿" + capitalize(query_verb()) + " el qué?\n")
#define _LANG_BASEOBS_TORCH_HOLD_FIRST "Tienes que sostener la antorcha primero.\n"
#define _LANG_BASEOBS_TORCH_ALREADY_LIT "La antorcha ya está encendida.\n"
#define _LANG_BASEOBS_TORCH_ALREADY_OUT "La antorcha ya está apagada.\n"

#define _LANG_BASEOBS_TORCH_YOU_LIGHT "%^YELLOW%^Enciendes la antorcha.%^RESET%^\n"
#define _LANG_BASEOBS_TORCH_SOMEBODY_LIGHTS(who) ("%^YELLOW%^" + who->query_cap_name() + \
      " enciende una antorcha.%^RESET%^\n")
#define _LANG_BASEOBS_TORCH_YOU_EXTINGUISH "Apagas la antorcha.\n"
#define _LANG_BASEOBS_TORCH_SOMEBODY_EXTINGUISHES(who) (who->query_cap_name() + \
      " apaga una antorcha.\n")
#define _LANG_BASEOBS_TORCH_GOES_OUT "Tu antorcha se apaga.\n"
#define _LANG_BASEOBS_TORCH_SOMEBODYS_GOES_OUT(who) ("La antorcha de " + who->query_cap_name() + \
      " se apaga.\n")
