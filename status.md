###Project status and roadmap

- [ ] **MudOS compatibility layer**

    Being done, see the [MudOS compatibility status document](status_compat.md) for more detailed information about each function.

- [x] Include the **AuroraMud command (cmds) system**

    **Done**. This will help test and debug everything as new commands are ported.

- [x] **Test and document** the current status of the project

    Being done, as you can see both in this document and in the [MudOS compatibility status document](status_compat.md).

- [x] **heart_beats** management

    **Done**.

- [x] **Rooms and movement**

    **Done**. Environments, inventories, `move` functions, move commands between rooms.

- [x] **`exec` command**

    **Done**. Command to execute LPC code on the fly to help test everything.

- [x] **Login with multiple types of users**

    Login process for coders, players, administrators, etc.

- [x] **Shadows**

    Emulate MudOS shadows.

- [ ] **Full inheritance tree from the original ccmudlib**

    [x] **coder, manager, administrator** to the different user roles
    [ ] **living** to `/lib/living/living.c`
    [ ] **player** to `/lib/user.c`
    [ ] **item** to `/lib/item.c`
    [ ] **monster** to `/lib/npc.c`
