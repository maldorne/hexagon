
#ifndef __EDITOR_H
#define __EDITOR_H

#define EDITOR_OB "/lib/obj/editor"

#define EDITOR_MODE_MENU    "menu"
#define EDITOR_MODE_COMMAND "command"
#define EDITOR_MODE_ED      "ed"
#define EDITOR_MODES ({ EDITOR_MODE_MENU, EDITOR_MODE_COMMAND, EDITOR_MODE_ED })

#define EDITOR_END_OF_TEXT "**"
#define EDITOR_TMP_DIR "/tmp/editor/"

#endif
