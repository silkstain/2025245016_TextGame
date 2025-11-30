#pragma once

typedef enum {
    LOC_ROAD,
    LOC_TOWN,
    LOC_CAVE,
    LOC_FOREST,
    LOC_BIG_CITY,
    LOC_DEMON_CASTLE
} Location;


typedef struct {
    char name[20];

    int hp, maxHp;
    int gold;

    int Str, Dex, Def, Int;

    int turnCount;
    int lastTownTurn;
    int lastCaveTurn;

    int caveDepth;
    int knowsCave;
    int heardLegend;
    int hasHolySword;
    int hasCityPass;   // 대도시 출입증 보유 여부
    int isBarrierBroken;
	// 마왕성 장벽 파괴 여부
    char* inventory[100];
    Location location;

    int lastRoadEvent;
    int lastTownEvent;
    int lastCampingTurn; /* 캠핑 강제 발생 기준 턴 기록 추가 */
} Player;

typedef struct {
    char name[20];
    int hp, maxHp;
    int attack;
    int gold;
} Enemy;

void initPlayer(Player* player);
void initEnemy(Enemy* enemy, const char* name, int hp, int attack, int gold);
void printPlayerStatus(const Player* player);
void addItem(Player* player, const char* item);
void useItem(Player* player, int index);
void printInventory(const Player* player);
void saveGame(Player* player, const char* filename);
int loadGame(Player* player, const char* filename);
