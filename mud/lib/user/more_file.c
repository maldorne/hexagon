/*
 * inheritable more object. Just say bing!
 * The wonder new improved morer, based loosely on a more/less object
 * written by alice(ember). But this one is written by pinkfish like
 * most of the rest of this mud, bar what is not of course ;)
 */
// Eliminados archivos nroff, neverbot 06/2010

static int fsize2,
           topl2,
           search_flag,
           botl;
static mixed mark_data;
static string fname,
              *mark_name,
              *filenames,
              finish_func2,
              stat_line2,
              last_search2;

#define ROWS  ((int)this_player()->query_rows()-4) 

void create() 
{

}

void display_file() 
{
  string scrn;
  botl = topl2 + ROWS;
  if (botl > fsize2) 
    botl = fsize2;
  // Radix
  // cat(fname,topl2,ROWS);
  scrn = read_file(fname,topl2,ROWS);
  if(scrn)
    tell_object(this_object(),sprintf("%s",scrn));
} /* display_file() */

void get_next_filename() 
{
  // string str;

  fname = filenames[0];
  // Fix by Wonderflug, 96.  Check file_size > 0 to filter dirs out
  // cause this crashes file_length.
  fsize2 = 0;
  // if(file_size(fname) > 0)
  fsize2 = file_length(fname);
  
  if (fsize2 == 0) 
  {
    write("Fichero vacío.\n");
    if(sizeof(filenames) > 1) 
    {
      filenames = filenames[1..sizeof(filenames) - 1];
      get_next_filename();
      return;
    }
    else 
      return;
  }
  
  if (fsize2 == -1) 
  {
    write("El fichero no existe.\n");
    if(sizeof(filenames) > 1) 
    {
      filenames = filenames[1..sizeof(filenames) - 1];
      get_next_filename();
      return;
    }
    else 
      return;
  }
  
  if (fsize2 == -2) 
  {
    write("No puedes hacer un more de un directorio.\n");
    if(sizeof(filenames) > 1) 
    {
      filenames = filenames[1..sizeof(filenames)-1];
      get_next_filename();
      return;
    }
    else 
      return;
  }
  
  topl2 = 1;
  if (fsize2 < ROWS)
    botl = fsize2;
  else
    botl = ROWS;
  
  if(sizeof(filenames) > 1) 
    filenames = filenames[1..sizeof(filenames) - 1];
  else 
    filenames = ({ });

  write("\n");
  
  /*
  str = read_file(fname, 1, 1);
  
  if (str[0] == '.') 
  {
    string s2;

    // Nroff file...  do horrible things!  Turn into a mushroom!  Blow up
    // all the hairy crater eating womble doovalacies...
    // In the case of a nroff file we push the file through nroff then shove it
    // through more_string...
    str = "/tmp/nroffed_file";
    if (NROFF_HAND->create_nroff(fname, "/tmp/nroffed_file")) 
    {
      s2 = NROFF_HAND->cat_file("/tmp/nroffed_file");
      rm("/tmp/nroffed_file.o");
      if (s2) 
      {
        this_object()->more_string(s2, fname);
        fsize2 = ROWS-4;
        return ;
      }
    }
  }
  */
  
  display_file();

} /* get_next_file_name() */

string *expand_path(string s) 
{
  string *s1;

  s1 = (string *)get_files(s);
  if (!s1)
    return ({ s });
  return s1;
} /* expand_path() */

int set_mark(string s)
 {
  int i;
  if (!mark_name) {
    mark_name = ({ });
    mark_data = ({ });
  }
  if ((i=member_array(s,mark_name))!=-1)
    mark_data[i] = ({ fname, topl2 });
  else {
    mark_name += ({ s });
    mark_data += ({ ({ fname, topl2 }) });
  }
  return 1;
} /* set_mark() */

void status_line() 
{
  string *frog;
  string s;
  int i, percentage;

  if (!stat_line2)
    stat_line2 = "$N Desde $T hasta $B de $S ($%%) - h para ayuda. ";
  s = "";
  frog = explode(stat_line2, "$");
  for (i=0;i<sizeof(frog);i++) {
    if (frog[i] == "") {
      s += "$";
      i ++;
    } else switch (frog[i][0]) {
      case 'N' :
          s += fname+frog[i][1..1000];
          break;
      case 'T' :
          s += topl2+frog[i][1..1000];
          break;
      case 'B' :
          if (botl > fsize2)
            s += fsize2+frog[i][1..100];
          else
            s += botl+frog[i][1..100];
          break;
      case '%' : {
          percentage = (botl*100)/fsize2;
          if(percentage > 100) percentage = 100;
          s += percentage+frog[i][1..100];
          break;
      }
      case 'S' :
         s += fsize2+frog[i][1..1000];
         break;
    }
  }
  tell_object(this_object(), s);
  return;
  write(fname+" Desde "+topl2+" hasta "+botl+" de "+fsize2+"  ("+
       ((botl*100)/fsize2)+"%)"+" - h para ayuda. ");
} /* status_line() */

