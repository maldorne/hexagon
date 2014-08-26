int save_ed_setup(object wiz, int setup) {
  wiz->set_ed_setup(setup);
  return 1;
} /* save_ed_setup() */

int retrieve_ed_setup(object wiz) {
  return (int)wiz->query_ed_setup();
} /* retrieve_ed_setup() */

/*
 * make_path_absolute:
 *   Used by ed.c to expand relative path names on read and write.
 */
string make_path_absolute(string str) {
  if (this_player()) {
    return (string)get_path(str);
  }
} /* make_path_absolute() */

/*
 * Give a file name for edit preferences to be saved in.
 */
string get_ed_buffer_save_file_name(string file) {
  string *file_ar;

  file_ar = explode(file,"/") - ({ "" });
  file = file_ar[sizeof(file_ar)-1];
  write("File saved in \"/home/.dead_ed_files/" +
        this_player()->query_name() + "-" + file + "\"\n");
  return "/home/.dead_ed_files/" + this_player()->query_name() + "-" + file;
} /* get_ed_buffer_save_file_name() */

// fix for new driver, Radix (thanks to Donky@Phoenix)
string get_save_file_name(string file) {
  string *file_ar;

  file_ar = explode(file,"/") - ({ "" });
  file = file_ar[sizeof(file_ar)-1];
  write("File saved in \"/home/.dead_ed_files/" +
        this_player()->query_name() + "-" + file + "\"\n");
  return "/home/.dead_ed_files/" + this_player()->query_name() + "-" + file;
}
