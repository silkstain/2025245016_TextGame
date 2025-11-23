#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Character.h"
#include "Event.h"
#include "Battle.h"

int main() {
    Player player;
    srand((unsigned)time(NULL) ^ (unsigned)clock());

    initPlayer(&player);
    StoryEvent_Start(&player);

    while (1) {
        if (player.hp <= 0) {
            printf("당신은 모험에서 쓰러졌습니다...\n");
            break;
        }

        player.turnCount++;
        ClearScreen();
        printPlayerStatus(&player);

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
        case LOC_BIG_CITY:              // ★ 추가
            Event_BigCity(&player);
            break;
        case LOC_DEMON_CASTLE:
            Event_DemonCastle(&player);
            break;
        }

    }
    return 0;
}
