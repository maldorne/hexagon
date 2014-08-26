
string get_domain(object ob)
{
   string fname;
   string * words;

   fname = file_name(ob);
   words = explode(fname, "/");
   
   // if (member_array(dom, get_dir("/d/")) != -1)
   //    return dom;

   if ((sizeof(words) > 2) && (words[0] == "d"))
    return words[1];
   
   return "";
}

int is_in_domain(object ob)
{
  if (explode(base_name(ob), "/")[0] == "d")
    return 1;
  return 0;
}
