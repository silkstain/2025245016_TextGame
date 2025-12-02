#ifndef EVENT_H
#define EVENT_H
#include "Character.h"

/* 유틸 */
void printDivider(const char* title);
void EnterToContinue();
void ClearScreen();
int StatCheck(const char* statName, int statValue, int difficulty);

/* 스토리 */
void Prologue_Event(Player* player);
void StoryEvent_Start(Player* player);

/* 메인 위치 디스패처 */
void Event_Road(Player* player);
void Event_Town(Player* player);
void Event_Cave(Player* player);
void Event_Forest(Player* player);

/* ROAD 이벤트 */
void RoadEvent_SmallTownEntrance(Player* player);
void RoadEvent_CaveEntrance(Player* player);
void RoadEvent_ForestEntrance(Player* player);
void RoadEvent_HugeCityEntrance(Player* player);
void RoadEvent_DemonCastleEntrance(Player* player);
void TreasureEvent(Player* player);
void TrapEvent(Player* player);
void FondEvent(Player* player);
void Encounter_goblin(Player* player);
void FindFood(Player* player);
void FallenTreeEvent(Player* player);
void BanditAmbushEvent(Player* player);
void TravelingSageEvent(Player* player);
void MeteorShardEvent(Player* player);
void CampingEvent(Player* player);

/* Town 상세 */
void Town_Inn(Player* player);
void Town_Shop(Player* player);
void Town_Blacksmith(Player* player);
void Town_Legend(Player* player);


/* Cave 이벤트 */
void Cave_BatSwarm(Player* player);
void Cave_ShiningOre(Player* player);
void Cave_PoisonMist(Player* player);
void Cave_DeadAdventurer(Player* player);
void Cave_StoneGolem(Player* player);
void Cave_FallingFloor(Player* player);
void Cave_UndergroundLake(Player* player);
void Cave_AncientCarving(Player* player);
void Cave_DeadGuard(Player* player);

/* 대도시 이벤트 */
void Event_BigCity(Player* player);
void BigCity_Inn(Player* player);
void BigCity_Gamble(Player* player);
void BigCity_Arena(Player* player);
void BigCity_Temple(Player* player);
void BigCity_PremiumShop(Player* player);
void BigCity_MageGuild(Player* player);
void BigCity_AdventurerGuild(Player* player);
void BigCity_Park(Player* player);
void BigCity_Blacksmith(Player* player);
void BigCity_RoyalPalace(Player* player);

/* 마왕성 이벤트 */
void DemonKingCastle_Entrance(Player* player);
#endif