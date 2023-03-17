# Current status: Commands

List of commands available to every player, depending of its user type/role (`player`, `coder`, `admin`, and the common type `login` always available, even in the login screen before choosing a player character).

**Be aware**: most of the commands listed here are already implemented and working (might not be translated), but as I've just started working in this document recently, I'll update every entry as I test each command.

## Login (see issue [#13](https://github.com/maldorne/hexagon/issues/13))

| Command | Working in Hexagon | Translated en/es | Help available | Notes |
| ------- | :----------------: | :--------------: | :------------: |----- |
| characters | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| finger     | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| help       | :white_check_mark: | :x: | :white_check_mark: | The basic help command is translated, but its contents depend of several files not yet reviewed (see issue [#7](https://github.com/maldorne/hexagon/issues/7)). |
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
| armours      | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| backup       |          |          |    |    |
| cat          | :white_check_mark: | :heavy_minus_sign: | :white_check_mark: |    |
| clone        | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| compare      |          |          |    |    |
| cp           |          |          |    |    |
| cplines      |          |          |    |    |
| cprange      |          |          |    |    |
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
| apuntador     |          |          |    |    |
| ausente       |          |          |    |    |
| buscar        |          |          |    |    |
| cabalgar      |          |          |    |    |
| calendar      |          |          |    |    |
| carga         |          |          |    |    |
| cobardia      |          |          |    |    |
| commands      | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| combates      |          |          |    |    |
| configuration | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| considerar    |          |          |    |    |
| coste         |          |          |    |    |
| detener       |          |          |    |    |
| dinero        |          |          |    |    |
| diplomacia    |          |          |    |    |
| enterrar      |          |          |    |    |
| equipo        |          |          |    |    |
| estado        |          |          |    |    |
| glance        | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| grope         |          |          |    |    |
| grupo         |          |          |    |    |
| inform        | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| inventory     |          |          |    |    |
| kill          | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| look          | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| map           |          |          |    |    |
| monitor       |          |          |    |    |
| past          |          |          |    |    |
| prompt        | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| quests        |          |          |    |    |
| ranking       |          |          |    |    |
| retire        | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| score         | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| sheet         | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| time          |          |          |    |    |
| verbose       | :white_check_mark: | :white_check_mark: | :white_check_mark: |    |
| whoami        |          |          |    |    |

## Legend

| Symbol | Meaning |
| :----: | ------- |
| :white_check_mark: | Already tested. |
| :heavy_minus_sign: | Not included/necessary. |
| :x:                | Not at the moment. |
