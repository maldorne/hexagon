// Quick command made by Radix, Sept 1996
#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;

void setup()
{
    position = 0;
}

string query_usage()
{
    return "command";
}


protected int cmd(string str, object me, string verb) 
{
    string *mem;
	// string *mems2;
    int cols, i;
    string line = "";

    // mems2 = "";
    cols = (int)me->query_cols();
    mem = "/secure/net/ftpd"->query_connections();
    for (i = 0; i < cols; i++){
		line += "-";
	}
    /*
    mems2 = implode(mem, "\n");
    write("---------------------------------------\n");
    write("  Conexiones abiertas en el ftpd:\n");
    write("---------------------------------------\n");
    write(sprintf("  %*#-s\n", cols, mems2));
    write("---------------------------------------\n");
    */
    write(line + "\n");
    write("  Conexiones abiertas en el ftpd:\n");
    write(line + "\n");

	for (i = 0; i < sizeof(mem)-1; i+=2){
	    // write("   " + mem[i] + "\n");
	    write(sprintf("  %10s   %s\n", mem[i], mem[i+1]));
	}

    write(line + "\n");

    return 1;
}
