#ifndef CHARACTER_H
#define CHARACTER_H

// 플레이어 위치 상태 열거형
typedef enum {
    LOC_ROAD,
    LOC_CAVE,
    LOC_TOWN
} Location;

typedef struct Player {
    char name[20];

    int hp;
    int maxHp;
    int gold;

    int Str;
    int Dex;
    int Int;
    int Def;

    int turnCount;
    int lastTownTurn;
    int lastCaveTurn;

    char* inventory[100];

    Location location; // 현재 플레이어 위치(상태)
} Player;

typedef struct Enemy {
    char name[20];
    int hp;
    int maxHp;
    int gold;
    int Str;
} Enemy;

// 초기화 함수
void initPlayer(Player* ch);
void initEnemy(Enemy* ch, const char* name, int hp, int str, int gold);
void printPlayerStatus(Player* ch);

#endif
