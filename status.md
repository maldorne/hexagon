###Project status and roadmap

- [ ] **MudOS compatibility layer**

    Being done, see the [MudOS compatibility status document](status_compat.md) for more detailed information about each function.

- [x] Include the **AuroraMud command (cmds) system**

    **Done**. This will help test and debug everything as new commands are ported.

- [x] **Test and document** the current status of the project

    Being done, as you can see both in this document and in the [MudOS compatibility status document](status_compat.md).

- [x] **heart_beats** management

    **Done**. Idea (*improvement from MudOS*): have different priority lists for different `hbs`: every second, every two seconds (default), every five seconds, etc.

- [x] **Rooms and movement**

    **Done**. Environments, inventories, `move` functions, move commands between rooms.

- [x] **`exec` command**

    **Done**. Command to execute LPC code on the fly to help test everything.

- [ ] **Login with multiple types of users**

    Login process for coders, players, administrators, etc.
