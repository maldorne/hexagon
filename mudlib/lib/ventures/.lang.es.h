
// shop.c

#define _LANG_SHOP_SIGN_DESC "Es un pequeño cartel de madera.\n"
#define _LANG_SHOP_SIGN_INFO "" + \
    "Información sobre las tiendas:                       \n" + \
    "  - 'listar' para ver qué objetos están a la venta.  \n" + \
    "  - 'examinar <objeto>' para ver las condiciones     \n" + \
    "     de los objetos en venta (precio, estado, etc.)  \n" + \
    "  - 'comprar <objeto>' para comprar uno de los       \n" + \
    "     objetos que estén en venta.                     \n"
#define _LANG_SHOP_SIGN_INFO_SELL "" + \
    "  - 'valorar <objeto>' para saber cuánto te ofrecen  \n" + \
    "     en la tienda por tus pertenencias.              \n" + \
    "  - 'vender <objeto>' para vender algo que poseas.   \n"
#define _LANG_SHOP_STORE_IN_USE "El almacén está siendo utilizado, inténtalo de nuevo en unos segundos.\n"
#define _LANG_SHOP_STORE_FULL "El almacén ya está lleno, no se puede guardar nada más.\n"
#define _LANG_SHOP_DEFAULT_SELL_MSGS ({ "Vendes $ob$ por $money$.\n", "$client$ vende $ob$.\n" })
#define _LANG_SHOP_DEFAULT_BUY_MSGS ({ "Compras $ob$ por $money$.\n", "$client$ compra $ob$.\n" })
#define _LANG_SHOP_DEFAULT_VALUE_MSG "$ob$ está valorado en $money$.\n"
#define _LANG_SHOP_DEFAULT_BROWSE_MSG "$ob$ cuesta $money$, descripción:\n$extra$"
#define _LANG_SHOP_NOTHING_TO_VERB "Nada que " + query_verb() + ".\n"
#define _LANG_SHOP_NOT_FOUND "No encuentro " + str + ".\n"

#define _LANG_SHOP_VERBS_LIST ({ "listar" })
#define _LANG_SHOP_LIST_SUMMARY "Tenemos a la venta " + query_num(sizeof(all), 20) + " objetos:\n\n"
#define _LANG_SHOP_LIST_JUST_ONE "Sólo nos queda " + list[name_list[i]][1]->query_numeral() + " " + name_list[i] 
#define _LANG_SHOP_LIST_LOTS "Una gran selección de " + (string)list[name_list[i]][1]->query_plural()
#define _LANG_SHOP_LIST_EMPTY "El almacén está vacío en estos momentos.\n"

#define _LANG_SHOP_VERBS_SELL ({ "vender" })
#define _LANG_SHOP_ONLY_SELL "Esta tienda no compra mercancías.\n"
#define _LANG_SHOP_SELL_SYNTAX "Sintaxis: vender <objetos>\n"
#define _LANG_SHOP_NOT_THIS_TYPE "Esta tienda no admite ese tipo de objetos.\n"
#define _LANG_SHOP_SELL_NOT_MORE "Parece que ya tienen suficientes " + obs[0]->query_plural() + " en el almacén.\n"
#define _LANG_SHOP_SELL_TOO_MANY "No puedes vender todos esos objetos a la vez.\n"
#define _LANG_SHOP_SELL_NOT_THIS_TYPE "No puedes vender tu "+obs[i]->short()+", no pueden " + \
          "venderse este tipo de objetos.\n"
#define _LANG_SHOP_SELL_CANNOT_SELL "No puedes vender "+query_multiple_short(cannot)+", tal vez lo lleves puesto, " + \
        "o simplemente sea algo que aquí no aceptan.\n"
#define _LANG_SHOP_SELL_NOTHING "Nada que vender.\n"

#define _LANG_SHOP_VERBS_BROWSE ({ "examinar" })
#define _LANG_SHOP_BROWSE_SYNTAX "Sintaxis: examinar <objetos>\n"

