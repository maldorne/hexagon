
/*
  Nanvaent Industries International

  LPC: Search and Replace

  Written by  : Weazel@Nanvaent Nov 92
  Modified by : Bill@Nanvaent Jan 93
  Modified by : Bill@Nanvaent+@Discworld Mar 93
  Modified by : Dimmak@RlMud Apr 99 para poder hacer cosas mas complejas
  Modified by : neverbot@CcMud Feb 10 para admitir un modo 'silent' para modificar grandes listas de archivos
*/

#include <mud/cmd.h>


inherit CMD_BASE;

void setup()
{
	position = 1;
}

string query_short_help()
{
	return "Reemplaza un texto por otro en uno o varios archivos.";
}

static int cmd(string str, object me, string verb)
{
	int i;
	string *files;
	string s1, s2, s3, s4, s5;
	int silent;

  silent = 0;
	
	// if (sscanf(str, s4+"%s"+s4+" "+s4+"%s"+s4+" %s", s1, s2, s3) != 3)
	if (!stringp(str) || (sscanf(str, "$%s$ $%s$ %s", s1, s2, s3) != 3))
	{
		if ((sscanf(str, "%s $%s$ $%s$ %s", s5, s1, s2, s3) == 4) && (s5 == "-s"))
		{
			silent = 1;
		}
		else
		{
			notify_fail("Sintaxis: sar [-s] $texto a buscar$ $texto nuevo$ <archivo(s)>\n");
			return 0;
		}
	}
	
	s4 = str[0..0];
	
	// No hacemos log de lo que se haga con el sar
	// log_file("SAR",me->query_name()+" : "+str+"\n");
	files = (string *)get_files(s3);
	
	if (!sizeof(files))
	{
		notify_fail("Archivo(s) " + s3 + " no encontrados.\n");
		return 0;
	}
	
	if (silent)
		write("Buscando en los archivos...\n");
	
	for (i = 0; i < sizeof(files); i++)
	{
		if (file_size(files[i]) <= 0) 
			continue;
			
		if (!silent)
			write("Buscando en el archivo '"+files[i]+"'.\n");
			
		s4 = read_file(files[i]);
		
		if (s4)
		{
			s4 = replace(s4, s1, s2);
			rm(files[i]);
			write_file(files[i], s4);
		}
		else
			write("... fallo: El archivo '"+files[i]+"' no se ha podido leer.\n");
	}
	
	if (silent)
		write("\t... terminado.\n");
		
	return 1;
}

