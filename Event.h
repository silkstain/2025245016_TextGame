#ifndef EVENT_H
#define EVENT_H
#include "Character.h"

void printDivider(const char* title);
void EnterToContinue();
void ClearScreen();

// 시작 이벤트
void StoryEvent_Start(Player* player);

// 각각의 위치 처리
void Event_Road(Player* player);
void Event_Town(Player* player);
void Event_Cave(Player* player);

// Road 내부 이벤트
void TreasureEvent(Player* player);
void TrapEvent(Player* player);
void FondEvent(Player* player);
void Encounter_goblin(Player* player);
void RoadEvent_TownEntrance(Player* player);

// Town 내부 이벤트 3종
void Town_Pickpocket(Player* player);
void Town_StreetFood(Player* player);
void Town_Shop(Player* player);

#endif
