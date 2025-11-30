#include <stdio.h>
#include <stdlib.h>
#include "Battle.h"
#include "Character.h"
#include "Event.h"

void printBattleUI(Player* player, Enemy* enemy) {
    printf("\n====================================\n");
    printf("              전투 상황\n");
    printf("====================================\n");
    printf("플레이어: %s\n", player->name);
    printf(" HP: %3d / %3d   골드: %d\n", player->hp, player->maxHp, player->gold);
    printf("------------------------------------\n");
    printf("적: %s\n", enemy->name);
    printf(" HP: %3d / %3d\n", enemy->hp, enemy->maxHp);
    printf("====================================\n");
    printf("1. 공격\n");
    printf("2. 방어\n");
    printf("3. 아이템 사용\n");
    printf("4. 도망가기\n");
    printf("------------------------------------\n");
    printf("선택: ");
}

void battle(Player* player, Enemy* enemy) {
    int choice;
    int defending = 0;

    printf("\n*** 전투 시작! ***\n");

    while (player->hp > 0 && enemy->hp > 0) {

        defending = 0;
        printBattleUI(player, enemy);

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("잘못된 입력입니다.\n");
            continue;
        }
        while (getchar() != '\n');

        if (choice == 1) {
            int dmg = player->Str + (rand() % 6);
            enemy->hp -= dmg;
            if (enemy->hp < 0) enemy->hp = 0;
            printf("%s의 공격! %d 데미지를 입혔습니다.\n", player->name, dmg);
        }
        else if (choice == 2) {
            defending = 1;
            printf("%s가 방어 태세를 취했습니다.\n", player->name);
        }
        else if (choice == 3) {
            printf("\n아이템을 사용합니다.\n");
            useItem(player, -1);
            continue;
        }
        else if (choice == 4) {
            int escapeChance = 30 + player->Dex * 5;
            if (escapeChance > 90) escapeChance = 90;
            int r = rand() % 100;
            if (r < escapeChance) {
                printf("민첩함을 이용해 전투에서 벗어났습니다! (도망 확률 %d%%)\n", escapeChance);
                EnterToContinue();
                return;
            }
            else {
                printf("도망에 실패했습니다! (확률 %d%%)\n", escapeChance);
                EnterToContinue();
            }
        }
        else {
            printf("잘못된 선택입니다.\n");
            continue;
        }

        if (enemy->hp <= 0) {
            printf("%s를 물리쳤습니다!\n", enemy->name);
            player->gold += enemy->gold;
            printf("골드 %d 획득!\n", enemy->gold);
            return;
        }

        int enemyDmg = enemy->attack + (rand() % 4);
        if (defending) {
            enemyDmg -= player->Def;
            if (enemyDmg < 0) enemyDmg = 0;
        }

        player->hp -= enemyDmg;
        if (player->hp < 0) player->hp = 0;

        printf("%s의 공격! %s가 %d 피해를 입었습니다.\n",
            enemy->name, player->name, enemyDmg);

        if (player->hp <= 0) {
            printf("%s가 쓰러졌습니다...\n", player->name);
            EnterToContinue();
            return;
        }
    }
}
