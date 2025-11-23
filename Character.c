#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Character.h"

void initPlayer(Player* player) {
    printf("플레이어 이름을 입력하세요: ");
    scanf("%19s", player->name);
    while (getchar() != '\n');

    player->hp = 100;
    player->maxHp = 100;
    player->gold = 100;

    player->Str = 10;
    player->Dex = 10;
    player->Def = 10;
    player->Int = 10;

    player->turnCount = 0;
    player->lastTownTurn = -999;
    player->lastCaveTurn = -999;
	player->lastCampingTurn = -999;

    player->heardLegend = 0;
    player->hasHolySword = 0;
    player->hasCityPass = 0;

    for (int i = 0; i < 100; i++)
        player->inventory[i] = NULL;

    player->location = LOC_ROAD;

    addItem(player, "HP 포션");
    addItem(player, "HP 포션");
    addItem(player, "HP 포션");
}

void initEnemy(Enemy* enemy, const char* name, int hp, int attack, int gold) {
    strncpy(enemy->name, name, sizeof(enemy->name) - 1);
    enemy->name[19] = '\0';
    enemy->hp = hp;
    enemy->maxHp = hp;
    enemy->attack = attack;
    enemy->gold = gold + (rand() % 31);
}

void printPlayerStatus(const Player* p) {
    printf("\n┌──────────────────────────────────────────────┐\n");
    printf("│ 이름: %-10s HP: %3d/%3d\n", p->name, p->hp, p->maxHp);
    printf("│ 골드: %-5d 힘:%2d 민첩:%2d 방어:%2d 지능:%2d\n",
        p->gold, p->Str, p->Dex, p->Def, p->Int);
    printf("└──────────────────────────────────────────────┘\n");
}

void addItem(Player* p, const char* item) {
    for (int i = 0; i < 100; i++) {
        if (p->inventory[i] == NULL) {
#ifdef _WIN32
            p->inventory[i] = _strdup(item);
#else
            p->inventory[i] = strdup(item);
#endif
            printf("%s을(를) 획득했습니다!\n", item);
            return;
        }
    }
    printf("인벤토리가 가득 찼습니다.\n");
}

void printInventory(const Player* p) {
    printf("\n[인벤토리]\n");
    int empty = 1;
    for (int i = 0; i < 100; i++) {
        if (p->inventory[i]) {
            printf("%2d) %s\n", i, p->inventory[i]);
            empty = 0;
        }
    }
    if (empty) printf("(비어 있음)\n");
}

void useItem(Player* p, int index) {
    if (index == -1) {
        printInventory(p);
        printf("사용할 아이템 번호 입력(-1 취소): ");
        scanf("%d", &index);
        while (getchar() != '\n');
        if (index < 0) {
            printf("취소했습니다.\n");
            return;
        }
    }

    if (index < 0 || index >= 100 || p->inventory[index] == NULL) {
        printf("잘못된 번호입니다.\n");
        return;
    }

    char* item = p->inventory[index];
    if (strcmp(item, "HP 포션") == 0) {
        p->hp += 30;
        if (p->hp > p->maxHp) p->hp = p->maxHp;
        printf("HP 포션 사용! 현재 HP: %d\n", p->hp);
    }

    free(p->inventory[index]);
    p->inventory[index] = NULL;
}
