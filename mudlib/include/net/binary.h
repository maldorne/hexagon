
#ifndef _BINARY_H
#define _BINARY_H

#define BINARY_OB "/lib/binary"

// seconds a connection may take to show which protocol it speaks
#define BINARY_DETECT_TIMEOUT 10

// rlimits of every command issued by a binary connection
#define MAX_BINARY_DEPTH 100
#define MAX_BINARY_TICKS 10000000

#endif
