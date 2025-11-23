#ifndef EVENT_H
#define EVENT_H
#include "Character.h"

/* 유틸 */
void printDivider(const char* title);
void EnterToContinue();
void ClearScreen();

/* 스토리 */
void Prologue_Event(Player* player);
void StoryEvent_Start(Player* player);
void Forest_HolySword(Player* player);
void DemonKingCastle_Entrance(Player* player);

/* 메인 위치 디스패처 */
void Event_Road(Player* player);
void Event_Town(Player* player);
void Event_Cave(Player* player);
void Event_Forest(Player* player);
void Event_DemonCastle(Player* player);

/* ROAD 이벤트 */
void RoadEvent_SmallTownEntrance(Player* player);
void RoadEvent_CaveEntrance(Player* player);
void RoadEvent_ForestEntrance(Player* player);
void TreasureEvent(Player* player);
void TrapEvent(Player* player);
void FondEvent(Player* player);
void Encounter_goblin(Player* player);
void FindFood(Player* player);
void CampingEvent(Player* player);
void FallenTreeEvent(Player* player);
void BanditAmbushEvent(Player* player);
void TravelingSageEvent(Player* player);
void MeteorShardEvent(Player* player);
void RoadEvent_HugeCityEntrance(Player* player);

/* Camping */
void CampingEvent(Player* player);

/* Town 상세 */
void Town_Inn(Player* player);
void Town_StreetFood(Player* player);
void Town_Shop(Player* player);
void Town_MarketStall(Player* player);
void Town_Volunteer(Player* player);
void Town_Blacksmith(Player* player);
void Town_GuardCheck(Player* player);
void Town_Square(Player* player);
void Town_Gamble(Player* player);
void Town_Training(Player* player);
void Town_Pickpocket(Player* player);
void Town_Legend(Player* player);
void Town_Library(Player* player);
void Town_Arena(Player* player);
void Town_AlchemyLab(Player* player);
void Town_ThievesGuild(Player* player);

/* Cave Events */
void Cave_BatSwarm(Player* player);
void Cave_ShiningOre(Player* player);
void Cave_PoisonMist(Player* player);
void Cave_DeadAdventurer(Player* player);
void Cave_StoneGolem(Player* player);
void Cave_FallingFloor(Player* player);
void Cave_UndergroundLake(Player* player);
void Cave_AncientCarving(Player* player);
void Cave_DeadGuard(Player* player);

/* 대도시(Big City) 허브 */
void Event_BigCity(Player* player);

/* 대도시 시설 상세 이벤트 */
void BigCity_Temple(Player* player);
void BigCity_PremiumShop(Player* player);
void BigCity_MageGuild(Player* player);
void BigCity_AdventurerGuild(Player* player);
void BigCity_Park(Player* player);

/* 마왕성(성) 내부 이벤트 */
void Castle_CursedHall(Player* player);
void Castle_ShadowKnight(Player* player);
void Castle_DarkAltar(Player* player);
void Castle_MaliceStorm(Player* player);
void Castle_FinalBattle(Player* player);

#endif
