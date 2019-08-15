/** coded by Raskolnikov July 1996 **/
#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;

void setup(){

    position = 1;
}



protected int cmd(string str, object me, string verb) {
    string *filename, fname, text, wfile;
    int minint, numtimes, loop;

    if (this_player(1) != this_player()) return 0;

    if(!str) {
	notify_fail("Usage : cprange file | minint | numtimes\n");
	return 0;
    }
    sscanf(str, "%s %d %d", fname, minint, numtimes);
    filename = get_files(fname);
    if(!sizeof(filename)) {
	notify_fail("Not a file : "+fname+"\n");
	return 0;
    }
    if(minint < 0) {
	notify_fail("Invalid minimum integer\n");
	return 0;
    }
    if(!numtimes || numtimes < 1) {
	notify_fail("Invalid number of times specified\n");
	return 0;
    }
    if(numtimes > 100) numtimes = 100;
    text = read_file(filename[0]);
    if(!text) {
	notify_fail("Not a file : "+fname+"\n");
	return 0;
    }
    sscanf(filename[0], "%s.c", wfile);
    if((numtimes+minint) < 10) {
	for(loop=minint;loop<(numtimes+minint);loop++) {
	    write_file(wfile+"0"+loop+".c", text, 0);
	}
	tell_object(this_player(),"Ok.\n");
	return 1;
    }
    if(minint < 10 && numtimes >= 10) {
	for(loop=minint;loop<10;loop++) {
	    write_file(wfile+"0"+loop+".c", text, 0);
	}
	loop = 10;
	for(loop=10;loop<(numtimes+minint);loop++) {
	    write_file(wfile+loop+".c", text, 0);
	}
	tell_object(this_player(),"Ok.\n");
	return 1;
    }
    for(loop=minint;loop<(minint+numtimes);loop++) {
	write_file(wfile+loop+".c", text, 0);
    }
    tell_object(this_player(),"Ok.\n");
    return 1;
} /* cprange() */

string query_short_help() {
	return "Copia un archivo repetidas veces, con un contador que se incrementa en el nombre.";
	/*
    return "This command allows the user to copy a file numtimes "
    "starting at minint.  The files will be numbered in increasing "
    "order and will be copied into the directory of the parent "
    "file.  The max for numtimes is 100.\n\n"
    "Ex: cprange test.c 5 10\n"
    "will copy test.c 10 times starting with test05.c and "
    "ending with test14.c all placed in the directory of "
    "test.c\n\n"
    "Note: The command will only copy to .c files and "
    "the .c must be specified when entering the file name.\n";
*/
}

string query_usage() {
    return "cprange file | minint | numtimes";
}
