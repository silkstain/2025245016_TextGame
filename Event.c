#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Event.h"
#include "Character.h"
#include "Battle.h"

static int caveStage = 0;

//──────────────────────────────
// 공통 출력 도우미
//──────────────────────────────
void printDivider(const char* title) {
    printf("\n========================================\n");
    printf("  %s\n", title);
    printf("========================================\n");
}

void ClearScreen() {
#ifdef _WIN32
    system("cls");
#else
    printf("\x1b[2J\x1b[H");
#endif
}

void EnterToContinue() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {};
}

//──────────────────────────────
// 시작 이벤트
//──────────────────────────────
void StoryEvent_Start(Player* player) {
    printDivider("게임 시작");
    printf("환영합니다, %s님! 모험이 시작됩니다.\n", player->name);
    printf("당신은 길 위에서 모험을 시작합니다.\n");

    player->location = LOC_ROAD;

    printf("\n(엔터를 눌러 계속...)");
    EnterToContinue();
}

//──────────────────────────────
// ROAD 전용 이벤트 (마을 진입 없음)
//──────────────────────────────

// Road → Town 유일한 진입 이벤트
void RoadEvent_TownEntrance(Player* player) {
    printDivider("마을 발견");
    printf("작은 마을을 발견했습니다.\n");
    printf("마을로 들어갑니다.\n");

    printf("\n(엔터를 눌러 계속...)");
    EnterToContinue();

    player->location = LOC_TOWN;
}

void Event_Road(Player* player) {
    printDivider("길거리 (Road)");
    printf("당신은 길을 걷고 있습니다.\n");

    int ev = rand() % 6;

    // 마을/동굴 쿨다운 설정
    int townCooldown = 5;
    int caveCooldown = 6;

    // 마을 이벤트를 뽑았지만 쿨다운이 끝나지 않았다면 다시 reroll
    if (ev == 5 && player->turnCount - player->lastTownTurn < townCooldown) {
        ev = rand() % 5;  // 마을 제외
    }

    // 동굴을 뽑았지만 쿨다운 중이면 다시 reroll
    if (ev == 3 && player->turnCount - player->lastCaveTurn < caveCooldown) {
        ev = rand() % 5;  // 동굴 제외
    }

    switch (ev) {
    case 0: TreasureEvent(player); break;
    case 1: TrapEvent(player); break;
    case 2: FondEvent(player); break;
    case 3:
        // 동굴 쿨다운 통과 → 동굴 등장
        player->lastCaveTurn = player->turnCount;
        printDivider("동굴 발견");
        printf("동굴을 발견했습니다. 들어갑니다.\n");
        break;
    case 4:
        printDivider("평화로운 길");
        printf("아무 일도 일어나지 않았습니다.\n");
        break;
    case 5:
        // 마을 쿨다운 통과 → 마을 등장
        player->lastTownTurn = player->turnCount;
        RoadEvent_TownEntrance(player);
        break;
    }
}

//──────────────────────────────
// CAVE (2단계 던전)
//──────────────────────────────
void Event_Cave(Player* player) {

    if (caveStage == 0) {
        printDivider("동굴 입구");
        printf("어두운 동굴에 들어왔습니다.\n");

        Encounter_goblin(player);

        printf("\n(엔터를 눌러 계속...)");
        EnterToContinue();

        caveStage = 1;
        return;
    }

    if (caveStage == 1) {
        printDivider("동굴 보물실");
        printf("전투 후 상자를 발견했습니다.\n");

        int gold = 50 + rand() % 51;
        printf("골드 %d 획득!\n", gold);
        player->gold += gold;

        printf("\n동굴을 빠져나갑니다.\n");
        printf("(엔터를 눌러 계속...)");
        EnterToContinue();

        player->location = LOC_ROAD;
        caveStage = 0;
        return;
    }
}

