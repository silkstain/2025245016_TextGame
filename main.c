#include <stdio.h>
#include <stdlib.h>
#include "Player.h"
#include "Story.h"

int main() {
    Player player;
    initPlayer(&player);

    storyEvent_Start(&player);

    if (player.hp > 0)
        storyEvent_End(&player);
    else
        printf("\n플레이어가 사망했습니다. 모험 실패.\n");

    return 0;
}
