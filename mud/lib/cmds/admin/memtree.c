// A inherit tree with memory_info
#include <cmd.h>
inherit CMD_BASE;

private object SearchObject( string name ) 
{
  if ( !name ) 
	return 0;
  catch( name -> trams() );
  return find_object( name );
}

private void WriteObject( string str, object ob ) 
{
  if ( ob ) 
  {
    string *Inherits = inherit_list( ob );
    int i = sizeof( Inherits );
	object Tmp;
	int Mem;
    
    while ( i-- ) 
    {
		Tmp = SearchObject( Inherits[ i ] );
		if (!Tmp)
			continue;
				
		Mem = memory_info( Tmp );
		printf( "%-45s %6d Bytes\n", str + Inherits[ i ], Mem ); 
		WriteObject( "  " + str , SearchObject( Inherits[ i ] ) );
    }
  }
}

int cmd(string str, object me, string verb) 
{
	object Tmp;
	
	if (!str || (str == ""))
	{
		notify_fail("Sintaxis: memtree <nombre de archivo>\n");
		return 0;
	}
	
	if ( str && (str != "")) 
	{
		Tmp = SearchObject( str );
		if (!Tmp)
		{
			notify_fail("No se ha encontrado '" + str + "'.\n");
			return 0;
		}
		printf( "%-45s %6d Bytes\n", str, 
		       memory_info( Tmp ) );
		WriteObject( "  ", Tmp );
	}
	
	return 1;
}