//──────────────────────────────
// TOWN (3종 이벤트 + 자동 Road 복귀)
//──────────────────────────────
void Event_Town(Player* player) {
    printDivider("마을 (Town)");
    printf("당신은 작은 마을에 들어왔습니다.\n");

    int ev = rand() % 3;

    switch (ev) {
    case 0: Town_Pickpocket(player); break;
    case 1: Town_StreetFood(player); break;
    case 2: Town_Shop(player); break;
    }

    printf("\n마을에서 휴식을 취했습니다. HP +5\n");
    player->hp += 5;
    if (player->hp > player->maxHp) player->hp = player->maxHp;

    printf("\n(엔터를 눌러 마을을 떠납니다...)");
    EnterToContinue();

    player->location = LOC_ROAD;
}

//──────────────────────────────
// ROAD 세부 이벤트
//──────────────────────────────
void TreasureEvent(Player* player) {
    printDivider("보물 상자");

    int gold = 100 + rand() % 101;
    int mimic = rand() % 2;

    if (mimic == 0) {
        printf("골드 %d 획득!\n", gold);
        player->gold += gold;
    }
    else {
        printf("상자는 미믹이었다!\n");

        Enemy e;
        initEnemy(&e, "미믹", 30 + rand() % 11, 5 + rand() % 4, 30);
        battle(player, &e);
    }

    printf("\n(엔터를 눌러 계속...)");
    EnterToContinue();
}

void TrapEvent(Player* player) {
    printDivider("함정");
    printf("함정을 밟았습니다! HP -10\n");

    player->hp -= 10;
    if (player->hp < 0) player->hp = 0;

    printf("\n(엔터를 눌러 계속...)");
    EnterToContinue();
}

void FondEvent(Player* player) {
    printDivider("마법 우물");
    printf("Dex +1 상승!\n");
    player->Dex += 1;

    printf("\n(엔터를 눌러 계속...)");
    EnterToContinue();
}

void Encounter_goblin(Player* player) {
    printDivider("고블린 조우");

    Enemy goblin;
    initEnemy(&goblin, "고블린",
        25 + rand() % 11,
        3 + rand() % 5,
        20 + rand() % 31);

    battle(player, &goblin);

    printf("\n(엔터를 눌러 계속...)");
    EnterToContinue();
}

//──────────────────────────────
// TOWN 세부 이벤트
//──────────────────────────────
void Town_Pickpocket(Player* player) {
    printDivider("소매치기!");

    int loss = 10 + rand() % 11;
    if (player->gold < loss) loss = player->gold;

    printf("소매치기를 당했습니다! 골드 %d 잃음.\n", loss);
    player->gold -= loss;

    printf("\n(엔터를 눌러 계속...)");
    EnterToContinue();
}

void Town_StreetFood(Player* player) {
    printDivider("노상 음식점");

    int cost = 10;
    int heal = 20;

    if (player->gold >= cost) {
        player->gold -= cost;
        player->hp += heal;
        if (player->hp > player->maxHp) player->hp = player->maxHp;
        printf("노상에서 따뜻한 음식을 먹었습니다! HP +20\n");
    }
    else {
        printf("골드가 부족합니다.\n");
    }

    printf("\n(엔터를 눌러 계속...)");
    EnterToContinue();
}

void Town_Shop(Player* player) {
    printDivider("상점");

    printf("1) HP 포션(+30) - 20골드\n");
    printf("2) 힘의 반지(Str +1) - 50골드\n");
    printf("3) 나가기\n\n> ");

    int sel;
    scanf("%d", &sel);
    while (getchar() != '\n');

    if (sel == 1) {
        if (player->gold >= 20) {
            player->gold -= 20;
            player->hp += 30;
            if (player->hp > player->maxHp) player->hp = player->maxHp;
            printf("포션을 사용했습니다!\n");
        }
        else printf("골드 부족!\n");
    }
    else if (sel == 2) {
        if (player->gold >= 50) {
            player->gold -= 50;
            player->Str += 1;
            printf("힘의 반지를 장착했습니다! Str +1\n");
        }
        else printf("골드 부족!\n");
    }
    else {
        printf("아무것도 구매하지 않았습니다.\n");
    }

    printf("\n(엔터를 눌러 계속...)");
    EnterToContinue();
}
