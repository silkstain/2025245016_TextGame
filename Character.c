#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Character.h"

/* 게임 세이브 함수 */
void saveGame(Player* p, const char* filename) {
    FILE* fp = fopen(filename, "w"); // 쓰기 모드로 파일 열기

    if (!fp) {
        printf("오류: 세이브 파일을 생성할 수 없습니다.\n");
        return;
    }

    // Player 구조체의 각 필드를 순서대로 저장
    fprintf(fp, "%s\n", p->name);
    fprintf(fp, "%d %d %d\n", p->hp, p->maxHp, p->gold);
    fprintf(fp, "%d %d %d %d\n", p->Str, p->Dex, p->Def, p->Int);
    fprintf(fp, "%d %d %d %d\n", p->turnCount, p->lastTownTurn, p->lastCaveTurn, p->lastCampingTurn);
    fprintf(fp, "%d %d %d %d %d %d\n", p->caveDepth, p->knowsCave, p->heardLegend, p->hasHolySword, p->hasCityPass, p->isBarrierBroken);
    fprintf(fp, "%d %d %d\n", p->location, p->lastRoadEvent, p->lastTownEvent);

    // 인벤토리 저장
    // 1. 아이템 개수 세기
    int itemCount = 0;
    for (int i = 0; i < 100; i++) {
        // 첫 글자가 NULL이 아니면 아이템이 있는 것으로 간주
        if (p->inventory[i][0] != '\0') {
            itemCount++;
        }
    }

    // 2. 개수 먼저 저장
    fprintf(fp, "%d\n", itemCount);

    // 3. 실제 아이템 이름 저장
    for (int i = 0; i < 100; i++) {
        if (p->inventory[i][0] != '\0') {
            fprintf(fp, "%s\n", p->inventory[i]);
        }
    }

    fclose(fp);
    printf(">> 게임이 저장되었습니다! (파일명: %s)\n", filename);
}

/* 게임 로드 함수 (단순화됨) */
int loadGame(Player* p, const char* filename) {
    FILE* fp = fopen(filename, "r");

    if (!fp) {
        printf(">> 저장된 파일이 없습니다.\n");
        return 0;
    }

    // 인벤토리 초기화 (싹 비우기)
    for (int i = 0; i < 100; i++) {
        p->inventory[i][0] = '\0';
    }

    // 순서대로 불러오기
    fscanf(fp, "%s", p->name);
    fscanf(fp, "%d %d %d", &p->hp, &p->maxHp, &p->gold);
    fscanf(fp, "%d %d %d %d", &p->Str, &p->Dex, &p->Def, &p->Int);
    fscanf(fp, "%d %d %d %d", &p->turnCount, &p->lastTownTurn, &p->lastCaveTurn, &p->lastCampingTurn);
    fscanf(fp, "%d %d %d %d %d %d", &p->caveDepth, &p->knowsCave, &p->heardLegend, &p->hasHolySword, &p->hasCityPass, &p->isBarrierBroken);
    fscanf(fp, "%d %d %d", &p->location, &p->lastRoadEvent, &p->lastTownEvent);

    // 인벤토리 불러오기
    int itemCount = 0;
    fscanf(fp, "%d", &itemCount); // 개수 읽기

    char tempItem[256];

    for (int i = 0; i < itemCount; i++) {
        // 공백 포함해서 한 줄 읽기 ("%[^\n]" 사용)
        fscanf(fp, " %[^\n]", tempItem);

        // [중요] strdup 없이 그냥 복사 (strcpy)
        strcpy(p->inventory[i], tempItem);
    }

    fclose(fp);
    printf(">> 저장된 게임을 불러왔습니다!\n");
    return 1;
}


/* Player 이름 입력 및 초기화 함수 */
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
    player->lastTownTurn = 0;
    player->lastCaveTurn = 0;
    player->lastCampingTurn = 0;
    player->caveDepth = 0;
    player->knowsCave = 0;
    player->heardLegend = 0;
    player->hasHolySword = 0;
    player->hasCityPass = 0;
    player->isBarrierBroken = 0;

    player->location = LOC_ROAD;
    player->lastRoadEvent = 0;
    player->lastTownEvent = 0;

    // 인벤토리 초기화 (NULL 포인터 대신 빈 문자열로)
    for (int i = 0; i < 100; i++)
        player->inventory[i][0] = '\0';

    // 기본 아이템 지급
    addItem(player, "HP 포션");
    addItem(player, "HP 포션");
    addItem(player, "HP 포션");
}

/* Enemy 초기화 함수 */
void initEnemy(Enemy* enemy, const char* name, int hp, int attack, int gold) {
    strncpy(enemy->name, name, sizeof(enemy->name) - 1);
    enemy->name[sizeof(enemy->name) - 1] = '\0';

    enemy->hp = hp;
    enemy->maxHp = hp;
    enemy->attack = attack;
    enemy->gold = gold + (rand() % 31);
}

/* 플레이어 상태 출력 함수 */
void printPlayerStatus(const Player* p) {
    printf("\n┌──────────────────────────────────────────────┐\n");
    printf("│ 이름: %-10s HP: %3d/%3d\n", p->name, p->hp, p->maxHp);
    printf("│ 골드: %-5d 힘:%2d 민첩:%2d 방어:%2d 지능:%2d\n",
        p->gold, p->Str, p->Dex, p->Def, p->Int);
    printf("└──────────────────────────────────────────────┘\n");
}

