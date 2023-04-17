# Current status: Commands

List of commands available to every player, depending of its user type/role (`player`, `coder`, `admin`, and the common type `login` always available, even in the login screen before choosing a player character).

**Be aware**: most of the commands listed here are already implemented and working (might not be translated), but as I've just started working in this document recently, I'll update every entry as I test each command.

## Login (see issue [#13](https://github.com/maldorne/hexagon/issues/13))

| Command | Working in Hexagon | Translated en/es | Help available | Notes |
| ------- | :----------------: | :--------------: | :------------: |----- |
| characters | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| finger     | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| help       | :white_check_mark: | :white_check_mark: | :white_check_mark: | Pending organizing some docuemnts, but the help command is working (see issue [#7](https://github.com/maldorne/hexagon/issues/7)). |
| who        | :white_check_mark: | :white_check_mark: | :white_check_mark: | The weather time (depends of the game) has to be translated yet. |
| games      | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |

## Admin (see issue [#16](https://github.com/maldorne/hexagon/issues/16))

Admin commands will be only in english, except texts shown to the players, which will be multilanguage.

| Command | Working in Hexagon | Translated en/es | Help available | Notes |
| ------- | :----------------: | :--------------: | :------------: |----- |
| ban        |          |          |    |    |
| clonein    |          |          |    |    |
| dirs       |          |          |    |    |
| domain     |          |          |    |    |
| domainwho  |          |          |    |    |
| donde      |          |          |    |    |
| echoall    |          |          |    |    |
| fds        |          |          |    |    |
| findfile   |          |          |    |    |
| fsearch    |          |          |    |    |
| guildwho   |          |          |    |    |
| ident      |          |          |    |    |
| in         |          |          |    |    |
| ll         |          |          |    |    |
| memory     |          |          |    |    |
| memtree    |          |          |    |    |
| money_info |          |          |    |    |
| netstat    |          |          |    |    |
| people     |          |          |    |    |
| qatt       |          |          |    |    |
| qc         |          |          |    |    |
| qpeople    |          |          |    |    |
| register   |          |          |    |    |
| resolve    |          |          |    |    |
| rotate     |          |          |    |    |
| shutdown   |          |          |    |    |
| singletons |          |          |    |    |
| snetstat   |          |          |    |    |
| snoopers   |          |          |    |    |
| sockets    |          |          |    |    |
| stats      | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| status     |          |          |    |    |
| su         |          |          |    |    |
| suspend    |          |          |    |    |
| terms      |          |          |    |    |
| unicos     |          |          |    |    |
| whois      |          |          |    |    |
| xdate      |          |          |    |    |
| xfinger    |          |          |    |    |

## Coder (see issue [#15](https://github.com/maldorne/hexagon/issues/15))

Coder commands will be only in english, except texts shown to the players, which will be multilanguage.

| Command | Working in Hexagon | Translated en/es | Help available | Notes |
| ------- | :----------------: | :--------------: | :------------: |----- |
| actions      | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| armours      | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| backup       | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| cat          | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| clone        | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| compare      | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| cp           | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| cplines      | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| discard      |          |          |    |    |
| du           |          |          |    |    |
| exec         | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| exits        | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| ftpwho       |          |          |    |    |
| goto         | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| grep         | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| head         | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| home         | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| invisible    |          |          |    |    |
| ioulist      |          |          |    |    |
| load         |          |          |    |    |
| ls           | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| mail         |          |          |    |    |
| malloc       |          |          |    |    |
| man          |          |          |    |    |
| mkdir        |          |          |    |    |
| more         | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| mudlist      |          |          |    |    |
| multi        |          |          |    |    |
| mv           |          |          |    |    |
| nls          |          |          |    |    |
| peace        | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| resistencias |          |          |    |    |
| restore      | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| rm           | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| rmdir        |          |          |    |    |
| sar          | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| tail         | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| uptime       | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| visible      |          |          |    |    |
| wget         |          |          |    |    |

## Player (see issue [#14](https://github.com/maldorne/hexagon/issues/14))

| Command | Working in Hexagon | Translated en/es | Help available | Notes |
| ------- | :----------------: | :--------------: | :------------: |----- |
| away          | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| bury          | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| ~calendar~      |          |          |    | Converted to game cmd [#27](https://github.com/maldorne/hexagon/issues/27) |
| commands      | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| combats       | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| condition     | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| configuration | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| consider      | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| cost          | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| diplomacy     |          |          |    | Postponed for now |
| encumbrance   | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| equipment     | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| glance        | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| grope         | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| group         |          |          |    | Postponed for now |
| inform        | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| inventory     | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| kill          | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| look          | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| map           | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| money         | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| past          | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| pov           | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| prompt        | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| quests        |          |          |    | [#26](https://github.com/maldorne/hexagon/issues/26) |
| ranking       |          |          |    | Postponed for now |
| retire        | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| score         | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| ~search~        |          |          |    | Converted to skill [#25](https://github.com/maldorne/hexagon/issues/25) |
| sheet         | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| stop          | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| time          | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| travel        | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| verbose       | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| whoami        | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| wimpy         | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |

## Legend

| Symbol | Meaning |
| :----: | ------- |
| :white_check_mark: | Already tested. |
| :heavy_minus_sign: | Not included/necessary. |
| :x:                | Not at the moment. |
