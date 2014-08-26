/*
 * Default language functions used by parse_command() in non -o mode
 *
 * Folken 02/06, intentando traducir los "all" y otras cosas...
 * 
 */

string *parse_command_id_list() {
  // return ({ "one", "thing" });
  // return ({ "cosa", "objeto" });
  // Por alguna razon esto no funciona como debiera
  return ({ "algo", "cosa", "objeto" });
} 

string *parse_command_plural_id_list() {
  // return ({ "ones", "things", "them" });
  return ({ "cosas", "objetos" });
} 

/*
string *parse_command_adjectiv_id_list() {
  return ({ "iffish" });
} 
*/

string *parse_command_prepos_list() {
  // return ({ "in", "on", "at", "along", "upon", "by", "under", "behind",
  //          "beside", "into", "onto", "inside", "within" });
  return ({ "en", "sobre", "encima", "debajo", "detras", "detrás", "dentro", "de" });
}

string parse_command_all_word() {
  // return "all";
  return "todo";
}
