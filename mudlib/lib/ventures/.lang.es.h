
// shop.c

#define _LANG_SHOP_VERBS_LIST ({ "listar" })
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

#define _LANG_SHOP_LIST_SUMMARY "Tenemos a la venta " + query_num(sizeof(all), 20) + " objetos:\n\n"
#define _LANG_SHOP_LIST_JUST_ONE "Sólo nos queda " + list[name_list[i]][1]->query_numeral() + " " + name_list[i] 
#define _LANG_SHOP_LIST_LOTS "Una gran selección de " + (string)list[name_list[i]][1]->query_plural()
#define _LANG_SHOP_LIST_EMPTY "El almacén está vacío en estos momentos.\n"
