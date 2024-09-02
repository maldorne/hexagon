
#ifndef _KERNEL_INCLUDE
#define _KERNEL_INCLUDE

#define DRIVER "/lib/core/driver"
#define AUTO   "/lib/core/auto"

#define MUDOS_PATH "/lib/core/secure/mudos"
#define MUDOS      "/lib/core/secure/mudos"
#define FILTERS    "/lib/core/filters"
// #define MUDOS find_object(DRIVER)->mudos()

#define LOGIN find_object(DRIVER)->login()
#define LOGIN_OB "/lib/core/login"

// Longer logs in the driver stderr
#define DRIVER_LONG_LOGS

#define ERROR_HANDLER     "/lib/core/errors"
#define USER_HANDLER      "/lib/core/secure/users"
#define OBJECT_HANDLER    "/lib/core/secure/objects"
#define SINGLETON_HANDLER "/lib/core/secure/singletons"
#define BINARY_HANDLER    "/lib/core/secure/binaries"

// driver messages
#define DRIVER_RUNTIME_ERROR 0
#define DRIVER_COMPILE_ERROR 1
#define DRIVER_ATOMIC_ERROR  2

#endif
