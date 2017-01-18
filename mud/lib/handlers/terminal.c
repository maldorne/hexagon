
/* The terminal handler.  This has all sorts of utterly useless junk on it
 * Share and enjoy.
 * Thrown up by Pinkfish.

    Modified by Godot to add the xterm terminal. (10/16/92)
    Modified by Hamlet to add REVERSE and UNDERLINE (Oct 95)
    Modified more by Hamlet to make a noblink termtype (Oct 95)
    Modified more by Baldrick to make this a nonblink mud (Oct 95)
    Modified more by Hamlet to add 'text' and 'monoflag' and 'monoansi'
             termtypes (Dec 97)
    Improved with ansi_enye, neverbot 12/2010

    Definitions:

        <colour>    foreground colour
        B_<colour>  background colour
        BOLD        bold or bright letters
        ENDTERM     unknown
        FLASH       flashing or blinking letters
        INITTERM    unknown
        RESET       return to the normal font -- not a terminal reset.
        STATUS      unknown
        WINDOW      unknown
 */

#define ANSI(p) sprintf("%c["+(p)+"m", 27)
#define ESC(p) sprintf("%c"+(p), 27)
mapping terms;

void create() {
  terms = ([ "dumb" : ([ "RESET" : "",
                         "BOLD" : "",
                         "FLASH" : "",
                         "UNDERLINE" : "",
                         "REVERSE" : "",
                         "BLACK" : "",
                         "RED" : "",
                         "BLUE" : "",
                         "CYAN" : "",
                         "MAGENTA" : "",
                         "ORANGE" : "",
                         "YELLOW" : "",
                         "GREEN" : "",
                         "WHITE" : "",
                         "B_RED" : "",
                         "B_ORANGE" : "",
                         "B_YELLOW" : "",
                         "B_BLACK" : "",
                         "B_CYAN" : "",
                         "B_WHITE" : "",
                         "B_GREEN" : "",
                         "B_MAGENTA" : "",
                         "B_BLUE" : "",
                         "STATUS": "",
                         "WINDOW": "",
                         "INITTERM": "",
                         "ENDTERM": "",
                         ]),
             "ansi" : ([ "RESET" : ANSI("0;37;40"),
                         "BOLD" : ANSI(1),
                         "FLASH" : "",
                         "UNDERLINE" : ANSI(4),
                         "REVERSE" : ANSI(7),
                         "BLACK" : ANSI(30),
                         "RED" : ANSI(31),
                         "GREEN" : ANSI(32),
                         "ORANGE" : ANSI(33),
                         "YELLOW" : ANSI(1) + ANSI(33),
                         "BLUE" : ANSI(34),
                         "CYAN" : ANSI(36),
                         "MAGENTA" : ANSI(35),
                         "WHITE" : ANSI(37),
                         "B_RED" : ANSI(41),
                         "B_GREEN" : ANSI(42),
                         "B_ORANGE" : ANSI(43),
                         "B_YELLOW" : ANSI(1)+ANSI(43),
                         "B_BLUE" : ANSI(44),
                         "B_CYAN" : ANSI(46),
                         "B_BLACK" : ANSI(40),
                         "B_WHITE" : ANSI(47), /* 47? */
                         "B_MAGENTA" : ANSI(45),
                         "STATUS": "",
                         "WINDOW": "",
                         "INITTERM": "",
                         "ENDTERM": "",
                         ]),
       "ansi-status": ([ "RESET" : ANSI("0;37;40"),
                         "BOLD" : ANSI(1),
                         "FLASH" : "",
                         "UNDERLINE" : ANSI(4),
                         "REVERSE" : ANSI(7),
                         "BLACK" : ANSI(30),
                         "RED" : ANSI(31),
                         "GREEN" : ANSI(32),
                         "ORANGE" : ANSI(33),
                         "YELLOW" : ANSI(33),
                         "BLUE" : ANSI(34),
                         "CYAN" : ANSI(36),
                         "MAGENTA" : ANSI(35),
                         "WHITE" : ANSI(37),
                         "B_RED" : ANSI(41),
                         "B_GREEN" : ANSI(42),
                         "B_ORANGE" : ANSI(43),
                         "B_YELLOW" : ANSI(1)+ANSI(43),
                         "B_BLUE" : ANSI(44),
                         "B_CYAN" : ANSI(46),
                         "B_BLACK" : ANSI(40),
                         "B_WHITE" : ANSI(47), /* 47? */
                         "B_MAGENTA" : ANSI(45),
                         "STATUS" : ESC("[23;24r") + ESC(8),
                         "WINDOW" : ESC(7)+ESC("[0;22r")+ESC("[22H\n"),
                         "INITTERM" : ESC("[H")+ESC("[J")+
                                      ESC("[23;24r")+ESC("23H\n"),
                         "ENDTERM" : ESC("[0r")+ESC("[H")+ESC("[J"),
                         ]),
        // same colour_map as ansi, take a look to the functions below
        "ansi-enye" : ([ "RESET" : ANSI("0;37;40"),
                         "BOLD" : ANSI(1),
                         "FLASH" : "",
                         "UNDERLINE" : ANSI(4),
                         "REVERSE" : ANSI(7),
                         "BLACK" : ANSI(30),
                         "RED" : ANSI(31),
                         "GREEN" : ANSI(32),
                         "ORANGE" : ANSI(33),
                         "YELLOW" : ANSI(1) + ANSI(33),
                         "BLUE" : ANSI(34),
                         "CYAN" : ANSI(36),
                         "MAGENTA" : ANSI(35),
                         "WHITE" : ANSI(37),
                         "B_RED" : ANSI(41),
                         "B_GREEN" : ANSI(42),
                         "B_ORANGE" : ANSI(43),
                         "B_YELLOW" : ANSI(1)+ANSI(43),
                         "B_BLUE" : ANSI(44),
                         "B_CYAN" : ANSI(46),
                         "B_BLACK" : ANSI(40),
                         "B_WHITE" : ANSI(47), /* 47? */
                         "B_MAGENTA" : ANSI(45),
                         "STATUS": "",
                         "WINDOW": "",
                         "INITTERM": "",
                         "ENDTERM": "",
                         ]),
          "freedom" : ([ "RESET" : ESC("G0"),
                         // not working with new sprintf conversion characters
                         // not sure how to fix, although maybe noone uses 
                         // freedom term anymore...
                         // "BOLD" : ESC("G@"),
                         "BOLD" : ESC("G@@"),
                         "FLASH" : "",
                         "UNDERLINE" : "",
                         "REVERSE" : "",
                         "BLACK" : "",
                         "RED" : "",
                         "GREEN" : "",
                         "ORANGE" : "",
                         "YELLOW" : "",
                         "BLUE" : "",
                         "CYAN" : "",
                         "MAGENTA" : "",
                         "WHITE" : "",
                         "B_RED" : ESC("GD"),
                         "B_GREEN" : ESC("GD"),
                         "B_ORANGE" : ESC("G4"),
                         "B_YELLOW" : ESC("G4"),
                         "B_BLUE" : ESC("G4"),
                         "B_CYAN" : ESC("GD"),
                         "B_BLACK" : ESC("GD"),
                         "B_WHITE" : ESC("G4"), /* 47? */
                         "B_MAGENTA" : ESC("G4"),
                         "STATUS": "",
                         "WINDOW": "",
                         "INITTERM": "",
                         "ENDTERM": "",
                         ]),
             "xterm": ([ "RESET" : ESC(">")+ESC("[1;3;4;5;6l")+ESC("[?7h")+ESC("[m"),
                         "BOLD" : ESC("[7m"),
                         "FLASH" : "",
                         "UNDERLINE" : "",
                         "REVERSE" : "",
                         "BLACK" : ANSI(30),
                         "RED" : ANSI(31),
                         "GREEN" : ANSI(32),
                         "ORANGE" : ANSI(33),
                         "YELLOW" : ANSI(33),
                         "BLUE" : ANSI(34),
                         "CYAN" : ANSI(36),
                         "MAGENTA" : ANSI(35),
                         "WHITE" : ANSI(38),
                         "B_RED" : ANSI(41),
                         "B_GREEN" : ANSI(42),
                         "B_ORANGE" : ANSI(43),
                         "B_YELLOW" : ANSI(43),
                         "B_BLUE" : ANSI(44),
                         "B_CYAN" : ANSI(46),
                         "B_BLACK" : ANSI(40),
                         "B_WHITE" : ANSI(47),
                         "B_MAGENTA" : ANSI(45),
                         "STATUS" : "",
                         "WINDOW" : "",
                         "INITTERM" : "",
                         "ENDTERM" : "",
                         ]),
             "text" : ([ "RESET" : "",
                         "BOLD" : "[Bold]",
                         "FLASH" : "",
                         "UNDERLINE" : "[Und]",
                         "REVERSE" : "[Rev]",
                         "BLACK" : "[Bla]",
                         "RED" : "[R]",
                         "GREEN" : "[G]",
                         "ORANGE" : "[O]",
                         "YELLOW" : "[Y]",
                         "BLUE" : "[Blu]",
                         "CYAN" : "[C]",
                         "MAGENTA" : "[M]",
                         "WHITE" : "[W]",
                         "B_RED" : "[B_R]",
                         "B_GREEN" : "[B_G]",
                         "B_ORANGE" : "[B_O]",
                         "B_YELLOW" : "[B_Y]",
                         "B_BLUE" : "[B_Blu]",
                         "B_CYAN" : "[B_C]",
                         "B_BLACK" : "[B_Bla]",
                         "B_WHITE" : "[B_W]", /* 47? */
                         "B_MAGENTA" : "[B_M]",
                         "STATUS": "",
                         "WINDOW": "",
                         "INITTERM": "",
                         "ENDTERM": "",
                         ]),
         "monoansi" : ([ "RESET" : ANSI("0;37;40"),
                         "BOLD" : ANSI(1),
                         "FLASH" : "",
                         "UNDERLINE" : ANSI(4),
                         "REVERSE" : ANSI(7),
                         "BLACK" : "[Bla]",
                         "RED" : "[R]",
                         "GREEN" : "[G]",
                         "ORANGE" : "[O]",
                         "YELLOW" : "[Y]",
                         "BLUE" : "[Blu]",
                         "CYAN" : "[C]",
                         "MAGENTA" : "[M]",
                         "WHITE" : "",
                         "B_RED" : "[B_R]",
                         "B_GREEN" : "[B_G]",
                         "B_ORANGE" : "[B_O]",
                         "B_YELLOW" : "[B_Y]",
                         "B_BLUE" : "[B_Blu]",
                         "B_CYAN" : "[B_C]",
                         "B_BLACK" : "[B_Bla]",
                         "B_WHITE" : ANSI(47), /* 47? */
                         "B_MAGENTA" : "[B_M]",
                         "STATUS": "",
                         "WINDOW": "",
                         "INITTERM": "",
                         "ENDTERM": "",
                         ]),
         "monoflag" : ([ "RESET" : ANSI("0;37;40"),
                         "BOLD" : ANSI(1),
                         "FLASH" : "",
                         "UNDERLINE" : ANSI(4),
                         "REVERSE" : ANSI(7),
                         "BLACK" : "[Bla]",
                         "RED" : ANSI(4),
                         "GREEN" : "[G]",
                         "ORANGE" : ANSI(4),
                         "YELLOW" : ANSI(33),
                         "BLUE" : ANSI(34),
                         "CYAN" : "[C]",
                         "MAGENTA" : "[M]",
                         "WHITE" : "",
                         "B_RED" : "[B_R]",
                         "B_GREEN" : "[B_G]",
                         "B_ORANGE" : "[B_O]",
                         "B_YELLOW" : "[B_Y]",
                         "B_BLUE" : "[B_Blu]",
                         "B_CYAN" : "[B_C]",
                         "B_BLACK" : "[B_Bla]",
                         "B_WHITE" : ANSI(47), /* 47? */
                         "B_MAGENTA" : "[B_M]",
                         "STATUS": "",
                         "WINDOW": "",
                         "INITTERM": "",
                         "ENDTERM": "",
                         ]),
          ]);
}

