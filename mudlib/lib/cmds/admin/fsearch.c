/* comando para busqueda de definiciones de funciones en lpc
* by Kimmuriel '00
*/
#include <standard.h>
#include <cmd.h>
	inherit CMD_BASE;

void setup(){

	position = 1;
}
object creator; //el creador que ejecuta el comando

string *types,      //tipos de dato
	*special_types, //tipos de funcion
	*file_strings;  //archivo convertido a array de strings

int line_n,     //contador de numero de linea que estamos leyendo
	recursion,  //contador del numero de recursiones del metodo complete
	llaves,     //contador de la profundidad en que nos encontramos medida en llaves
	parentesis, //contador de la profundidad en parentesis
	inserts;    //contador del numero de lineas que hemos insertado, para que el numero
                //de linea que se le pase al inmortal no se vea afectado por las lineas
                //que insertemos al recortar lineas

void debug(string str)
{
	tell_object(TP,str+"\n"); 
	return;
}

string trim(string str)
{
	if(strlen(str) == 0)
		return str;
	if((str[strlen(str)-1] != 32 && str[strlen(str)-1] != 9) && 
		(str[0] != 32 && str[0] != 9))
		return str;
	while(str[strlen(str)-1] == 32 || str[strlen(str)-1] == 9)
	{
		if(strlen(str) == 1)
			return str;
		str = str[0..strlen(str)-2];
	}
	while(str[0] == 32 || str[0] == 9)
	{
		if(strlen(str) == 1)
			return str;
		str = str[1..strlen(str)-1];
	}	
	return str;
}

int in_comment()
{
	int comm1, comm2, comm3, comment;

	file_strings[line_n] = trim(file_strings[line_n]);  
	comm1 = strsrch(file_strings[line_n],"/*");
	comm2 = strsrch(file_strings[line_n],"*/");
	comm3 = strsrch(file_strings[line_n],"//");
	comment = 0;

	file_strings[line_n] = trim(file_strings[line_n]);  

	if(comm1 != -1)
	{
		if(comm2 != -1)
		{
			if(comm2 == (strlen(file_strings[line_n]) -2))
				comment = 2;
			file_strings[line_n] = replace_string(file_strings[line_n],
				extract(file_strings[line_n],comm1,(comm2+1)),"");
		}
		else
			return 1;

	}
	if(comm3 != -1)
		file_strings[line_n] = extract(file_strings[line_n],0,comm3-1);

	return comment;
}

int completed()
{
	/*posiciones de parentesis, comillas, apertura de llave y punto y coma*/
	int par1, par2, comilla1, comilla2, llave, pun_coma, llave2, ok, comment;

	comment = in_comment();
	if(comment == 1)
		return 0;
	if(comment == 2)
		return 1;

	/*rellenamos las posiciones*/
	ok = 0;

	par1 = strsrch(file_strings[line_n],"(");
	par2 = strsrch(file_strings[line_n],")");
	comilla1 = strsrch(file_strings[line_n],"\"");
	comilla2 = strsrch(file_strings[line_n],"\"",-1);
	llave = strsrch(file_strings[line_n],"{");
	llave2 = strsrch(file_strings[line_n],"}");
	pun_coma = strsrch(file_strings[line_n],";");
	/*si hay un punto y coma y no esta en una cadena y no es un for*/
	if(pun_coma != -1 &&
		!(pun_coma >par1 && pun_coma < par2) &&
		!(pun_coma > comilla1 && pun_coma <comilla2))
	{  
	/*si el punto y coma no esta al final de la cadena*/
		if(pun_coma < (strlen(file_strings[line_n])-1))
		{
		/*recortamos la cadena hasta dejarla con el ';' al final*/
			file_strings[line_n] = extract(file_strings[line_n],0,pun_coma);
		/*y pasamos el resto a la linea siguiente*/
			file_strings = slice_array(file_strings,0,line_n) +
				({extract(file_strings[line_n],pun_coma + 1,strlen(file_strings[line_n]))}) +
				slice_array(file_strings,(line_n+1),sizeof(file_strings)-1);
			inserts++;
		}
		ok = 1;
	}
	/*si hay una llave y no es de apertura de array*/   
	if(llave != -1 &&
		((strlen(file_strings[line_n]) >1 && file_strings[line_n][llave-1] != 40) ||    /* ( */
		(llave == 0)))
	{ 

	/*si la llave no esta al final de la cadena*/
		if(llave < (strlen(file_strings[line_n])-1))
		{
		/*recortamos la cadena hasta dejarla con la llave al final*/
			file_strings[line_n] = extract(file_strings[line_n],0,llave);
		/*y pasamos el resto a la linea siguiente*/
			file_strings = slice_array(file_strings,0,line_n) +
				({extract(file_strings[line_n],llave + 1,strlen(file_strings[line_n]))}) +
				slice_array(file_strings,(line_n+1),sizeof(file_strings)-1);
			inserts++;
		}
		ok = 1;
	}

	/*si hay una llave cerrando y no es de cerrar array*/   
	if(llave2 != -1 &&
		((strlen(file_strings[line_n]) >1 && file_strings[line_n][llave+1] != 41) ||    /* ( */
		(llave2 == 0)))
	{ 

	/*si la llave no esta al final de la cadena*/
		if(llave2 < (strlen(file_strings[line_n])-1))
		{
		/*recortamos la cadena hasta dejarla con la llave al final*/
			file_strings[line_n] = extract(file_strings[line_n],0,llave2);
		/*y pasamos el resto a la linea siguiente*/
			file_strings = slice_array(file_strings,0,line_n) +
				({extract(file_strings[line_n],llave2 + 1,strlen(file_strings[line_n]))}) +
				slice_array(file_strings,(line_n+1),sizeof(file_strings)-1);
			inserts++;
		}
		ok = 1;
	}   

	return ok;
}

