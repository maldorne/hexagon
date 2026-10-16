// Strings for the location components under this directory.

// temple.c -- a place consecrated to a deity

#define _LANG_TEMPLE_CLOSED "Nadie oficia aquí. El altar está cubierto de " + \
    "polvo y las ofrendas que quedan se secaron hace tiempo.\n"

// jail.c -- a cell

#define _LANG_JAIL_CMDS_BRIBE ({ "sobornar", "bribe" })
#define _LANG_JAIL_LOCKED "La puerta de la celda sigue cerrada: te queda cumplir $time$ de condena.\n"
#define _LANG_JAIL_NOT_PRISONER "Nadie te ha encerrado aquí, no tienes a quién sobornar.\n"
#define _LANG_JAIL_PASSED_OUT "Estás inconsciente, no puedes hacer nada.\n"
#define _LANG_JAIL_BRIBE_NO_MONEY "No llevas encima el dinero que el carcelero espera: $cost$ monedas.\n"
#define _LANG_JAIL_BRIBE_OK "Pasas $cost$ monedas al carcelero entre los barrotes. Mira a otro lado y la puerta queda abierta.\n"
#define _LANG_JAIL_BRIBE_FAILED "El carcelero se guarda las monedas, sonríe y grita que un preso ha intentado sobornarlo. Tu condena se alarga.\n"

// fines.c -- the office where fines are paid

#define _LANG_FINES_CMDS_OWED ({ "perdon", "perdón", "multas", "fines", "owe" })
#define _LANG_FINES_CMDS_PAY ({ "pagar", "pay" })
#define _LANG_FINES_CMDS_BAIL ({ "fianza", "bail" })
#define _LANG_FINES_NO_LAW "Aquí no rige ninguna ley, nadie te cobrará nada.\n"
#define _LANG_FINES_OWE_NOTHING "El escribiente revisa sus libros: no debes nada.\n"
#define _LANG_FINES_OWE "El escribiente revisa sus libros: acumulas $points$ puntos de delito, y la multa son $fine$ monedas. Puedes saldarla con 'pagar'.\n"
#define _LANG_FINES_NO_MONEY "No llevas dinero suficiente: la multa son $fine$ monedas.\n"
#define _LANG_FINES_PAID "Pagas $fine$ monedas. El escribiente tacha tu nombre del libro: estás limpio.\n"
#define _LANG_FINES_BAIL_USAGE "Sintaxis: fianza <jugador>\n"
#define _LANG_FINES_BAIL_NOBODY "No encuentras a nadie con ese nombre.\n"
#define _LANG_FINES_BAIL_NOT_JAILED "Esa persona no cumple condena aquí.\n"
#define _LANG_FINES_BAIL_NOTHING "Esa persona ya no debía nada: queda libre.\n"
#define _LANG_FINES_BAIL_NO_MONEY "No llevas dinero suficiente: la fianza son $fine$ monedas.\n"
#define _LANG_FINES_BAIL_PAID "Pagas $fine$ monedas de fianza por $who$. Quedará libre de inmediato.\n"
