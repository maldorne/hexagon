/* *****************************************************************
 *
 *  lpc + exec
 *
 *  - (lpc) original from Jubal E. Harshaw (GumbiMud)
 *  - (exec) original from who knows who, modified by neverbot for RLMud
 *  - (lpc + exec) MudOS version (CcMudlib) by neverbot, 11/2008
 *  - ported to DGD (CcMudlib) by neverbot, 08/2015
 *
 * ***************************************************************** */

// #include <standard.h>

#include <mud/cmd.h>
#include <mud/secure.h>

inherit CMD_BASE;

#define CODER_DIR "/home/" + (string)this_player()->query_name()
#define CODER_FILE "/home/"+this_player()->query_name()+"/.lpc_exec.c"

private int show, dont_overwrite, dont_remove;
private mixed arg;

private string parse_args(string str);
private void usage();
private string defines();
private string vars();
private string before();
private string after();
private int check_file();
private int check_code(string str);

private int check_file()
{
  string wiz_dir;
  string wiz_file_str;
  object wiz_file_ob;

  wiz_dir = CODER_DIR;
  wiz_file_str = CODER_FILE;

  if (file_size(wiz_dir) != -2)
  {
    notify_fail("Directory: '" + wiz_dir + "' non existant.\n");
    return 0;
  }

  if (wiz_file_ob = find_object(wiz_file_str))
    wiz_file_ob->dest_me();

  return 1;
}

private int check_code(string str)
{
  /*
  string file_target;
  // int auxvar = 0;
  object ob;
  ob = nil;

  if (!str || (str == ""))
    return 1;

  // Aqui metio mano (mucho) Folken 3/2002, para arreglar el capado
  //  del exec que funcionaba mal :)
  if (sizeof(explode(str,"->"))>1)
  {
    // auxvar = sizeof(explode(explode(str,"->")[0]," "));
    // file_target = explode(explode(str,"->")[0]," ")[auxvar?auxvar-1:auxvar];
    file_target = explode(explode(str,"->")[0]," ")[0];
    if (file_target == "return")
      file_target = explode(explode(str,"->")[0]," ")[1];

    // tell_object(this_player(), "Test: '"+file_target+"'\n");
    // Si llamamos por ejemplo a load_object("blabla.c")->lala()
    if (sizeof(explode(file_target,"(")) > 1)
    {
      file_target = explode(file_target,"(")[1];
      file_target = explode(file_target,")")[0];
    }
    file_target = replace_string(file_target,"\"","");

    // tell_object(this_player(), "Test: '"+file_target+"'\n");
    // Podemos estar haciendo un find_living("blahblah")->algo

    if (file_target == "TP")
    {
      file_target = this_player()->query_cap_name();
    }
    else if (file_target == "TO")
    {
      file_target = CODER_FILE;
    }

    ob = find_living(file_target);

    if (ob)
    {
      if (!ob->query_coder())
      {
        tell_object(this_player(), "No esta permitido hacer execs sobre usuarios.\n");
        return 0;
      }
    }
    else
    {
      if ((file_target != CODER_FILE) && (file_size(file_target) < 0))
      {
        tell_object(this_player(), "Exec sobre el archivo: '"+file_target+"' (no existe)\n");
        return 0;
      }

      tell_object(this_player(), "Exec sobre el archivo: '"+file_target+"'\n");

      if (!valid_read(file_target, this_player()))
      {
        log_file("illegal_exec", ctime(time(), 4) + " " + (string)this_player()->query_cap_name() +
                                 " illegal exec: "+str+"\n");
        tell_object(this_player(), "Tu intento de hacer un exec ilegal ha sido grabado.\n");
        return 0;
      }
    }
  }
  */

  return 1;
}

