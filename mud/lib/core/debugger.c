
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

  if (undefinedp(_listeners[type])) 
  {
    _listeners[type] = ({ });
  }
  else
  {
    listeners = _listeners[type];

    for (i = 0; i < sizeof(listeners); i++)
    {
      // TODO improve log
      tell_object(listeners[i], "Debug (" + type + "): " + message);
    }
  }

#ifdef USE_STANDARD_LOG
#ifdef USE_STANDARD_LOG_TYPE
  stderr(sprintf(" %-8s || ", type) + message);
#else
  stderr(message);
#endif
#endif
}
