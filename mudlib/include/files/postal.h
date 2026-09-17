
#ifndef __POSTAL_H
#define __POSTAL_H

#define POSTAL_D "/lib/handlers/postal"
#define MAILER_OB "/lib/obj/mailer"

// The objects allowed to change a mailbox through the postal handler.
#define TRUSTED_MAILERS ({ MAILER_OB, "/lib/player", \
                           "/lib/core/secure/retire", "/lib/core/secure/coders" })

// Each character's mailbox, inside their save directory.
#define MAILBOX_SAVE "mailbox"

// Letters older than this are removed when their owner logs in.
#define MAIL_AGED 60*60*24*62        // Two months

#define MAX_MAIL_SUBJECT_LENGTH 60

#endif
