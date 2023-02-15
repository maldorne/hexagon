// backup.c -- Automagically creates a backup file
// Code originally by Mordred, around 1 May 96
// 06 May 96  Agamemnon  Added file informs
//            Also added EDIT logging
//            Thirdly, added support for multiple files
//            Finally cleaned it up and added more descriptive fails
// 06 June 96 Agamemnon  Changed to use the "cp" efun instead of read
//            and write_file... (faster, and less chance
//            to barf and die)
// 25 Abril 03 neverbot Traducido y compatible con nuevo ctime()

#include <mud/cmd.h>

#define COL ""

inherit CMD_BASE ;

void setup()
{
  position = 1 ;
}

string get_date() 
{
  string date;
  // int i;
  date = ctime( time(), 2 ) ;
  /*
  i = member_array( date[ 3..5 ], ({ "Ene", "Feb", "Mar", "Abr", "May",
    "Jun", "Jul", "Ago", "Sep", "Oct", "Nov", "Dic" }) ) ;
  i++ ;
  date = date[ 0..1 ] + i + date[ 7..8 ] ;
  if ( strlen( date ) == 5 ) date = date[ 0..1 ] + "0" + date[2..4];

  date = replace( date, " ", "0" );
  */
  return date;
}

static int cmd( string str, object me, string verb)
{
  string *files, orig, backup, date;
  int i, ver;

  if ( this_player( 0 ) != this_player( 1 ) )
  {
    return 0 ;
  }

  if ( !strlen(str) )
  {
    notify_fail( "Sintaxis: backup <archivo>.\n" ) ;
    return 0;
  }

  date = get_date();
  files = get_files( str );

  if ( !sizeof( files ) ) 
  {
    // write( "Cannot find file(s): "+str+".\n" );
    write( "No se puede encontrar el archivo: "+str+".\n" );
    return 1;
  }

  date = get_date();

  for ( i=0; i<sizeof( files ); i++ ) 
  {
    orig = files[i];
    if ( file_size( orig ) == -1 ) 
    {
      //write( "Cannot back up file "+orig+
      //     ": No such file or directory.\n" );
      write( "No se puede crear el backup de "+orig+
           ".\nNo existe el archivo o directorio.\n" );
      continue;
    }

    if ( file_size( orig ) == -2 ) 
    {
      // write( "Cannot backup file "+orig+
      //     ": Is a directory.\n" );
      write( "No se puede crear el backup de "+orig+
           ".\nEs un directorio.\n" );
      continue;
    }

    backup = orig + "." + date + "."+this_player()->query_name();

    if ( file_size( backup ) == -2 ) 
    {
      write( "No se puede crear el backup de "+orig+
           ", ya existe (como directorio).\n");
      // write( "Cannot backup to "+orig+
      //    ": Is a directory.\n" );
      continue;
    }
    
    if ( file_size( backup ) != -1 ) 
    {
      // Arreglado problema con las versiones, funcionaba mal
      // neverbot 4/03
      ver = 2;
      while( file_size( backup + ".v" + ver ) > 0 ) ver++;
      backup += ".v" + ver;
    }

    if ( cp( orig, backup ) ) 
    {
      //write( COL + "File "+orig+" backed up to "+ backup+"\n" );
      write( COL + "Archivo "+orig+" copiado como "+ backup+"\n");

    } 
    else 
    {
      // write( "Unable to write file : "+backup+"\n" );
      write( "Imposible crear archivo: "+backup+"\n" );
    }
  }
  // Añadido neverbot 4/03
  me->set_trivial_action();
  return 1 ;
}

