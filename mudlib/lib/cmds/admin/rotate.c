#include <standard.h>
#include <cmd.h>

inherit CMD_BASE;

// Comando para "rotar" o espejar zonas modificando todas las salidas de
// sus rooms, cambiando los nombres de las salidas
// Creado a partir de sar.c, Folken 02/2010

string query_short_help()
{
	return "Modifica una zona cambiando los nombres de las salidas de todas sus rooms.";
}

protected int cmd(string str, object me, string verb)
{
	int i;
	string *files;
	string s1, s2, content;
	int mirrorX = 0;
	int mirrorY = 0;
	
	if(!str)
	{
		notify_fail("Sintaxis: rotate [ -espejoX | -espejoY ] <archivo(s)>\n"
					"          (sin flags para rotar 45º en el sentido de las agujas del reloj)\n");
		return 0;
	}

	// Ejemplo: rotate -espejoX *.c
	if (sscanf(str, "%s %s", s1, s2) == 2)
	{
		if (s1 == "-espejoX")
		{
			write("Ok, espejando sobre el eje X.\n");
			mirrorX = 1;
		}
		else if (s1 == "-espejoY")
		{
			write("Ok, espejando sobre el eje Y.\n");
			mirrorY = 1;
		}
				
		str = s2;
	}
	
		
	files = (string *)get_files(str);
	
	if(!sizeof(files))
	{
		notify_fail("Archivo(s) " + str + " no encontrados.\n");
		return 0;
	}
		
	
	for (i = 0; i < sizeof(files); i++)
	{
		if (file_size(files[i]) <= 0) 
			continue;
			
		write("Buscando en el archivo '"+files[i]+"'.\n");
	
		content = read_file(files[i]);
		
		if (content)
		{
			if (mirrorX)
			{
				// Espejamos dejando un valor temporal para no machacar 
				// valores con las sucesivas sustituciones
				content = replace(content, "norte", "TMP_SUR");	
				content = replace(content, "sur", "TMP_NORTE");				
				content = replace(content, "sudeste", "TMP_NORESTE");				
				content = replace(content, "sudoeste", "TMP_NOROESTE");				
				content = replace(content, "noreste", "TMP_SUDESTE");				
				content = replace(content, "noroeste", "TMP_SUDOESTE");				
			}
			else if (mirrorY)
			{
				// Espejamos dejando un valor temporal para no machacar 
				// valores con las sucesivas sustituciones
				content = replace(content, "sudeste", "TMP_SUDOESTE");				
				content = replace(content, "sudoeste", "TMP_SUDESTE");				
				content = replace(content, "noreste", "TMP_NOROESTE");				
				content = replace(content, "noroeste", "TMP_NORESTE");				
				// Importante que sean los ultimos para no machacar los norOESTE, etc...
				content = replace(content, "oeste", "TMP_ESTE");				
				content = replace(content, "este", "TMP_OESTE");	
			}
			else
			{
				content = replace(content, "norte", "TMP_ESTE");	
				content = replace(content, "sur", "TMP_OESTE");				
				content = replace(content, "sudeste", "TMP_SUDOESTE");				
				content = replace(content, "sudoeste", "TMP_NOROESTE");				
				content = replace(content, "noreste", "TMP_SUDESTE");				
				content = replace(content, "noroeste", "TMP_NORESTE");				
				// Importante que sean los ultimos para no machacar los norOESTE, etc...
				content = replace(content, "oeste", "TMP_NORTE");				
				content = replace(content, "este", "TMP_SUR");	
			}

			// Dejamos los textos finales
			content = replace(content, "TMP_SUR", "sur");				
			content = replace(content, "TMP_NORTE", "norte");				
			content = replace(content, "TMP_OESTE", "oeste");				
			content = replace(content, "TMP_ESTE", "este");				
			content = replace(content, "TMP_NORESTE", "noreste");				
			content = replace(content, "TMP_NOROESTE", "noroeste");				
			content = replace(content, "TMP_SUDESTE", "sudeste");				
			content = replace(content, "TMP_SUDOESTE", "sudoeste");						
			
			// rm(files[i]);
			write_file(files[i], content, 1); // flag 1 == overwrite
		}
		else
			write("... fallo: El archivo no se ha podido leer.\n");
	}
	
	return 1;
}

