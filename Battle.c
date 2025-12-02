#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Battle.h"
#include "Character.h"
#include "Event.h"

/*
* 전투 UI 출력 함수
*/
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

/*
* 전투 함수
* 플레이어와 적 간의 턴제 전투 진행
* 플레이어 선택지: 공격, 방어, 아이템 사용, 도망가기
*/
void battle(Player* player, Enemy* enemy) {
	int choice; // 플레이어 선택지
	int defending = 0; // 플레이어 방어 상태 체크
	int enemyStunned = 0; // 적 기절 상태 체크

    printf("\n*** 전투 시작! ***\n");
    printf("야생의 [%s]이(가) 나타났습니다!\n", enemy->name);

	// 한쪽 체력이 0 이하가 될 때까지 전투 반복
    while (player->hp > 0 && enemy->hp > 0) {
		defending = 0; // 매 턴 시작 시 방어 상태 초기화
		printBattleUI(player, enemy); // 전투 UI 출력

		// 플레이어 입력 처리
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("잘못된 입력입니다.\n");
            continue;
        }
        while (getchar() != '\n');

        // 플레이어 턴
        if (choice == 1) { // 공격
            int dmg = player->Str + (rand() % 6);
			// 치명타 판정
            if (rand() % 100 < player->Dex) { // 민첩 비례 치명타 확률
                dmg = (int)(dmg * 1.5);
                printf(">> 치명타! 급소를 가격했습니다!\n");
            }

            enemy->hp -= dmg;

			// HP 음수 방지
            if (enemy->hp < 0) 
                enemy->hp = 0;
            printf(">> %s의 공격! %d의 피해를 입혔습니다.\n", player->name, dmg);
        }
        else if (choice == 2) { // 방어
            defending = 1;
            printf(">> %s가 방패를 들고 방어 태세를 취합니다. (피해량 감소)\n", player->name);
        }
        else if (choice == 3) { // 아이템 사용
            printf("\n[ 전투 중 사용 가능한 아이템 ]\n");
            printInventory(player);
            printf("사용할 아이템 번호 입력 (-1 취소): ");

            int itemIdx;
            scanf("%d", &itemIdx);
            while (getchar() != '\n');

            // 취소하거나 잘못된 번호면 턴을 넘기지 않음
            if (itemIdx == -1 || itemIdx >= 100 || player->inventory[itemIdx][0] == '\0') {
                printf(">> 아이템 사용을 취소했습니다.\n");
                continue;
            }

            char* itName = player->inventory[itemIdx];
            int turnEnded = 1; // 아이템 사용 성공 시 턴 종료 여부

			// 1: 공격 아이템
			if (strcmp(itName, "불타는 핵") == 0) { // 불타는 핵
                printf(">> [불타는 핵]을 적에게 던졌습니다! 콰아앙-!!\n");
                printf(">> 거대한 화염 폭발이 적을 집어삼킵니다. (고정 피해 50)\n");
                enemy->hp -= 50;
                player->inventory[itemIdx][0] = '\0'; // 아이템 소모
            }
            
			else if (strcmp(itName, "운석 광석") == 0) { // 운석 광석
                printf(">> [운석 광석]을 힘껏 투척했습니다! 퍽!\n");
                printf(">> 묵직한 돌덩이가 적의 머리를 강타합니다. (물리 피해 20)\n");
                enemy->hp -= 20;
                player->inventory[itemIdx][0] = '\0'; // 아이템 소모
            }
			// 2: 상태 이상 아이템
			else if (strcmp(itName, "별의 가루") == 0) { // 별의 가루
                printf(">> [별의 가루]를 공중에 흩뿌립니다. 반짝이는 입자가 시야를 가립니다!\n");
                printf(">> 적이 혼란에 빠져 다음 행동을 하지 못합니다! (기절)\n");
                enemyStunned = 1; // 적 기절 상태 ON
                player->inventory[itemIdx][0] = '\0'; // 아이템 소모
            }
			// 3: 회복 아이템
            else if (strcmp(itName, "HP 포션") == 0 || strcmp(itName, "현자의 도시락") == 0) {
                useItem(player, itemIdx);
            }

            if (!turnEnded) 
                continue; // 아이템을 안 썼으면 다시 선택지로
        }
        else if (choice == 4) { // 도망가기
            int escapeChance = 30 + player->Dex * 5; // 민첩 비례
            if (escapeChance > 90)
                escapeChance = 90;

            int r = rand() % 100;
			// 성공: 도망
            if (r < escapeChance) {
                printf(">> 민첩한 몸놀림으로 전투에서 벗어났습니다! (성공)\n");
                return;
            }
			// 실패: 도망 실패
            else {
                printf(">> 도망치려 했지만 적에게 따라잡혔습니다! (실패)\n");
                printf(">> 적에게 등을 보여 큰 빈틈을 보였습니다.\n");
            }
        }
        else {
            printf("잘못된 선택입니다.\n");
            continue;
        }

        // 적 처치 체크
        if (enemy->hp <= 0) {
            printf("\n*** 승리! ***\n");
            printf("%s를 물리쳤습니다!\n", enemy->name);
            printf(">> [ Gold +%d ]\n", enemy->gold);
			player->gold += enemy->gold; // 골드 획득
            return;
        }

        // 적 턴
        printf("\n");
		if (enemyStunned) { // 적이 기절 상태면 행동 불가
            printf(">> %s은(는) 별의 가루 때문에 정신을 못 차리고 비틀거립니다! (턴 스킵)\n", enemy->name);
			enemyStunned = 0; // 기절 상태 해제
        }
		// 적 일반 공격
        else {
            int enemyDmg = enemy->attack + (rand() % 4);

            if (defending) {
                // 방어 시 데미지 감소 공식
                enemyDmg = (enemyDmg - player->Def) * 0.7;
                if (enemyDmg < 0) enemyDmg = 0;
                printf(">> 방어 성공! 피해를 최소화했습니다.\n");
            }

            player->hp -= enemyDmg;
			// HP 음수 방지
            if (player->hp < 0) 
                player->hp = 0;

            printf(">> %s의 공격! %s에게 %d의 피해를 입혔습니다.\n", enemy->name, player->name, enemyDmg);
        }

		// 플레이어 처치 체크
        if (player->hp <= 0) {
            printf("\n%s가 쓰러졌습니다...\n", player->name);
            return;
        }
    }
}