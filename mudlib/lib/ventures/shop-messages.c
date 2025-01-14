
#include <language.h>

static mixed buy_mess, sell_mess, list_mess, value_mess, browse_mess;

string shop_parse(string str, mixed ob, object client, string money, string extra);
void do_parse(mixed arr, mixed ob, object client, string money, string extra);

void create() 
{
  buy_mess = _LANG_SHOP_DEFAULT_BUY_MSGS;
  sell_mess = _LANG_SHOP_DEFAULT_SELL_MSGS;
  list_mess = "$extra$";
  value_mess = _LANG_SHOP_DEFAULT_VALUE_MSG;
  browse_mess = _LANG_SHOP_DEFAULT_BROWSE_MSG;
}

/* These make spiffy messages for various actions. */
void set_sell_message(mixed str) { sell_mess = str; }
void set_buy_message(mixed str) { buy_mess = str; }
void set_value_message(mixed str) { value_mess = str; }
void set_browse_message(mixed str) { browse_mess = str; }
void set_list_message(mixed str) { list_mess = str; }

mixed query_sell_mess() { return sell_mess; }
mixed query_list_mess() { return list_mess; }
mixed query_value_mess() { return value_mess; }
mixed query_buy_mess() { return buy_mess; }
mixed query_browse_mess() { return browse_mess; }

// show messages after buying or selling
void do_parse(mixed arr, mixed ob, object client, string money, string extra) 
{
  if (stringp(arr))
    write(shop_parse(arr, ob, client, money, extra));
  else
  {
    write(shop_parse(arr[0], ob, client, money, extra));
    say(shop_parse(arr[1], ob, client, money, extra));
  }
}

string shop_parse(string str, mixed ob, object client, string money, string extra) 
{
  string s1, s2, s3, rest;
  rest = "";

  while (sscanf(str,"%s$%s$%s", s1, s2, s3) == 3)
  {
    switch (s2) 
    {
      case "ob":
        if (pointerp(ob))
          str = s1 + query_multiple_short(ob) + s3;
        else
          str = s1 + ob->short() + s3;
        break;
      case "client":
        str = s1 + client->query_cap_name() + s3;
        break;
      case "extra":
        str = s1 + extra + s3;
        break;
      case "money":
        str = s1 + money + s3;
        break;
      default:
        rest = s1 + "$" + s2 + "$";
        str = s3;
        break;
    }
  }

  return rest + str;
}
