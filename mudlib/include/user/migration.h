
// Aqui se define cual es la version actual de la ficha que estan usando los jugadores
// Al hacer login, se comprueba tu query_last_migration() contra este numero.
// Si es menor, se actualiza tu ficha.

// Por lo tanto: NO TOQUES ESTE NUMERO a menos que sepas lo que estas haciendo, 
// puedes cargarte las fichas de todos los jugadores

#define LAST_MIGRATION 2

#define MIGRATIONS_PATH "/lib/user/migrations/"
#define BASE_MIGRATION MIGRATIONS_PATH+"base_migration"

#define LOG_MIGRATIONS "migrations"
