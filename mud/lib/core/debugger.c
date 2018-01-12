
#include <mud/config.h>

// Debugger, every log goes through this 

static mapping _listeners;

void create() 
{
  _listeners = ([ ]);
  ::create();
}

void add_listener(string type, object ob)
{
  if (!_listeners[type])
    _listeners[type] = ({ ob });
  else
    _listeners[type] = ({ ob }) + _listeners[type];
}

void log(string type, string message)
{
  object * listeners;
  int i;

  listeners = _listeners[type];

  for (i = 0; i < sizeof(listeners); i++)
  {
    tell_object(listeners[i], "Debug (" + type + "): " + message);
  }

#ifdef USE_STANDARD_LOG
  stderr(message);
#endif
}
