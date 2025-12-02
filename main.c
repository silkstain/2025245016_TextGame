#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Character.h"
#include "Event.h"
#include "Battle.h"

int main() {
	Player player; // 플레이어 캐릭터
	// 인벤토리 초기화
    for (int i = 0; i < 100; i++) 
        player.inventory[i][0] = '\0';

	srand((unsigned)time(NULL) ^ (unsigned)clock()); // 난수 초기화

	// 메인 메뉴
    int menu;
    printf("1) 새 게임 시작\n");
    printf("2) 불러오기\n");
    printf("선택: ");
    scanf("%d", &menu);
    while (getchar() != '\n');

	const char* savePath = "save.txt"; // 저장 파일 경로

	// 불러오기 선택 시 처리
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
	// 새 게임 선택 시 처리
    else {
        initPlayer(&player);
        Prologue_Event(&player);
        StoryEvent_Start(&player);
    }

	// 메인 게임 루프
    while (1) {
        if (player.hp <= 0) {
            printf("당신은 모험에서 쓰러졌습니다...\n");
            break;
        }

        // 새 턴 시작 시 화면 클리어 및 상태 출력 
        player.turnCount++;
        ClearScreen();
        printPlayerStatus(&player);

        // [수정] 메뉴에 인벤토리 옵션 추가
        printf("\n>> [Enter] 모험 계속하기");
        printf("\n>> [S] 게임 저장하기");
        printf("\n>> [I] 인벤토리 / 아이템 사용\n");
        printf("선택: ");

        char input[10];
        fgets(input, sizeof(input), stdin); // 입력 받기

        // 'S' 입력 시 저장
        if (input[0] == 'S' || input[0] == 's') {
            saveGame(&player, savePath);
            printf("엔터를 누르면 모험을 다시 시작합니다...");
            while (getchar() != '\n');
            continue;
        }
        // 'I' 입력 시 아이템 사용
        else if (input[0] == 'I' || input[0] == 'i') {
            useItem(&player, -1); // 인벤토리 열기
            printf("엔터를 누르면 모험을 다시 시작합니다...");
            while (getchar() != '\n');
            continue; // 턴 소모 없이 다시 대기
        }

        // 위치별 이벤트 실행
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