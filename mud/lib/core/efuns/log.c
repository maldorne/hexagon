
#include <files/log.h>
#include <mud/secure.h>

// Code by Aragorn, who likes his log files...

static nomask void log_file( string file, string text )
{
  string logfile;

  if ( !strlen( file ) || !strlen( text ) ) 
    return; // syntax error

  if ( -1 != strsrch( file, ".." ) ) 
    return; // security error
  
  logfile = LOG_DIR + file; // would 'sprintf' be much better here?
  
  seteuid(ROOT);

  if ( file_size( logfile ) > MAX_LOG_SIZE )
    rename( logfile, logfile + ".old" );
  
  write_file( logfile, text );
  
  seteuid("");
}

// log_attacks.c
// Logs all attacks on a player from a player. with level.
// This is used to look after playerkillers.
// Baldrick, feb '95

// void log_attack(string attacker, int att_level, string victim, int vic_level)
// {
//   log_file("attacks", "["+ctime(time(), 4)+"] " + attacker + " with level: " + att_level + " attacked " +
//          victim + " with level: " + vic_level +"\n");
// } 
