#ifndef PLAYER_H
#define PLAYER_H

typedef struct Player {
    char name[20];
    int hp;
    int maxHp;
    int gold;
    int Str;
    int Dex;
    int Int;
} Player;

void initPlayer(Player* player);
void printPlayerStatus(Player* player);

#endif
