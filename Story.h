#ifndef STORY_H
#define STORY_H

#include "Player.h"

void storyEvent_Start(Player* player);
void storyEvent_Battle(Player* player);
void storyEvent_Treasure(Player* player);
void storyEvent_End(Player* player);

#endif