mixed set_term_type(string str) 
{
  if (!terms[str]) 
  {
    tell_object(this_player(), "No existe el tipo de terminal '"+str+"', se usará dumb.\n");
    str = "dumb";
  }
  return terms[str];
}

string *query_term_types() 
{
  return m_indices(terms);
}

// new terms for ccmud, neverbot 7/03

// change ñ's and accents to common letters
string fix_enye_mode(string mess)
{
  int i;
  string ret;

  ret = "";
  
  for (i = 0; i < strlen(mess); i++)
  {
    switch(mess[i..i])
    { 
      case "ñ": ret += "ny"; break;
      case "Ñ": ret += "Ny"; break;
      
      case "¡": break;
      case "¿": break;

      case "á": 
      case "ä":       
      case "â":       
        ret += "a"; break;
      case "é": 
      case "ë":       
      case "ê":       
        ret += "e"; break;
      case "í": 
      case "ï": 
      case "î": 
        ret += "i"; break;
      case "ó": 
      case "ö": 
      case "ô": 
        ret += "o"; break;
      case "ú": 
      case "ü": 
      case "û": 
        ret += "u"; break;
    
      case "Á": 
      case "Ä":       
      case "Â":       
        ret += "A"; break;
      case "É": 
      case "Ë":       
      case "Ê":       
        ret += "E"; break;
      case "Í": 
      case "Ï": 
      case "Î": 
        ret += "I"; break;
      case "Ó": 
      case "Ö": 
      case "Ô": 
        ret += "O"; break;
      case "Ú": 
      case "Ü": 
      case "Û": 
        ret += "U"; break;
      default:
        ret += mess[i..i];
    }
  }
  
    return ret;
}

string _fix_string(string ret, varargs object user)
{
  string *st;
  int i;
  string term_name;
  mapping colour_map;

  if (user && interactive(user))
    term_name = user->query_term_name();

  if (!term_name) 
    // term_name = "dumb";
    // ansi by default
    term_name = "ansi";

  colour_map = set_term_type(term_name);

  if (!stringp(ret) || ret == "")
    return ret;

  // ret += "%^RESET%^";

  // new terms for ccmud, neverbot 7/03

  if (term_name == "ansi-enye")
    ret = fix_enye_mode(ret);

  st = explode(ret, "%^");
  ret = "";

  for (i = 0; i < sizeof(st); i++) 
  {
    if (colour_map[st[i]])
      ret += colour_map[st[i]];
    else
      ret += st[i];
  }

  return ret + colour_map["RESET"];
} 
