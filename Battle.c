#include <stdio.h>
#include <stdlib.h>
#include "Battle.h"
#include "Character.h"
#include "Event.h"
void battle(Player* player, Enemy* enemy) {
    int choice;
    int won = 0;
    int gainedGold = 0;

    printf("\n--- 전투 시작! ---\n");
    while (player->hp > 0 && enemy->hp > 0) {
        int defending = 0;

        // 플레이어의 행동 선택
        printf("\n1. 공격 2. 방어\n선택: ");
        if (scanf("%d", &choice) != 1) {
            // 입력 오류 처리: 버퍼 비우고 반복
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("잘못된 입력입니다. 다시 선택하세요.\n");
            continue;
        }

        if (choice == 1) {
            int playerDamage = player->Str + (rand() % 6);
            enemy->hp -= playerDamage;
            if (enemy->hp < 0)
                enemy->hp = 0;

            printf("%s가 %s에게 %d의 피해를 입혔습니다! (남은 HP: %d)\n", player->name, enemy->name, playerDamage, enemy->hp);
        } else if (choice == 2) {
            defending = 1;
            printf("%s가 방어 자세를 취했습니다. 다음 적의 공격 데미지가 감소합니다.\n", player->name);
        } else {
            printf("유효하지 않은 선택입니다. 턴을 건너뜁니다.\n");
        }

        // 적이 죽었는지 확인
        if (enemy->hp == 0) {
            printf("%s를 물리쳤습니다!\n", enemy->name);
            gainedGold = enemy->gold;
            player->gold += gainedGold;
            printf("골드 %d를 획득했습니다!\n", gainedGold);
            won = 1;
            break;
        }

        // 적의 공격
        int enemyDamage = enemy->Str + (rand() % 4); // 0..3 추가 데미지
        if (defending) {
            enemyDamage -= player->Def;
            if (enemyDamage < 0)
                enemyDamage = 0;
            printf("%s의 방어로 인해 받은 데미지가 감소했습니다.\n", player->name);
        }

        player->hp -= enemyDamage;
        if (player->hp < 0) 
            player->hp = 0;

        printf("%s가 %s에게 %d의 피해를 입혔습니다! (남은 HP: %d)\n",
            enemy->name, player->name, enemyDamage, player->hp);

        if (player->hp == 0) {
            printf("%s가 쓰러졌습니다...\n", player->name);
            won = 0;
            break;
        }
    }

    // 전투 요약 출력 (전투가 끝난 직후에 요약을 보여줌)
    printf("--- 전투 종료 ---\n");
    if (won) {
        printf("\n[전투 요약] 승리! 획득 골드: %d\n", gainedGold);
    } else {
        printf("\n[전투 요약] 패배... 남은 HP: %d\n", player->hp);
    }

	EnterToContinue();
}