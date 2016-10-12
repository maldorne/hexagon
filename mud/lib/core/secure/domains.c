
int create_domain(string name, string lord) 
{
   string file;
  
   if(is_administrator(this_player()->query_name()))
   {
      notify_fail("Debes ser un Dios para crear un nuevo dominio.\n");
      return 0;
   }

   if(is_administrator(capitalize(name))) 
   {
      notify_fail("Nombre inválido...\n");
      return 0;
   }

   if(file_size("/d/"+name) != -1) 
   {
      notify_fail("El dominio ya existe (o es inválido).\n");
      return 0;
   }

   if(sizeof(explode(name, "/")) > 1) 
   {
      notify_fail("No es válido un dominio con el símbolo / dentro " +
          "de su nombre.\n");
      return 0;
   }

   if(!"/lib/core/login"->test_creator(lord)) 
   {
      notify_fail("El Lord debe existir y ser un creador.\n");
      return 0;
   }

   mkdir("/d/"+name);
   file = read_file("/room/dom/master.c");
   write_file("/d/"+name+"/master.c", "#define DOM_LORD \""+lord+"\"\n");
   write_file("/d/"+name+"/master.c", "#define DOMAIN \""+name+"\"\n"); 
   write_file("/d/"+name+"/master.c", file);
   
   file = read_file("/room/dom/common.c");
   write_file("/d/"+name+"/common.c", "#define DOM_TITLE \"el dominio de "+name+"\"\n");
   write_file("/d/"+name+"/common.c", "#define LORD \""+lord+"\"\n");
   write_file("/d/"+name+"/common.c", "#define DOM_NAME \""+name+"\"\n");
   write_file("/d/"+name+"/common.c", file);
   
   file = read_file("/room/dom/loader.c");
   write_file("/d/"+name+"/loader.c", "#define DOMAIN \""+name+"\"\n");
   write_file("/d/"+name+"/loader.c", file);
   
   save_object(SECURE_SAVE_PATH);
   write("Creado el dominio "+name+".\n");
   
   log_file("promotions", "El Dominio "+name+" ha sido creado para el Lord "+lord+
              " por: "+geteuid(previous_object())+"\n");
   
   return 1;
}

