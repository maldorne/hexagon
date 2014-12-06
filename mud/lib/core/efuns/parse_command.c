/*
 * Default language functions used by parse_command() in non -o mode
 *
 * neverbot 02/06, trying to translate the "all" and other things
 * 
 */

string *parse_command_id_list() {
  // return ({ "one", "thing" });
  // return ({ "cosa", "objeto" });
  // this doest not work as it should
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
