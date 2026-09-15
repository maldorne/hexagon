#ifndef FILES_HELP_H
#define FILES_HELP_H

#define HELP_HANDLER        "/lib/handlers/help"
#define HELP_SAVE_FILE      "/save/help"

// how much of a document is read to find its header
#define HELP_HEADER_BYTES   512

// who a document is for; a document says so with "@tier <name>"
#define HELP_TIER_PLAYER    "player"
#define HELP_TIER_CODER     "coder"
#define HELP_TIER_ADMIN     "admin"

// a section says what it is in a file of this name inside its directory
#define HELP_SECTION_FILE   ".section"

// documents that sit directly under help/, outside any section
#define HELP_GENERAL_SECTION "general"

// the driver manual travels with DGD and is only in English
#define HELP_DRIVER_SECTION "driver"
#define HELP_DRIVER_LANG    "en"

#endif
