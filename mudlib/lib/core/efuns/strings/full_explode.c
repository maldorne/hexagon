
// explode counting with possible delimiters at the beginning 
// or end of the string

static nomask string * full_explode(string str, string delimiter) 
{
  string *result;
  int sz_str, sz_delimiter;
  
  result = ::explode(str, delimiter);
    
  sz_str = strlen(str);
  sz_delimiter = strlen(delimiter);

  if (str == delimiter)
    return ({ "", "" });

  if (sz_delimiter > sz_str)
    return ({ str });

  if (str[0..sz_delimiter - 1] == delimiter)
      result = ({ "" }) + result;

  if (str[sz_str - sz_delimiter..] == delimiter)
    result += ({ "" });

  return result;
}