/* 아이템 추가 함수 */
void addItem(Player* p, const char* item) {
    for (int i = 0; i < 100; i++) {
        // 빈 칸인지 확인 (첫 글자가 널 문자면 빈 칸)
        if (p->inventory[i][0] == '\0') {
            // 문자열 복사
            strcpy(p->inventory[i], item);
            printf(">> 획득: [%s]\n", item);
            return;
        }
    }
    printf(">> 인벤토리가 가득 찼습니다!\n");
}

/* 인벤토리 출력 함수 */
void printInventory(const Player* p) {
    printf("\n[ 인벤토리 목록 ]\n");
    int empty = 1;

    for (int i = 0; i < 100; i++) {
        // 빈 칸이 아니면 출력
        if (p->inventory[i][0] != '\0') {
            printf("%2d) %s\n", i, p->inventory[i]);
            empty = 0;
        }
    }

    if (empty)
        printf("(비어 있음)\n");
}

/* 아이템 사용 함수 */
void useItem(Player* p, int index) {
    // 1. 인벤토리 목록 출력 및 선택
    if (index == -1) {
        printInventory(p);
        printf("사용할 아이템 번호 입력 (-1 취소): ");
        if (scanf("%d", &index) != 1) {
            while (getchar() != '\n');
            index = -1;
        }
        while (getchar() != '\n');

        if (index < 0) {
            printf(">> 취소했습니다.\n");
            return;
        }
    }

    // 유효성 검사
    if (index < 0 || index >= 100 || p->inventory[index][0] == '\0') {
        printf(">> 잘못된 번호이거나 아이템이 없습니다.\n");
        return;
    }

    char* itemName = p->inventory[index];


    // 1. 회복 및 성장 아이템 (즉시 사용)
    if (strcmp(itemName, "HP 포션") == 0) {
        int healAmount = 30;
        p->hp += healAmount;
        if (p->hp > p->maxHp) p->hp = p->maxHp;
        printf(">> 포션을 들이켰습니다. (HP +%d) 현재 HP: %d\n", healAmount, p->hp);
        p->inventory[index][0] = '\0'; // 소모
    }
    else if (strcmp(itemName, "현자의 지혜서") == 0) {
        printf(">> 책을 펼치자 고대의 지식이 머릿속으로 흘러들어옵니다!\n");
        p->Int += 1;
        printf(">> [ 지능(Int) +1 영구 상승! ]\n");
        p->inventory[index][0] = '\0'; // 소모
    }
    else if (strcmp(itemName, "활력의 비약") == 0) {
        printf(">> 붉은 비약을 마시자 심장이 강하게 뜁니다!\n");
        p->maxHp += 5;
        p->hp = p->maxHp; // 체력 완전 회복
        printf(">> [ 최대 HP +5 영구 상승 ] [ HP 완전 회복 ]\n");
        p->inventory[index][0] = '\0'; // 소모
    }
    else if (strcmp(itemName, "현자의 도시락") == 0) {
        printf(">> 도시락 뚜껑을 열자 맛있는 냄새가 진동합니다. 순식간에 비웠습니다!\n");
        int heal = 50;
        p->hp += heal;
        if (p->hp > p->maxHp) p->hp = p->maxHp;
        printf(">> 배가 든든해집니다. [ HP +%d 회복 ]\n", heal);
        p->inventory[index][0] = '\0'; // 소모
    }
    else if (strcmp(itemName, "화려한 버섯") == 0) {
        printf(">> 화려한 버섯을 먹어봅니다. 달콤하고 톡 쏘는 맛이 납니다.\n");
        int heal = 20;
        p->hp += heal;
        if (p->hp > p->maxHp) p->hp = p->maxHp;
        printf(">> 기운이 납니다! [ HP +%d 회복 ]\n", heal);
        p->inventory[index][0] = '\0'; // 소모
    }
    else if (strcmp(itemName, "건강한 버섯") == 0) {
        printf(">> 건강한 버섯을 씹어 먹습니다. 쓴맛이 나지만 몸에는 좋아 보입니다.\n");
        int heal = 15;
        p->hp += heal;
        if (p->hp > p->maxHp) p->hp = p->maxHp;
        printf(">> 속이 편안해집니다. [ HP +%d 회복 ]\n", heal);
        p->inventory[index][0] = '\0'; // 소모
    }

    // 2. 전투용 아이템 (설명 출력)
    else if (strcmp(itemName, "불타는 핵") == 0) {
        printf(">> [불타는 핵]은 매우 뜨겁습니다.\n");
        printf(">> 전투 중에 적에게 던져서 강력한 화염 피해(50)를 줄 수 있습니다.\n");
    }
    else if (strcmp(itemName, "운석 광석") == 0) {
        printf(">> [운석 광석]은 한 손에 쥐기 딱 좋은 무게입니다.\n");
        printf(">> 전투 중에 투척 무기로 사용하여 물리 피해(20)를 줄 수 있습니다.\n");
    }
    else if (strcmp(itemName, "별의 가루") == 0) {
        printf(">> [별의 가루]는 신비롭게 반짝입니다.\n");
        printf(">> 전투 중에 뿌리면 적을 혼란(기절)시킬 수 있습니다.\n");
    }
    else if (strcmp(itemName, "행운의 부적") == 0) {
        printf(">> [행운의 부적]을 지니고 있으면 마음이 편안해집니다.\n");
        printf(">> 전투 중에 사용하면 100%% 확률로 도망칠 수 있습니다.\n");
    }

}