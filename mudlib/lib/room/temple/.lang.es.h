// Strings for the temple room and the temple location component, which share
// temple-actions.c. $god$ is a deity's name, $noun$ dios or diosa.

#define _LANG_TEMPLE_CMDS_WORSHIP ({ "adorar", "worship" })
#define _LANG_TEMPLE_CMDS_OFFER ({ "ofrecer", "ofrendar", "offer" })
#define _LANG_TEMPLE_YES ({ "s", "si", "sí" })
#define _LANG_TEMPLE_COIN_WORDS ({ "monedas", "moneda", "coins", "coin" })

#define _LANG_TEMPLE_NO_GOD "Este lugar no está consagrado a ningún dios.\n"
#define _LANG_TEMPLE_CLOSED_NOW "Nadie oficia ya en este templo: los que gobiernan " + \
      "estas tierras no consienten el culto a $god$.\n"
#define _LANG_TEMPLE_ALREADY_YOURS "Ya perteneces a $god$, y para siempre.\n"
#define _LANG_TEMPLE_OTHER_GOD "Tu $noun$, $god$, se enfurecería si honrases a otro. " + \
      "Le perteneces para siempre.\n"
#define _LANG_TEMPLE_NOT_FOLLOWER "Solo los fieles de $god$ pueden hacerle ofrendas.\n"
#define _LANG_TEMPLE_WORSHIP_CONFIRM "Adorar a $god$ es para siempre: nunca podrás " + \
      "cambiar de $noun$. ¿Estás seguro? (s/n) "
#define _LANG_TEMPLE_WORSHIP_CANCELLED "Te lo piensas mejor y te levantas.\n"
#define _LANG_TEMPLE_WORSHIP_FAILED "$god$ no te acepta entre sus fieles.\n"
#define _LANG_TEMPLE_WORSHIP_DONE "Te postras ante el altar y consagras tu vida a $god$. " + \
      "Desde hoy le perteneces, y puedes rezarle donde se le rinde culto.\n"
#define _LANG_TEMPLE_WORSHIP_ROOM "$who$ se postra ante el altar y consagra su vida a $god$.\n"

#define _LANG_TEMPLE_OFFER_USAGE "Sintaxis: ofrecer <objeto | cuerpo> | ofrecer <cantidad> monedas\n"
#define _LANG_TEMPLE_OFFER_NOTHING "No llevas nada parecido, ni hay aquí un cuerpo así.\n"
#define _LANG_TEMPLE_OFFER_IN_USE "Tendrás que dejar de usarlo antes de ofrecerlo.\n"
#define _LANG_TEMPLE_OFFER_REFUSED "$god$ no quiere eso. Te lo guardas.\n"
#define _LANG_TEMPLE_OFFER_REFUSED_HERE "$god$ no quiere eso.\n"
#define _LANG_TEMPLE_OFFER_NO_COINS "$god$ no quiere tus monedas.\n"
#define _LANG_TEMPLE_OFFER_NO_MONEY "No llevas tanto dinero encima.\n"
#define _LANG_TEMPLE_OFFER_COINS_ROOM "$who$ deja unas monedas sobre el altar, que desaparecen.\n"
#define _LANG_TEMPLE_OFFER_ITEM_ROOM "$who$ deja $item$ sobre el altar, y desaparece.\n"

#define _LANG_TEMPLE_SIGN_LONG "Es una placa de mármol junto a la entrada.\n"
#define _LANG_TEMPLE_ROOM_SHORT "Templo"
#define _LANG_TEMPLE_ROOM_LONG "Una sala de techos altos y luz tenue, con un altar al fondo. " + \
      "Aquí se adora a un dios y se le hacen ofrendas.\n"
