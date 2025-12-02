#pragma once

/* 이벤트 위치 풀 */
typedef enum {
    LOC_ROAD, // 길거리, 기본 위치
    LOC_TOWN, // 마을
    LOC_CAVE, // 동굴
    LOC_FOREST, // 전설의 숲
    LOC_BIG_CITY, // 대도시
    LOC_DEMON_CASTLE // 마왕성
} Location;

/* 플레이어 및 적 구조체 정의 */
typedef struct {
    char name[20]; // 플레이어 이름

    int hp, maxHp; // 현재 체력 및 최대 체력
    int gold; // 소지 골드

    int Str, Dex, Def, Int; // 능력치: 힘, 민첩, 방어, 지능

    int turnCount; // 전체 턴 카운트
    int lastTownTurn; // 마을 방문 기준 턴 기록
    int lastCaveTurn; // 동굴 방문 기준 턴 기록

    int caveDepth; // 동굴 깊이
    int knowsCave; // 동굴 위치 인지 여부
    int heardLegend; // 전설에 대해 들었는지 여부
    int hasHolySword; // 전설의 성검 소지 여부
    int hasCityPass; // 대도시 출입증 소지 여부
    int isBarrierBroken; // 마왕성 결계 파괴 여부

	char inventory[100][50]; // 인벤토리 (최대 100개 아이템, 각 아이템 이름 최대 50자)

    Location location; // 현재 위치

    int lastRoadEvent; // 최근 길 이벤트 기록
    int lastTownEvent; // 최근 마을 이벤트 기록
    int lastCampingTurn; // 최근 야영 이벤트 턴 기록
} Player;

typedef struct {
    char name[20]; // 적 이름
    int hp, maxHp; // 현재 체력 및 최대 체력
    int attack; // 공격력
    int gold; // 처치 시 획득 골드
} Enemy;

/* 함수 원형 선언 */
void initPlayer(Player* player); // 플레이어 초기화
void initEnemy(Enemy* enemy, const char* name, int hp, int attack, int gold); // 적 초기화
void printPlayerStatus(const Player* player); // 플레이어 상태 출력
void addItem(Player* player, const char* item); // 아이템 추가
void useItem(Player* player, int index); // 아이템 사용
void printInventory(const Player* player); // 인벤토리 출력
void saveGame(Player* player, const char* filename); // 게임 저장
int loadGame(Player* player, const char* filename); // 게임 불러오기