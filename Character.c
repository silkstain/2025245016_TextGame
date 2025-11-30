#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Character.h"

/* ================================================
   JSON KEY/VALUE 유틸
   ================================================ */

static const char* skip_spaces(const char* p) {
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    return p;
}

static int json_get_int(const char* json, const char* key, int defaultVal) {
    char pattern[64];
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);

    const char* p = strstr(json, pattern);
    if (!p) return defaultVal;

    p = strchr(p, ':');
    if (!p) return defaultVal;
    p++;
    p = skip_spaces(p);

    int val;
    if (sscanf(p, "%d", &val) != 1) return defaultVal;
    return val;
}

static void json_get_string(const char* json, const char* key,
    char* out, size_t outSize,
    const char* def) {
    char pattern[64];
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);

    const char* p = strstr(json, pattern);
    if (!p) { strncpy(out, def, outSize); return; }

    p = strchr(p, ':');
    if (!p) { strncpy(out, def, outSize); return; }
    p++;
    p = skip_spaces(p);

    if (*p != '\"') { strncpy(out, def, outSize); return; }
    p++;

    const char* start = p;
    while (*p && *p != '\"') p++;

    size_t len = p - start;
    if (len >= outSize) len = outSize - 1;
    memcpy(out, start, len);
    out[len] = '\0';
}

/* ================================================
   인벤토리 JSON 파서
   ================================================ */

static void json_load_inventory(const char* json, Player* p) {

    const char* inv = strstr(json, "\"inventory\"");
    if (!inv) return;

    const char* pcur = strchr(inv, '[');
    if (!pcur) return;

    pcur++;

    for (int i = 0; i < 100; i++) {
        if (p->inventory[i]) {
            free(p->inventory[i]);
            p->inventory[i] = NULL;
        }
    }

    while (*pcur && *pcur != ']') {
        pcur = skip_spaces(pcur);

        if (*pcur == '\"') {
            pcur++;
            const char* start = pcur;
            while (*pcur && *pcur != '\"') pcur++;

            size_t len = pcur - start;
            if (len > 0) {
                char item[256];
                if (len >= sizeof(item)) len = sizeof(item) - 1;

                memcpy(item, start, len);
                item[len] = '\0';

                for (int i = 0; i < 100; i++) {
                    if (!p->inventory[i]) {
#ifdef _WIN32
                        p->inventory[i] = _strdup(item);
#else
                        p->inventory[i] = strdup(item);
#endif
                        break;
                    }
                }
            }
            if (*pcur == '\"') pcur++;
        }
        while (*pcur == ' ' || *pcur == ',' || *pcur == '\n') pcur++;
    }
}

/* ================================================
   SAVE(JSON)
   ================================================ */

void saveGame(Player* p, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) { printf("세이브 파일 생성 실패.\n"); return; }

    fprintf(fp, "{\n");

    fprintf(fp, "  \"name\": \"%s\",\n", p->name);
    fprintf(fp, "  \"hp\": %d,\n", p->hp);
    fprintf(fp, "  \"maxHp\": %d,\n", p->maxHp);
    fprintf(fp, "  \"gold\": %d,\n", p->gold);

    fprintf(fp, "  \"Str\": %d,\n", p->Str);
    fprintf(fp, "  \"Dex\": %d,\n", p->Dex);
    fprintf(fp, "  \"Def\": %d,\n", p->Def);
    fprintf(fp, "  \"Int\": %d,\n", p->Int);

    fprintf(fp, "  \"turnCount\": %d,\n", p->turnCount);
    fprintf(fp, "  \"lastTownTurn\": %d,\n", p->lastTownTurn);
    fprintf(fp, "  \"lastCaveTurn\": %d,\n", p->lastCaveTurn);
    fprintf(fp, "  \"lastCampingTurn\": %d,\n", p->lastCampingTurn);

    fprintf(fp, "  \"caveDepth\": %d,\n", p->caveDepth);
    fprintf(fp, "  \"knowsCave\": %d,\n", p->knowsCave);
    fprintf(fp, "  \"heardLegend\": %d,\n", p->heardLegend);
    fprintf(fp, "  \"hasHolySword\": %d,\n", p->hasHolySword);
    fprintf(fp, "  \"hasCityPass\": %d,\n", p->hasCityPass);
    fprintf(fp, "  \"isBarrierBroken\": %d,\n", p->isBarrierBroken);

    fprintf(fp, "  \"location\": %d,\n", p->location);
    fprintf(fp, "  \"lastRoadEvent\": %d,\n", p->lastRoadEvent);
    fprintf(fp, "  \"lastTownEvent\": %d,\n", p->lastTownEvent);

    fprintf(fp, "  \"inventory\": [");

    int first = 1;
    for (int i = 0; i < 100; i++) {
        if (p->inventory[i]) {
            if (!first) fprintf(fp, ", ");
            fprintf(fp, "\"%s\"", p->inventory[i]);
            first = 0;
        }
    }

    fprintf(fp, "]\n}\n");
    fclose(fp);
    printf("게임이 저장되었습니다.\n");
}

