/* Who.c, once upon in /global/finger.c 
 * made splitted by Ducky, some changes by Baldrick and Aragorn.
 * Externalized by someone@RD.
 * Moved back to FR and Fr'ized by Baldrick. oct '95
 *
 * Color added by Radix ... splish splash
 * Damned Hide command, fixed for players and immortals, Radix 1996
 * For an overall ~12% increase, you get who sorted - Radix 1996
 *  - ie with 25 users, it's about 8k cpu cycles sorted, normally 7k
 *  Nevertheless, will improve greatly if sorted the FINAL string
 *  array, instead of doing the obs first
 *
 * Added race option, and cleaned up the 'logon the unknown's - Radix
 * Some translations made by Osucaru@Rl. Nov '97
 * Modificado para Who selectivo Spp@Rl Ene'98
 *
 * Folken 21/01/03, adaptado para Ciudad Capital, cambiado el funcionamiento
 *  generico de busqueda de datos. En lugar de hacer multiples filter_array
 *  (que cargan mucho mas la maquina), se realizan los menores recorridos posibles
 *  a la lista de objetos.
 * Añadida opcion who <mi nombre>, para eliminar el comando whoami (quiensoy)
 *  y añadir nuevo sistema de guild_joined, Folken 18/7/03
 *
 */

// #define FLAG_GAME "/d/flagomiq/handler/hand_flag"
// #include <standard.h>
#include <mud/cmd.h>
#include <living/races.h>

inherit CMD_BASE;

string *races = ({ "humano", "tresio", "velan", "aenur", });

void setup(){
    position = 0;
}

string query_usage()
{
    return "command";
}

// Sorted and race functions - Radix 1996
int compare_obs(object ob1, object ob2)
{
    string s1 = ob1->query_name();
    string s2 = ob2->query_name();
    if(s1 == s2) return 0;
    if(!s1 || !s2) return 1;
    if(s1 > s2) return 1;
    return -1;
}

int query_valid_race(string race)
{ return member_array(lower_case(race),races) != -1;}

int query_inmortal(object me)
{ return me->query_coder(); }

int no_inmortal(object me)
{ return !(me->query_coder()); }

// Añadida base_race, para nuevo sistema de subrazas, Folken 6/03
int compare_race(object ob, string str){ 
  return ((lower_case(ob->query_race_name()) == str) ||
          (lower_case(ob->query_base_race_name()) == str));
}

int remove_logon(object ob)
{  return (!(ob->query_short()=="logon")); }

string who_am_i(object player){
  mapping guilds = player->query_guild_joined();
  mapping jobs = player->query_job_joined();
  string ret;
  string * g_names = keys(guilds);
  string * j_names = keys(jobs);
  int i;

  ret = player->query_cap_name() + " " + 
        player->query_article() + " " +
        player->query_race_name() + ":\n";

  if (player->query_class_ob())
  {
   ret += "   Clase:\n     " + (player->query_class_ob())->query_short() + 
          " de nivel " + player->query_level() + ".\n";
  }

  for (i = 0; i < sizeof(g_names); i++)
  {
    if (i == 0)
      ret += "   Gremios:\n";
    if (!load_object(g_names[i])) continue;
    ret += "     " + capitalize(g_names[i]->query_short());
    ret += " de nivel "+guilds[g_names[i]][0]+".\n";
  }
  
  for (i = 0; i < sizeof(j_names); i++)
  {
    if (i == 0) 
      ret += "   Oficios:\n";
  	if (!load_object(j_names[i])) continue;
    ret += "     " + capitalize(j_names[i]->query_short());
    ret += " de nivel "+jobs[j_names[i]][0]+".\n";
  }

  // ret += ".\n";
  return ret;
}

