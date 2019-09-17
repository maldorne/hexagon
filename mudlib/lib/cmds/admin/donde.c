#include <standard.h>
#include <cmd.h>
inherit CMD_BASE;
 
protected int cmd(string input, object me, string verb)  
{
    string *doms;
    object *ob;
    int i;
    string str;
    int yep = 0;
    string *bass;
    ob = users();
    if(!input)
    {
       write("Uso: en <nombre de dominio>.\n");
       return 1;
    }
    for (i=0; i<sizeof(ob); i++) 
    {
        if ((string)ob[i]->query_gender_string() != "blue") 
        {
            str = file_name(environment(ob[i]));
            bass = explode (str, "/");
            doms = get_dir("/d/");
            if (member_array(input, doms) == -1) { return 0; }
            if (bass[1] == input) 
            {
                yep = 1;
                switch(ob[i]->query_grupo())
                 {
                 case 1: {
                         write ("%^CYAN%^%^BOLD%^"+ob[i]->query_cap_name()+ "%^RESET%^ en "+
                         environment(ob[i])->query_short()+ "   \n("+
                         file_name(environment(ob[i]))+ ")\n");
                         break;
                         }
                 case 2: {
                         write ("%^RED%^%^BOLD%^"+ob[i]->query_cap_name()+ "%^RESET%^ en "+
                         environment(ob[i])->query_short()+ "   \n("+
                         file_name(environment(ob[i]))+ ")\n");
                         break;
                         }
                 case 3: {
                         write ("%^BOLD%^%^BLACK%^"+ob[i]->query_cap_name()+ "%^RESET%^ en "+
                         environment(ob[i])->query_short()+ "   \n("+
                         file_name(environment(ob[i]))+ ")\n");
                         break;
                         }
                 default:{
                         write ("%^YELLOW%^"+ob[i]->query_cap_name()+ "%^RESET%^ en "+
                         environment(ob[i])->query_short()+ "   \n("+
                         file_name(environment(ob[i]))+ ")\n");
                         break;
                         }
            }
            }
        }
    }
    if (yep == 0) 
    {
        write (sprintf("No hay nadie en el dominio de %s\n", 
               capitalize(input)));
        return 1;
    }
    return 1;
}