void next_page(string str) 
{
  int num,
      noargs,
      i,j,k,
      redraw;
  string s1, *s3, s4, s5;

  if (!str)
    str = "";

  if (sscanf(str,"%d%s", num, str)!=2)
    noargs = 1;
  s1 = extract(str,1);
  str = extract(str,0,0);
/* case statements WEEEEEE */
  switch(str) {
  case "" :
  case " " :
  case "f" :
/* go on a number of pages... */
    if (noargs)
      num = 1;
    topl2 += (ROWS)*num;
    redraw = 1;
   break;
  case "q" :
    write("Ok\n");
    return ;
  case "/" :
/* sigh */
    j = topl2+4;
    redraw = 1;
    if (s1[0] == '!') {
      search_flag = 1;
      s1 = extract(s1,1);
    }
    set_mark("'");
    if (s1=="")
      s1 = last_search2;
    if(!s1 || (s1 = ""))
    {
      write("Lo siento, no encontrado.\n");
      return ;
    }
    do {
      i = j;
      j = i+900;
      if (j>fsize2)
        j = fsize2;
      s3 = explode(read_file(fname,i,j),"\n");
      for (k=0;k<sizeof(s3);k++)
        if (!search_flag) {
          if (sscanf(s3[k],"%s"+s1+"%s",s4,s5)==2)
            if (num--<=0)
              break;
        } else
          if (sscanf(s3[k],"%s"+s1+"%s",s4,s5)!=2)
            if (num--<=0)
              break;
    } while (j<fsize2 && k == sizeof(s3));
    if (k==sizeof(s3))
      write("Lo siento, "+s1+" no encontrado.\n");
    else
      topl2 = i+k-3;
    last_search2 = s1;
    redraw = 1;
    break;
  case "?" :
    i = topl2;
    redraw = 1;
    if (s1[0] == '!') {
      s1 = extract(s1,1);
      search_flag = 3;
    } else
      search_flag = 2;
    set_mark("'");
    if (s1=="" || !s1)
      s1 = last_search2;
    do {
      j = i-900;
      if (j<0)
        j = 0;
      s3 = explode(read_file(fname,j,i),"\n");
      for (k=sizeof(s3);k>0;k--)
        if (search_flag == 2) {
          if (sscanf(s3[k],"%s"+s1+"%s",s4,s5)==2)
            if (num--<=0)
              break;
        } else
          if (sscanf(s3[k],"%s"+s1+"%s",s4,s5)!=2)
            if (num--<=0)
              break;
      i = j;
    } while (j>0 && k == sizeof(s3));
    if (k==sizeof(s3))
      write("Lo siento, "+s1+" no encontrado.\n");
    else
      topl2 = k+j;
    last_search2 = s1;
    redraw = 1;
    break;
  case "n" :
    switch (search_flag) {
      case 0 : next_page(num+"/");
               break;
      case 1 : next_page(num+"/!");
               break;
      case 2 : next_page(num+"?");
               break;
      case 3 : next_page(num+"?!");
               break;
    }
    return;
  case "b" :
    if (noargs)
      num = 1;
    if (topl2>0) {
      topl2 -= (ROWS)*num;
      redraw = 1;
      if (topl2<0)
        topl2 = 0;
    }
    break;
  case "G" :
    if (noargs)
      num = fsize2-ROWS+2;
  case "g" :
    set_mark("'");
    topl2 = num;
    if (topl2>=fsize2)
      topl2 = fsize2-2;
    redraw = 1;
    break;
  case "P" :
    if (noargs)
      num = 100;
  case "p" :
  case "%" :
    redraw = 1;
    set_mark("'");
    topl2 = (num*fsize2)/100;
    if (topl2+ROWS-2>fsize2)
      topl2 -= ROWS-2;
    break;
  case "d" :
    if (noargs)
      num = ROWS/2;
    topl2 += num;
    redraw = 1;
    break;
  case "u" :
    if (noargs)
      num = ROWS/2;
    topl2 -= num;
    redraw = 1;
    break;
  case "r" :
    redraw = 1;
    break;
  case "m" :
    if (s1=="") {
      write("Lo siento, debes especificar un nombre.\n");
      break;
    }
    set_mark(s1);
    write("Ok. Marca "+s1+" introducida.\n");
    break;
  case "'" :
    if (!mark_name) {
      write("Lo siento, debes hacer goto a una marca.\n");
      break;
    }
    if ((i=member_array(s1,mark_name))!=-1) {
      if (fname != mark_data[i][0]) {
        fname = mark_data[i][0];
        redraw = 1;
      }
      if (topl2 != mark_data[i][1]) {
        topl2 = mark_data[i][1];
        redraw = 1;
      }
    } else
      write("Marca "+s1+" no introducida.\n");
    break;
  case "F" :
    filenames = expand_path(s1);
    if (file_size(s1)<=0)
      write("Nombre de fichero inválido "+s1+"\n");
    else {
      fname = s1;
      fsize2 = file_size(s1)/10;
      redraw = 1;
    }
    break;
  case "h" :
    cat("/doc/coder/more");
    break;
  }
  if (redraw)
    display_file();
  if (topl2 <= fsize2) {
    status_line();
    input_to("next_page");
  } else
    if(sizeof(filenames)) {
      get_next_filename();
      input_to("next_page");
    }
    else {
      if (finish_func2)
        call_other(this_object(), finish_func2);
    }
} /* next_page() */

int more_file(string str) 
{
  filenames = expand_path(str);
  if (!sizeof(filenames)) {
    notify_fail("No hay archivos coincidentes.\n");
    return 0;
  }
  
  last_search2 = "";
  get_next_filename();
  
  if (fsize2 >= ROWS) 
  {
    status_line();
    input_to("next_page");
  } 
  else
    if (finish_func2)
      call_other(this_object(), finish_func2);

  return 1;

} /* more_file() */
