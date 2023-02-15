
#include <item/board.h>
#include <mud/secure.h>

mapping boards,
        security,
        priv,
        timeouts,
        archives;
int num;

void expire_boards();
string query_archive(string board);
static int zap_message(string board, int num);

void create() {
  num = 1;
  boards = ([ ]);
  security = ([ ]);
  priv = ([ ]); /* Private boards... only lords or specified people
                 * can read. */
  timeouts = ([ ]);
  archives = ([ ]);
  seteuid((string)MASTER->creator_file(file_name(this_object())));
  restore_object(BOARD_FILE,1);
  expire_boards();
} /* create() */

string *query_boards() {
  return m_indices(boards);
} /* query_boards() */

int query_admin(string str) {
  return (int)MASTER->query_admin(str);
} /* query_admin() */

mixed get_subjects(string name) {
  string pl;
  int bit;

  if (file_name(previous_object())[0..10] != "/lib/obj/board"[0..10])
    return ({ });

  pl = (string)this_player()->query_name();
  bit = priv[name] && !query_admin(pl) &&
        (member_array(pl, security[name]) == -1);
  if (!boards[name] || bit)
    return ({ });
  return boards[name];
} /* get_subjects() */

string get_message(string board, int num) {
  string name;

  if (file_name(previous_object())[0..10] != "/lib/obj/board"[0..10])
    return "";
  name = (string)this_player()->query_name();
  if (!boards[board] || (priv[board] && !query_admin(name)
                        && (member_array(geteuid(previous_object()),
                                         security[board]) == -1)))
    return "";
  if (num < 0 || num >= sizeof(boards[board]))
    return "";
  name = sprintf("%s%d",BOARD_DIR, boards[board][num][B_NUM]);
  if (file_size(name) <= 0)
    return "";
  return read_file(name);
} /* get_message() */

int save_me() {
  save_object(BOARD_FILE,1);
} /* save_me() */

int add_message(string board, string name, string subject, string body) {
  string fname;
  int test, irp;

  test = priv[board] && !query_admin(name) && (-1 == member_array(name,
                                                        security[board]));
  if (!boards[board] || test)
    return 0;
  else
    boards[board] += ({ ({ subject, name, num++, time() }) });
  fname = sprintf("%s%d", BOARD_DIR, num-1);

  // Fix by Aragorn
  body = replace_string(body,"@@","");

  write_file(fname, body);
  save_me();
  if (timeouts[board] && timeouts[board][T_MAX] &&
      sizeof(boards[board]) > timeouts[board][T_MAX]) {
    while (sizeof(boards[board]) > timeouts[board][T_MAX]) {
      zap_message(board, 0);
      irp++;
    }
      event(users(), "inform", capitalize(name)+" escribe un mensaje en "+board, "message");
/*
    event(users(), "inform", capitalize(name)+" escribe un mensaje en "+board+
                   " y "+irp+" mensaje"+(irp>1?"s":"")+
                   " estallan de alegria", "message");
*/
  } else
    event(users(), "inform", capitalize(name)+" escribe un mensaje en "+board,
                   "message");
  return num-1;
} /* add_message() */

int create_board(string board, int priva) {
  board = lower_case(board);
  if (boards[board])
    return 0;
  boards[board] = ({ });
  security[board] = ({ this_player()->query_name() });
  if (priva)
    priv[board] = priva;
  save_me();
  write("Creado tablón "+board+".\n");
  return 1;
} /* create_board() */

int add_allowed(string board, string name) {
  string nam;

  if (sscanf(file_name(previous_object()), "/lib/obj/board%s", nam) != 1)
    return 0;
  nam = (string)this_player()->query_name();
  if (member_array(nam, security[board]) == -1 &&
      !MASTER->query_admin(nam))
    return 0;
  if (!"/lib/core/login"->test_user(name))
    return 0;
  security[board] += ({ name });
  save_me();
  write("Añadido "+name+" a la lista de seguridad de "+board+".\n");
  return 1;
} /* add_allowed() */

int remove_allowed(string board, string name) {
  string nam;
  int i;

  if (sscanf(file_name(previous_object()), "/lib/obj/board%s", nam) != 1)
    return 0;
  nam = geteuid(previous_object());
  if ((i= member_array(name, security[board])) == -1 &&
      !MASTER->query_admin(nam))
    return 0;
  security[board] = delete(security[board], i, 1);
  save_me();
  write("Quitado "+name+" de la lista de seguridad de "+board+".\n");
  return 1;
} /* add_allowed() */

static int zap_message(string board, int off) {
  int num;
  string nam, archive;

  if (off < 0 || off >= sizeof(boards[board]))
    return 0; /* out of range error */
  num = boards[board][off][B_NUM];
  nam = sprintf("%s%d", BOARD_DIR, num);
  archive = query_archive(board);
  if (archive) {
    mixed *stuff;

    stuff = boards[board][off];
    write_file(archive,
              sprintf("\n----\nNota #%d por %s escrita el %s\nTítulo: '%s'\n\n",
              off, capitalize(stuff[B_NAME]), ctime(stuff[B_TIME]),
              stuff[B_SUBJECT])+
              read_file(nam));
  }
  boards[board] = delete(boards[board],off,1);
  rm(nam);
  save_me();
  return 1;
} /* zap_message() */

