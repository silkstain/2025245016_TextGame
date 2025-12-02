#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Event.h"
#include "Character.h"
#include "Battle.h"

/* 스탯 체크 함수 
   스탯 + 난수로 체크*/
int StatCheck(const char* statName, int statValue, int difficulty) {
    int roll = statValue + (rand() % 6);
    printf("[판정] %s: %d + 난수 => %d / 난이도 %d : %s\n", statName, statValue, roll, difficulty, (roll >= difficulty ? "성공" : "실패"));
    return roll >= difficulty;
}

/* 구분자 출력 함수 */
void printDivider(const char* title) {
    printf("\n========================================\n");
    printf("  %s\n", title);
    printf("========================================\n");
}

/* 화면 클리어 함수 */
void ClearScreen() {
	system("cls||clear");
}

/* 엔터 대기 함수 */
void EnterToContinue() {
    int c;
    printf("\n(엔터를 눌러 계속...)");
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/* 스토리 이벤트 함수들 */
void Prologue_Event(Player* player) {
    printDivider("프롤로그");
    printf("\n");
    EnterToContinue();
}

void StoryEvent_Start(Player* player) {
    printDivider("게임 시작");
    printf("“%s”... 그 이름을 마음속으로 되뇌이며 당신은 첫 걸음을 내딛습니다.\n", player->name);
    printf("새벽 안개가 걷히는 길 위, 갈라진 바위 사이로 바람이 스치고 먼 곳에서 종이 울리는 듯 잔향이 번집니다.\n");
    printf("땅은 아직 침묵하고 있지만, 위협은 그늘 아래에서 움직이고 있습니다.\n");
    printf("당신의 모험은 지금 시작되었습니다.\n");
    player->location = LOC_ROAD;
    EnterToContinue();
}

/* Road 이벤트 디스패처 */
void Event_Road(Player* player) {
    printDivider("길거리 (Road)");
    printf("갈라진 자갈과 발자국 흔적이 뒤섞인 길. 바람은 주변 수풀의 잎을 뒤집으며 작은 짐승들의 경계심을 건드립니다.\n");
    printf("오늘도 이 길 위에서 당신은 성장하거나, 상처받거나, 아무 일도 없이 지나갈 것입니다.\n");

	// 이벤트 쿨다운 설정
    int townCooldown = 4;
    int caveCooldown = 5;
    int ev, attempts = 0;

	// 강제 야영 이벤트
    if (player->turnCount - player->lastCampingTurn >= 7) {
        CampingEvent(player);
        return;
    }

    while (1) {
        ev = rand() % 14;

		// 쿨다운 및 조건 체크
        if (ev == 3) {
            if (player->knowsCave == 0)
                continue;
            if (player->hasCityPass == 1)
                continue;
            if (player->turnCount - player->lastCaveTurn < caveCooldown) {
                attempts++;
                if (attempts > 50) { ev = 4; break; }
                continue;
            }
        }

        // 마왕성 입장 조건
        if (ev == 13) {
            if (player->hasCityPass == 0)
                continue;
        }

        // 마을 쿨다운
        if (ev == 5 && player->turnCount - player->lastTownTurn < townCooldown) {
            attempts++;
            if (attempts > 50) {
                ev = 4; break;
            }
            continue;
        }

		// 직전 이벤트 중복 방지
        if (ev == player->lastRoadEvent) {
            attempts++;
            if (attempts > 50) {
                ev = 4; break;
            }
            continue;
        }

        break;
    }

	// 이벤트 디스패치
    switch (ev) {
	case 0: // 보물상자 이벤트
        TreasureEvent(player);
        break;
	case 1: // 함정 이벤트
        TrapEvent(player);
        break;
	case 2: // 우물 이벤트
        FondEvent(player);
        break;
    case 3: // 동굴 쿨다운 체크 및 동굴 입장 이벤트
        player->lastCaveTurn = player->turnCount;
        RoadEvent_CaveEntrance(player);
        break;
	case 4: // 평화로운 길
        printDivider("평화로운 길");
        printf("잠시 아무 위협도 없는 구간. 잔잔한 호흡이 안쪽 근육을 풀어줍니다.\n");
        EnterToContinue();
        break;
	case 5: // 마을 쿨다운 체크 및 마을 입장 이벤트
        player->lastTownTurn = player->turnCount;
        RoadEvent_SmallTownEntrance(player);
        break;
	case 6: // 음식 발견 이벤트
        FindFood(player);
        break;
	case 7: // 전설의 숲 입구 이벤트
        if (player->heardLegend == 1 && !player->hasHolySword)
            RoadEvent_ForestEntrance(player);
        else
			Encounter_goblin(player); // 전설을 모르거나 성검 보유시 고블린 조우
        break;
	case 8: // 쓰러진 나무 이벤트
        FallenTreeEvent(player);
        break;
	case 9: // 산적 매복 이벤트
        BanditAmbushEvent(player);
        break;
	case 10: // 여행하는 현자 이벤트
        TravelingSageEvent(player);
        break;
	case 11: // 운석 조각 이벤트
        MeteorShardEvent(player);
        break;
	case 12: // 대도시 입장 이벤트
        RoadEvent_HugeCityEntrance(player);
        break;
	case 13: // 마왕성 입장 이벤트
        RoadEvent_DemonCastleEntrance(player);
        break;
	default: // 대도시 입장 이벤트
        RoadEvent_HugeCityEntrance(player);
        break;
    }

	player->lastRoadEvent = ev; // 최근 이벤트 기록
}

/* SmallTown 입장 이벤트 */
void RoadEvent_SmallTownEntrance(Player* player) {
    int choice;
    printDivider("마을 발견");
    printf("먼지 쌓인 길 끝, 낮게 피어오른 굴뚝 연기와 목재 울타리가 얇게 둘러진 작은 마을이 시야에 들어옵니다.\n");
    printf("지나가는 행인은 적고, 간헐적으로 들리는 망치질 소리가 생존 의지를 알려줍니다.\n");
    printf("마을로 들어가시겠습니까?\n");
    printf("1) 예\n");
    printf("2) 아니오\n");
    printf("> ");
    scanf("%d", &choice); while (getchar() != '\n');
    if (choice != 1) {
        printf("당신은 잠깐 멈춰 바라보다 다시 길을 선택합니다.\n");
        EnterToContinue(); return;
    }
    printf("낡은 문짝을 밀고 안으로 들어섭니다. 사람들의 시선이 잠깐 모였다가 흩어집니다.\n");
    EnterToContinue();
	player->location = LOC_TOWN; // 마을로 이동
}

/* 동굴 입장 이벤트 */
void RoadEvent_CaveEntrance(Player* player) {
    int choice;
    printDivider("동굴 발견");
    printf("지면이 움푹 꺼져 검은 틈이 생겼고, 안쪽에서는 축축한 흙냄새와 오래된 광물의 쇳내가 희미하게 배어 나옵니다.\n");
    printf("안쪽으로 이어지는 바람은 마치 허기진 짐승의 숨처럼 간헐적으로 들립니다.\n");
    printf("동굴로 들어가시겠습니까?\n");
    printf("1) 예\n");
    printf("2) 아니오\n");
    printf("> ");
    scanf("%d", &choice); while (getchar() != '\n');
    if (choice != 1) {
        printf("위험을 뒤로한 채 다시 햇빛 아래 길을 이어 갑니다.\n");
        EnterToContinue(); return;
    }
    printf("어둠에 눈을 적응시키며 조심스럽게 발을 들입니다.\n");
    EnterToContinue();
    player->location = LOC_CAVE; // 동굴로 이동
    player->caveDepth = 1;       // 수정: 동굴 진입 시 항상 지하 1층부터 시작하도록 초기화
}

/* 대도시 입장 이벤트 */
void RoadEvent_HugeCityEntrance(Player* player) {
    int c;
    printDivider("대도시 검문소");

    printf("거대한 성벽이 도시를 감싸고 있으며, 예전보다도 훨씬 많은 경비병들이 문 앞에 배치되어 있습니다.\n");
    printf("최근 마왕의 재림 이후 대도시 전역에 걸쳐 검문이 강화되었다고 들었습니다.\n");
    printf("경비병들은 지나가는 사람들을 하나하나 철저하게 조사하고 있습니다.\n\n");

	// 출입증 미보유 시 입장 불가
    if (!player->hasCityPass) {
        printf("경비병: \"마왕군이 변장하고 침투하려 한다는 보고가 있다.\"\n");
        printf("경비병: \"출입증 없는 자는 누구든 도시 안으로 들일 수 없다. 돌아가라.\"\n\n");
        printf("당신은 출입증이 없어 이곳을 통과할 수 없습니다.\n");
        EnterToContinue();
        return;
    }

    printf("경비병이 출입증을 확인합니다...\n");
    printf("경비병: \"출입증 확인됐다. 이 문을 지나도 좋다. 안쪽에서도 검문이 계속되니 주의하라.\"\n\n");

    printf("1) 대도시에 입장한다\n");
    printf("2) 그냥 지나친다\n");
    printf("> ");

    if (scanf("%d", &c) != 1) {
        while (getchar() != '\n');
        EnterToContinue();
        return;
    }
    while (getchar() != '\n');

	// 대도시 입장
    if (c == 1) {
        printf("철문이 천천히 열리고, 당신은 강화된 경비 아래의 대도시로 들어갑니다.\n");
		player->location = LOC_BIG_CITY; // 대도시로 이동
    }
    else {
        printf("당신은 다시 길로 발길을 돌립니다.\n");
    }
    EnterToContinue();
}

/* 숲 입장 이벤트 */
void RoadEvent_ForestEntrance(Player* player) {
    printDivider("전설의 숲 입구");
    printf("전설로만 듣던 숲의 경계. 나무들은 정상적인 성장 방향을 잊은 듯 서로 엮여 아치형 통로를 만들고\n");
    printf("희미한 빛의 입자들이 공중에서 숨을 쉬듯 천천히 맥동합니다.\n");
    printf("숲으로 들어가겠습니까?\n");
    printf("1) 들어간다\n");
    printf("2) 아직 아니다\n");
    printf("> ");
    int c;
    scanf("%d", &c);
    while (getchar() != '\n');
	// 숲 입장
    if (c == 1) {
        printf("당신은 얽힌 뿌리 사이로 발을 들입니다. 공기는 한층 조용하고 또렷합니다.\n");
        player->location = LOC_FOREST;
    }
    else {
        printf("아직 준비가 더 필요하다고 느끼며 길을 계속 걷습니다.\n");
    }
    EnterToContinue();
}

/* 
* 보물상자 이벤트  
* 50% 확률로 미믹 전투, 아니면 골드 획득
*/
void TreasureEvent(Player* player) {
    printDivider("오래된 보물 상자");
    printf("수풀 사이, 반쯤 흙에 묻힌 고풍스러운 나무 상자가 발견되었습니다.\n");
    printf("녹슨 금속 테두리 장식이 희미하게 빛나며 호기심을 자극합니다.\n");
    printf("하지만 이런 곳에 상자가 있다는 게 조금 수상하기도 합니다.\n\n");

    printf("상자를 열어보시겠습니까?\n");
    printf("1) 조심스럽게 연다\n");
    printf("2) 함정일지도 모른다. 무시한다\n");
    printf("> ");

	// 사용자 입력
    int choice;
    scanf("%d", &choice);
    while (getchar() != '\n');

	// 1: 상자 열기 선택
    if (choice == 1) {
        printf("\n당신은 숨을 죽이고 천천히 상자 뚜껑을 들어 올립니다...\n");
        EnterToContinue();

        int mimic = rand() % 2; // 50% 확률

        // 미믹 전투
        if (mimic) {
            printf("끼이익... 컥!\n");
            printf("뚜껑이 열리다 말고, 상자 안쪽에서 날카로운 이빨과 붉은 혀가 튀어나옵니다!\n");
            printf("보물 상자로 위장한 마물, [미믹]의 기습입니다!\n");

            // 미믹 생성
            Enemy mimicEnemy;
            initEnemy(&mimicEnemy, "미믹", 30 + rand() % 21, 5 + rand() % 6, 0);
            battle(player, &mimicEnemy);
        }

		// 보물 획득
        else {
            int gold = 100 + rand() % 101; // 100~200 골드
            printf("상자 안에는 먼지 쌓인 금화가 가득 들어있습니다!\n");
            printf("운이 좋았습니다.\n");

			// Player 골드 증가
            player->gold += gold;
            printf(">> [ Gold +%d ]\n", gold);
        }
    }
	// 2: 상자 무시 선택
    else {
        printf("과도한 욕심은 화를 부르는 법입니다.\n");
        printf("당신은 미련 없이 상자를 뒤로하고 발길을 돌립니다.\n");
    }
    EnterToContinue();
}

/* 
*함정 이벤트 
*플레이어 선택에 따라 민첩 또는 지능 판정
*피해 혹은 보상 획득
*/
void TrapEvent(Player* player) {
    printDivider("위험한 징조");
    printf("발을 내디딘 순간, 발밑의 흙이 푹 꺼지는 서늘한 감각이 느껴집니다.\n");
    printf("동시에 수풀 속에서 '철컥' 하는 기계 장치 소리가 들려옵니다.\n");
    printf("함정입니다! 반응할 시간은 찰나에 불과합니다.\n\n");

    printf("1) 반사적으로 몸을 날린다 (민첩 활용)\n");
    printf("2) 침착하게 작동 부위를 파악해 해제한다 (지능 활용)\n");
    printf("3) 당황해서 굳어버렸다\n");
    printf("> ");

    int c;
    if (scanf("%d", &c) != 1) {
        while (getchar() != '\n');
        printf("잘못된 입력입니다.\n");
        EnterToContinue();
        return;
    }
    while (getchar() != '\n');

	// 민첩 판정
    if (c == 1) {
        printf("당신은 생각보다 몸이 먼저 반응하여 옆으로 굴렀습니다!\n");

		// 성공 시 피해 회피
        if (StatCheck("민첩", player->Dex, 12)) {
            printf("슈욱-! 날카로운 화살들이 당신이 있던 자리를 스치고 지나갑니다.\n");
            printf("옷자락만 살짝 찢어졌을 뿐, 다행히 다치지 않으셨습니다.\n");
        }

		// 실패 시 피해
        else {
            int dmg = 10 + rand() % 6;
            printf("하지만 반응이 조금 늦으셨습니다. 날아온 파편에 어깨를 맞았습니다.\n");
            player->hp -= dmg;
            if (player->hp < 0) player->hp = 0;
            printf(">> [ HP -%d ]\n", dmg);
        }
    }

	// 지능 판정
    else if (c == 2) {
        printf("당신은 순간적으로 함정의 구조를 꿰뚫어 보고 핵심 부품을 노립니다.\n");

		// 성공 시 보상 획득
        if (StatCheck("지능", player->Int, 12)) {
            printf("철컥. 기어가 맞물리기 직전에 돌을 끼워 작동을 멈췄습니다.\n");
            printf("정교한 함정 부품을 회수했습니다. 상점에 팔면 돈이 될 겁니다.\n");

            int gainGold = 30 + rand() % 21; // 30~50 골드
            player->gold += gainGold;
            printf(">> [ Gold +%d ]\n", gainGold);
        }

		// 실패 시 피해
        else {
            printf("하지만 구조가 너무 복잡했습니다! 손을 대는 순간 함정이 폭발합니다.\n");
            int dmg = 15 + rand() % 6;
            player->hp -= dmg;
            if (player->hp < 0) player->hp = 0;
            printf(">> [ HP -%d ] (폭발 피해)\n", dmg);
        }
    }
	// 3: 아무 행동도 하지 않음
    else {
        printf("아무것도 하지 못하고 멍하니 서 있었습니다...\n");
        int dmg = 12 + rand() % 6;
        printf("함정이 무자비하게 당신을 덮칩니다.\n");
        player->hp -= dmg;
        if (player->hp < 0) player->hp = 0;
        printf(">> [ HP -%d ]\n", dmg);
    }

    EnterToContinue();
}

/*
* 샘물 이벤트
* 70% 확률로 최대 체력 증가, 30% 확률로 중독 피해
*/
void FondEvent(Player* player) {
    printDivider("신비한 샘물");
    printf("숲속 깊은 곳, 몽환적인 푸른 빛을 내는 작은 샘물을 발견했습니다.\n");
    printf("수면 위로 은은한 마력이 안개처럼 피어오르고 있습니다.\n");
    printf("전설에 따르면 이런 샘물은 신체의 한계를 넓혀준다고 합니다.\n\n");

    printf("1) 샘물을 마신다 (성공 시 최대 HP 증가)\n");
    printf("2) 의심스러우니 지나친다\n");
    printf("> ");

    int c;
    if (scanf("%d", &c) != 1) {
        while (getchar() != '\n');
        printf("입력 오류입니다.\n");
        EnterToContinue();
        return;
    }
    while (getchar() != '\n');

	// 샘물 마시지 않음
    if (c == 2) {
        printf("당신은 검증되지 않은 물을 마시는 위험을 감수하지 않기로 합니다.\n");
        EnterToContinue();
        return;
    }

	// 샘물 마심
    printf("손을 모아 샘물을 한 모금 떠서 마십니다...\n");

    int rottenChance = 30; // 30% 확률로 오염됨
	int roll = rand() % 100; // 0~99

	// 성공: 최대 체력 증가
    if (roll >= rottenChance) {
        printf("!!\n");
        printf("청량한 기운이 식도를 타고 내려가 온몸의 혈관을 확장시킵니다.\n");
        printf("몸안의 노폐물이 씻겨 나가고 생명력이 넘치는 것을 느낍니다.\n");

        player->maxHp += 10;
        player->hp = player->maxHp; // 체력 완전 회복 보너스

        printf(">> [ 최대 HP +10 영구 상승 ] [ HP 완전 회복 ]\n");
    }
	// 실패: 중독 피해
    else {
        int dmg = 15 + rand() % 6;
        printf("윽... 역겨운 냄새가 코를 찌릅니다. 겉보기와 달리 물이 썩어 있었습니다!\n");
        printf("극심한 복통이 밀려옵니다.\n");

        player->hp -= dmg;
        if (player->hp < 0) player->hp = 0;

        printf(">> [ HP -%d ] (중독)\n", dmg);
    }

    EnterToContinue();
}

/*
* 고블린 조우 이벤트
* 전투 발생
*/
void Encounter_goblin(Player* player) {
    printDivider("고블린 조우");
    printf("작은 그린 스킨이 낡은 가죽 조각과 삐뚤어진 칼을 들고 우물거리는 소리를 냅니다. 눈빛은 허기와 탐욕의 중간.\n");
    Enemy goblin; initEnemy(&goblin, "고블린", 25 + rand() % 11, 3 + rand() % 5, 20 + rand() % 31);
    battle(player, &goblin);
    EnterToContinue();
}

/*
* 음식 발견 이벤트
* 플레이어 선택에 따라 3종류 버섯 중 선택
* 각 버섯마다 다른 효과 발생
*/
void FindFood(Player* player) {
    printDivider("야생의 식재료");
    printf("축축한 이끼와 썩은 나무뿌리 사이로 형형색색의 버섯들이 고개를 내밀고 있습니다.\n");
    printf("허기가 느껴져 군침이 돌지만, 숲의 식물은 겉만 봐선 독인지 약인지 알 수 없습니다.\n");
    printf("어떤 것을 선택하시겠습니까?\n\n");

    printf("1) 평범해 보이는 흰색 버섯\n");
    printf("2) 화려한 붉은 점박이 버섯\n");
    printf("3) 기괴하게 생긴 검은 버섯\n");
    printf("4) 지식을 동원해 식용 버섯을 골라낸다 (지능 판정)\n");
    printf("5) 위험을 감수하지 않고 지나친다\n");
    printf("> ");

    int choice;
    if (scanf("%d", &choice) != 1) {
        while (getchar() != '\n');
        printf("잘못된 입력입니다.\n");
        EnterToContinue();
        return;
    }
    while (getchar() != '\n');

	// 선택지 5: 지나침
    if (choice == 5) {
        printf("당신은 배고픔을 참고 안전을 택하기로 했습니다. 발길을 돌립니다.\n");
        EnterToContinue();
        return;
    }

    int roll = rand() % 100;

	// 선택지별 효과
    switch (choice) {
	case 1: { // 1: 흰 버섯: HP 회복 or 피해
        printf("가장 무난해 보이는 흰 버섯을 골라 조심스럽게 씹어봅니다...\n");
        int successChance = 65; // 65% 안전
		// 성공: HP 회복
        if (roll < successChance) {
            int heal = 10 + rand() % 11; // 10~20
            player->hp += heal;
            if (player->hp > player->maxHp) player->hp = player->maxHp;
            printf("다행히 독은 없는 것 같습니다. 담백한 맛이 배를 채워줍니다.\n");
            printf(">> [ HP +%d ]\n", heal);
        }
		// 실패: 피해
        else {
            int dmg = 10 + rand() % 11; // 10~20
            player->hp -= dmg; if (player->hp < 0) player->hp = 0;
            printf("으윽...! 씹자마자 비릿한 즙이 터져 나옵니다. 상한 버섯이었습니다.\n");
            printf(">> [ HP -%d ]\n", dmg);
        }
        break;
    }
    case 2: { // 2: 붉은 점: HP 회복 + 아이템
        printf("위험해 보이지만 달콤한 향기가 나는 붉은 버섯을 한입 베어 뭅니다...\n");
		int successChance = 60; // 60% 안전
		// 성공: HP 회복 + 아이템
        if (roll < successChance) {
            int heal = 20 + rand() % 11; // 20~30
            player->hp += heal;
            if (player->hp > player->maxHp) player->hp = player->maxHp;
            addItem(player, "화려한 버섯");
            printf("놀라울 정도로 감칠맛이 돕니다! 활력이 솟구치는 기분입니다.\n");
            printf(">> [ HP +%d ] [ 아이템 획득: 화려한 버섯 ]\n", heal);
        }
		// 실패: 피해
        else {
            int dmg = 12 + rand() % 14; // 12~25
            player->hp -= dmg; if (player->hp < 0) player->hp = 0;
            printf("혀가 마비되고 세상이 빙글빙글 돕니다. 독버섯이었습니다!\n");
            printf(">> [ HP -%d ] (중독)\n", dmg);
        }
        break;
    }
    case 3: { // 3: 검은 기둥형: 힘 상승 or 큰 피해
        printf("역겨운 냄새가 나지만, 약초 도감에서 본 영약일지도 모릅니다. 눈 딱 감고 삼킵니다!\n");
		int successChance = 35; // 35% 안전
		// 성공: 힘 상승 + HP 회복
        if (roll < successChance) {
            int heal = 25 + rand() % 26; // 25~50
            player->hp += heal;
            if (player->hp > player->maxHp) player->hp = player->maxHp;
            player->Str += 1;
            printf("지옥 같은 쓴맛 뒤로, 근육이 부풀어 오르는 듯한 뜨거운 기운이 전신을 감쌉니다!\n");
            printf(">> [ HP +%d ] [ Str +1 ]\n", heal);
        }
		// 실패: 큰 피해
        else {
            int dmg = 18 + rand() % 23; // 18~40
            player->hp -= dmg; if (player->hp < 0) player->hp = 0;
            printf("위장이 뒤틀리는 듯한 격통이 덮칩니다! 맹독입니다!\n");
            printf(">> [ HP -%d ] (치명상)\n", dmg);
        }
        break;
    }
    case 4: { // 지능 판정 
        printf("무턱대고 먹는 건 자살 행위입니다. 당신은 지식을 동원해 버섯을 분류합니다...\n");
		// 성공: HP 회복 + 아이템
        if (StatCheck("지능", player->Int, 12)) {
            int heal = 20;
            player->hp += heal;
            if (player->hp > player->maxHp) player->hp = player->maxHp;
            addItem(player, "건강한 버섯");
            printf("독버섯과 식용 버섯을 정확히 구별해냈습니다. 이제 안심하고 먹을 수 있습니다.\n");
            printf(">> [ HP +%d ] [ 아이템 획득: 건강한 버섯 ]\n", heal);
        }
		// 실패: 피해
        else {
            int dmg = 10 + rand() % 11;
            player->hp -= dmg; if (player->hp < 0) player->hp = 0;
            printf("분석이 틀렸습니다... 안전하다고 생각한 버섯이 목을 찌릅니다.\n");
            printf(">> [ HP -%d ]\n", dmg);
        }
        break;
    }
    default:
        printf("망설이는 사이, 버섯 주변의 벌레들을 보고 입맛이 떨어졌습니다.\n");
        break;
    }

    EnterToContinue();
}

/*
* 쓰러진 나무 이벤트
* 플레이어 선택에 따라 힘/민첩/지능 판정
* 성공 시 보상, 실패 시 피해
*/
void FallenTreeEvent(Player* player) {
    printDivider("길을 막은 고목");
    printf("폭풍에 뿌리째 뽑힌 거대한 고목이 길을 완전히 가로막고 있습니다.\n");
    printf("나무 껍질은 이끼로 뒤덮여 있고, 부러진 가지들이 창처럼 튀어나와 있어 지나가기 쉽지 않아 보입니다.\n");
    printf("어떻게 하시겠습니까?\n\n");

    printf("1) 온 힘을 다해 나무를 밀어 길을 만든다\n");
    printf("2) 가지 사이를 밟고 날렵하게 넘어간다\n");
    printf("3) 나무의 구조를 분석해 약점이나 가치를 찾는다\n");
    printf("4) 안전하게 먼 길로 우회한다\n");
    printf("> ");

    int c;
    scanf("%d", &c);
    while (getchar() != '\n');

	// 1: 힘 판정
    if (c == 1) {
        printf("당신은 거친 숨을 몰아쉬며 거목에 어깨를 밀어 넣습니다...\n");
		// 성공: 힘 1 상승
        if (StatCheck("힘", player->Str, 12)) {
            printf("우지끈! 근육이 비명을 지르지만, 엄청난 힘으로 나무를 옆으로 굴려버렸습니다!\n");
            printf("한계를 뛰어넘은 근력 운동이 되었습니다.\n");
            player->Str += 1;
            printf(">> [ Str +1 ]\n");
        }
		// 실패: 피해
        else {
            printf("끄응...! 꿈쩍도 하지 않습니다. 무리하게 힘을 주다 허리에 통증이 옵니다.\n");
            int dmg = 8;
            player->hp -= dmg;
            if (player->hp < 0) player->hp = 0;
            printf(">> [ HP -%d ]\n", dmg);
        }
    }
    // 민첩 판정
    else if (c == 2) {
        printf("미끄러운 이끼와 날카로운 가지 사이를 파쿠르 하듯 뛰어넘으려 합니다...\n");

		// 성공: 골드 획득
        if (StatCheck("민첩", player->Dex, 11)) {
            int foundGold = 45; // 보상을 골드로 변경
            player->gold += foundGold;
            printf("가볍게 도약하여 공중에 뜬 순간, 높은 가지에 걸려있던 낡은 가죽 주머니를 발견합니다!\n");
            printf("당신은 착지와 동시에 주머니를 낚아챘습니다. 묵직한 동전 소리가 들립니다.\n");
            printf(">> [ Gold +%d ]\n", foundGold);
        }

		// 실패: 피해
        else {
            printf("앗! 이끼를 밟고 미끄러지며 튀어나온 가지에 긁혔습니다.\n");
            int dmg = 5;
            player->hp -= dmg;
            if (player->hp < 0) player->hp = 0;
            printf(">> [ HP -%d ]\n", dmg);
        }
    }

	// 지능 판정
    else if (c == 3) {
        printf("무작정 힘을 쓰기보단 나무의 상태를 관찰합니다...\n");
		// 성공: 골드 + 아이템 획득
        if (StatCheck("지능", player->Int, 11)) {
            printf("나무 안쪽이 썩어 비어있는 것을 발견했습니다! 쉽게 부수고 통과합니다.\n");
            printf("추가로, 부서진 틈새에서 희귀한 [마른 수액] 결정을 채취했습니다.\n");
            player->gold += 10;
            printf(">> [ Gold +10 ]\n");
        }

		// 실패: 시간 낭비
        else {
            printf("구조를 파악하려 했지만, 시간만 지체되고 뾰족한 수가 보이지 않습니다.\n");
            printf("결국 고생해서 덤불을 헤치고 지나갑니다. 소득은 없었습니다.\n");
        }
    }
	// 4: 우회
    else {
        printf("무리하지 않고 안전한 오솔길을 찾아 빙 둘러갑니다.\n");
        printf("시간은 좀 걸렸지만, 몸은 멀쩡합니다.\n");
    }

    EnterToContinue();
}

/*
* 산적 매복 이벤트
* 플레이어 선택에 따라 전투/민첩/지능 판정/골드 지불
*/
void BanditAmbushEvent(Player* player) {
    printDivider("산적 습격");
    printf("길을 막아선 산적들이 흉흉한 무기를 들고 위협해옵니다.\n");
    printf("산적 두목: \"목숨이 아깝다면 가진 걸 다 내놓고 꺼져라!\"\n\n");

    printf("1) 무기를 들고 정면으로 싸운다 (전투)\n");
    printf("2) 빈틈을 노려 포위망을 뚫는다 (민첩 판정)\n");
    printf("3) 거짓말로 산적들을 속인다 (지능 판정)\n");
    printf("4) 순순히 통행료를 낸다 (50 Gold)\n");
    printf("> ");

    int c;
    scanf("%d", &c);
    while (getchar() != '\n');

    // 1. 전투 선택: 산적과 직접 교전
    if (c == 1) {
        printf("당신은 무기를 뽑아 들고 전투 태세를 갖춥니다.\n");

        // 산적 생성 및 전투
        Enemy bandit;
        initEnemy(&bandit, "산적", 100 + rand() % 11, 10 + rand() % 3, 30);
        battle(player, &bandit);
    }

    // 2. 민첩 판정: 강행 돌파
    else if (c == 2) {
        printf("산적들의 배치가 허술한 곳을 찾아 빠르게 달립니다.\n");

		// 성공: 민첩 1 상승
        if (StatCheck("민첩", player->Dex, 13)) {
            printf("바람처럼 산적들 사이를 빠져나갔습니다! (Dex +1)\n");
            player->Dex += 1;
        }
		// 실패: 피해
        else {
            printf("발이 꼬여 넘어지며 산적들에게 둘러싸여 구타당했습니다. (HP -15)\n");
            player->hp -= 15;
            if (player->hp < 0) player->hp = 0;
        }
    }
	// 3. 지능 판정: 거짓말 사용
    else if (c == 3) {
        printf("당신은 뒤쪽을 가리키며 왕실 근위대가 오고 있다고 외칩니다.\n");

		// 성공: 지능 1 상승
        if (StatCheck("지능", player->Int, 13)) {
            printf("산적들이 당황하며 흩어진 사이 유유히 현장을 벗어납니다. (Int +1)\n");
            player->Int += 1;
        }

		// 실패: 피해
        else {
            printf("산적 두목은 속지 않았습니다. 화가 난 그가 무기를 휘두릅니다. (HP -15)\n");
            player->hp -= 15;
            if (player->hp < 0) player->hp = 0;
        }
    }
    // 4. 통행료 지불: 골드 소모 후 안전 통과
    else if (c == 4) {
        int toll = 50;
        if (player->gold >= toll) {
            printf("돈주머니를 던져주고 싸움을 피했습니다. (Gold -%d)\n", toll);
            player->gold -= toll;
        }
        else {
            printf("산적: \"돈도 없는 거지가 감히 우릴 놀려? 죽어라!\"\n");
            printf("협상이 결렬되어 강제 전투가 시작됩니다!\n");

            // 골드 부족 시 강제 전투
            Enemy bandit;
            initEnemy(&bandit, "분노한 산적", 110 + rand() % 11, 15 + rand() % 3, 0);
            battle(player, &bandit);
        }
    }
    // 잘못된 입력
    else {
        printf("우물쭈물하다가 기습을 허용했습니다. (HP -10)\n");
        player->hp -= 10;
        if (player->hp < 0) player->hp = 0;
    }

    EnterToContinue();
}

/*
* 여행하는 현자 이벤트
* 플레이어 선택에 따라 지능/방어/민첩 판정 or 보상 획득
*/
void TravelingSageEvent(Player* player) {
    printDivider("여행하는 현자");
    printf("낡은 회색 외투를 두르고, 바람이 휘감기는 지팡이를 든 노인이 바위에 앉아 쉬고 있습니다.\n");
    printf("그는 깊은 눈으로 당신을 응시하며, 마치 당신의 운명을 읽으려는 듯합니다.\n");
    printf("현자: \"여행자여, 자네에게서 흥미로운 기운이 느껴지는구먼.\"\n\n");

    printf("1) 현자의 수수께끼를 푼다\n");
    printf("2) 현자의 고행을 따라 해 본다\n");
    printf("3) 정중하게 예의를 갖춰 인사한다\n");
    printf("4) 무시하고 지나간다\n");
    printf("> ");

    int c;
    scanf("%d", &c);
    while (getchar() != '\n');
	// 지혜의 인도 받기
    if (c == 1) {
        printf("현자가 고대어로 된 난해한 수수께끼를 던집니다.\n");
        printf("당신은 머릿속의 지식을 총동원해 답을 찾으려 합니다...\n");

		// 성공: 현자의 지혜서 획득
        if (StatCheck("지능", player->Int, 13)) {
            printf("당신이 정확한 답을 말하자, 현자의 주름진 얼굴에 미소가 번집니다.\n");
            printf("현자: \"훌륭하네. 이 지식이 담긴 두루마리가 자네에게 도움이 될 걸세.\"\n");

            addItem(player, "현자의 지혜서");
            printf(">> [ 아이템 획득: 현자의 지혜서 ] (사용 시 지능 영구 상승)\n");
        }
		// 실패
        else {
            printf("말문이 막혀 머뭇거립니다. 현자는 아쉬운 듯 고개를 젓습니다.\n");
            printf("현자: \"아직 깨달음이 부족하구만...\"\n");
        }
    }
	// 2. 방어 판정: 고행 체험
    else if (c == 2) {
        printf("현자 옆에서 가부좌를 틀고 명상에 잠깁니다.\n");
        printf("단순한 명상이 아닙니다. 뼈를 깎는 듯한 고통이 전신을 짓누릅니다...\n");

        if (StatCheck("방어", player->Def, 13)) {
            printf("식은땀을 비 오듯 흘리면서도 끝까지 자세를 유지했습니다.\n");
            printf("현자: \"대단한 인내심이야. 자네 육체는 축복받을 자격이 있네.\"\n");

            addItem(player, "활력의 비약");
            printf(">> [ 아이템 획득: 활력의 비약 ] (사용 시 최대 체력 영구 상승)\n");
        }
		// 실패: 피해
        else {
            printf("다리에 쥐가 나고 허리가 끊어질 것 같아 비명을 지르며 쓰러졌습니다.\n");
            int dmg = 8;
            player->hp -= dmg;
            if (player->hp < 0) player->hp = 0;
            printf(">> [ HP -%d ] (부상)\n", dmg);
        }
    }
	// 3. 민첩 판정: 예의 바른 인사
    else if (c == 3) {
        printf("당신은 무리한 요구를 하는 대신, 여행자의 예법으로 정중히 고개를 숙입니다.\n");

		// 성공: 현자의 도시락 획득
        if (StatCheck("민첩", player->Dex, 10)) {
            printf("현자는 당신의 겸손하고 절제된 태도를 마음에 들어 합니다.\n");
            printf("현자: \"허허, 예의 바른 젊은이로군. 배고프지 않나? 이걸 들게나.\"\n");
            printf("현자가 주섬주섬 보따리를 풀어 따뜻한 음식을 건넸습니다.\n");

            addItem(player, "현자의 도시락");
            printf(">> [ 아이템 획득: 현자의 도시락 ] (사용 시 HP 대량 회복)\n");
        }
		// 실패
        else {
            printf("어색하게 인사하다가 발이 꼬여 우스꽝스러운 꼴이 되었습니다.\n");
            printf("현자는 못 본 척 다시 책으로 시선을 돌립니다.\n");
        }
    }
	// 4. 무시하고 지나감
    else {
        printf("무시무시한 존재감에 눌려, 차마 말을 걸지 못하고 지나갑니다.\n");
    }

    EnterToContinue();
}

/*
* 떨어진 별의 파편 이벤트
* 플레이어 선택에 따라 인내/힘/지능 판정
* 성공 시 아이템 획득, 실패 시 피해
*/
void MeteorShardEvent(Player* player) {
    printDivider("떨어진 별의 파편");
    printf("숲 한가운데 타버린 웅덩이가 있고, 그 중심에 붉게 맥동하는 운석 파편이 박혀 있습니다.\n");
    printf("가까이 가기만 해도 피부가 따끔거릴 정도의 열기와 신비한 기운이 뿜어져 나옵니다.\n");
    printf("위험해 보이지만, 평범한 물건이 아님은 확실합니다.\n\n");

    printf("1) 뜨거움을 참고 맨손으로 집어 든다 (인내심 시험)\n");
    printf("2) 무기로 내리쳐 조각을 떼어낸다 (힘 활용)\n");
    printf("3) 마법적인 기운을 분석해 정수를 추출한다 (지식 활용)\n");
    printf("4) 위험해 보이니 그냥 둔다\n");
    printf("> ");

    int c;
    if (scanf("%d", &c) != 1) {
        while (getchar() != '\n');
        printf("잘못된 입력입니다.\n");
        EnterToContinue();
        return;
    }
    while (getchar() != '\n');

	// 선택지별 처리
    switch (c) {
	case 1: // 방어 판정: 인내심 시험
        printf("당신은 화상을 각오하고 불타는 운석을 향해 손을 뻗습니다...\n");

		// 성공: 불타는 핵 획득
        if (StatCheck("방어", player->Def, 14)) {
            printf("치이익! 장갑이 그을렸지만, 고통을 참고 열기를 식혀 주머니에 넣는 데 성공했습니다!\n");

            addItem(player, "불타는 핵");
            printf(">> [ 아이템 획득: 불타는 핵 ] (전투용: 적에게 강력한 화염 폭발)\n");
        }

		// 실패: 화상 피해
        else {
            printf("뜨거워!! 손이 타들어 가는 고통에 비명을 지르며 물러났습니다.\n");
            int dmg = 12;
            player->hp -= dmg; if (player->hp < 0) player->hp = 0;
            printf(">> [ HP -%d ] (화상)\n", dmg);
        }
        break;

	case 2: // 힘 판정: 무기로 내리침
        printf("단단한 운석을 무기로 강하게 내리치려고 합니다!\n");
		// 성공: 운석 광석 획득
        if (StatCheck("힘", player->Str, 13)) {
            printf("깡-! 묵직한 타격음과 함께 운석이 쪼개지며 안쪽의 광석이 드러납니다.\n");

            addItem(player, "운석 광석");
            printf(">> [ 아이템 획득: 운석 광석 ] (전투용: 적에게 투척하여 물리 피해)\n");
        }
		// 실패: 반동 피해
        else {
            printf("챙-! 운석은 너무 단단했습니다. 오히려 무기를 쥔 손목에 충격이 옵니다.\n");
            int dmg = 6;
            player->hp -= dmg; if (player->hp < 0) player->hp = 0;
            printf(">> [ HP -%d ] (충격)\n", dmg);
        }
        break;

	case 3: // 지능 판정: 마법 기운 분석
        printf("운석의 파장을 분석하여 에너지만 추출하려 시도합니다...\n");
		// 성공: 별의 가루 획득
        if (StatCheck("지능", player->Int, 13)) {
            printf("복잡한 에너지 흐름을 읽어내리고, 빈 병에 신비한 가루를 담아냈습니다.\n");

            addItem(player, "별의 가루");
            printf(">> [ 아이템 획득: 별의 가루 ] (전투용: 적을 기절시킴)\n");
        }
		// 실패: 기운 역류 피해
        else {
            printf("패턴이 너무 불규칙해 해석에 실패했습니다. 눈만 아픕니다.\n");
        }
        break;

	case 4: // 그냥 둠
        printf("목숨을 걸 만큼의 가치는 없다고 판단하고 지나갑니다.\n");
        break;

	default: // 잘못된 입력
        printf("우물쭈물하는 사이 운석의 빛이 꺼져버렸습니다. 평범한 돌멩이가 되었습니다.\n");
        break;
    }
    EnterToContinue();
}

/*
* 야영 이벤트
* 일정 턴마다 발생
* 캠핑 장비 보유 시 안정적인 휴식
* 미보유 시 플레이어 선택에 따라 판정 or 전투 발생
*/
void CampingEvent(Player* player) {
    printDivider("야영");

    printf("해가 기울어 주변이 점점 어두워지고 있습니다.\n");
    printf("숲길에는 긴 그림자만 남았고, 더 이상 이동하기엔 위험해 보입니다.\n");
    printf("오늘은 이 근처에서 야영을 준비해야 할 것 같습니다.\n\n");

	int hasGear = 0; // 캠핑 장비 보유 여부
    int gearIndex = -1; // 장비 위치 저장용

	// 인벤토리에서 캠핑 장비 탐색
    for (int i = 0; i < 100; i++) {
        if (player->inventory[i][0] != '\0' && strcmp(player->inventory[i], "캠핑 장비") == 0) {
            hasGear = 1;
            gearIndex = i;
            break;
        }
    }

	// 캠핑 장비 보유 시 안정적인 휴식
    if (hasGear) {
        printf("캠핑 장비를 사용해 안정적으로 야영을 준비합니다.\n");
        int heal = 40;
        player->hp += heal;
        if (player->hp > player->maxHp) player->hp = player->maxHp;
        printf("편안한 휴식을 취했습니다. HP +%d (%d/%d)\n", heal, player->hp, player->maxHp);

		// 10% 확률로 장비 파손 이벤트
        int broken = rand() % 100;
        if (broken < 10) {
            printf("밤사이 사고가 발생해 캠핑 장비가 파손되었습니다.\n");
            // [변경] free() 삭제하고 빈 문자열로 만들기
            if (gearIndex != -1) {
                player->inventory[gearIndex][0] = '\0';
            }
        }
    }
    
	// 캠핑 장비 미보유 시 판단 or 전투 발생
    else {
        printf("장비가 없어 적당한 자리를 찾기 위해 판단이 필요합니다.\n");
        printf("어떤 방법으로 야영을 시도하시겠습니까?\n\n");

        printf("1) 주변 지형을 살펴 민첩하게 안전한 곳을 찾는다.\n");
        printf("2) 지식을 활용해 안정적인 장소를 판단한다.\n");
        printf("> ");

        int choice;
        scanf("%d", &choice);
        while (getchar() != '\n');

        int difficulty = 12;
        int success = 0;

		// 선택지별 판정
        switch (choice) {
		case 1: // 민첩 판정
            printf("주변 지형을 빠르게 살펴봅니다...\n");
            success = StatCheck("민첩", player->Dex, difficulty);
            break;
		case 2: // 지능 판정
            printf("환경을 분석해 안전한 장소를 찾습니다...\n");
            success = StatCheck("지능", player->Int, difficulty);
            break;
		default: // 잘못된 입력 시 민첩 판정
            printf("올바르지 않은 선택입니다. 민첩 판정을 진행합니다.\n");
            success = StatCheck("민첩", player->Dex, difficulty);
            break;
        }

		// 성공: 안전한 장소 확보
        if (success) {
            printf("안전한 장소를 확보하고 조용히 휴식을 취합니다.\n");
            int heal = 20;
            player->hp += heal;
            if (player->hp > player->maxHp)
                player->hp = player->maxHp;
            printf("HP +%d (%d/%d)\n", heal, player->hp, player->maxHp);
        }
		// 실패: 곰 습격 이벤트 발생
        else {
            printDivider("곰 습격");
            printf("야영 도중 갑작스러운 곰의 습격을 받았습니다!\n");
            Enemy bear;
            initEnemy(&bear, "야생 곰", 100, 15, 0);
            battle(player, &bear);
        }
    }
    player->lastCampingTurn = player->turnCount;
    EnterToContinue();
}

/*
* 동굴 탐험 이벤트
* 플레이어의 동굴 깊이에 따라 층별 이름/설명 출력, 선택 처리(탐험 진행/복귀),
* 각 층에서 발생하는 하위 이벤트를 호출하고 보스 처치 후 보상/복귀를 처리합니다.
*/
void Event_Cave(Player* player) {
    printDivider("어둠 속으로");

	char* depthName = ""; // 층 이름
	char* depthDesc = ""; // 층 설명

	// 층별 이름 및 설명 설정
    if (player->caveDepth == 1) {
        depthName = "지하 1층 [동굴 입구]";
        depthDesc = "축축한 이끼 냄새와 썩은 흙냄새가 진동합니다. 아직은 빛이 조금 들어옵니다.";
    }
    else if (player->caveDepth == 2) {
        depthName = "지하 2층 [심층부]";
        depthDesc = "완전한 암흑입니다. 바닥이 불안정하고, 멀리서 짐승의 숨소리가 들립니다.";
    }
    else {
        depthName = "지하 3층 [보스 룸]";
        depthDesc = "거대한 진동이 동굴 전체를 울립니다. 이 앞에는 강력한 존재가 있습니다.";
    }

    printf("[ 현재 위치: %s ]\n", depthName);
    printf("%s\n\n", depthDesc);

    printf("1) 더 깊은 곳으로 나아간다 (탐험 진행)\n");
    printf("2) 지상으로 돌아간다 (포기 및 복귀)\n");
    printf("> ");

    int choice;
    if (scanf("%d", &choice) != 1) {
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

	// 포기 선택 시 복귀 처리
    if (choice == 2) {
        printf("무리하지 않기로 합니다. 왔던 길을 되돌아 나갑니다.\n");
		player->location = LOC_ROAD; // 도로로 복귀
        player->caveDepth = 0; // 동굴 깊이 초기화
        EnterToContinue();
        return;
    }

    printf("\n랜턴을 높이 들고 어둠 속으로 발을 내딛습니다...\n");
    EnterToContinue();

    // 층별 이벤트 발생
    if (player->caveDepth == 1) {
        int r = rand() % 4;
		// 랜덤 이벤트 4개 중 1개 발생
        switch (r) {
		case 0: // 박쥐 떼
            Cave_BatSwarm(player);
            break;
		case 1: // 독 안개
            Cave_PoisonMist(player);
            break;
		case 2: // 빛나는 광석
            Cave_ShiningOre(player);
            break;
		case 3: // 고대 조각상
            Cave_AncientCarving(player); 
            break;
        }

		// 다음 층으로 이동
        if (player->hp > 0) {
            printf("\n더 깊은 곳으로 이어지는 비탈길을 발견했습니다.\n");
            printf("지하 2층으로 내려갑니다.\n");
            player->caveDepth = 2;
        }
    }

	// 지하 2층 이벤트
    else if (player->caveDepth == 2) {
        int r = rand() % 4;
		// 랜덤 이벤트 4개 중 1개 발생
        switch (r) {
		case 0: // 낙석 함정
            Cave_FallingFloor(player); 
            break;
		case 1: // 지하 호수
            Cave_UndergroundLake(player);
            break;
		case 2: // 죽은 모험가
            Cave_DeadAdventurer(player);
            break;
		case 3: // 죽은 경비병
            Cave_DeadGuard(player);
            break;
        }

		// 다음 층으로 이동
        if (player->hp > 0) {
            printf("\n거대한 돌문이 앞을 막고 있습니다. 문 틈으로 붉은 안광이 보입니다.\n");
            printf("이 문을 열면 되돌아갈 수 없습니다.\n");
            printf("지하 3층(보스)으로 진입합니다.\n");
            player->caveDepth = 3;
        }
    }
	// 지하 3층 이벤트
    else {
        // 문지기 골렘 보스전
        printDivider("문지기 골렘");
        printf("어둠 속에서 바위들이 뭉쳐 거대한 인간형 형상을 이룹니다!\n");
        printf("고대 마법으로 움직이는 [문지기 골렘]이 앞을 가로막습니다!\n");

		// 보스 생성 및 전투
        Enemy boss;
        initEnemy(&boss, "문지기 골렘", 200, 15, 100);
        battle(player, &boss);

		// 보스 격파 시 보상 및 복귀 처리
        if (player->hp > 0) {
            printDivider("동굴 정복!");
            printf("쿠구구궁...!\n");
            printf("골렘이 무너져 내린 잔해 속에서 반짝이는 물건을 발견했습니다.\n");

            if (player->hasCityPass) {
                printf("이미 가지고 있는 [대도시 출입증]입니다.\n");
            }
            else {
                printf("실종된 경비병의 유품인 [대도시 출입증]을 획득했습니다!\n");
                printf("이제 검문소를 통과하여 대도시로 들어갈 수 있습니다.\n");
                player->hasCityPass = 1;
            }

            printf("\n힘겨운 전투를 통해 성장했습니다.\n");
            player->Str += 2;
            player->Def += 2;
            printf(">> [ Str +2 ] [ Def +2 ]\n");

            printf("\n목표를 달성했습니다. 동굴 입구로 귀환합니다.\n");
            player->location = LOC_ROAD;
            player->caveDepth = 0;
        }
		// 보스전 패배 시 처리
        else {
            printf("눈앞이 캄캄해집니다...\n");
        }
    }

    EnterToContinue();
}

/*
* 박쥐 떼의 습격 이벤트
* 민첩 판정으로 회피 가능, 실패 시 HP 감소
*/
void Cave_BatSwarm(Player* player) {
    printDivider("박쥐 떼의 습격");
    printf("천장의 어둠 속에서 수많은 붉은 눈동자가 당신을 내려다보고 있습니다.\n");
    printf("랜턴 빛에 자극받은 박쥐 떼가 기괴한 비명과 함께 쏟아져 내립니다!\n");

    // [난이도 상향] 민첩 12
    if (StatCheck("민첩", player->Dex, 12)) {
        printf("당신은 본능적으로 바닥에 엎드리며 망토로 몸을 감쌌습니다.\n");
        printf("박쥐들은 허공을 할키다 흩어졌습니다. 피해 없이 넘깁니다.\n");
    }
    else {
        int dmg = 6 + rand() % 5;
        printf("피할 곳이 없습니다! 날카로운 발톱과 이빨이 살갗을 파고듭니다.\n");
        player->hp -= dmg;
        if (player->hp < 0) player->hp = 0;
        printf(">> [ HP -%d ]\n", dmg);
    }
    EnterToContinue();
}

/*
* [지하 1층] 독 안개 이벤트
* 지능 판정으로 무사 통과, 실패 시 중독 피해
*/
void Cave_PoisonMist(Player* player) {
    printDivider("유독 가스 지대");
    printf("갈라진 바닥 틈새로 매캐한 녹색 기체가 뿜어져 나오고 있습니다.\n");
    printf("들이마시는 순간 폐가 타들어 갈 것 같은 지독한 냄새입니다.\n");

    // [난이도 상향] 지능 12
    if (StatCheck("지능", player->Int, 12)) {
        printf("젖은 천으로 입을 막고, 공기 흐름이 빠른 쪽으로 신속하게 이동했습니다.\n");
        printf("지식 덕분에 질식하지 않고 통과했습니다.\n");
    }
    else {
        int dmg = 10 + rand() % 6;
        printf("콜록! 콜록! 가스를 마시고 말았습니다. 시야가 흐려지고 속이 메스껍습니다.\n");
        player->hp -= dmg;
        if (player->hp < 0) player->hp = 0;
        printf(">> [ HP -%d ] (중독 피해)\n", dmg);
    }
    EnterToContinue();
}

/*
* [지하 1층] 희귀 광맥 이벤트
* 선수택형: 힘으로 채굴 시 성공하면 골드 획득, 실패 시 피해
*/
void Cave_ShiningOre(Player* player) {
    printDivider("희귀한 광맥");
    printf("동굴 벽면의 바위 틈새에서 영롱한 보랏빛이 새어 나오고 있습니다.\n");
    printf("상당히 값나가는 광석 같지만, 바위가 단단해 캐내기가 쉽지 않아 보입니다.\n");

    printf("1) 힘으로 바위를 부수고 캐낸다 (힘 활용)\n");
    printf("2) 장비가 상할 것 같다. 지나간다\n");
    printf("> ");

    int c;
    scanf("%d", &c);
    while (getchar() != '\n');

    // 1: 힘 판정
    if (c == 1) {
        printf("곡괭이 대신 무기로 바위를 강하게 내리칩니다!\n");
        // [난이도 상향] 힘 13
        if (StatCheck("힘", player->Str, 13)) {
            int gold = 40 + rand() % 31;
            printf("콰앙! 바위가 쪼개지며 고순도 마력석이 쏟아집니다.\n");
            printf("상점에 팔면 꽤 돈이 될 것입니다.\n");
            player->gold += gold;
            printf(">> [ Gold +%d ]\n", gold);
        }
		// 실패: 피해
        else {
            printf("깡! 불꽃만 튀었을 뿐, 바위는 꿈쩍도 하지 않습니다.\n");
            printf("오히려 충격으로 손목을 삐끗했습니다.\n");
            int dmg = 8;
            player->hp -= dmg; if (player->hp < 0) player->hp = 0;
            printf(">> [ HP -%d ]\n", dmg);
        }
    }
    else {
        printf("욕심부리지 않고 안전하게 지나갑니다.\n");
    }
    EnterToContinue();
}

/*
* [지하 1층/2층] 고대 벽화 이벤트
* 지능 판정 성공 시 지식 습득(지능 증가), 실패 시 아무 변화 없음
*/
void Cave_AncientCarving(Player* player) {
    printDivider("고대 벽화");
    printf("침식된 암벽에 오래된 각인이 남아있습니다.\n");
    printf("순환, 결박, 승화를 나타내는 기하학적 문양이 희미하게 빛나고 있습니다.\n");

    // [난이도 상향] 지능 13
    if (StatCheck("지능", player->Int, 13)) {
        printf("문양의 배열을 해석하자, 고대인들이 남긴 마력 운용법이 머릿속에 들어옵니다.\n");
        player->Int += 1;
        printf(">> [ Int +1 ] (영구 상승)\n");
    }
    else {
        printf("머리가 지끈거립니다. 해석하기엔 지식이 부족하거나 문양이 너무 훼손되었습니다.\n");
    }
    EnterToContinue();
}

/*
* [지하 2층] 바닥 붕괴 이벤트
* 민첩 판정으로 낙하 회피, 실패 시 낙하 피해
*/
void Cave_FallingFloor(Player* player) {
    printDivider("무너지는 바닥");
    printf("발을 딛는 순간, '뚝' 하는 소리와 함께 지반이 아래로 꺼집니다!\n");
    printf("지하 공동으로 추락할 위기입니다.\n");

    // [난이도 상향] 민첩 14
    if (StatCheck("민첩", player->Dex, 14)) {
        printf("순간적으로 벽면에 박힌 나무 뿌리를 낚아챘습니다!\n");
        printf("아슬아슬하게 매달려 위기로부터 탈출합니다. 곡예사 같은 움직임이었습니다.\n");
    }
    else {
        int dmg = 15 + rand() % 6;
        printf("으아악! 중심을 잃고 어둠 속으로 굴러떨어집니다.\n");
        printf("한참을 구르다 바닥에 처박혔습니다.\n");
        player->hp -= dmg;
        if (player->hp < 0) player->hp = 0;
        printf(">> [ HP -%d ] (낙하 피해)\n", dmg);
    }
    EnterToContinue();
}

/*
* [지하 2층] 검은 지하 호수 이벤트
* 선택지: 물 마시기(회복 또는 피해), 아니면 회피
*/
void Cave_UndergroundLake(Player* player) {
    printDivider("검은 지하 호수");
    printf("빛 한 점 없는 지하 깊은 곳, 거울처럼 잔잔한 검은 호수가 앞을 막습니다.\n");
    printf("물가에는 누군가가 제를 올린 듯한 흔적이 남아 있습니다.\n");
    printf("목이 마르지만, 물에 무엇이 섞여 있을지 모릅니다.\n\n");

    printf("1) 목을 축인다 (체력 회복 시도)\n");
    printf("2) 불길하다. 건드리지 않는다\n");
    printf("> ");

    int c;
    scanf("%d", &c);
    while (getchar() != '\n');

    // 1: 체력 회복 시도
    if (c == 1) {
        printf("조심스럽게 물을 떠서 마십니다...\n");
        int r = rand() % 2; // 50:50
        if (r == 0) {
            printf("놀랍도록 맑고 시원한 물입니다! 피로가 씻은 듯이 사라집니다.\n");
            int heal = 30;
            player->hp += heal;
            if (player->hp > player->maxHp) player->hp = player->maxHp;
            printf(">> [ HP +%d ]\n", heal);
        }
        else {
            printf("퉤! 혀가 타들어 가는 듯한 쓴맛입니다. 독이 섞여 있습니다!\n");
            int dmg = 15;
            player->hp -= dmg;
            if (player->hp < 0) player->hp = 0;
            printf(">> [ HP -%d ]\n", dmg);
        }
    }
	// 2: 회피
    else {
        printf("수면 아래에서 무언가 꿈틀거린 것 같습니다. 조용히 자리를 뜹니다.\n");
    }
    EnterToContinue();
}

/*
* [지하 2층] 모험가의 시체 이벤트
* 배낭 뒤지기 선택 시 전투 또는 아이템/골드 획득 가능
*/
void Cave_DeadAdventurer(Player* player) {
    printDivider("최후를 맞이한 모험가");
    printf("구석진 곳에 낡은 갑옷을 입은 백골 시체가 앉아 있습니다.\n");
    printf("손에는 다 쓴 횃불을 쥐고 있고, 배낭은 반쯤 열려 있습니다.\n");
    printf("동굴의 마기에 오염되었을 수도 있습니다.\n\n");

    printf("1) 배낭을 뒤져본다\n");
    printf("2) 죽은 자에 대한 예의를 지켜 지나간다\n");
    printf("> ");

    int c;
    scanf("%d", &c);
    while (getchar() != '\n');

    if (c == 1) {
        int r = rand() % 100;
        if (r < 40) { // 40% 확률로 전투 발생 (언데드)
            printf("덜그럭... 뼈가 맞춰지는 소리가 들립니다.\n");
            printf("검은 기운이 백골을 감싸더니, 해골 병사가 되어 일어납니다!\n");

            Enemy undead;
            initEnemy(&undead, "해골 병사", 60 + rand() % 20, 10 + rand() % 5, 30);
            battle(player, &undead);
        }
        else {
            printf("조심스럽게 가방을 열어봅니다.\n");
            // 60% 확률로 아이템 획득
            int itemR = rand() % 2;
            if (itemR == 0) {
                printf("유리병 부딪히는 소리가 납니다. 다행히 깨지지 않은 포션을 찾았습니다!\n");
                addItem(player, "HP 포션");
                printf(">> [ 아이템 획득: HP 포션 ]\n");
            }
            else {
                int gold = 50 + rand() % 30;
                printf("가방 깊숙한 곳에서 비상금을 발견했습니다.\n");
                player->gold += gold;
                printf(">> [ Gold +%d ]\n", gold);
            }
        }
    }
    else {
        printf("당신은 잠시 묵념을 올리고 자리를 떠납니다.\n");
    }
    EnterToContinue();
}

/*
* [지하 2층] 대도시 경비병의 시체 이벤트
* 유품 확인으로 대도시 출입증 및 골드 획득 가능
*/
void Cave_DeadGuard(Player* player) {
    printDivider("사망한 대도시 경비병");
    printf("바위 뒤편에 익숙한 문장이 새겨진 갑옷을 입은 시신이 있습니다.\n");
    printf("대도시 경비병입니다. 실종되었다던 그 사람인 것 같습니다.\n");
    printf("몬스터에게 당한 듯 처참한 몰골입니다.\n\n");

    printf("1) 유품을 확인한다\n");
    printf("2) 건드리지 않는다\n");
    printf("> ");

    int c;
    scanf("%d", &c);
    while (getchar() != '\n');

    if (c == 1) {
        printf("품속을 뒤지자 피 묻은 금속 카드가 나옵니다.\n");
        if (player->hasCityPass) {
            printf("이미 출입증을 가지고 있습니다. 고이 품에 넣어줍니다.\n");
        }
        else {
            printf("대도시의 정식 출입증입니다! 이것만 있으면 검문소를 통과할 수 있습니다.\n");
            player->hasCityPass = 1;
            printf(">> [ 중요 아이템 획득: 대도시 출입증 ]\n");
        }
        // 추가로 약간의 골드
        int gold = 20 + rand() % 11;
        player->gold += gold;
        printf(">> [ Gold +%d ] (경비병의 지갑)\n", gold);
    }
    else {
        printf("그의 안식을 방해하지 않기로 합니다.\n");
    }
    EnterToContinue();
}

/*
* [랜덤/중간 보스] 돌 골렘 전투 함수
* 일반 몬스터로 사용 가능하며 처치 시 방어력 상승 보상 부여
*/
void Cave_StoneGolem(Player* player) {
    printDivider("거대한 그림자");
    printf("통로를 막고 있던 바위더미가 갑자기 일어나 당신을 내려다봅니다.\n");
    printf("동굴의 파수꾼, [돌 골렘]입니다!\n");

    Enemy golem;
    initEnemy(&golem, "돌 골렘", 150, 15, 80);
    battle(player, &golem);

    if (player->hp > 0) {
        printf("골렘이 무너지며 단단한 광물 파편을 남깁니다.\n");
        player->Def += 1;
        printf(">> [ Def +1 ] (단단한 껍질 연구)\n");
    }
    EnterToContinue();
}

/*
* 마을 진입 이벤트 메인
* 마을 메뉴 출력 및 각 건물/상점/행동으로 분기 처리
*/
void Event_Town(Player* player) {
    int choice;
    while (1) {
        ClearScreen();
        printDivider("변방의 작은 마을 (Outpost)");
        printf("동굴 근처에 위치한, 나무 울타리로 둘러싸인 작은 마을입니다.\n");
        printf("굴뚝에서는 저녁 짓는 연기가 피어오르고, 경비병들이 졸린 눈으로 보초를 서고 있습니다.\n");
        printf("모험에 지친 몸을 쉬어가기엔 충분해 보입니다.\n");
        printPlayerStatus(player);

        printf("\n[ 마을 메뉴 ]\n");
        printf("1) 낡은 여관 (휴식: 10G)\n");
        printf("2) 잡화점 (물약/도구 구매)\n");
        printf("3) 대장간 (장비 강화)\n");
        printf("4) 촌장님 댁 (정보/소문)\n");
        printf("5) 인벤토리 / 아이템 사용\n");
        printf("6) 마을을 떠난다\n");
        printf("> ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
        case 1:
            Town_Inn(player);
            break;
        case 2:
            Town_Shop(player);
            break;
        case 3:
            Town_Blacksmith(player);
            break;
        case 4:
            Town_Legend(player);
            break;
        case 5:
            useItem(player, -1); // 인벤토리 열기
            EnterToContinue();
            break;
        case 6:
            printf("당신은 채비를 마치고 마을 문을 나섭니다.\n");
            printf("다시 차가운 바람이 부는 길거리로 향합니다.\n");
            EnterToContinue();
            player->location = LOC_ROAD;
            return;
        default:
            printf("잘못된 선택입니다.\n");
            EnterToContinue();
            break;
        }
    }
}

/*
* 여관 이벤트
* 선택에 따라 골드를 소모하고 HP 회복 처리
*/
void Town_Inn(Player* player) {
    printDivider("낡은 여관");
    printf("삐그덕거리는 나무 문을 열자 따뜻한 온기와 스튜 냄새가 반겨줍니다.\n");
    printf("주인은 행주로 잔을 닦으며 무뚝뚝하게 가격표를 가리킵니다.\n");
    printf("1) 하룻밤 묵는다 (10 골드 / HP +40)\n");
    printf("2) 그냥 나간다\n");
    printf("> ");

    int c;
    if (scanf("%d", &c) != 1) { while (getchar() != '\n'); return; }
    while (getchar() != '\n');

    // 1: 숙박 선택
    if (c == 1) {
        // 골드 부족 체크
        if (player->gold < 10) {
            printf("주인: \"돈이 없으면 헛간에서 자야지. 방은 못 내주네.\"\n");
        }
        // 숙박 진행
        else {
            player->gold -= 10; // 골드 차감
            player->hp += 40;   // 체력 회복

            // 최대 체력 초과 방지
            if (player->hp > player->maxHp) player->hp = player->maxHp;

            printf("딱딱한 침대였지만, 오랜만에 지붕 아래에서 깊은 잠을 잤습니다.\n");
            printf("몸이 한결 가벼워졌습니다.\n");
            printf(">> [ Gold -10 ] [ HP 회복 ] (현재 HP: %d)\n", player->hp);
        }
    }
	// 2: 나가기
    else {
        printf("아직은 쉴 때가 아니라고 생각하며 여관을 나옵니다.\n");
    }
    EnterToContinue();
}

/*
* 잡화점 이벤트
* 여러 아이템을 판매, 골드 체크 및 아이템 추가 처리
*/
void Town_Shop(Player* player) {
    printDivider("잡화점");
    printf("온갖 잡동사니가 천장까지 쌓여 있는 가게입니다.\n");
    printf("상인은 당신의 주머니 사정을 계산하듯 눈을 가늘게 뜨고 쳐다봅니다.\n");

    printf("1) HP 포션 (20 G) - 체력 30 회복\n");
    printf("2) 용기의 반지 (50 G) - 힘(Str) +1 영구 상승\n");
    printf("3) 캠핑 장비 (40 G) - 야영 시 체력 회복량 증가\n");
    printf("4) 나가기\n");
    printf("> ");

    int sel;
    scanf("%d", &sel);
    while (getchar() != '\n');

    // 1: HP 포션 구매
    if (sel == 1) {
        if (player->gold >= 20) {
            player->gold -= 20;
            addItem(player, "HP 포션");
            printf("상인: \"가장 잘 팔리는 물건이지. 목숨은 소중하니까.\"\n");
            printf(">> [ Gold -20 ] [ 아이템 획득: HP 포션 ]\n");
        }
        else printf("상인: \"외상은 사절이야.\"\n");
    }
    // 2: 용기의 반지 구매
    else if (sel == 2) {
        if (player->gold >= 50) {
            player->gold -= 50;
            player->Str += 1; // 힘 스탯 상승
            printf("반지를 손가락에 끼우자 미세한 전류가 흐르며 근력이 강화됩니다.\n");
            printf(">> [ Gold -50 ] [ Str +1 ]\n");
        }
        else printf("상인: \"그건 자네가 살 수 있는 물건이 아니야.\"\n");
    }
    // 3: 캠핑 장비 구매
    else if (sel == 3) {
        if (player->gold >= 40) {
            player->gold -= 40;
            addItem(player, "캠핑 장비");
            printf("상인: \"이것만 있으면 숲속에서도 꿀잠을 잘 수 있지.\"\n");
            printf(">> [ Gold -40 ] [ 아이템 획득: 캠핑 장비 ]\n");
        }
        else printf("상인: \"돈을 더 모아오게.\"\n");
    }
    // 4: 나가기
    else {
        printf("구경만 하다가 가게를 나옵니다.\n");
    }
    EnterToContinue();
}

/*
* 대장간 이벤트
* 골드 지불 시 장비 강화(능력치 상승)
*/
void Town_Blacksmith(Player* player) {
    printDivider("대장간");
    printf("훅 끼쳐오는 열기와 쇠를 두드리는 굉음이 가득한 곳입니다.\n");
    printf("근육질의 대장장이가 땀을 닦으며 당신의 장비를 훑어봅니다.\n");
    printf("대장장이: \"흐음, 날이 많이 무뎌졌군. 손 좀 봐줄까?\"\n\n");

    printf("1) 무기 연마 (50 G) - 힘(Str) +1\n");
    printf("2) 갑옷 보강 (50 G) - 방어(Def) +1\n");
    printf("3) 떠난다\n");
    printf("> ");

    int sel;
    scanf("%d", &sel);
    while (getchar() != '\n');

	// 1: 무기 연마
    if (sel == 1) {
        if (player->gold >= 50) {
            player->gold -= 50;
            player->Str += 1;
            printf("대장장이가 당신의 무기를 정밀하게 갈고 닦았습니다! 공격력이 크게 상승합니다. (Str +1)\n");
        }
		// 골드 부족
        else {
            printf("대장장이: \"최고의 기술에는 정당한 대가가 필요한 법이네. 돈을 더 가져오게.\n");
        }
    }
	// 방어구 보강
    else if (sel == 2) {
        if (player->gold >= 50) {
            player->gold -= 50;
            player->Def += 1;
            printf("특수 합금으로 갑옷의 약점을 완벽하게 보강했습니다! 방어력이 크게 상승합니다. (Def +1)\n");
        }
		// 골드 부족
        else {
            printf("대장장이: \"재료비도 안 나오겠군. 돌아가게.\n");
        }
    }
    else {
        printf("대장간을 나옵니다.\n");
    }
    EnterToContinue();
}

/*
* 촌장님 댁 이벤트
* 동굴 위치 해금 퀘스트 및 상황별 대화 처리
*/
void Town_Legend(Player* player) {
    printDivider("촌장님 댁");

    // 1. 대도시 출입증을 가져왔을 때 (퀘스트 완료)
    if (player->hasCityPass) {
        printf("촌장님은 당신이 보여준 경비병의 출입증을 보고 잠시 침묵합니다.\n\n");
        printf("촌장: \"...결국 그렇게 되었구만. 안타까운 일이야.\"\n");
        printf("촌장: \"하지만 자네 덕분에 그의 영혼도 위로받았을 걸세.\"\n");
        printf("촌장: \"자, 이제 대도시로 가게. 자네라면 마왕을 막을 수 있을지도 몰라.\"\n");
        EnterToContinue();
        return;
    }

    // 2. 동굴 위치를 이미 알고 있을 때 (힌트 제공)
    if (player->knowsCave) {
        printf("촌장님은 지도를 펴놓고 고민에 빠져 있습니다.\n\n");
        printf("촌장: \"내가 말해준 [동굴]은 마을 밖으로 나가면 찾을 수 있을 걸세.\"\n");
        printf("촌장: \"소문에 따르면 동굴 3층에 무시무시한 괴물이 살고 있다더군.\"\n");
        printf("촌장: \"[대도시 출입증]도 놈의 뱃속에 있을지 몰라. 부디 조심하게.\"\n");
        EnterToContinue();
        return;
    }

    // 3. 일반 대화 (랜덤 출력 + 퀘스트 시작)
    int talk = rand() % 5;

    switch (talk) {
    case 0:
        printf("촌장: \"올해는 가뭄이 들어서 큰일이야... 마왕의 기운 때문인가?\"\n");
        printf("촌장: \"자네도 밥은 굶지 말고 다니게. 건강이 최고야.\"\n");
        printf("(촌장님은 곰방대를 태우며 한숨을 쉬십니다.)\n");
        break;
    case 1:
        printf("촌장: \"내 무릎이 쑤시는 걸 보니 곧 비가 오려나 보네.\"\n");
        printf("촌장: \"이 늙은이의 무릎은 기상청보다 정확하다니까... 허허.\"\n");
        break;
    case 2:
        printf("촌장: \"마을의 젊은이들은 다들 성공하겠다며 대도시로 떠나버렸어.\"\n");
        printf("촌장: \"남은 건 우리 같은 늙은이들 뿐이라네.\"\n");
        printf("(촌장님은 쓸쓸한 표정으로 창밖을 바라봅니다.)\n");
        break;
    case 3:
        printf("촌장: \"들리는 소문으로는 국왕 폐하께서 [전설의 검]을 찾고 계신다더군.\"\n");
        printf("촌장: \"전설의 숲 깊은 곳에 잠들어 있다는데... 그게 정말 존재하기나 하는 건지 원.\"\n");
        break;
    case 4:
        // 퀘스트 시작: 동굴 위치 해금
        printf("촌장: \"...자네, 눈빛이 예사롭지 않구만. 혹시 대도시로 가려는가?\"\n");
        printf("촌장: \"그렇다면 내 안타까운 부탁을 하나 들어주겠나.\"\n\n");

        printf("촌장: \"얼마 전 경비병 하나가 마을 근처 [동굴]로 정찰을 나갔다가 소식이 끊겼다네.\"\n");
        printf("촌장: \"그가 지니고 있던 [대도시 출입증]이 있다면, 자네도 검문소를 통과할 수 있을 거야.\"\n");
        printf("촌장: \"내 자네에게 동굴이 있는 곳을 알려주지.\"\n");

        printf("\n>> [ 정보 획득 ] 동굴의 위치가 해금되었습니다!\n");
        printf(">> 이제 길거리(Road) 탐험 중 [동굴]을 발견할 수 있습니다.\n");

        player->knowsCave = 1;
        break;
    }

    EnterToContinue();
}

/*
* 대도시(왕국 수도) 이벤트 메인
* 도시 내 여러 구역과 상점, 기능으로 분기 처리
*/
void Event_BigCity(Player* player) {
    int choice;
    while (1) {     
        ClearScreen();
        printDivider("왕국 수도 (Capital City)");
        printf("거대한 성벽과 뾰족한 첨탑들이 하늘을 찌를 듯 솟아있는 대도시입니다.\n");
        printf("잘 닦인 대리석 바닥 위로 귀족, 상인, 모험가들이 바쁘게 오가고 있습니다.\n");
        printf("이곳에는 당신이 상상할 수 있는 모든 즐거움과 기회가 있습니다.\n");
        printPlayerStatus(player);

        printf("\n[ 편의 시설 ]\n");
        printf("1) 그랜드 호텔 (최고급 휴식)\n");
        printf("2) 왕립 백화점 (상급 아이템)\n");
        printf("3) 왕실 대장간 (정밀 강화)\n");

        printf("\n[ 도시 구역 ]\n");
        printf("4) 왕궁 (국왕 알현)\n");
        printf("5) 대성당 (축복/치유)\n");
        printf("6) 마법 길드 (마력 훈련)\n");
        printf("7) 모험가 길드 (현상금 사냥)\n");

        printf("\n[ 유흥 지구 ]\n");
        printf("8) 로얄 카지노 (도박)\n");
        printf("9) 지하 투기장 (전투 대회)\n");
        printf("10) 인벤토리 / 아이템 사용\n");
        printf("11) 도시를 떠난다\n");
        printf("> ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); 
            continue;
        }
        while (getchar() != '\n');

		// 각 선택지에 따른 분기 처리
        switch (choice) {
		case 1: // 그랜드 호텔 
            BigCity_Inn(player);
            break;
		case 2: // 왕립 백화점
            BigCity_PremiumShop(player); 
            break;
		case 3: // 왕실 대장간
            BigCity_Blacksmith(player); 
            break;
		case 4: // 왕궁
            BigCity_RoyalPalace(player);
            break;
		case 5: // 대성당
            BigCity_Temple(player);
            break;
		case 6: // 마법 길드
            BigCity_MageGuild(player);
            break;
		case 7: // 모험가 길드
            BigCity_AdventurerGuild(player);
            break;
		case 8: // 로얄 카지노
            BigCity_Gamble(player); 
            break;
		case 9: // 지하 투기장
            BigCity_Arena(player);
            break;
		case 10: // 인벤토리 / 아이템 사용
            useItem(player, - 1);
            EnterToContinue();
            break;
		case 11: // 도시 떠나기
            printf("성문을 나섭니다. 수문장들이 경례를 붙입니다.\n");
            player->location = LOC_ROAD;
            EnterToContinue();
            return;
        default:
            break;
        }

        if (player->hp <= 0) return;
    }
}

/*
* 왕실 대장간
* 고단위 강화(골드 소모)로 능력치 크게 상승
*/
void BigCity_Blacksmith(Player* player) {
    printDivider("왕실 대장간");
    printf("왕궁에 납품하는 장인들이 모인 곳입니다. 마을 대장간과는 차원이 다른 기술력을 자랑합니다.\n");
    printf("더 비싸지만, 훨씬 강력한 장비로 개조해줍니다.\n\n");

    printf("1) 무기 정밀 개조 (100G / 힘 +2)\n");
    printf("2) 갑옷 특수 보강 (100G / 방어 +2)\n");
    printf("3) 떠난다\n");
    printf("> ");

    int c;
    scanf("%d", &c); while (getchar() != '\n');

	// 무기 개조
    if (c == 1) {
        if (player->gold >= 100) {
            player->gold -= 100;
            player->Str += 2;
            printf("장인이 당신의 무기를 정밀하게 갈고 닦았습니다! 공격력이 크게 상승합니다. (Str +2)\n");
        }
		// 골드 부족
        else {
            printf("대장장이: \"최고의 기술에는 정당한 대가가 필요한 법이네. 돈을 더 가져오게.\n");
        }
    }
	// 방어구 보강
    else if (c == 2) {
        if (player->gold >= 100) {
            player->gold -= 100;
            player->Def += 2;
            printf("특수 합금으로 갑옷의 약점을 완벽하게 보강했습니다! 방어력이 크게 상승합니다. (Def +2)\n");
        }
		// 골드 부족
        else {
            printf("대장장이: \"재료비도 안 나오겠군. 돌아가게.\n");
        }
    }
    else {
        printf("대장간을 나옵니다.\n");
    }
    EnterToContinue();
}

/*
* 그랜드 호텔
* 고가의 숙박으로 HP 완전 회복
*/
void BigCity_Inn(Player* player) {
    printDivider("그랜드 호텔");
    printf("대리석 바닥과 샹들리에가 빛나는 최고급 숙소입니다.\n");
    printf("이곳의 침대는 구름 위에 누운 듯한 편안함을 제공합니다.\n");

    printf("1) 스위트룸 숙박 (50 골드 / HP 완전 회복)\n");
    printf("2) 로비에서 나간다\n");
    printf("> ");

    int c;
    if (scanf("%d", &c) != 1) { while (getchar() != '\n'); return; }
    while (getchar() != '\n');

	// 1: 숙박 선택
    if (c == 1) {
        // 골드 부족 체크
        if (player->gold < 50) {
            printf("지배인: \"손님, 골드가 부족해 보이시는군요.\"\n");
        }
        // 숙박 진행
        else {
            player->gold -= 50;
            player->hp = player->maxHp;
            printf("최고급 요리와 마사지, 숙면을 즐겼습니다.\n");
            printf("HP가 완전히 회복되었습니다! (%d/%d)\n", player->hp, player->maxHp);
        }
    }
	// 2: 나가기
    else {
        printf("아직은 쉴 때가 아니라고 생각하며 여관을 나옵니다.\n");
    }
    EnterToContinue();
}

/*
* 로얄 카지노
* 사용자 배팅 후 주사위로 승패 결정, 골드 증감 처리
*/
void BigCity_Gamble(Player* player) {
    printDivider("로얄 카지노");
    printf("자욱한 연기 사이로 칩 부딪히는 소리와 환호성, 비명소리가 뒤섞입니다.\n");
    printf("딜러가 매끄러운 손놀림으로 주사위를 쥐고 당신을 쳐다봅니다.\n");
    printf("딜러: \"판돈을 거시겠습니까? 이기면 2배, 비기면 본전입니다.\"\n");

    printf("배팅할 금액을 입력하세요 (0: 나가기, 보유: %d G): ", player->gold);

    int bet;
	// 입력 검증
    if (scanf("%d", &bet) != 1) {
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

	// 0: 나가기
    if (bet <= 0) {
        printf("도박장을 떠납니다.\n");
        EnterToContinue();
        return;
    }

	// 배팅금이 소지금 초과 시
    if (bet > player->gold) {
        printf("딜러: \"돈이 없으시군요. 장난치지 마십시오.\"\n");
        EnterToContinue();
        return;
    }

	// 배팅금 차감
    player->gold -= bet;
    printf("\n주사위가 굴러갑니다...\n");

	// 플레이어와 딜러의 주사위 합계 계산
    int pDice1 = 1 + rand() % 6;
    int pDice2 = 1 + rand() % 6;
    int pSum = pDice1 + pDice2;

    int dDice1 = 1 + rand() % 6;
    int dDice2 = 1 + rand() % 6;
    int dSum = dDice1 + dDice2;

    printf("당신: [%d, %d] 합계 %d\n", pDice1, pDice2, pSum);
    printf("딜러: [%d, %d] 합계 %d\n", dDice1, dDice2, dSum);

	// 승패 판정 및 보상 처리
    if (pSum > dSum) {
        printf("\n*** 승리! ***\n");
        printf("배팅금의 2배인 %d 골드를 획득했습니다!\n", bet * 2);
        player->gold += (bet * 2);
    }
    else if (pSum == dSum) {
        printf("\n- 무승부 -\n");
        printf("배팅금을 돌려받습니다.\n");
        player->gold += bet;
    }
    else {
        printf("\n...패배...\n");
        printf("돈을 잃었습니다.\n");
    }
    EnterToContinue();
}

/*
* 지하 투기장
* 리그 선택에 따라 참가비 납부, 전투 후 승리 시 상금 지급
*/
void BigCity_Arena(Player* player) {
    printDivider("지하 투기장");
    printf("철창으로 둘러싸인 경기장 안에서 관중들이 피를 원하며 소리칩니다.\n");
    printf("자신의 강함을 증명하고 상금을 획득할 수 있습니다.\n");
    printf("(주의: 투기장에서 죽으면 게임이 끝납니다!)\n\n");

    printf("1) 루키 리그 (참가비 50G / 상금 100G / 난이도 하)\n");
    printf("2) 베테랑 리그 (참가비 100G / 상금 300G / 난이도 중)\n");
    printf("3) 챔피언 리그 (참가비 300G / 상금 1000G / 난이도 상)\n");
    printf("4) 나간다\n");
    printf("> ");

    int c;
    scanf("%d", &c);
    while (getchar() != '\n');

	// 1: 루키 리그
    if (c == 1) {
        if (player->gold < 50) {
            printf("참가비가 부족합니다.\n");
        }
        else {
            player->gold -= 50;
            printf("경기장관리자: \"자, 이제 시작해보게나!\"\n");
            EnterToContinue();

            // 루키 리그 전투
            Enemy rookieEnemy;
            initEnemy(&rookieEnemy, "길거리 싸움꾼", 70, 10, 0);
            battle(player, &rookieEnemy);

            if (player->hp > 0) {
                printf("소규모 토너먼트를 승리로 이끌었습니다! 상금 100G를 획득했습니다.\n");
                player->gold += 100;
            }
            else {
                printf("전투 중 부상을 당했습니다. 체력 -20\n");
                player->hp -= 20;
                if (player->hp < 0) player->hp = 0;
            }
        }
    }
	// 2: 베테랑 리그
    else if (c == 2) {
        if (player->gold < 100) {
            printf("참가비가 부족합니다.\n");
        }
        else {
            player->gold -= 100;
            printf("경기장관리자: \"이제 좀 흥미로워지는군. 준비되었나?\"\n");
            EnterToContinue();

            // 베테랑 리그 전투
            Enemy veteranEnemy;
            initEnemy(&veteranEnemy, "검투사", 90, 15, 0);
            battle(player, &veteranEnemy);

            if (player->hp > 0) {
                printf("검투사와의 치열한 대결에서 승리했습니다! 상금 300G를 획득했습니다.\n");
                player->gold += 300;
            }
            else {
                printf("전투 중 큰 부상을 당했습니다. 체력 -30\n");
                player->hp -= 30;
                if (player->hp < 0) player->hp = 0;
            }
        }
    }
	// 3: 챔피언 리그
    else if (c == 3) {
        if (player->gold < 300) {
            printf("참가비가 부족합니다.\n");
        }
        else {
            player->gold -= 300;
            printf("경기장관리자: \"최강의 전사들만이 모이는 곳이지. 각오하라구!\"\n");
            EnterToContinue();

            // 챔피언 리그 전투
            Enemy championEnemy;
            initEnemy(&championEnemy, "챔피언", 150, 25, 0);
            battle(player, &championEnemy);

            if (player->hp > 0) {
                printf("챔피언을 쓰러뜨리고 승리했습니다! 막대한 상금 1000G를 획득했습니다.\n");
                player->gold += 1000;
            }
            else {
                printf("치명적인 부상을 당했습니다. 체력 -50\n");
                player->hp -= 50;
                if (player->hp < 0) player->hp = 0;
            }
        }
    }
	// 4: 나가기
    else {
        printf("경기장을 떠납니다.\n");
    }
    EnterToContinue();
}

/*
* 대성당
* 골드를 지불하여 치유 혹은 축복(능력치 상승)을 받음
*/
void BigCity_Temple(Player* player) {
    printDivider("대성당");
    printf("높은 천장과 스테인드글라스를 통해 들어오는 빛이, 먼지 위로 조용히 흩어집니다.\n");
    printf("사제는 피곤해 보이지만, 아직 사람들을 돕고자 하는 의지를 잃지 않았습니다.\n\n");

    printf("1) 치유 의식 (30 골드, HP 완전 회복)\n");
    printf("2) 축복 의식 (60 골드, 모든 능력치 +1)\n");
    printf("3) 그냥 떠난다\n");
    printf("> ");

    int c;
	// 입력 검증
    if (scanf("%d", &c) != 1) {
        while (getchar() != '\n');
        printf("입력 오류.\n");
        EnterToContinue();
        return;
    }
    while (getchar() != '\n');

	// 1: 치유 의식
    if (c == 1) {
        if (player->gold < 30) {
            printf("골드가 부족합니다.\n");
        }
        else {
            player->gold -= 30;
            player->hp = player->maxHp;
            printf("따뜻한 빛이 몸을 감싸며 상처가 아물어 갑니다. HP가 완전히 회복되었습니다.\n");
        }
    }
	// 2: 축복 의식
    else if (c == 2) {
		// 골드 부족 체크
        if (player->gold < 60) {
            printf("골드가 부족합니다.\n");
        }
		// 축복 진행
        else {
            player->gold -= 60;
            player->Str += 1;
            player->Dex += 1;
            player->Def += 1;
            player->Int += 1;
            player->maxHp += 50;
            player->hp = player->maxHp;
            printf("짧은 기도와 함께 이마에 새긴 문양이 희미하게 빛납니다. 모든 능력치가 1씩 증가했습니다.\n");
        }
    }
	// 3: 나가기
    else {
        printf("당신은 조용히 대성당을 빠져나옵니다.\n");
    }
    EnterToContinue();
}

/*
* 상급 상점
* 고급 회복약 및 능력치 아이템 판매
*/
void BigCity_PremiumShop(Player* player) {
    printDivider("상급 상점");
    printf("정돈된 진열대 위로 고급 장비와 특제 약품들이 가지런히 놓여 있습니다.\n");
    printf("상인은 가격을 말하기 전에 당신의 지갑 두께를 먼저 훑어봅니다.\n\n");

    printf("1) 고급 회복약 (40 골드, HP +50)\n");
    printf("2) 힘의 상급 반지 (70 골드, Str +2)\n");
    printf("3) 지식 서적 (70 골드, Int +2)\n");
    printf("4) 떠난다\n");
    printf("> ");

    int c;
    if (scanf("%d", &c) != 1) {
        while (getchar() != '\n');
        printf("입력 오류.\n");
        EnterToContinue();
        return;
    }
    while (getchar() != '\n');

	// 1: 고급 회복약
    if (c == 1) {
        if (player->gold < 40) printf("골드가 부족합니다.\n");
        else {
            player->gold -= 40;
            player->hp += 50;
            if (player->hp > player->maxHp) player->hp = player->maxHp;
            printf("강한 약초 향이 목을 타고 내려갑니다. HP +50\n");
        }
    }
	// 2: 힘의 상급 반지
    else if (c == 2) {
        if (player->gold < 70) printf("골드가 부족합니다.\n");
        else {
            player->gold -= 70;
            player->Str += 2;
            printf("손가락에 낀 반지가 묵직한 힘을 전합니다. Str +2\n");
        }
    }
	// 3: 지식 서적
    else if (c == 3) {
        if (player->gold < 70) printf("골드가 부족합니다.\n");
        else {
            player->gold -= 70;
            player->Int += 2;
            printf("두꺼운 책을 읽고 나니 머리가 맑아진 느낌입니다. Int +2\n");
        }
    }
    else {
        printf("당신은 거래를 하지 않고 상점을 떠납니다.\n");
    }
    EnterToContinue();
}

/*
* 마법 길드
* 판정 기반 훈련으로 능력치/최대체력 보너스 획득 가능
*/
void BigCity_MageGuild(Player* player) {
    printDivider("마법 길드");
    printf("공중에 떠 있는 서적과 빙글도는 룬 조각들이 공간 전체에 흐르고 있습니다.\n");
    printf("마법사는 당신을 흥미로운 눈빛으로 바라봅니다.\n\n");

    printf("1) 마나 구조 강의 (Int 판정 난이도 13, 성공 시 Int +2)\n");
    printf("2) 정신 집중 훈련 (Def 판정 난이도 12, 성공 시 최대 HP +5)\n");
    printf("3) 떠난다\n");
    printf("> ");

    int c;
    if (scanf("%d", &c) != 1) {
        while (getchar() != '\n');
        printf("입력 오류.\n");
        EnterToContinue();
        return;
    }
    while (getchar() != '\n');

	// 1: 마나 구조 강의
    if (c == 1) {
        if (StatCheck("지능", player->Int, 13)) {
            player->Int += 2;
            printf("복잡한 마나 흐름이 조금은 읽히는 것 같습니다. Int +2\n");
        }
        else {
            printf("개념이 뒤엉켜 머리가 지끈거립니다. HP -5\n");
            player->hp -= 5;
            if (player->hp < 0) player->hp = 0;
        }
    }
	// 2: 정신 집중 훈련
    else if (c == 2) {
        if (StatCheck("방어", player->Def, 12)) {
            player->maxHp += 5;
            player->hp += 5;
            printf("정신 집중을 통해 신체의 회복력이 향상됩니다. 최대 HP +5\n");
        }
        else {
            printf("집중이 잘 되지 않습니다. 큰 변화는 없었습니다.\n");
        }
    }
	// 3: 나가기
    else {
        printf("당신은 마법의 기운을 뒤로하고 길드를 나옵니다.\n");
    }
    EnterToContinue();
}

/*
* 모험가 길드 의뢰
* 전투 수락 시 적과 전투하고 보상 획득
*/
void BigCity_AdventurerGuild(Player* player) {
    printDivider("모험가 길드");
    printf("벽에는 수많은 의뢰서와 성공/실패 기록이 빽빽하게 붙어 있습니다.\n");
    printf("접수원이 말합니다. \"도시 주변을 괴롭히는 강적이 하나 있소. 처리해볼 텐가?\"\n\n");

    printf("1) 수락한다 (전투)\n");
    printf("2) 거절한다\n");
    printf("> ");

    int c;
    if (scanf("%d", &c) != 1) {
        while (getchar() != '\n');
        printf("입력 오류.\n");
        EnterToContinue();
        return;
    }
    while (getchar() != '\n');

	// 2: 거절
    if (c != 1) {
        printf("당신은 아직 때가 아니라 생각하고 의뢰를 거절합니다.\n");
        EnterToContinue();
        return;
    }

	// 1: 수락 - 전투 시작
    Enemy elite;
    initEnemy(&elite, "도시 주변 위협자", 80 + rand() % 21, 14 + rand() % 5, 120);

    printf("당신은 길드를 나와 목표를 찾아 나섭니다.\n");
    battle(player, &elite);

    if (player->hp > 0) {
        printf("의뢰를 완수했습니다! 보상: 골드 +120, Str +1, Dex +1\n");
        player->gold += 120;
        player->Str += 1;
        player->Dex += 1;
    }
    else {
        printf("전투 중 부상을 당했습니다. 체력 -20\n");
        player->hp -= 20;
        if (player->hp < 0) player->hp = 0;
    }
    EnterToContinue();
}

/*
* 도시 공원
* 간단한 랜덤 보상(HP/Int/Gold)
*/
void BigCity_Park(Player* player) {
    printDivider("도시 공원");
    printf("어느 정도 정돈된 나무와 벤치, 사람들의 웃음소리가 들리는 한켠입니다.\n");
    printf("잠시 숨을 고르기에 좋은 장소입니다.\n\n");

	// 랜덤 이벤트 선택
    int r = rand() % 3;
    if (r == 0) {
        printf("조용히 벤치에 앉아 쉬었습니다. HP +10\n");
        player->hp += 10;
        if (player->hp > player->maxHp) player->hp = player->maxHp;
    }
    else if (r == 1) {
        printf("길거리 공연을 보며 기분이 좋아졌습니다. Int +1\n");
        player->Int += 1;
    }
    else {
        int g = 15;
        printf("벤치 아래에서 떨어진 주머니를 발견했습니다. 골드 +%d\n", g);
        player->gold += g;
    }
    EnterToContinue();
}

/*
* 왕궁 알현
* 성검 소지 여부에 따른 국왕의 반응과 전설의 숲 해금 처리
*/
void BigCity_RoyalPalace(Player* player) {
    printDivider("왕궁 알현실");
    printf("화려한 레드카펫을 지나 국왕 앞에 섭니다.\n");
    printf("전운이 감도는 왕궁은 매우 분주해 보입니다.\n\n");

	// 성검 소지 여부에 따른 대화 분기
	// 1: 성검 소지
    if (player->hasHolySword) {
        printf("국왕: \"오오! 자네 등 뒤의 그 검... 드디어 성검을 손에 넣었구만!\"\n");
        printf("국왕: \"그 검의 신성한 빛이라면 마왕성의 [검은 결계]도 단숨에 걷어낼 수 있을 걸세.\"\n");
        printf("국왕: \"길거리 북쪽 끝에서 마왕성을 찾게. 그리고 부디 승리하고 돌아오게나!\"\n");
    }
	// 2: 성검 미소지
    else {
        printf("국왕: \"마왕을 물리치기 위해서는 [전설의 성검]이 필요하다네.\"\n");

		// 전설의 숲 위치를 아직 모를 때
        if (!player->heardLegend) {
            printf("국왕: \"자네에게 [미혹의 숲] 지도를 주겠네. 그곳 깊은 곳에 성검이 잠들어 있다네.\"\n");
            printf("\n[정보 획득] 전설의 숲 위치가 해금되었습니다!\n");
            printf("이제 길거리(Road) 탐험 중 [전설의 숲] 입구를 발견할 수 있습니다.\n");
			player->heardLegend = 1; // 숲 해금
        }
		// 이미 전설의 숲 위치를 알고 있을 때
        else {
            printf("국왕: \"어서 숲으로 가서 성검을 찾아오게. 시간이 없다네.\"\n");
            printf("(이미 숲의 위치를 알고 있습니다.)\n");
        }
    }
    EnterToContinue();
}

// 미혹의 숲 시련 함수 선언
static void Forest_Stage1_Maze(Player* player);
static void Forest_Stage2_Guardian(Player* player);
static void Forest_Stage3_Sword(Player* player);

/*
* 미혹의 숲 진입 이벤트
* 성검 소지 여부에 따른 처리 및 시련 선택 분기
*/
void Event_Forest(Player* player) {
    printDivider("미혹의 숲 (Lost Woods)");

	// 성검 미소지 시 숲 통과
    if (player->hasHolySword) {
        printf("성검의 주인이 숲에 들어서자, 안개가 걷히고 정령들이 경의를 표합니다.\n");
        printf("평화로운 숲의 기운을 느낍니다. (HP/MP가 조금 회복됩니다.)\n");
        player->hp += 10;
        if (player->hp > player->maxHp) 
            player->hp = player->maxHp;

        printf("\n다시 숲 밖으로 나갑니다.\n");
        player->location = LOC_ROAD;
        EnterToContinue();
        return;
    }

    printf("왕궁의 지도를 따라 숲 깊은 곳으로 들어왔습니다.\n");
    printf("이곳은 시공간이 뒤틀려 있어, 자격 없는 자를 영원히 헤매게 만듭니다.\n");
    printf("성검을 얻기 위한 세 가지 시련이 당신을 기다립니다.\n\n");

    printf("1) 시련에 도전한다\n");
    printf("2) 돌아간다\n");
    printf("> ");

    int choice;
    if (scanf("%d", &choice) != 1) {
        while (getchar() != '\n'); 
        return; 
    }
    while (getchar() != '\n');

	// 1: 시련 도전
    if (choice == 1) {
        Forest_Stage1_Maze(player);
    }
	// 2: 돌아가기
    else {
        printf("아직 준비가 되지 않았습니다. 숲을 빠져나갑니다.\n");
        player->location = LOC_ROAD;
        EnterToContinue();
    }
}

/*
* 숲 제1시련: 환영의 미로
* 플레이어 선택에 따라 다양한 능력치 판정, 성공 시 다음 시련으로 진행
*/
static void Forest_Stage1_Maze(Player* player) {
    printDivider("제1의 시련: 환영의 미로");
    printf("짙은 안개가 시야를 가리고, 동서남북의 감각이 사라집니다.\n");
    printf("바람 소리가 마치 사람의 목소리처럼 들려 유혹합니다.\n");
    printf("올바른 길을 찾아내야 합니다.\n\n");

    printf("1) 마력의 흐름을 읽어 길을 찾는다 (지능 판정)\n");
    printf("2) 감각에 의존해 빠르게 돌파한다 (민첩 판정)\n");
    printf("3) 힘으로 덤불을 베며 직진한다 (힘 판정)\n");
    printf("> ");

    int c;
    scanf("%d", &c); while (getchar() != '\n');

    int success = 0;
	// 선택지에 따른 능력치 판정
	// 1: 지능 판정
    if (c == 1)
        success = StatCheck("지능", player->Int, 13 + rand() % 3);
	// 2: 민첩 판정
    else if (c == 2)
        success = StatCheck("민첩", player->Dex, 15 + rand() % 3); 
	// 3: 힘 판정
    else if (c == 3)
        success = StatCheck("힘", player->Str, 25 + rand() % 3);
	// 그 외: 실패
    else
        success = 0;

	// 판정 결과에 따른 분기
	// 성공: 다음 시련으로 진행
    if (success) {
        printf("\n[성공] 환영을 꿰뚫어 보고 안개 너머의 통로를 발견했습니다.\n");
        EnterToContinue();
        Forest_Stage2_Guardian(player);
    }
	// 실패: 체력 소모 후 숲 입구로 강제 이동  
    else {
        printf("\n[실패] 당신은 같은 자리를 맴돌다 지쳐버렸습니다.\n");
        int dmg = 10 + rand() % 6;
        player->hp -= dmg;
        if (player->hp < 0) player->hp = 0;
        printf("체력 소모: -%d\n", dmg);

        printf("결국 숲의 입구로 쫓겨났습니다.\n");
        player->location = LOC_ROAD;
        EnterToContinue();
    }
}

/*
* 숲 제2시련: 숲의 수호자 전투
* 수호자와의 전투에 승리하면 최종 시련으로 진행
*/
static void Forest_Stage2_Guardian(Player* player) {
    printDivider("제2의 시련: 숲의 수호자");
    printf("미로를 통과하자 신비로운 공터가 나타납니다.\n");
    printf("그곳에는 덩굴과 고목으로 이루어진 거대한 [고대 수호자]가 길을 막고 있습니다.\n");
    printf("수호자: \"성검을 원하는 자여... 그 힘을 증명하라.\"\n");

    EnterToContinue();

	// 수호자 전투 시작
    Enemy guardian;
    initEnemy(&guardian, "고대 수호자", 400, 18, 0);

    battle(player, &guardian);

	// 전투 결과에 따른 분기
	// 승리: 최종 시련으로 진행
    if (player->hp > 0) {
        printf("수호자: \"자격... 인정한다...\"\n");
        printf("수호자가 빛이 되어 흩어지며, 뒤쪽의 성역으로 가는 길이 열립니다.\n");
        EnterToContinue();
        Forest_Stage3_Sword(player);
    }
	// 패배: 체력 0 처리
    else {
        printf("수호자의 일격에 정신을 잃습니다...\n");
    }
}

/*
* 숲 제3시련: 성검 획득
* 힘 판정 성공 시 성검 획득 및 능력치 대폭 상승, 실패 시 피해
*/
static void Forest_Stage3_Sword(Player* player) {
    printDivider("제3의 시련: 선택받은 자");
    printf("숲의 가장 깊은 곳, 성역(Sanctuary)입니다.\n");
    printf("아무런 소음도 없는 고요한 공간 중앙에, 바위에 박힌 [성검]이 빛나고 있습니다.\n");
    printf("검에서는 압도적인 신성력이 흘러나와 당신을 거부하려 합니다.\n");
    printf("올바른 길을 찾아내야 합니다.\n\n");

    printf("1) 온 힘을 다해 검을 뽑는다 (힘 판정)\n");
    printf("2) 포기하고 돌아간다\n");
    printf("> ");

    int c;
    scanf("%d", &c); 
    while (getchar() != '\n');

	// 2: 포기
    if (c == 2) {
        printf("성검의 기운에 압도되어 발길을 돌립니다.\n");
        player->location = LOC_ROAD;
        EnterToContinue();
        return;
    }

    printf("검자루를 쥐자 엄청난 저항감이 팔을 타고 흐릅니다!\n");
	// 힘 판정
	// 성공: 성검 획득 및 능력치 대폭 상승
    if (StatCheck("힘", player->Str, 15)) {
        printf("\n[성공] 당신의 기백이 성검의 저항을 눌렀습니다.\n");
        printf("콰아앙-!!\n");
        printf("강렬한 빛기둥과 함께 성검이 바위에서 뽑혀 나옵니다!\n\n");

        printf("======================================\n");
        printf("       [전설의 성검] 획득! \n");
        printf("======================================\n");
        printf("성검의 힘이 몸속으로 흘러들어옵니다.\n");
        printf("모든 능력치가 대폭 상승합니다! (All +5)\n");

        player->hasHolySword = 1;
        player->Str += 5;
        player->Dex += 5;
        player->Def += 5;
        player->Int += 5;
        player->maxHp += 50;
        player->hp = player->maxHp;

        printf("이제 마왕을 쓰러뜨릴 준비가 되었습니다.\n");
        printf("마왕성으로 진격하십시오!\n");
    }
	// 실패: 피해
    else {
        printf("\n[실패] 검은 꿈쩍도 하지 않습니다.\n");
        printf("오히려 성검의 반동으로 튕겨져 나옵니다. HP -20\n");
        player->hp -= 20;
        if (player->hp < 0) player->hp = 0;
        printf("아직은 육체적 능력이 부족한 것 같습니다.\n");
    }

	player->location = LOC_ROAD; // 숲 밖으로 이동
    EnterToContinue();
}

/*
* 마왕성 입구 이벤트
* 성검 여부/결계 상태에 따라 입장 허용, 결계 파괴 처리, 진입 선택
*/
void RoadEvent_DemonCastleEntrance(Player* player) {
    printDivider("마왕성 입구");
    printf("검은 안개가 자욱한 황무지 끝, 하늘을 찌를 듯 솟은 거대한 마왕성이 보입니다.\n");
    printf("성 주변은 시공간이 뒤틀린 듯한 [검은 결계]로 굳게 닫혀 있습니다.\n\n");

	// 성검 미소지 시 결계 통과 불가
    if (!player->hasHolySword) {
        printf("가까이 다가가려 하자, 보이지 않는 벽이 당신을 강하게 밀어냅니다.\n");
        printf("...알 수 없는 강력한 힘이 성을 보호하고 있습니다.\n");
        printf("지금의 상태로는 결계를 뚫을 수 없습니다.\n");
        EnterToContinue();
        return;
    }

	// 성검 소지 시 결계 파괴 처리
    if (!player->isBarrierBroken) {
        printf("그때, 당신의 등 뒤에 있는 [성검]이 격렬하게 진동하기 시작합니다!\n");
        printf("검을 뽑아들자, 눈부신 빛기둥이 스스로 결계를 향해 뻗어 나갑니다.\n\n");

        EnterToContinue();

        printf("콰아아아앙-!!\n");
        printf("성검의 빛이 검은 장막을 종잇장처럼 찢어발깁니다.\n");
        printf("대지가 울리고 사악한 기운이 흩어집니다.\n");
        printf("[알림] 마왕성으로 향하는 길이 열렸습니다!\n");

		player->isBarrierBroken = 1; // 결계 파괴 처리
    }
	// 성검 소지 및 결계 파괴 상태
    else {
        printf("성검에 의해 결계가 파괴된 상태입니다. 성문은 무방비하게 열려 있습니다.\n");
    }

    printf("\n1) 최후의 결전을 시작한다 (마왕성 진입)\n");
    printf("2) 마지막 정비를 하러 간다 (돌아간다)\n");
    printf("> ");

    int c;
    if (scanf("%d", &c) != 1) {
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

	// 1: 마왕성 진입
    if (c == 1) {
        printf("\n운명의 시간이 다가왔습니다. 당신은 성 안으로 발을 내딛습니다.\n");
        player->location = LOC_DEMON_CASTLE;

        DemonKingCastle_Entrance(player);
    }
	// 2: 돌아가기
    else {
        printf("아직 준비가 필요합니다. 발길을 돌립니다.\n");
    }
    EnterToContinue();
}

/*
* 마왕성 내부 진입 및 최종 보스전
* 보스 처치 시 게임 클리어 처리, 패배 시 게임 오버 연출
*/
void DemonKingCastle_Entrance(Player* player) {
    ClearScreen();
    printDivider("마왕성 - 알현실 (Final Stage)");
    printf("적막만이 감도는 긴 회랑을 지나, 마침내 거대한 옥좌 앞에 도착했습니다.\n");
    printf("천장 높이 솟은 검은 기둥들 사이로 차가운 냉기가 흐릅니다.\n");
    printf("옥좌에 앉아있던 거대한 그림자가 천천히 몸을 일으킵니다.\n\n");

    printf("마왕: \"호오... 결계를 찢고 여기까지 도달한 인간이 있을 줄이야.\"\n");
    printf("마왕: \"그 성검의 빛... 역겹기 그지없구나.\"\n");
    printf("마왕: \"내 직접 그 희망을 꺽어주마. 영원한 어둠 속에서 후회하거라!\"\n");

    EnterToContinue();

	// 최종 보스 전투
    Enemy boss;
    initEnemy(&boss, "마왕(Demon King)", 600, 30, 0);

    battle(player, &boss);

	// 엔딩
    if (player->hp > 0) {
        ClearScreen();
        printDivider("GAME CLEAR");
        printf("마왕: \"크아아크...!! 인간 따위에게... 이 내가...!!\"\n\n");

        printf("당신의 성검이 마왕의 심장을 꿰뚫자, 검은 마력이 폭주하며 옥좌가 무너져 내립니다.\n");
        printf("마왕의 육체가 먼지가 되어 흩어지고, 마왕성을 덮고 있던 어둠이 걷힙니다.\n");
        printf("창문 틈으로 들어온 따스한 햇살이 당신의 지친 어깨를 비춥니다.\n\n");

        printf("당신은 세상을 구했습니다.\n");
        printf("이 이야기는 전설이 되어 영원히 기억될 것입니다.\n");

        printf("\n========================================\n");
        printf("       축하합니다! 게임을 클리어했습니다.       \n");
        printf("           Thank you for playing!           \n");
        printf("========================================\n");

        EnterToContinue();
        exit(0);
    }
	// 패배 엔딩
    else {
        printf("마왕: \"가소롭구나. 이것이 너희들의 한계다.\"\n");
        printf("당신의 의식이 흐려집니다. 세계는 어둠에 잠식될 것입니다...\n");
    }
}