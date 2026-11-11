// Strings for the location components under this directory.

// crossing.c -- a crossing that leads astray

#define _LANG_CROSSING_LOST "Desorientado, miras a tu alrededor y te vas hacia el "
#define _LANG_CROSSING_LOST_VERTICAL "Desorientado, miras a tu alrededor y te vas hacia "

// temple.c -- a place consecrated to a deity

#define _LANG_TEMPLE_CLOSED "Nadie oficia aquí. El altar está cubierto de " + \
    "polvo y las ofrendas que quedan se secaron hace tiempo.\n"

// rope.c -- a rope tied to something, to climb or descend by

#define _LANG_ROPE_TIE_VERBS ({ "atar" })
#define _LANG_ROPE_UNTIE_VERBS ({ "desatar" })
#define _LANG_ROPE_WORDS ({ "cuerda", "soga" })
#define _LANG_ROPE_ARTICLES ({ "la", "una", "esa", "esta" })
#define _LANG_ROPE_ARTICLE "el "
#define _LANG_ROPE_ARTICLE_CONTRACTED "al "
#define _LANG_ROPE_TO "a "
#define _LANG_ROPE_NO_ROPE "No llevas ninguna cuerda.\n"
#define _LANG_ROPE_ALREADY_TIED "Ya hay una cuerda atada aquí.\n"
#define _LANG_ROPE_NOTHING_TIED "No hay ninguna cuerda atada aquí.\n"
#define _LANG_ROPE_TIE_SELF "Atas la cuerda con firmeza"
#define _LANG_ROPE_TIE_OTHERS " ata una cuerda con firmeza"
#define _LANG_ROPE_UNTIE_SELF "Desatas la cuerda y la recoges.\n"
#define _LANG_ROPE_UNTIE_SELF_FLOOR "Desatas la cuerda, que cae al suelo.\n"
#define _LANG_ROPE_UNTIE_OTHERS " desata la cuerda.\n"
#define _LANG_ROPE_LINE "Una cuerda atada"
#define _LANG_ROPE_HANGS " cuelga hacia "
#define _LANG_ROPE_STRETCHES " se tiende en dirección "

// swamp.c -- mud that holds back whoever leaves

#define _LANG_SWAMP_STUCK "Los pies se te hunden en el fango y, por más que lo intentas, no consigues avanzar. Cuesta mucho andar por aquí.\n"

// acid.c -- a pool of acid that burns whoever stands in it

#define _LANG_ACID_ENTER "Al entrar te metes en un charco de ácido, que te causa un terrible dolor.\n"
#define _LANG_ACID_BURN "El ácido comienza a quemarte, provocándote un gran dolor. Será mejor que te pongas a salvo.\n"

// fire.c -- a burning location

#define _LANG_FIRE_ENTER "Las llamas te envuelven al entrar y te provocan terribles quemaduras.\n"
#define _LANG_FIRE_BURN "El fuego sigue quemándote, provocándote un gran dolor. Será mejor que te pongas a salvo.\n"
#define _LANG_FIRE_OUT "Las llamas se debilitan hasta apagarse del todo.\n"
#define _LANG_FIRE_DOUSED "Las llamas se debilitan, pero siguen ardiendo.\n"

// mount-shop.c -- un lugar que vende monturas
// Los textos usan las variables de la función que los escribe: label (el
// corto de la montura), price_text (su precio en palabras), number (su lugar
// en la lista), description (el primer párrafo de su descripción), max
// (cuántas monturas puede tener un jugador) y buyer.

#define _LANG_MOUNT_SHOP_LIST_VERBS ({ "listar", "lista", "list" })
#define _LANG_MOUNT_SHOP_EXAMINE_VERBS ({ "examinar", "examine", "browse" })
#define _LANG_MOUNT_SHOP_BUY_VERBS ({ "comprar", "buy" })
#define _LANG_MOUNT_SHOP_ARTICLES ({ "el", "la", "un", "una", "the", "a", "an" })

#define _LANG_MOUNT_SHOP_SIGN_DESC "Un cartel de madera, con las letras talladas a mano.\n"
#define _LANG_MOUNT_SHOP_SIGN_TEXT "" + \
    "Monturas a la venta:\n" + \
    "  - 'listar' para ver las monturas a la venta y sus precios.\n" + \
    "  - 'examinar <montura>' para verla de cerca y saber cuánto cuesta.\n" + \
    "  - 'comprar <montura>' para comprarla: desde ese momento es tuya.\n" + \
    "  - 'monturas' para ver las monturas que tienes y dónde están.\n"

#define _LANG_MOUNT_SHOP_EMPTY "Ahora mismo no hay nada a la venta aquí.\n"
#define _LANG_MOUNT_SHOP_LIST_HEADER "Monturas a la venta:\n"
#define _LANG_MOUNT_SHOP_LIST_LINE "  " + number + ". " + label + ": " + price_text + ".\n"
#define _LANG_MOUNT_SHOP_EXAMINE label + "\n" + description + "\nCuesta " + price_text + ".\n"
#define _LANG_MOUNT_SHOP_EXAMINE_CANNOT_AFFORD "No puedes permitírtela.\n"
#define _LANG_MOUNT_SHOP_BUY_WHAT "¿Qué montura quieres comprar?\n"
#define _LANG_MOUNT_SHOP_NOT_SOLD "Eso no está a la venta aquí.\n"
#define _LANG_MOUNT_SHOP_BUY_LIMIT "Ya tienes todas las monturas que puedes atender (" + max + ").\n"
#define _LANG_MOUNT_SHOP_BUY_CANNOT_AFFORD "No puedes permitírtela: " + label + " cuesta " + price_text + ".\n"
#define _LANG_MOUNT_SHOP_BUY_FAILED "Ahora mismo no se puede sacar la montura.\n"
#define _LANG_MOUNT_SHOP_BUY_ME "Pagas " + price_text + " y la montura es tuya: " + label + ".\n"
#define _LANG_MOUNT_SHOP_BUY_ROOM buyer->query_cap_name() + " compra una montura: " + label + ".\n"