/* ================================================
   LOAD(JSON)
   ================================================ */

int loadGame(Player* p, const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) { printf("불러올 파일 없음.\n"); return 0; }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* buf = malloc(size + 1);
    fread(buf, 1, size, fp);
    buf[size] = '\0';
    fclose(fp);

    json_get_string(buf, "name", p->name, sizeof(p->name), "Player");

    p->hp = json_get_int(buf, "hp", 100);
    p->maxHp = json_get_int(buf, "maxHp", 100);
    p->gold = json_get_int(buf, "gold", 100);

    p->Str = json_get_int(buf, "Str", 10);
    p->Dex = json_get_int(buf, "Dex", 10);
    p->Def = json_get_int(buf, "Def", 10);
    p->Int = json_get_int(buf, "Int", 10);

    p->turnCount = json_get_int(buf, "turnCount", 0);
    p->lastTownTurn = json_get_int(buf, "lastTownTurn", -999);
    p->lastCaveTurn = json_get_int(buf, "lastCaveTurn", -999);
    p->lastCampingTurn = json_get_int(buf, "lastCampingTurn", -999);

    p->caveDepth = json_get_int(buf, "caveDepth", 0);
    p->knowsCave = json_get_int(buf, "knowsCave", 0); // <--- [추가]
    p->heardLegend = json_get_int(buf, "heardLegend", 0);
    p->hasHolySword = json_get_int(buf, "hasHolySword", 0);
    p->hasCityPass = json_get_int(buf, "hasCityPass", 0);
    p->isBarrierBroken = json_get_int(buf, "isBarrierBroken", 0);

    p->location = json_get_int(buf, "location", LOC_ROAD);
    p->lastRoadEvent = json_get_int(buf, "lastRoadEvent", 0);
    p->lastTownEvent = json_get_int(buf, "lastTownEvent", 0);

    json_load_inventory(buf, p);

    free(buf);
    printf("저장 파일에서 불러왔습니다.\n");
    return 1;
}

/* ================================================
   INIT / ENEMY INIT / STATUS / INVENTORY
   ================================================ */

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

	player->isBarrierBroken = 0; // 마왕성 장벽 파괴 여부 초기화
    player->caveDepth = 0;
    player->knowsCave = 0;
    player->heardLegend = 0;
    player->hasHolySword = 0;
    player->hasCityPass = 0;

    for (int i = 0; i < 100; i++) player->inventory[i] = NULL;

    player->location = LOC_ROAD;

    addItem(player, "HP 포션");
    addItem(player, "HP 포션");
    addItem(player, "HP 포션");
}

void initEnemy(Enemy* enemy, const char* name, int hp, int attack, int gold) {
    strncpy(enemy->name, name, sizeof(enemy->name) - 1);
    enemy->name[sizeof(enemy->name) - 1] = '\0';

    enemy->hp = hp;
    enemy->maxHp = hp;
    enemy->attack = attack;
    enemy->gold = gold + (rand() % 31);
}

void printPlayerStatus(const Player* p) {
    printf("\n┌──────────────────────────────────────────────┐\n");
    printf("│ 이름: %-10s HP: %3d/%3d\n", p->name, p->hp, p->maxHp);
    printf("│ 골드: %-5d 힘:%2d 민첩:%2d 방어:%2d 지능:%2d\n",
        p->gold, p->Str, p->Dex, p->Def, p->Int);
    printf("└──────────────────────────────────────────────┘\n");
}

void addItem(Player* p, const char* item) {
    for (int i = 0; i < 100; i++) {
        if (!p->inventory[i]) {
#ifdef _WIN32
            p->inventory[i] = _strdup(item);
#else
            p->inventory[i] = strdup(item);
#endif
            printf("%s을(를) 획득했습니다!\n", item);
            return;
        }
    }
    printf("인벤토리가 가득 찼습니다.\n");
}

void printInventory(const Player* p) {
    printf("\n[인벤토리]\n");
    int empty = 1;

    for (int i = 0; i < 100; i++) {
        if (p->inventory[i]) {
            printf("%2d) %s\n", i, p->inventory[i]);
            empty = 0;
        }
    }

    if (empty)
        printf("(비어 있음)\n");
}

void useItem(Player* p, int index) {
    if (index == -1) {
        printInventory(p);
        printf("사용할 아이템 번호 입력(-1 취소): ");
        scanf("%d", &index);
        while (getchar() != '\n');
        if (index < 0) {
            printf("취소했습니다.\n");
            return;
        }
    }

    if (index < 0 || index >= 100 || !p->inventory[index]) {
        printf("잘못된 번호입니다.\n");
        return;
    }

    if (strcmp(p->inventory[index], "HP 포션") == 0) {
        p->hp += 30;
        if (p->hp > p->maxHp)
            p->hp = p->maxHp;
        printf("HP 포션 사용! 현재 HP: %d\n", p->hp);
    }

    free(p->inventory[index]);
    p->inventory[index] = NULL;
}
