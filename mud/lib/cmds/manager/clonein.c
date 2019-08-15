/** coded by Raskolnikov July 1996 **/
#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;

void setup(){

    position = 1;
}

protected int cmd(string str, object me, string verb) {
    object ob;
    string *path, dest, err;
    object *dest2;
    string patron, thane, mudlibber, lord, god;
int mov;

    patron = "/d/oficios/master"->query_patron(geteuid(me));
    thane = "/secure/thanes"->query_of(me->query_name());
    mudlibber = "/secure/mudlibber"->query_boo(me->query_name());
    lord = "/secure/lords"->query_boo(me->query_name());
    god = "/secure/gods"->query_boo(me->query_name());
if(!patron && !thane && !mudlibber && !lord && !god) {
        notify_fail("Clonein es para Patrones y superior unicamente.\n");
	return 0;
    }
    if (!str) {
        notify_fail("Uso: clonein fichero|destino.\n");
	return 0;
    }
    sscanf(str, "%s %s", path, dest);
    log_file("CLONEIN", me->query_cap_name()+" intento "
      "ejecutar clonein "+str+" "+ctime(time())+"\n");
    path = get_cfiles(path);
    if(!sizeof(path)) {
	notify_fail("Path invalido.\n");
	return 0;
    }
    if(file_size(path[0]) < 0 && file_size(path[0]+".c") < 0) {
        notify_fail("Fichero inexistente: "+path[0]+"\n");
	return 0;
    }
    dest = me->expand_nickname(dest);
    dest2 = me->wiz_present(dest, this_player());
    if(!sizeof(dest2)) {
	notify_fail("Destino invalido : "+dest+"\n");
	return 0;
    }
    ob = clone_object(path[0]);
    if (ob) {
	err = catch((mov = (int)ob->move(dest2[0])));
	me->handle_error(err, "move(dest)");
        tell_object(me,"Ok. Objeto "+file_name(ob)+" clonado y puesto "+
          "en "+dest+".\n");
        tell_room(environment(dest2[0]), this_player()->query_cap_name()+" "+
          "coge "+(ob->query_short()?ob->query_short():"algo")+" "+
          "de otra dimension y lo mete en "+capitalize(dest)+"\n", ({me}) );
    } else {
        tell_object(me,"Fallo al clonar.\n");
	return 1;
    }
    log_file("CLONEIN", me->query_cap_name()+" clono satisfactoriamente "
      +file_name(ob)+" en "+dest+" "+ctime(time())+"\n");
    return 1;
} /* clonein() */

string query_short_help() {
	return "Clona un objeto en el inventario de otro objeto ya existente.";
	/*
return "Este comando clona ficheros en destinos. Tambien se puede "
    "especificar un path entero. El destino puede ser cualquier objeto.\n"
    "Para Patrones y superior unicamente.\n\n"
    "Ejemplo: clonein /baseobs/armours/chainmail raskolnikov\n"
    "clonara una chainmail en Raskolnikov.\n";
	*/
}

string query_usage() {
    return "clonein fichero|destino";
}
