/*
   /secure/master/create_dom_creator.c
   Originally bashed at Final Realms
   Fixed and tweaked by Radix : March 5, 1997
   Translated by neverbot, 4/2003
*/

#include <mud/secure.h>
#include <files/postal.h>

int create_creator(string s);
int demote_creator(string s);

int create_creator(string name) 
{
   string file;
   object player;
 
   seteuid(ROOT);

   if(!is_administrator(geteuid(previous_object())))
      return 0;

   if(!"/lib/core/login"->test_user(name))
      return 0;

   if(file_size("/home/"+name) != -1) 
   {
      notify_fail("El directorio para el creador ya existe o es inválido.\n");
      return 0;
   }

   mkdir("/home/"+name);
   file = read_file("/room/creator/workroom.c");
   write_file("/home/"+name+"/workroom.c", "#define CREATOR \""+name+"\"\n");
   write_file("/home/"+name+"/workroom.c", file);
   file = read_file("/room/creator/workroom2.c");
   write_file("/home/"+name+"/workroom2.c", "#define CREATOR \""+name+"\"\n");
   write_file("/home/"+name+"/workroom2.c", file);
   log_file("promotions", name+" promocionado a creador por "+
           geteuid(previous_object())+" - "+ctime(time())+".\n");
   write("Creador "+name+" promocionado.\n");

   player = find_player(name);

   if(player) 
   {
      player->set_creator(1);
      player->save();
      tell_object(player, "Has sido promocionado al rango de " +
         "creador por "+capitalize(geteuid(previous_object()))+".\n");
   } 
   else 
      write_file("/save/players/"+name[0..0]+"/"+name+".o","creator 1\n");
   return 1;
} 

int demote_creator(string str) 
{
   string name, reason;

   if(!is_administrator(geteuid(previous_object())))
      return 0;
   if(sscanf(str, "%s %s", name, reason) != 2) 
   {
      notify_fail("Sintaxis: demote <creador> <razón>.\n");
      return 0;
   }

   if(!"/secure/login"->test_creator(name))
      return 0;

   if(is_administrator(name))
   { 
      notify_fail("Los Lords no pueden ser demoteados.\n");
      return 0;
   }

   log_file("demote", name+" demoteado por "+
              previous_object()->query_cap_name()+" - "+ctime(time())+
              "\nRazón: "+reason+"\n");

   if(find_player(name)) 
   {
      find_player(name)->set_creator(0);
      find_player(name)->save();
      tell_object(find_player(name), "Has sido demoteado por "+
                capitalize(previous_object()->query_cap_name())+".\n");
      find_player(name)->really_quit();
   } 
   write_file("/save/players/"+name[0..0]+"/"+name+".o","creator 0\n");
   rename("/home/"+name,"/home/oldcreators/"+name);
   rename("/save/players/"+name[0..0]+"/"+name+".o","/home/oldcreators/"+name);
   write("Creador "+capitalize(name)+" demoteado.\n");
   write("Borrando su correo...");
   POSTAL_D->retire_user(name);
   write("Hecho.\n");
   write_file("/home/oldcreators/"+name+"/DEMOTED",
              capitalize(name)+" demoteado por "+
              previous_object()->query_cap_name()+" : "+ctime(time())+
              "\nRazón: "+reason+"\n\n");
   return 1;
} 

