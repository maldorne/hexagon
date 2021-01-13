// Created by Paris, Apr'98. Random room generator V1.2Linux

#include "../path.h"

inherit "/lib/room.c";

#include "describe.h"

#define NUM 4

void setup()
{
  create_descriptions(NUM);
  create_exits(NUM);
  create_monsters(NUM);
}
