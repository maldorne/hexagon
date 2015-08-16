
#define DRIVER "/lib/core/driver"
#define AUTO   "/lib/core/auto"
#define SECURE "/lib/core/secure"

#define MUDOS_PATH  "/lib/core/mudos"
#define MUDOS find_object(DRIVER)->mudos()

// Longer logs in the driver stderr
#define DRIVER_LONG_LOGS


#define ERROR_HANDLER  "/lib/handlers/errors"
#define USER_HANDLER   "/lib/handlers/users"
#define OBJECT_HANDLER "/lib/handlers/objects"

// driver messages
#define DRIVER_MESSAGE_PLAIN 0
#define DRIVER_MESSAGE_ERROR 1