#define _LANG_SHOP_VERBS_VALUE ({ "valorar" })
#define _LANG_SHOP_VALUE_SYNTAX "Sintaxis: valorar <objetos>\n"

#define _LANG_SHOP_VERBS_BUY ({ "comprar" })
#define _LANG_SHOP_BUY_SYNTAX "Sintaxis: comprar <objetos>\n"
#define _LANG_SHOP_BUY_TOO_MANY "No puedes comprar todos esos objetos a la vez.\n"
#define _LANG_SHOP_BUY_CANNOT_MOVE "No puedes coger " + query_multiple_short(cannot) + ".\n"
#define _LANG_SHOP_BUY_TOO_EXPENSIVE capitalize(query_multiple_short(too_much)) + " cuesta demasiado.\n"
#define _LANG_SHOP_BUY_NOTHING "Nada que comprar.\n"

#define _LANG_SHOP_VERBS_LOGS ({ "logs" })
#define _LANG_SHOP_LOGS_SYNTAX "Sintaxis: logs\n"
#define _LANG_SHOP_LOGS_EMPTY "El registro de actividades está vacío.\n"
#define _LANG_SHOP_LOGS_NO_PERMISSION "No tienes permiso para ver el registro de actividades.\n"

// pub.c

#define _LANG_PUB_SIGN_DESC "Es un pequeño cartel de papel.\n"
#define _LANG_PUB_BUY_CMDS ({ "comprar", "pedir" })
#define _LANG_PUB_MENU_FOODS "Comida"
#define _LANG_PUB_MENU_DRINKS "Bebidas"
#define _LANG_PUB_MENU_ALCOHOL "Bebidas alcohólicas"
#define _LANG_PUB_MENU_INSTRUCTIONS "Utiliza 'pedir' o 'comprar' para encargar lo que te interese.\n"
#define _LANG_PUB_BUY_WHAT "¿" + capitalize(query_verb()) + " el qué?\n"
#define _LANG_PUB_BUY_SYNTAX query_verb() + " <algo> [para <jugador>]"
#define _LANG_PUB_BUY_PREPOSITION "para"
#define _LANG_BUY_FOR_NOT_HERE "Esa persona no parece estar aquí.\n"
#define _LANG_PUB_BUY_FOR_REJECT_MSG_ME forwho->query_cap_name() + " rechaza tu oferta.\n"
#define _LANG_PUB_BUY_FOR_REJECT_MSG_THEIR this_player()->query_cap_name() + " intenta invitarte a algo, pero lo rechazas.\n"
#define _LANG_PUB_BUY_FOR_REJECT_MSG_ENV this_player()->query_cap_name() + " intenta invitar a " + forwho->query_cap_name() + ", pero " + forwho->query_demonstrative() + " lo rechaza.\n"
#define _LANG_PUB_BUY_ITEM_NOT_FOUND "Lo siento, pero '"+str+"' no está disponible.\n"
#define _LANG_PUB_BUY_CANNOT_AFFORD "No puedes permitirte algo tan caro.\n"
#define _LANG_PUB_BUY_FOR_MSG_ME "Invitas a " + forwho->query_cap_name() + " a " + str + ".\n"
#define _LANG_PUB_BUY_FOR_MSG_THEIR this_player()->query_cap_name() + " te invita a " + str + ".\n"
#define _LANG_PUB_BUY_FOR_MSG_ENV this_player()->query_cap_name() + " invita a " + forwho->query_cap_name() + " a " + str + ".\n"

#define _LANG_PUB_DEFAULT_DRINK_MESS_ME "Te bebes un vaso de " + name
#define _LANG_PUB_DEFAULT_DRINK_MESS_OTHERS "se bebe un vaso de " + name
#define _LANG_PUB_DEFAULT_FOOD_MESS_ME "Te comes un plato de " + name
#define _LANG_PUB_DEFAULT_FOOD_MESS_OTHERS "se come un plato de " + name
