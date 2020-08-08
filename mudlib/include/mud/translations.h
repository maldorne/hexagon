
/*
 * Language
 *
 * every time we include the language header file
 * #include <language.h>
 * a .lang.<language>.h from the current directory will be included instead
 * take a look to the include_file function in the driver object to see
 * how this is done
 *
 * currently supported languages: es, en
 *
 * important: use the same extension for the include, will be used
 * (only) in the driver and auto objects, which will not do the
 * include_file call
 */
#define GLOBAL_COMPILE_LANG "en"
#include <mud/texts.en.h>