int delete_message(string board, int off) {
  string nam;

  if (file_name(previous_object())[0..10] != "/lib/obj/board"[0..10])
    return 0;
  nam = geteuid(this_player());
  if(!security[board])
    security[board] = ({ });

  // Added by Folken, 07/04
  if (off < 0 || off >= sizeof(boards[board]))
    return 0;

  if (member_array(nam, security[board]) == -1 &&
      boards[board][off][B_NAME] != nam &&
      !MASTER->query_admin(nam))
    return 0; /* not allowed to delete the notes */
  return zap_message(board, off);
} /* delete_message() */

string *query_security(string board) {
  string *str;

  str = security[board];
  if (!str) return str;
  return str + ({ });
} /* query_security() */

int delete_board(string board)
{
  string nam;

  if (!boards[board])
    return 0;
  nam = geteuid(previous_object());
  if (member_array(nam, security[board]) == -1 &&
      !MASTER->query_admin(nam))
    return 0; /* not allowed to delete the notes */
  while (sizeof(boards[board]))
    delete_message(board, 0);
  boards = m_delete(boards, board);
  security = m_delete(security, board);
  map_delete(archives, board);
  map_delete(timeouts, board);
  save_me();
  return 1;
} /* delete_board() */

string *list_of_boards()
{
  return m_indices(boards);
} /* list_of_boards() */

int set_timeout(string board, int timeout)
{
  string nam;

  if (!boards[board]) return 0;
  nam = geteuid(previous_object());
  if (member_array(nam, security[board]) == -1 &&
      !MASTER->query_admin(nam))
    return 0; /* not allowed to delete the notes */
  if (!timeouts[board]) {
    timeouts[board] = ({ DEFAULT_MIN, DEFAULT_MAX, timeout });
    return 1;
  }
  timeouts[board][T_TIMEOUT] = timeout;
  save_me();
  write("Los mensajes se borrarán automáticamente tras "+timeout+
        " días en "+board+".\n");
  return 1;
} /* set_timeout() */

int set_minimum(string board, int min) {
  string nam;

  if (!boards[board]) return 0;
  nam = geteuid(previous_object());
  if (member_array(nam, security[board]) == -1 &&
      !MASTER->query_admin(nam))
    return 0; /* not allowed to delete the notes */
  if (!timeouts[board]) {
    timeouts[board] = ({ min, DEFAULT_MAX, DEFAULT_TIMEOUT });
    return 1;
  }
  timeouts[board][T_MIN] = min;
  save_me();
  write("El mínimo número de mensajes será "+min+" para el tablón "+board+".\n");
  return 1;
} /* set_minimum() */

int set_maximum(string board, int max) {
  string nam;

  if (!boards[board]) return 0;
  nam = geteuid(previous_object());
  if (member_array(nam, security[board]) == -1 &&
      !MASTER->query_admin(nam))
    return 0; /* not allowed to delete the notes */
  if (!timeouts[board]) {
    timeouts[board] = ({ DEFAULT_MIN, max, DEFAULT_TIMEOUT });
    return 1;
  }
  timeouts[board][T_MAX] = max;
  save_me();
  write("El máximo número de mensajes será "+max+" para el tablón "+board+".\n");
  return 1;
} /* set_maximum() */

int set_archive(string board, string file) {
  string nam;

  if (!boards[board]) return 0;
  nam = geteuid(previous_object());
  if (member_array(nam, security[board]) == -1 &&
      !MASTER->query_admin(nam))
    return 0; /* not allowed to delete the notes */
  archives[board] = file;
  save_me();
  write("El nombre del archivo ahora será "+file+" para el tablón "+board+".\n");
  return 1;
} /* set_archive() */

int query_timeout(string board)
{
  if (!timeouts[board])
    return 0;
  return timeouts[board][T_TIMEOUT];
} /* query_timeout() */

int query_minimum(string board)
{
  if (!timeouts[board])
    return 0;
  return timeouts[board][T_MIN];
} /* query_minimum() */

int query_maximum(string board)
{
  if (!timeouts[board])
    return 0;
  return timeouts[board][T_MAX];
} /* query_maximum() */

string query_archive(string board)
{
  if (!boards[board])
    return "";
  if (undefinedp(archives[board]))
    return ARCHIVE_DIR+board;
  return archives[board];
} /* query_archive() */

void expire_boards()
{
  string *bo, nam;
  int i, tim, num;

  bo = keys(timeouts);
  for (i=0;i<sizeof(bo);i++)
  {
    nam = bo[i];
    if ((tim=timeouts[nam][T_TIMEOUT]) &&
        (sizeof(boards[nam]) > timeouts[nam][T_MIN]) &&
        ((boards[nam][0][B_TIME]+(tim*(24*60*60))) < time())) {
      while (sizeof(boards[nam]) > timeouts[nam][T_MIN] &&
             boards[nam][0][B_TIME]+(tim*24*60*60) < time()) {
        zap_message(nam, 0);
        num++;
      }
      event(users(), "inform", "Borrando automáticamente "+num+" mensajes "+
                               "del tablón "+nam, "message");
    }
  }
  if (!find_call_out("expire_boards"))
    call_out("expire_boards", 60*60);
} /* expire_boards() */

int convert_board(string board, string *subject, string *body) {
  int i;
  string sub, name, tim;

  if (!boards[board])
    create_board(board, 0);
  for (i=0;i<sizeof(subject);i++) {
    sscanf(subject[i], "%s (%s, %s)", sub, name, tim);
    add_message(board, name, sub, body[i]);
  }
  return 1;
} /* convert_board() */

int query_prevent_shadow() { return 1; }

int query_num() { return num; }

