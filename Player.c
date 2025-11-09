#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Player.h"

void initPlayer(Player* player) {
    srand((unsigned int)time(NULL));
    printf("플레이어 이름을 입력하세요: ");
    scanf("%s", player->name);

    player->hp = 100;
    player->maxHp = 100;
    player->gold = 100;
    player->Str = 5;
    player->Dex = 5;
    player->Int = 5;
}

void printPlayerStatus(Player* player) {
    printf("\n┌────────────────────────────┐\n");
    printf("│ 이름: %-10s HP: %3d/%3d \n", player->name, player->hp, player->maxHp);
    printf("│ 골드: %-5d 힘:%2d 민첩:%2d 지능:%2d \n",
        player->gold, player->Str, player->Dex, player->Int);
    printf("└────────────────────────────┘\n");
}
