# Project status and roadmap

## Current milestone: [v19.10 Anduin](https://github.com/houseofmaldorne/hexagon/milestone/1)

Set of features we are currently working on. 

| Status | Title | Description |
| :----: | :---- | ----------- | 
| :white_check_mark: | **Multilanguage** | Allow multilanguage games. The language will be loaded in compilation time, using header files located automatically by the driver. |
| :white_check_mark: | **`this_player()` and `this_user()` review** | Full review of the initiator objects, during heartbeats, `call_out`, login, `move` and input/command execution. |
| :white_check_mark: | **User and player objects** | Separate the `user` object in `user` and `player`. Now there are two different linked objects, the `player` handles every in-game situation, while the `user` handles the connection, account info, input and output. |
|                    | **Login user type** | Basic user type to handle the connection with an account, creations of player characters, etc. |
| :white_check_mark: |  | :heavy_minus_sign: Allow execution of certain type of commands (`who`, `finger`, etc) during the login process. |
|                    |  | :heavy_minus_sign: Full review of the login process. Simplify the `login` object refactoring anything possible to the new login commands. |
|                    |  | :heavy_minus_sign: On disconnect, change the user type to allow changing to a different player character. |
| :white_check_mark: | **`visible_strlen`** | Be able to calculate the real length of a string having in mind only the characters visible to the player. |
| :white_check_mark: | **Deprecate manager user** | Remove code related to the `manager` user type. We have enough information with `coder` and `administrator`. |

## Next milestone: v19.?? Beregost

Set of features not yet started, but planned.

| Status | Title | Description |
| :----: | :---- | ----------- | 
|        | **Improve `query_verb()`** | Same treatment as the initiator objects to be sure we will never lose the original verb/command. |

## Previous milestones

### Up to [v19.05](https://github.com/houseofmaldorne/hexagon/tree/19.05)

After version 19.05, we started working with milestones, prior to that, we only had a list of tasks to be done. The global list stays here for historical reasons.

| Status | Title | Description |
| :----: | :---- | ----------- | 
| :white_check_mark: | **MudOS compatibility** | Being done, see the [MudOS compatibility status document](compatibility.md) for more detailed information about each function. |
| :white_check_mark: | **Cmds system** | Include an implementation of the **AuroraMud command (cmds) system**. This will help test and debug everything as new commands are ported. |
| :white_check_mark: | **Test and document** | Test and document the current status of the project. We use this document and the [MudOS compatibility status document](compatibility.md). |
| :white_check_mark: | **Heartbeats** | Handling of the object "turns" and pending actions. |
| :white_check_mark: | **Rooms and movement** | Environments, inventories, `move` functions, move commands between rooms. `init` function calling. |
| :white_check_mark: | **Actions and `this_player()`** | Actions, `init` function calling, `this_player()`, `query_verb()` and `notify_fail()`. |
| :white_check_mark: | **`exec` command** |  Command to execute LPC code on the fly to help test everything. |
| :white_check_mark: | **User types and login** | Players, coders, managers, administrators, etc. and their login process. |
| :white_check_mark: | **Shadows** | Emulate MudOS shadows. |
| :white_check_mark: | **Basic objects** | Full inheritance tree from the original `ccmudlib` |
| :white_check_mark: |  | :heavy_minus_sign: **coder, manager, administrator** to the different user roles. |
| :white_check_mark: |  | :heavy_minus_sign: **living** to `/lib/living/living.c`. |
| :white_check_mark: |  | :heavy_minus_sign: **player** to `/lib/player.c` (avatar in-game handling) and `/lib/user.c` (user account, connection, input and output) |
| :white_check_mark: |  | :heavy_minus_sign: **item** to `/lib/item.c`. |
|         :x:        |  | :heavy_minus_sign: **monster** to `/lib/npc.c` |
| :white_check_mark: | **Migrations** | User migrations to update user data. |
