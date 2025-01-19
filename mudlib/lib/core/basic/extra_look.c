
private static object * extra_looks;

void create() 
{
	extra_looks = ({ });
}

string calc_extra_look() 
{
	int i;
	string s1, s2;
	s1 = "";

	if (!extra_looks)
		extra_looks = ({ });

	for (i = 0; i < sizeof(extra_looks); i++)
	{
		if (extra_looks[i]) 
		{
			s2 = (string) extra_looks[i]->extra_look();
			if (s2)
				s1 += s2;
		} 
		else 
		{
			extra_looks = delete(extra_looks, i, 1);
			i--;
		}
	}
  
	return s1;
}

void add_extra_look(object ob) 
{
	if (member_array(ob, extra_looks) != -1) 
		return;
	extra_looks += ({ ob });
}

void remove_extra_look(object ob) 
{
	int i;
	i = member_array(ob, extra_looks);

	if (i < 0)
		return;

	extra_looks = delete(extra_looks, i, 1);
}

object * query_extra_look_list() 
{
	return extra_looks;
}

// stats added
mixed * stats()
{
	mixed * ret;
	ret = ({
		({"Extra Looks (nosave)", extra_looks, }),
			});
	return ret;
}

