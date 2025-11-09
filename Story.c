#include <stdio.h>
#include <stdlib.h>
#include "Story.h"
#include "Player.h"

// ────────────────────────────────
// 출력 도우미
// ────────────────────────────────
void printDivider(const char* title) {
    printf("\n========================================\n");
    printf("  %s\n", title);
    printf("========================================\n");
}

void waitForNext() {
    printf("\n----------------------------------------\n");
    printf("계속 진행하려면 Enter 키를 누르세요...");
    getchar();
    getchar();
    printf("\n");
}

// ────────────────────────────────
// 스토리 이벤트들
// ────────────────────────────────
void storyEvent_Start(Player* player) {
    printDivider("모험의 시작");
    printf("당신은 어두운 숲의 입구에 섰습니다.\n");
    printf("1. 앞으로 나아간다.\n");
    printf("2. 주변을 탐색한다.\n");

    int choice;
    printf("선택: ");
    scanf("%d", &choice);

    if (choice == 1) {
        storyEvent_Battle(player);
    }
    else {
        storyEvent_Treasure(player);
    }
    waitForNext();
}

void storyEvent_Battle(Player* player) {
    printDivider("이벤트: 전투 개시");
    printf("야생의 늑대가 나타났다!\n");

    int playerAttack = 10 + rand() % 10;
    int enemyAttack = 8 + rand() % 12;

    printf("당신의 공격력: %d | 늑대의 공격력: %d\n", playerAttack, enemyAttack);

    if (playerAttack >= enemyAttack) {
        printf("전투에서 승리했습니다! 골드 +50, 힘 +1\n");
        player->gold += 50;
        player->Str += 1;
    }
    else {
        printf("늑대에게 상처를 입었습니다! HP -20\n");
        player->hp -= 20;
        if (player->hp < 0) player->hp = 0;
    }

    printPlayerStatus(player);
    waitForNext();
}

void storyEvent_Treasure(Player* player) {
    printDivider("이벤트: 보물상자 발견");
    printf("당신은 이끼 낀 상자를 발견했습니다.\n");
    printf("1. 상자를 연다.\n");
    printf("2. 무시하고 지나간다.\n");

    int choice;
    printf("선택: ");
    scanf("%d", &choice);

    if (choice == 1) {
        int chance = rand() % 100;
        if (chance < 60) {
            printf("금화를 발견했습니다! 골드 +100\n");
            player->gold += 100;
        }
        else {
            printf("함정이 발동했습니다! HP -10\n");
            player->hp -= 10;
            if (player->hp < 0) player->hp = 0;
        }
    }
    else {
        printf("당신은 신중하게 상자를 지나쳤습니다.\n");
    }

    printPlayerStatus(player);
    waitForNext();
}

void storyEvent_End(Player* player) {
    printDivider("모험의 마무리");
    if (player->hp <= 0) {
        printf("당신은 상처를 이기지 못하고 쓰러졌습니다...\n");
    }
    else {
        printf("당신은 무사히 숲을 빠져나왔습니다.\n");
        printf("오늘의 모험은 성공입니다!\n");
    }

    printPlayerStatus(player);
    printf("\n게임이 종료됩니다.\n");
}
