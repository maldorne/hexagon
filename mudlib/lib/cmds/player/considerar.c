/*
 * Considerar quitado de /std/commands para que ocupe menos mem, Errante.
 * Retocado por Folken, 20/04/2003, para adaptarlo a Cc.
 */

#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;

void setup(){
    position = 0;
}
string query_usage(){
    return "Sintaxis: considerar <quién>\n";
}

string query_short_help(){
    return "Sirve para valorar si alguien es más fuerte que tú.";
}

protected int cmd (string arg, object me, string verb)
{
    object *aux, *obs;
    int i, ok, level, dif;

    arg = me->expand_nickname(arg);

    if (!arg) {
	tell_object(me,query_usage());
	return 1;
    }
    arg = lower_case(arg);

    // Cambiado por Folken
    // if(search_for_all(arg,me))
    /*
    if (arg == "todo") {
	tell_object(me,"¿¡Considerar a todo el mundo!?\n");
	return 1;
    }
    */

    aux = find_match(arg, environment(me), 1);
    obs = ({ });
    for (i = 0; i < sizeof(aux); i++) 
	{
        /*
		if (aux[i] == me) {
		    tell_object(me,"¿Para qué quieres considerar atacarte a ti mismo?\n");
		    return 1;
		}
        // Ya se filtran con el flag de find_match
		// if (!aux[i]->query_hide_shadow())
    	    if ((!aux[i]->query_hide_shadow()) && (!aux[i]->query_hidden()))
			    obs += ({ aux[i] });
        */
        if ((aux[i]->query_player() && (aux[i] != me)) || aux[i]->query_npc())
		    obs += ({ aux[i] });
    }

    if (!sizeof(obs)) 
    {
		tell_object(me,"Necesitas considerar algo que exista.\n");
		return 1;
    }

    level = (int)me->query_level();

    for (i = 0; i < sizeof(obs); i++) 
	{
		// if (obs[i]->query_hide_shadow()) continue;
		// else
		// {
	    if((int)obs[i]->query_coder() )
	    {
			tell_object(me,"Atacar a "+obs[i]->query_numeral()+" programador"+
				((obs[i]->query_vocal()=="a")?"a":"")+" no es recomendable.\n");
			ok = 1;
			continue;
	    }
	    /*
	    if ( obs[i] == me )
	    {
		tell_object(me, "¿Por qué quieres considerar atacarte?\n");
		continue;
	    }
	    */
	    if (!living(obs[i]))
			continue;

	    ok = 1;
	    dif = (int)obs[i]->query_level() - level;
	    // dif += random(3) - random(3);

	    if(dif > 15 )
	    {
			tell_object(me,"¡Ni se te ocurra atacar a "+obs[i]->query_cap_name()+", podría acabar "
				"contigo sólo con mirarte!\n");
			continue;
	    }

	    if (dif > 10) 
	    {
			tell_object(me,"¡Estás loco si quieres atacar a "+
			  obs[i]->query_cap_name()+"!\n");
			continue;
	    }

	    if (dif < -10) 
	    {
			tell_object(me,"¡Podrías matar a "+obs[i]->query_cap_name()+" con un soplido!\n");
			continue;
	    }

	    if (level > 13) 
	    {
			// dif is now  -10...10
			dif += 10;   // dif is now between 0...20
			dif /= 2;    // dif is now between 0...10
	
			// Fix by Aragorn, capitalized the short
			tell_object(me, capitalize((string)obs[i]->query_cap_name())+
			  ({" es demasiado débil para molestarse en atacar.\n",
			    " es mucho más débil que tú.\n",
			    " es bastante más débil que tú.\n",
			    " es más débil que tú.\n",
			    " es un poco más débil que tú.\n",
			    " es aproximadamente igual que tú.\n",
			    " es un poco más fuerte que tú.\n",
			    " es más fuerte que tú.\n",
			    " es bastante más fuerte que tú.\n",
			    " es mucho más fuerte que tú.\n",
			    " es "+(obs[i]->query_article())+" más fuerte de la región.\n"
			  })[dif]);
	    } 
	    else 
	    {   // if level is less than 14
			dif += 10;  // 0...20
			dif /=  5;  // 0...4
	
			tell_object(me, capitalize((string)obs[i]->short())+
			  ({" es demasiado débil para molestarse en atacar.\n",
			    " es más débil que tú.\n",
			    " es aproximadamente igual que tú.\n",
			    " es más fuerte que tú.\n",
			    " es "+(obs[i]->query_article())+" más fuerte de la región.\n"
			  })[dif]);
	
	    }  // end else

	// } // end else

    } // end for

    if (!ok) 
    {
		tell_object(me, "No puedes considerar eso.\n");
		return 1;
    }
    return 1;
}
