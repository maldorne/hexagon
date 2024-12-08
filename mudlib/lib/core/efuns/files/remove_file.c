
static nomask int remove_file(string name)
{
  debug("files", "remove_file: " + name + "\n");
  return ::remove_file(name);
}

static nomask int rm(string name)
{
  return remove_file(name);
}