static int cmd(string str, object me, string verb)
{
  object ob;
  string file;
  mixed error, ret;
  int file_size;

  if (!this_player())
    return 0;
  if (this_player(1) != this_player())
    return 0;

  // seteuid(geteuid(this_player()));

  if (!check_file())
    return 1;

  error = 0;
  show = 0;
  dont_overwrite = 0;
  dont_remove = 0;
  arg = 0;

  if (!strlen(str))
  {
    usage();
    return 1;
  }

  str = parse_args(str);

  if (!str)
  {
    write("Aborted.\n");
    return 1;
  }

  // check if trying to execute illegal code (for low level coders)
  if (!SECURE->is_administrator(this_player()->query_name()))
    if (!check_code(str))
      return 1;

  if (show)
  {
    write("--------------------------------------------------\n\n");
    write(before() + str + after());
    write("\n--------------------------------------------------\n\n");
  }

  file = CODER_FILE;

  if ((file_size = file_size(file)) > 0)
  {
    if (dont_overwrite)
      write("Aborted. The file '"+file+"' already exists (remove it first)\n");
    else
      rm(file);
  }

  // last flag: if 0 append the text, else is an offset
  // write_file(file, before() + str + after(), (dont_overwrite ? 0 : -file_size));
  write_file(file, before() + str + after());

  // remove from memory if it does exist
  if ((ob = find_object(file)))
    destruct(ob);

  // try to load the lpc_exec.c file
  // error = catch(ret = load_object(file)->main(arg));
  ret = load_object(file)->main(arg);

  if (error)
    write(sprintf("\nAn error occurred: %s\n", error));
  else
    write("Result:\n" + to_string(ret) + "\n");

  if ((ob = find_object(file)))
    destruct(ob);

  if (!dont_remove)
    rm(file);

  log_file("exec", ctime(time(),4) + " " + (string)this_player()->query_name()+
                   " executed : " + str + "\n");
  return 1;
}

private string parse_args(string str)
{
  string pre, rest, flags;

  if (sscanf(str, "%sABORT%s", pre, rest) == 2)
    return nil;
  if (sscanf(str, "-%s %s", flags, rest) == 2)
  {
    str = rest;
    if (sscanf(flags, "%ss%s", pre, rest) == 2)
      show = 1;
    if (sscanf(flags, "%sd%s", pre, rest) == 2)
      dont_overwrite = 1;
    if (sscanf(flags, "%sp%s", pre, rest) == 2)
      dont_remove = 1;
    if (sscanf(flags, "%sa", pre) ==1)
    {
      if ((sscanf(str, "\"%s\" %s", arg, str) != 2) &&
          (sscanf(str, "%s %s", arg, str) != 2))
      {
        usage();
        return nil;
      }
    }
  }
  return str;
}

private void usage()
{
  write("Syntax: exec [-flags] <code>\n");
  write("[Allows to test one line of LPC code]\n\n");
  write("Executes the line of code inside a main() function in the file\n");
  write(" "+CODER_FILE+" (it will be removed afterwards)\n");
  write("flags:  s: shows the file after being generated\n");
  write("        d: do not overwrite the file if it exists\n");
  write("        p: do not remove the file after the execution\n");
  write("        a <arg>: passes the argument 'arg' when calling main()\n");
  write("                 (must be the last flag)\n");
  write("\nThe following macros can be used: \n"+defines()+"\n");
  write("ie: exec object *a; a=users(); for(int i=0;i<sizeof(a);i++) tell_object(a[i], \"test\");\n");
}

// Code to include in the generated file

private string defines()
{
  // return INCLUDE;
  return "#define TP this_player()\n#define TO this_object()\n" +
         "#define QN query_name()\n#define QCN query_cap_name()\n";
}

private string vars()
{
  return "  // int i, j, k, l = 0;             /* initialized to 0 */\n" +
         "  // string str, pre, rest = \"\";     /* initialized to \"\" */\n" +
         "  // object ob, a, b;                /* not initialized */\n";
}

private string before()
{
  return defines() +
         "\nvoid create() { seteuid(geteuid(this_player())); }\n" +
         "void dest_me() { destruct(this_object()); }\n" +
         "\nmixed main(mixed arg)\n{\n" + // vars() +
         "  /* ONE LINE: */\n\n  ";
}

private string after()
{
  return ";\n\n  /* END ONE LINE */\n"+
        "  write(\"\\nEnd of execution.\\n\");\n" +
        "  return 1;\n}\n";
}

// string help() { main(); }

