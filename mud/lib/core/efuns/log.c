// Code by Aragorn, who likes his log files...
#define MAX_LOG_SIZE 200000

// TODO log_file

void log_file( string file, string text )
{
    // string logfile;

    // if( !stringp( file ) || !stringp( text ) ) 
    //     return; // syntax error
    // if( -1 != strsrch( file, ".." ) ) 
    //     return; // security error
    
    // logfile = LOG_DIR + file; // would 'sprintf' be much better here?
    // seteuid("Root");
    // if( file_size( logfile ) > MAX_LOG_SIZE )
    //     rename( logfile, logfile + ".old" );
    // write_file( logfile, text );
    
    // seteuid(0);
}


/* log_attacks.c
 * Logs all attacks on a player from a player. with level.
 * This is used to look after playerkillers.
 * Baldrick, feb '95
 */

void log_attack(string attacker, int att_level, string victim, int vic_level)
{
    log_file("attacks", "["+ctime(time(), 4)+"] " + attacker + " con nivel: " + att_level + " atacó a " +
           victim + " con nivel: " + vic_level +"\n");
} 
