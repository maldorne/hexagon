#include <move_failures.h>

#define DROP 1
#define GET 2

private int move_flag;
private object prev; /* added Mar 3 '93 by Dank to support goback command */

// Añadido create
void create()
{
	prev = nil;
}

object query_prev() { return prev; }

/* default to being gettable and dropable...
 * modify as you will...
 */
void reset_get() { move_flag |= GET; }   /* can't be gotten */
void set_get() { move_flag &= ~GET; }    /* can be gotten */
void reset_drop() { move_flag |= DROP; }   /* can't be dropped */
void set_drop() { move_flag &= ~DROP; }    /* can be dropped */  

/* these should have been called "query...", but they're not */
int drop() { return move_flag & DROP; }
int get() { return move_flag & GET; }
int gettable() { return !get(); }

void set_move_flag(int i) { move_flag = i; }

int move(mixed dest, varargs mixed messin, mixed messout) 
{
	prev = environment();
	
	if (!dest)
		return MOVE_EMPTY_DEST;
		
	if (environment() && !environment()->test_remove(this_object(),
		move_flag & DROP))
		return MOVE_NO_DROP;
		
	if (!dest->test_add(this_object(), move_flag & GET))
		return MOVE_NO_GET;
		
	if (environment())
		event(environment(), "exit", messout, dest);
		
	move_object(dest);
	
	if (objectp(dest))
		event(dest, "enter", messin, prev);
	else if (find_object(dest))
		event(find_object(dest), "enter", messin, prev);
		
	return MOVE_OK;
}

void dest_me() 
{
	/*
	int i;
	object* obs;
	object ob;

	if (environment())
		event(environment(), "dest_me");

	// Destruct shadows of this object, Wonderflug 96
	obs = ({ });
	ob = shadow(this_object(), 0);

	while ( ob )
	{
		obs += ({ ob });
		ob = shadow(ob, 0);
	}

	for ( i = 0; i < sizeof(obs); i++ )
		if ( obs[i] )
			destruct(obs[i]);
			*/

	::destruct(this_object());
}

/* Do not I repeat do not mask this function.
 * is used when all else fails to dest the object...
 * If we find you masking this function... We will cut you up
 * into little bits... slowly
 */
nomask mixed dwep() 
{
  ::destruct(this_object());
  return "Destruct With Extreme Prejuce";
}

/* global/basic/move: */
/*
mixed *query_init_data() {
   return ({ "move", move_flag, "set_move_flag/p/" });
}
*/

// Añadido stats
mixed stats()
{
	return ({
		({"Move Flag", move_flag, }),
		({"Previous", prev, }),
	   });
}