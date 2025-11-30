/* main.c 파일의 전체 내용 (수정됨) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Character.h"
#include "Event.h"
#include "Battle.h"

int main() {
    Player player = { 0 };
    for (int i = 0; i < 100; i++) player.inventory[i] = NULL;

    srand((unsigned)time(NULL) ^ (unsigned)clock());

    int menu;
    printf("1) 새 게임 시작\n");
    printf("2) 불러오기\n");
    printf("선택: ");
    scanf("%d", &menu);
    while (getchar() != '\n');

    const char* savePath = "save.json"; // 경로 단순화

    if (menu == 2) {
        if (!loadGame(&player, savePath)) {
            printf("불러오기 실패. 새 게임을 시작합니다.\n");
            initPlayer(&player);
            Prologue_Event(&player);
            StoryEvent_Start(&player);
        }
        else {
            printf("불러오기 성공.\n");
        }
    }
    else {
        initPlayer(&player);
        Prologue_Event(&player);
        StoryEvent_Start(&player);
    }

    while (1) {
        if (player.hp <= 0) {
            printf("당신은 모험에서 쓰러졌습니다...\n");
            break;
        }

        player.turnCount++;
        ClearScreen();
        printPlayerStatus(&player);

        printf("\n(S) 저장 (자동 진행 중에는 사용 불가할 수 있습니다)\n");
        /* 입력 대기 없이 바로 이벤트로 넘어가므로, 저장은 마을 내부 기능 등을 이용하는 구조입니다. */

        switch (player.location) {
        case LOC_ROAD:
            Event_Road(&player);
            break;

        case LOC_CAVE:
            Event_Cave(&player);
            break;

        case LOC_TOWN:
            Event_Town(&player);
            break;

        case LOC_FOREST:
            Event_Forest(&player);
            break;

        case LOC_BIG_CITY:
            Event_BigCity(&player);
            break;

        case LOC_DEMON_CASTLE:
            DemonKingCastle_Entrance(&player);
            break;
        }
    }

    return 0;
}