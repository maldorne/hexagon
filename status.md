# Project status and roadmap

- [ ] **MudOS compatibility layer**

    Being done, see the [MudOS compatibility status document](status_compat.md) for more detailed information about each function.

- [x] Include the **AuroraMud command (cmds) system**

    **Done**. This will help test and debug everything as new commands are ported.

- [x] **Test and document** the current status of the project

    Being done, as you can see both in this document and in the [MudOS compatibility status document](status_compat.md).

- [x] **heart_beats** management

    **Done**.

- [x] **Rooms and movement**

    **Done**. Environments, inventories, `move` functions, move commands between rooms. `init` function calling.

- [x] **Actions and `this_player()`**

    **Done**. Actions, `init` function calling, `this_player()`, `query_verb()` and `notify_fail()`.

- [x] **`exec` command**

    **Done**. Command to execute LPC code on the fly to help test everything.

- [x] **Login with multiple types of users**

    **Done**. Login process for coders, players, administrators, etc.

- [x] **Shadows**

    **Done**. Emulate MudOS shadows.

- [ ] **Full inheritance tree from the original `ccmudlib`**

    - [x] **coder, manager, administrator** to the different user roles. **Done**.

    - [x] **living** to `/lib/living/living.c`. **Done**.

    - [x] **player** to `/lib/player.c` (avatar in-game handling) and `/lib/user.c` (user account, connection, input and output)

    - [x] **item** to `/lib/item.c`. **Done**.

    - [ ] **monster** to `/lib/npc.c`

- [x] **User migrations to update user data**

    **Done**.

- [x] **Separate the user object in user and player**

    **Done**. Now there are two different linked objects, the `player` handles every in-game situation, while the `user` handles the connection, account info, input and output.
