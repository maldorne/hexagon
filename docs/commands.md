# Current status: Commands

List of commands available to every player, depending of its user type/role (`player`, `coder`, `admin`, and the common type `login` always available, even in the login screen before choosing a player character).

**Be aware**: most of the commands listed here are already implemented and working (might not be translated), but as I've just started working in this document recently, I'll update every entry as I test each command.

## Login (see issue [#13](https://github.com/maldorne/hexagon/issues/13))

| Command    | Working in Hexagon |  Translated en/es  |   Help available   | Notes                                                                                                                              |
| ---------- | :----------------: | :----------------: | :----------------: | ---------------------------------------------------------------------------------------------------------------------------------- |
| characters | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                                                                                    |
| finger     | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                                                                                    |
| help       | :white_check_mark: | :white_check_mark: | :white_check_mark: | Pending organizing some docuemnts, but the help command is working (see issue [#7](https://github.com/maldorne/hexagon/issues/7)). |
| who        | :white_check_mark: | :white_check_mark: | :white_check_mark: | The weather time (depends of the game) has to be translated yet.                                                                   |
| games      | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                                                                                    |

## Admin (see issue [#16](https://github.com/maldorne/hexagon/issues/16))

Admin commands will be only in english, except texts shown to the players, which will be multilanguage.

| Command    | Working in Hexagon |  Translated en/es  |   Help available   | Notes |
| ---------- | :----------------: | :----------------: | :----------------: | ----- |
| areas      | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |
| ban        | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |
| cleaner    | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |
| components | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |
| diplomacy  | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |
| echoall    | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: | Anonymous line to everybody connected; `emoteall` is the signed half. |
| emoteall   | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: | Was an action in `coder_communicate.c`. |
| families   | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |
| fights     | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: | Was `qatt`; covers every living now, not only players. |
| houses     | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |
| memory     | :white_check_mark: | :heavy_minus_sign: |                    |  |
| money-info | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: | Was `money_info`. |
| npcs       | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |
| parties    | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |
| people     | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: | Absorbed `dirs`, `netstat`, `qpeople`, `terms` and `snetstat`. |
| pks        | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |
| pois       | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |
| preloads   | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |
| qsnoop     | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: | Quiet half of `snoop`, which is a coder command. |
| room2loc   | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |
| rotate     | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: | Room zones only; a converted area keeps its exits in its locations. |
| sectors    | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |
| shutdown   | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |
| singletons | :white_check_mark: | :heavy_minus_sign: |                    |  |
| snoopers   | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |
| stats      | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |
| status     | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: | Reads the `status()` kfun; takes an object. |
| suspend    | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |
| targets    | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |
| uniques    | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: | Was `unicos`. A stub until the unique-object handler exists. |
| vacancies  | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |
| ventures   | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |

`register` is parked in `cmds/admin/postponed/`: it belongs to an application
system this mudlib does not have.

## Coder (see issue [#15](https://github.com/maldorne/hexagon/issues/15))

Coder commands will be only in english, except texts shown to the players, which will be multilanguage.

| Command     | Working in Hexagon |  Translated en/es  |   Help available   | Notes                                                |
| ----------- | :----------------: | :----------------: | :----------------: | ---------------------------------------------------- |
| actions     | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| armours     | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| backup      | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| cat         | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| clone       | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                      |
| compare     | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| cp          | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| cplines     | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| discard     | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| du          | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| exec        | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| exits       | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| findfile    | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: | Was an admin command; finds a file by name anywhere under a directory. |
| fsearch     | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: | Was an admin command; lists the functions a file defines. |
| ~ftpwho~    |                    |                    |                    | Moved to the `ftpd` package.                         |
| goto        | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| grep        | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| head        | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| heal        | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                      |
| home        | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| invisible   | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| ioulist     | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| load        | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| ls          | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| mail        | :white_check_mark: | :white_check_mark: | :white_check_mark: | [#40](https://github.com/maldorne/hexagon/issues/40) |
| malloc      | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| ~man~       |                    |                    |                    | [#7](https://github.com/maldorne/hexagon/issues/7)   |
| mkdir       | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| more        | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| ~mudlist~   |                    |                    |                    | Moved to the `intermud` package.                     |
| multi       | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| mv          | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| nls         | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| peace       | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                      |
| props       | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: | Inspect and set properties. |
| rain        | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: | Weather for testing. |
| resistances | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| restore     | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| rm          | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| rmdir       | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| sar         | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| snoop       | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: | Watch another session; `qsnoop` is the quiet, admin-only half. |
| tail        | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| testcolors  | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: | Print the colour table as this terminal renders it. |
| uptime      | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| visible     | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |                                                      |
| whereis     | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: | Was an action in `coder_object.c`; finds anything alive by name. |

## Player (see issue [#14](https://github.com/maldorne/hexagon/issues/14))

| Command       | Working in Hexagon |  Translated en/es  |   Help available   | Notes                                                                      |
| ------------- | :----------------: | :----------------: | :----------------: | -------------------------------------------------------------------------- |
| away          | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| bury          | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| calendar      | :white_check_mark: | :white_check_mark: | :white_check_mark: | Game cmd [#27](https://github.com/maldorne/hexagon/issues/27)               |
| commands      | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| combats       | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| condition     | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| configuration | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| consider      | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| cost          | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| description   | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| ~diplomacy~   |                    |                    |                    | Only as an admin command for now                                           |
| encumbrance   | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| equipment     | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| glance        | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| grope         | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| group         | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| idle          | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: | Its messages are translated, the verb has no Spanish alias yet.            |
| inform        | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| inventory     | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| kill          | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| look          | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| map           | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| money         | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| past          | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| pov           | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| read          | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| prompt        | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| ~quests~      |                    |                    |                    | [#26](https://github.com/maldorne/hexagon/issues/26)                       |
| ~ranking~     |                    |                    |                    | Postponed for now                                                          |
| retire        | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| score         | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| ~search~      |                    |                    |                    | Converted to skill [#25](https://github.com/maldorne/hexagon/issues/25)    |
| sheet         | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| speak         | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| stop          | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| time          | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| travel        | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| verbose       | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| whoami        | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| wimpy         | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |
| worldmap      | :white_check_mark: | :white_check_mark: | :white_check_mark: |                                                                            |

## Meta

Commands about the command system itself, available to whoever may load code.

| Command | Working in Hexagon |  Translated en/es  |   Help available   | Notes |
| ------- | :----------------: | :----------------: | :----------------: | ----- |
| cmds    | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |
| rehash  | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |  |

## Legend

|       Symbol       | Meaning                 |
| :----------------: | ----------------------- |
| :white_check_mark: | Already tested.         |
| :heavy_minus_sign: | Not included/necessary. |
|        :x:         | Not at the moment.      |