string who_string(int width, int cre, string str){

    object *arr;
    object *arr_aux;
    int i, gente, desconectados;
    int creators, what;
    string s, tmp, nam;
    string imm, play, prt;
    string race;
    //  mixed ee;
    string ttl= "======] %^GREEN%^"+mud_name()+"%^RESET%^ [======";
    // Radix...

    if(!str) what = 0;
    else if( str == "programadores"[0..sizeof(str)-1]) what = 1;
    else if( str == "jugadores"[0..sizeof(str)-1]) what = 2;
    else if( str == this_player()->query_name() || 
      this_player()->expand_nickname(str) == this_player()->query_name()) 
        what = 4;
    else if( query_valid_race(str)) what = 3;
    else
	return "Sintaxis: who\n"+
	"          who programadores\n"+
	"          who jugadores\n"+
	"          who <raza|cultura>\n"+
	"          who <mi nombre>\n";


    if (what == 4){
      return who_am_i(this_player());
    }

    arr = users();
    arr = filter_array(arr,"remove_logon",this_object());

    if (what == 1)
    {
    	arr = filter_array(arr,"query_inmortal",this_object());
    }
    else if (what == 3) 
    {  
    	// Si queremos buscar unicamente los jugadores de una raza determinada,
    	//  filtramos segun la funcion query_race_name
    	// Tambien filtramos los inmos, Folken 21/01/03
	arr = filter_array(arr,"no_inmortal",this_object());
	arr = filter_array(arr,"compare_race",this_object(),lower_case(str));
	race = str;
	str = "jugadores";
    }
    else
    {
	arr = sort_array(arr, "compare_obs", this_object());
	race = "jugadores";
    }

    creators = gente = 0;
    s = imm = play = "";

    // sprintf's adaptados por Folken para RlMud, ahora en Ciudad Capital
    prt = "\n";
    //prt += sprintf("%|*'-'s\n", width+4, ttl);
    prt += sprintf("%|*'-'s\n", width+18, ttl);
    // prt +=  sprintf("%|*s\n", width, "Hoy es "+ctime(time(),3)+" en "+mud_name(), width);
    prt +=  sprintf("%|*s\n", width, "Hoy es "+ctime(time())+" en el mundo real", width);
    prt +=  sprintf("%|*s\n", width, ctime(time(), 3), width);

    // Recorremos toda la lista de jugadores
    // Si what == 1 tenemos unicamente los inmos
    // Si what == 2 tenemos unicamente los jugadores
    // Si what == 3 tenemos unicamente jugadores de una raza determinada
    // Si !what tenemos inmos + jugadores
    for (i = 0; i < sizeof(arr);i++) 
    {
        if(!(tmp = (string)arr[i]->query_short()))
		continue;

        if (arr[i]->query_coder())
        {
        if (!what || what==1)
        {
    	    // Here it denies to show "super invis" 
	    if(arr[i]->query_invis() > 1) 
	    {
		if (!this_player()->query_administrator()) 
		    continue;
	    }

	    if(cre && arr[i]->query_invis() == 1)
		tmp = "*"+(string)arr[i]->query_short()+"*";
	    if(cre && arr[i]->query_invis() == 2)
		tmp = "**"+(string)arr[i]->query_short()+"**";
	    if(!cre && arr[i]->query_invis() == 1) continue;
	    s = "";
	    nam = tmp;

	    if(cre && (tmp = arr[i]->query_in_editor()))
	    {
	        // Solo dioses, semis y alquis pueden ver esto
		if(stringp(tmp))
		    if( this_player()->query_god() || 
		        this_player()->query_demi() || 
		        this_player()->query_administrator() )
			s += " %^GREEN%^(Editando: "+(string)tmp+")%^RESET%^";

	    }

	    if (tmp = (string)arr[i]->query_gtitle())
		s += ", " + tmp;
	    if (tmp = (string)arr[i]->query_title())
		s += ", " + tmp;
	    if (tmp = arr[i]->query_extitle())
		s += " ("+tmp+")";
	    if (cre && arr[i]->query_property("ausente"))
		s+= " (%^WHITE%^%^BOLD%^Ausente%^RESET%^)";
	    if (query_idle(arr[i]) > 120)
		s += " (%^CYAN%^%^BOLD%^Inactivo: "+(query_idle(arr[i])/60)+"%^RESET%^)";

	    imm += sprintf(" %s%*-=s", nam, width, s)+"%^RESET%^\n";

	    if (!arr[i]->query_invis() || cre)
	    { 
            	creators++;
	    }
	}
	} // fin de if (query_coder)
	else // no es creator
	if (what != 1) // tenemos que mostrar los jugadores
	{
	  tmp = (string)arr[i]->query_short() +	" " +
	         (arr[i]->query_race_ob()?
	            arr[i]->query_race_ob()->query_race_gender_string(arr[i]):
	            STD_RACE->query_race_gender_string(arr[i]) );
	        // GENDER_TABLE->query_race_gender_string(arr[i]);

          if(!tmp)
            continue;
	  s = "";
	  nam = tmp;

	  if (arr[i]->query_property("guest")) 
	     s += " invitad"+((arr[i]->query_gender()==2)?"a":"o")+" en "+mud_name();
	  if (tmp = (string)arr[i]->query_extitle()) 
	     s += " (" + tmp + ")";
	  if (query_idle(arr[i]) > 120) 
	     s += " (%^GREEN%^Inactivo: " + (query_idle(arr[i])/60) + "%^RESET%^)";
	  
	  play += sprintf("          %*-=s", width-10, nam+s)+"\n"; 
	  gente++;
	}
    } // fin del for 

    if (!what || (what == 1)){
      // ------------------------ Inmortales ----------------------------
      ttl = "] %^BOLD%^WHITE%^Programadores%^RESET%^ ["; 
	
      // Solo si tenemos que mostrar algun inmo mostramos el resultado
      if(creators) {
      // prt += sprintf("%*'-'|s\n",width+1, ttl);
      prt += sprintf("%*'-'|s\n",width+24, ttl);
      prt += imm;  // imprime (mete en la cadena vamos :P) los inmortales
      }
    }
    // ------------------------ Jugadores ----------------------------
    if (what != 1){
       if (what == 3){
         ttl = "] %^BOLD%^WHITE%^"+capitalize(race)+"%^RESET%^ ["; 
       }
       else {
         ttl = "] %^BOLD%^WHITE%^Jugadores%^RESET%^ ["; 
       }
       if(gente){
         prt += sprintf("%*'-'|s\n",width+24, ttl);
         prt += play;
       }
    }

        // Si solo hemos buscado programadores
	if(what == 1){
	    if (!creators)
	       tmp = "> %^GREEN%^No hay programadores conectados%^RESET%^ <";
	    else   
	    tmp = "> %^GREEN%^Hay "+query_num(creators, 100)+" programador"+
	            (creators<2?"":"es")+" en "+mud_name()+"%^RESET%^ <";
	}
	// Si solo hemos buscado jugadores
	else if (what == 2 || what == 3)
	{
	    if (!gente)
		tmp = "> %^GREEN%^No hay jugadores conectados%^RESET%^ <";
            else if( cre && gente == 1)
	        tmp = "> %^GREEN%^Es el único que está conectado en "+mud_name()+"%^RESET%^ <";
            else if( gente == 1)
	        tmp = "> %^GREEN%^Eres el único que está conectado en "+mud_name()+"%^RESET%^ <";
            else
	        tmp = "> %^GREEN%^Hay "+query_num(gente, 100)+" jugador"+(gente<2?"":"es")+
	              " en "+mud_name()+"%^RESET%^ <";
        }
        // what == 0, buscamos tanto jugadores como programadores
        else
        {
	    // Caso imposible a menos que estemos en invis 2 y seamos los unicos
	    // conectados
	    if (!gente && !creators)
		tmp = "> %^GREEN%^No hay jugadores conectados%^RESET%^ <";
            
            else if( gente + creators == 1)
	        tmp = "> %^GREEN%^Eres el único que está conectado en "+mud_name()+"%^RESET%^ <";
	
            else if(!creators && gente)
	        tmp = "> %^GREEN%^Hay "+query_num(gente, 100)+" jugador"+(gente<2?"":"es")+
	              " en "+mud_name()+"%^RESET%^ <";
	
	    else if(!gente && creators)
	        tmp = "> %^GREEN%^Hay "+query_num(creators, 100)+" programador"+
	               (creators<2?"":"es")+" en "+mud_name()+"%^RESET%^ <";
	
	    else
	        tmp = "> %^GREEN%^Hay "+query_num(creators, 100)+" programador"+
	               (creators<2?"":"es")+" y "+query_num(gente, 100)+ " jugador"+
	               (gente<2?"":"es")+" en "+mud_name()+"%^RESET%^ <";
	}
	
	prt += sprintf("%*'-'|s\n", width+18, tmp);
	//     tmp ="> Total gente conectada: "+number+" <";
	//  prt += sprintf("%*'-'|s\n", width+0, tmp);


        // Añadido por Folken, para comprobar gente con la conexion caida
        // (no salen en users())
        if (this_player()->query_coder()){
          // Todos los objectos con enable_commands() llamado
          arr_aux = livings();
          // Quitamos los del who
          arr_aux -= users();
          desconectados = 0;
          for(i = 0; i < sizeof(arr_aux); i++){
            if (arr_aux[i]->query_player() && (arr_aux[i]->query_invis() != 2)){
              prt += sprintf("          %*-=s", width-10, capitalize(arr_aux[i]->query_name()))+"\n"; 
              desconectados++;
            }
          }
          if (desconectados > 0){
            prt += sprintf("%*'-'|s\n", width+18, 
                   ((desconectados == 1)?
                       ("> %^GREEN%^Un personaje con la conexión caída%^RESET%^ <"):
                       ("> %^GREEN%^" + capitalize(number_as_string(desconectados)))+" personajes con la conexión caída%^RESET%^ <"));
          }
        }
        


	return prt;
} /* who_string() */


int do_who(string str) 
{
  tell_object(this_player(),  
  who_string((int)this_object()->query_cols(),
              this_object()->query_coder(),str));
  return 1;
} /* do_who() */

protected int cmd(string str, object me, string verb) 
{
  me->do_efun_write(who_string((int)me->query_cols(),
         (int)me->query_coder(),str));
  me->set_trivial_action();

  return 1;

} /* cmd() */
