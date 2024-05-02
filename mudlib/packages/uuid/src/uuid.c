
string uuid()
{
  // This is a "version 4" uuid.  This is based on random numbers, the others use time.
  // https://en.wikipedia.org/wiki/Universally_unique_identifier#Version_4_(random)
  // supposed to use lower case but accept upper case (so run lower_case when comparing incoming stuff)
  
  return sprintf("%04x%04x-%04x-4%03x-%0x%03x-%04x%04x%04x",
    random(0x10000),random(0x10000),random(0x10000),random(0x1000),
    (8+random(4)), random(0x1000),random(0x10000),random(0x10000),random(0x10000));
}
