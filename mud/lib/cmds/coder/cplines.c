/** coded by Raskolnikov Spetember 1996 **/
#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;

void setup(){

    position = 1;
}



protected int cmd(string str, object me, string verb) {
    string *sfile, sfname, *dfile, dfname, lines;
    int sstart, send, slength;

    if (this_player(1) != this_player()) return 0;

    if(!str) {
	notify_fail("Usage : cplines source <start> <end> destination\n");
	return 0;
    }
    sscanf(str, "%s %d %d %s", sfname, sstart, send, dfname);
    sfile = get_files(sfname);
    dfile = get_files(dfname);
    if(!sizeof(sfile)) {
	notify_fail("Not a file : "+sfname+"\n");
	return 0;
    }
    if(!sizeof(dfile)) {
	notify_fail("Not a file : "+dfname+"\n");
	return 0;
    }
    if(sstart < 1) {
	notify_fail("Invalid start integer.\n");
	return 0;
    }
    slength = file_length(sfile[0]);
    if(send > slength) {
	notify_fail("End Integer too large.\n");
	return 0;
    }
    lines = read_file(sfile[0], sstart, send);
    if(!sizeof(lines)) {
	notify_fail("Invalid lines specified.\n");
	return 0;
    }
    write_file(dfile[0], lines, 0);
    tell_object(this_player(),"Ok.\n");
    return 1;
} /* cplines() */

string query_short_help() {
	return "Copia las líneas indicadas de un archivo en el final de otro.";
	/*
    return "This command copies lines from source starting at "
    "<start> and ending at <end> to the end of destination.\n\n";
	*/
}

string query_usage() {
    return "cplines source <start> <end> destination";
}
