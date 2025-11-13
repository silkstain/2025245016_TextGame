#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Character.h"


void initPlayer(Player* player) {
    // 난수 시드는 main에서 한 번만 초기화하도록 변경되어 있으므로 여기서는 제거합니다.
    // srand((unsigned int)time(NULL));

    printf("플레이어 이름을 입력하세요: ");
    scanf("%s", player->name);
    player->hp = 100;
    player->maxHp = 100;
    player->gold = 100;
    player->Str = 5;
    player->Dex = 5;
    player->Def = 5;
    player->Int = 5;
    player->turnCount = 0;
    player->lastTownTurn = -999;
    player->lastCaveTurn = -999;

    for (int i = 0; i < 100; i++) {
        player->inventory[i] = NULL;
    }

    player->location = LOC_ROAD; // 기본 위치: 길거리
}

void initEnemy(Enemy* enemy, const char* name, int hp, int str, int gold) {
    snprintf(enemy->name, sizeof(enemy->name), "%s", name);
    enemy->hp = hp;
    enemy->maxHp = hp;
    enemy->gold = gold + (rand() % 31); // 0..30 추가
    enemy->Str = str;
}

void printPlayerStatus(Player* player) {
    printf("\n┌────────────────────────────┐\n");
    printf("│ 이름: %-10s HP: %3d/%3d \n", player->name, player->hp, player->maxHp);
    printf("│ 골드: %-5d 힘:%2d 민첩:%2d 지능:%2d \n",
        player->gold, player->Str, player->Dex, player->Int);
    printf("└────────────────────────────┘\n");
}
