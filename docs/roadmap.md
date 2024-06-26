# Project status and roadmap

## Current version being worked on:

### [v2.1 Daggerfall](https://github.com/maldorne/hexagon/milestone/4)

| Status | Title | Description |
| :----: | :---- | ----------- |
|        | **Commands** | Review and full translation of every `lib/cmds` command. |
|        |  | :heavy_minus_sign: Test and translate every command, by user type: `admin` [[#16](https://github.com/maldorne/hexagon/issues/16)]. |
|        | **Documentation** | Give some love to the `/docs` directory [[#7](https://github.com/maldorne/hexagon/issues/7)]. |
|        | **Player actions** | Review every communications action [[#23](https://github.com/maldorne/hexagon/issues/23)]. |
|        | **Skills** | Review skill system [[#25](https://github.com/maldorne/hexagon/issues/25)]. |

## Future versions: 

Set of features not yet started, but planned.

### [v2.2 Edoras](https://github.com/maldorne/hexagon/milestone/5)

| Status | Title | Description |
| :----: | :---- | ----------- |
|        | **Quests** | Review quest system [[#26](https://github.com/maldorne/hexagon/issues/26)]. |

## Previous versions/milestones

### [v1.24.04 Castle Black](https://github.com/maldorne/hexagon/milestone/3)

When finished, this version will be renamed as `v2.0` and it will be the last `alpha` version.

Set of features we are currently working on:

| Status | Title | Description |
| :----: | :---- | ----------- |
|        | **Commands** | Review and full translation of every `lib/cmds` command. |
| :white_check_mark: |  | :heavy_minus_sign: Add compatibility and status list in the repository ([done](commands.md)). |
| :white_check_mark: |  | :heavy_minus_sign: Test and translate every command, by user type: `login` [[#13](https://github.com/maldorne/hexagon/issues/13)]. |
| :white_check_mark: |  | :heavy_minus_sign: Test and translate every command, by user type: `player` [[#14](https://github.com/maldorne/hexagon/issues/14)]. |
| :white_check_mark: |  | :heavy_minus_sign: Test and translate every command, by user type: `coder` [[#15](https://github.com/maldorne/hexagon/issues/15)]. |
| :white_check_mark: | **Containers** | Dockerfile to execute the mud inside a container, and related documentation. |
| :white_check_mark: | **Encoding** | Review files in the repository, not all of them are using the same encoding [[#20](https://github.com/maldorne/hexagon/issues/20)]. |
| :white_check_mark: | **Basic objects** | Full inheritance tree from the original `ccmudlib`, objects related to combat [[#21](https://github.com/maldorne/hexagon/issues/21)]. |
| :white_check_mark: |  | :heavy_minus_sign: `weapon.c` |
| :white_check_mark: |  | :heavy_minus_sign: `armour.c` |
| :white_check_mark: |  | :heavy_minus_sign: `shield.c` |
| :white_check_mark: | **Basic binary connections** | Start working with binary connections to allow future development of httpd, ftpd, etc [[#22](https://github.com/maldorne/hexagon/issues/22)]. |

### [v1.21.09 Beregost](https://github.com/maldorne/hexagon/releases/tag/1.21.09)

| Status | Title | Description |
| :----: | :---- | ----------- |
| :white_check_mark: | **Product Identity** | Logo and banner for the `readme.md` home document [[#5](https://github.com/maldorne/hexagon/issues/5)]. |
| :white_check_mark: | **Improve `query_verb()`** | Same treatment as the initiator objects to be sure we will never lose the original verb/command [[#9](https://github.com/maldorne/hexagon/issues/9)]. Done with a full review and improvement of the `mudos.c` inner state, now using `query_execution_context()` and `restore_execution_context()`. |
| :white_check_mark: | **Multi Game** | Finish the multigame features to allow hosting different games in the same server. |
| :white_check_mark: |  | :heavy_minus_sign: Default test area as game example (`/games/demo-fantasy`). |
| :white_check_mark: | **Continuous integration** | Automatic deploy to hosting when the master branch of this project is updated [[#6](https://github.com/maldorne/hexagon/issues/6)]. |
| :white_check_mark: |  | :heavy_minus_sign: Deploy simultaneously to both english and spanish servers. |
| :white_check_mark: | **Basic objects** | Full inheritance tree from the original `ccmudlib`, some files pending review since the [v1.19.05](https://github.com/maldorne/hexagon/tree/19.05) milestone. |
| :white_check_mark: |  | :heavy_minus_sign: **monster** to `/lib/monster.c` |


### [v1.20.09 Anduin](https://github.com/maldorne/hexagon/releases/tag/1.20.09)

| Status | Title | Description |
| :----: | :---- | ----------- | 
| :white_check_mark: | **Multilanguage** | Allow multilanguage games. The language will be loaded in compilation time, using header files located automatically by the driver. |
| :white_check_mark: | **`this_player()` and `this_user()` review** | Full review of the initiator objects, during heartbeats, `call_out`, login, `move` and input/command execution. |
| :white_check_mark: | **User and player objects** | Separate the `user` object in `user` and `player`. Now there are two different linked objects, the `player` handles every in-game situation, while the `user` handles the connection, account info, input and output. |
| :white_check_mark: | **Login user type** | Basic user type to handle the connection with an account, creations of player characters, etc. |
| :white_check_mark: |  | :heavy_minus_sign: Allow execution of certain type of commands (`who`, `finger`, etc) during the login process. |
| :white_check_mark: |  | :heavy_minus_sign: Full review of the login process. Simplify the `login` object refactoring anything possible to the new login commands [[#1](https://github.com/maldorne/hexagon/issues/1)]. |
| :white_check_mark: | **`visible_strlen`** | Be able to calculate the real length of a string having in mind only the characters visible to the player [[#2](https://github.com/maldorne/hexagon/issues/2)]. |
| :white_check_mark: | **Deprecate manager user** | Remove code related to the `manager` user type. We have enough information with `coder` and `administrator`. |


### Up to [v1.19.05](https://github.com/maldorne/hexagon/releases/tag/1.19.05)

After version 1.19.05, we started working with milestones, prior to that, we only had a list of tasks to be done. The global list stays here for historical reasons.

| Status | Title | Description |
| :----: | :---- | ----------- | 
| :white_check_mark: | **MudOS compatibility** | Being done, see the [MudOS compatibility status document](compatibility.md) for more detailed information about each function. |
| :white_check_mark: | **Cmds system** | Include an implementation of the **AuroraMud command (cmds) system**. This will help test and debug everything as new commands are ported. |
| :white_check_mark: | **Test and document** | Test and document the current status of the project. We use this document and the [MudOS compatibility status document](compatibility.md). |
| :white_check_mark: | **Heartbeats** | Handling of the object "turns" and pending actions. |
| :white_check_mark: | **Rooms and movement** | Environments, inventories, `move` functions, move commands between rooms. `init` function calling. |
| :white_check_mark: | **Actions and `this_player()`** | Actions, `init` function calling, `this_player()`, `query_verb()` and `notify_fail()`. |
| :white_check_mark: | **`exec` command** |  Command to execute LPC code on the fly to help test everything. |
| :white_check_mark: | **User types and login** | Players, coders, managers, administrators, etc. and their login process. |
| :white_check_mark: | **Shadows** | Emulate MudOS shadows. |
| :white_check_mark: | **Basic objects** | Full inheritance tree from the original `ccmudlib` |
| :white_check_mark: |  | :heavy_minus_sign: **coder, manager, administrator** to the different user roles. |
| :white_check_mark: |  | :heavy_minus_sign: **living** to `/lib/living/living.c`. |
| :white_check_mark: |  | :heavy_minus_sign: **player** to `/lib/player.c` (avatar in-game handling) and `/lib/user.c` (user account, connection, input and output) |
| :white_check_mark: |  | :heavy_minus_sign: **item** to `/lib/item.c`. |
| :white_check_mark: | **Migrations** | User migrations to update user data. |

For more information about the different versions of the project, take a look to the [about page in maldorne.org](https://maldorne.org/about/).