/* este es el metodo que se encargara de buscar en la linea line_n si responde al patron
* que ha de seguir la declaracion de un metodo, si es asi, se lo dice al que llamo el 
* metodo
*/
void search_pattern(string str)
{
	string *aux, *aux2, *param, *par2;
	string tipos, parametros,useless, fin,par1;
	int ok, ok2, param_n, i, j;

	file_strings[line_n] = trim(file_strings[line_n]);  
	for(i = 0; i < strlen(file_strings[line_n]); i++)
	{
		if(file_strings[line_n][i] == 123 && file_strings[line_n][i-1] != 40)
		{
			llaves++;
			continue;
		}
		
		if(file_strings[line_n][i] == 125 && file_strings[line_n][i+1] != 41)
		{
			llaves--;   
		}
	}

	ok = sscanf(str,"%s(%s)%s",tipos, parametros, useless);

	fin = trim(useless);

	if(fin)
	{
		if(fin[0] != 123 &&
			fin[0] != 59)
			return;
		if(str == "dest_me();")
			debug(fin[0]+"<-----"+llaves);      
		if(fin[0] == 59 &&
			llaves)
			return;
	}

	if(!tipos )
		return;

	aux2 = explode(tipos," ");
	if(sizeof(aux2) && aux2[sizeof(aux2)-1] == "return")
		return;

	if(sizeof(aux2) > 1)
	{
		ok2 = 1;
		for(j = 0; j < (sizeof(aux2)-1); j++)
		{
			if(member_array(aux2[j], types) == -1 && member_array(aux2[j],special_types) == -1)
				ok2 = 0;
		}
		if(!ok2)
			return;
	}

	if(strlen(parametros))
	{ 
		if(strsrch(parametros,";") != -1 )
			return;

		if(strsrch(parametros,"{") != -1)
			return;

		if(strsrch(parametros,"[") != -1)
			return;

		if(strsrch(parametros,",") != -1)
			param = explode(parametros,",");
		else
			param = ({parametros});

		for(param_n = 0; param_n < sizeof(param); param_n++)
		{
			par1 = trim(param[param_n]);
			if(strsrch(par1," ") == -1)
				return;

			par2= explode(par1," ");
			if(sizeof(par2) != 2)
				return;

			if(member_array(par2[0],types) == -1)
				return;
		} 
	}
	aux = explode(trim(tipos)," ");

	if(!sizeof(aux) || sizeof(aux)>3)
		return;

	if(sizeof(aux) >= 2)
	{
		if(member_array(aux[(sizeof(aux)-2)],types) == -1 &&
			member_array(aux[(sizeof(aux)-2)],special_types) == -1)
			return;
	}

	if(sizeof(aux) == 3)
	{
		if(member_array(aux[0],special_types) == -1)
			return;
	}
	if(fin && fin[0] == 123)
		llaves = 1;
	tell_object(TP,(line_n+1-inserts)+": "+file_strings[line_n]+"\n");
	return;
}

protected int cmd(string name, object ob, string verb)
{
	string file;
	int j;
	types = ({"int","float","string","object","mapping","function","void","mixed","status"});
	special_types = ({"varargs","private","static","public","nomask"});
	creator = TP;
	inserts = 0;
	llaves = 0;
	
	if(!name)
	{
		debug("Sintaxis: fsearch <fichero.c>\n");
		return 1;
	}
	
	// Nueva opcion fsearch here, Folken
	if (name =="here")
	{
		name = file_name(environment(ob))+".c";
		file = read_file(name);
	}
	else
	{	
		file = read_file(name);
	
		// Incluimos directorio actual, Folken 
		if (!file)
		{
			 name = creator->query_path() + "/" + name;
			 file = read_file(name);
		}
	}

	if (!file)
	{
		notify_fail("El archivo "+name+" no existe, abortando ejecución.\n");
		return 0;   
	}
	
	tell_object(find_living("folken"), "name: "+name+"\n");


	file_strings = explode(file,"\n");

	for(line_n = 0; line_n < (sizeof(file_strings)-1); line_n++)
	{
		j = 1;
		file_strings[line_n] = trim(file_strings[line_n]); 

		if(file_strings[line_n] != "")
		{
			while(!completed())
			{
				if((line_n+j) > (sizeof(file_strings)-1))
					break;
				file_strings[line_n] = file_strings[line_n] +file_strings[line_n+j];
				file_strings[line_n+j] = "";
				j++;

			}
			search_pattern(file_strings[line_n]);
		}
	}
	return 1;
}
