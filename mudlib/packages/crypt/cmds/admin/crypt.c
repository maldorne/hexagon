/*
 * Jubal's encrypting tool
 *
 * This program reads in a file and:
 *   decrypts the file if it ends in EXTENSION (#defined below)
 *   encrypts the file otherwise
 * The encryption is done by rotating with a key that the user selects
 * The first lines of the file are the header
 * Then, the next line is the key encrypted with crypt() (for checking keys)
 * After that is the data.
 *
 * This is a bin/ type command.  To invoke it, call the function main()
 *
 *                Enjoy,   Jubal  (Dave Ljung, DaveSource.com/GetDave.com)
 */

/*
 * This program has been successfully ported to the following drivers:
 * 2.4.5, 3.1.2, 3.1.2 -DR, 3.1.2 -CD and 3.2 (amylaar)
 *
 * This code is copyright 1993, David Ljung Madison (DaveSource.com/GetDave.com)
 *
 * It may be freely redistributed, but it may not be sold in any form.
 */

/* consideration:  make the write_file a diff. function
 * and then do a call_out to help take care of too long eval error (big files)
 * This won't work on most systems, because call_outs often can't do file I/O
 */

/*
 * Trying to port this to Hexagon, neverbot 05/2024
 */

#include <mud/cmd.h>
#include <mud/config.h>

inherit CMD_BASE;

#define CRYPT_MAX_DEPTH 100
#define CRYPT_MAX_TICKS 10000000

/* last character of header MUST be '\n' */
/* Don't change the HEADER or else you won't be able to uncrypt files! */
#define HEADER                                                                 \
  "\
This file was created by Jubal's 'crypt' program\n\
Please do not alter it in ANY way as this will destroy the contents\n\
-------------------------------------------------------------------\n"

/* filename extension */
#define EXTENSION ".x"

/* don't touch this! :) */
#define CHARS                                                                  \
  ({"`", "1",  "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=",  " ",    \
    "q", "w",  "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\\", "a",    \
    "s", "d",  "f", "g", "h", "j", "k", "l", ";", "'", "z", "x", "c",  "v",    \
    "b", "n",  "m", ",", ".", "/", " ", "~", "!", "@", "#", "$", "%",  "^",    \
    "&", "*",  "(", ")", "_", "+", "Q", "W", "E", "R", "T", "Y", "U",  "I",    \
    "O", "P",  "{", "}", "|", "A", "S", "D", "F", "G", "H", "J", "K",  "L",    \
    ":", "\"", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?"})

/* global variables!  Don't let two players use this command at once! */
string name, newname;
int crypt, overwrite;
mixed tmpkey;

void setup()
{
  set_aliases(({ "crypt" }));
  set_usage("crypt <file>");
  set_help("crypts / decrypts a file.");
}

/* the encryption routine */
int file_crypt(mixed key, int decrypt, string name, string newname)
{
  int lines, len, i, j, s1, s2, c, k;
  string in, out, *chars, seed, *hdr;

  chars = CHARS;
  s1 = sizeof(chars);
  s2 = strlen(key) - 1;
  k = 0;

  i = 1;

  if (file_size(newname) != -1)
    if (!overwrite)
    {
      write(newname + " already exists\n");
      return 0;
    }
    else if (!rm(newname))
    {
      write("Could not remove " + newname + "\n");
      return 0;
    }

  if (!crypt)
  {
    hdr = explode(HEADER, "\n");

    while (i <= sizeof(hdr))
    {
      catch {
        if (in = read_file_line(name, i))
        {
          if (in != hdr[i - 1] + "\n")
          {
            write("Corrupt header\n");
            return 0;
          }
        }
      };

      i++;
    }

    in = read_file_line(name, i);
    
    if (crypt(key, in) != in[0..strlen(in)-2])
    {
      write("Wrong key\n");
      return 0;
    }

    i++;
  } 
  else 
  {
    j = random(s1);

    if (chars[j] == "\n")
      j = 5;

    seed = chars[j];
    j = random(s1);

    if (chars[j] == "\n")
      j = 6;

    seed += chars[j];

    if (!write_file(newname, HEADER + crypt(key, seed) + "\n"))
    {
      write("ERROR: writing header\n");
      return 0;
    }
  }

  catch {
    while (in = read_file_line(name, i) )
    {
      i++;
      len = strlen(in);
      out = "";
    
      for (j = 0; j < len; j++)
      {
        c = member_array(in[j..j], chars);
        
        if (c == -1)
          out += in[j..j];
        else 
        {
          c = decrypt ? c - key[k] : c + key[k];

          while (c < 0)
            c += s1;

          while (c >= s1)
            c -= s1;

          k = k + 1 > s2 ? 0 : k + 1;
          out += chars[c];
        }
      }

      if (!write_file(newname, out))
      {
        write("ERROR: writing line: " + i + " to: " + newname + "\n");
        return 0;
      }
    }
  };  

  return 1;
}

/* this can be altered in many ways -- use valid_write, mk_path, whatever */
/* make sure it returns the prepending "/" */
string mk_path(string name)
{
  /* resolve_path and prepending '/' for use on Virtual Realities */
  return resolve_path(name);
  // return "/" + resolve_path(this_player()->query_path(), name);
  // return this_player()->mk_path(name);
}

int main(string what)
{
  int totlines, l;

  if (!strlen(what))
  {
    notify_fail("Usage: crypt <file>\n");
    return 0;
  }

  name = mk_path(what);
  l = file_size(name);

  if (l == -1)
  {
    if (file_size(name + EXTENSION) == -1)
    {
      write(name + " does not exist\n");
      return 1;
    }

    name = name + EXTENSION;
    l = file_size(name);
  }

  if (l == -2)
  {
    write(name + " is a directory\n");
    return 1;
  }
  
  if (l == 0)
  {
    write(name + " is empty\n");
    return 1;
  }

  l = strlen(name);
  
  if (name[l - strlen(EXTENSION)..l - 1] == EXTENSION)
  {
    newname = name[0..l - strlen(EXTENSION) - 1];
    crypt = 0;
  } 
  else
  {
    newname = name + EXTENSION;
    crypt = 1;
  }
  
  overwrite = 0;

  write((crypt ? "En" : "De") + "crypting: " + name + " (" + (l / 1000) + "." +
        (l - (l / 1000 * 1000)) + "k) to " + newname + "\n");

  write("Set decrypting key (and remember it!): ");
  if (!crypt)
    input_to("get_key2", 1);
  else
    input_to("get_key1", 1);
  return 1;
}

int get_key1(mixed key) 
{
  tmpkey = key;
  write("\nSet decrypting key (again): ");
  input_to("get_key2", 1);
}

int get_key2(mixed key) 
{
  int i, l;
  write("\n");

  if (crypt && key != tmpkey)
  {
    write("You changed!\n");
    tmpkey = 0;
    key = 0;
    return 1;
  }

  tmpkey = 0;

  rlimits(CRYPT_MAX_DEPTH; CRYPT_MAX_TICKS)
  {
    if (file_crypt(key, !crypt, name, newname))
    {
      // if (!rm(name))
      //   write("Couldn't remove file: " + name + "\n");
      // else
        write("Done.\n");
    }
  }

  key = 0;
  return 1;
}

static int cmd(string str, object me, string verb)
{
  main(str);
  return 1;
}
