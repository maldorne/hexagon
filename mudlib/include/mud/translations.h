/*
 * Language
 *
 * This file will and must be included ONLY from the driver and auto 
 * objects, because they are the only files that will not use the 
 * driver include_file call. After both are initilized, every include
 * in the code will be controlled by the driver, and will depend
 * of the language defined here.
 *
 * In the /include/translations/common.<lang>.h file functions and
 * definitions related to each language will be defined.
 *
 * Every time we include the language header file
 *   #include <language.h>
 * a .lang.<GLOBAL_COMPILE_LANG>.h from the current directory will 
 *   be included instead
 *
 * Take a look to the include_file function in the driver object to see
 * how this is done
 *
 * Currently supported languages: es, en
 */

// the real includes have been moved to a file inside the /save directory
// so we can use the same mudlib containers just changing the contents of 
// /save, /home and /log

// here GLOBAL_COMPILE_LANG is defined
#include "/save/language.h"
