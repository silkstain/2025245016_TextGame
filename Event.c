#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Event.h"
#include "Character.h"
#include "Battle.h"

static int caveStage = 0;

/* 판정 헬퍼 */
static int StatCheck(const char* statName, int statValue, int difficulty) {
    int roll = statValue + (rand() % 6);
    printf("[판정] %s: %d + 난수 => %d / 난이도 %d : %s\n",
        statName, statValue, roll, difficulty, (roll >= difficulty ? "성공" : "실패"));
    return roll >= difficulty;
}

/* 유틸 */
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
    printf("\n(엔터를 눌러 계속...)");
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/* 스토리 / 프롤로그 */
void Prologue_Event(Player* player) {
    printDivider("프롤로그");
    printf("깊은 심연, 오래전 봉인의 틈새로 새어나온 검은 안개는 천천히 대지를 뒤덮였습니다.\n");
    printf("왕국의 기록관들은 그것을 ‘귀환하는 그림자’라 불렀고, 농부들은 땅이 썩는다며 두려움에 떨었습니다.\n");
    printf("그리고 며칠 전, 전설 속에만 남아있던 마왕이 다시 눈을 떴다는 소식이 퍼져 나갔습니다.\n");
    printf("마을 간을 잇는 길에는 괴이한 짐승과 돌연변이들이 출몰하기 시작했고, 상인들은 더 이상 대륙을 건너지 않습니다.\n");
    printf("당신은 평범하게 살아갈 수도 있었지만, 선택했습니다. 후퇴가 아닌 전진을.\n");
    printf("아직 미약한 힘이지만, 한 걸음씩 세상을 바로 세울 수 있을지도 모릅니다.\n");
    printf("이제, 당신의 이름이 이야기의 서문에 새겨집니다.\n");
    EnterToContinue();
}
void StoryEvent_Start(Player* player) {
    printDivider("게임 시작");
    printf("“%s”... 그 이름을 마음속으로 되뇌이며 당신은 첫 걸음을 내딛습니다.\n", player->name);
    printf("새벽 안개가 걷히는 길 위, 갈라진 바위 사이로 바람이 스치고 먼 곳에서 종이 울리는 듯 잔향이 번집니다.\n");
    printf("땅은 아직 침묵하고 있지만, 위협은 그늘 아래에서 움직이고 있습니다.\n");
    printf("당신의 모험은 지금 시작되었습니다.\n");
    player->location = LOC_ROAD;
    EnterToContinue();
}

/* 숲 / 마왕성 */
void Forest_HolySword(Player* player) {
    printDivider("전설의 숲");
    printf("노인이 속삭였던 ‘선택받은 공터’는 예상보다 고요했습니다. 새소리도, 벌레소리도, 바람조차도 멈춘 공간.\n");
    printf("푸른 이끼가 깔린 중심부에 은빛과 옥빛이 뒤섞인 검이 박혀 있고 주위에는 오래전 용사의 흔적처럼 희미한 빛의 문양들이 떠 있습니다.\n");
    printf("손을 뻗는 순간, 차갑지만 맑은 떨림이 뼛속까지 스며드는 기묘한 감각이 찾아옵니다.\n");
    printf("성검을 뽑으시겠습니까? (1: 예, 2: 아니오): ");
    int choice; scanf("%d",&choice); while(getchar()!='\n');

    if (choice == 1) {
        if (StatCheck("힘", player->Str, 15)) {
            printf("당신의 손아귀가 검을 움켜쥐자 땅에 새겨진 문양들이 순차적으로 밝아지며 고요가 파도처럼 흔들렸습니다.\n");
            printf("묵직했던 저항은 서서히 빛으로 변해 당신의 혈관을 타고 흐르고, 검은 조용히 당신을 ‘주인’으로 인정했습니다.\n");
            printf("성검을 획득했습니다! (힘 +10, 민첩 +5)\n");
            player->Str += 10; player->Dex += 5; player->hasHolySword = 1;
            EnterToContinue();
            DemonKingCastle_Entrance(player);
        } else {
            printf("검은 미동조차 없고 주변의 빛은 잠깐 움찔하다가 다시 가라앉습니다.\n");
            printf("당신의 숨결은 아직 이 숲이 인정하는 ‘강함’에 미치지 못했습니다.\n");
            printf("언젠가 다시 오겠다는 다짐을 남기고 발길을 돌립니다.\n");
            EnterToContinue();
        }
    } else {
        printf("당신은 기묘한 압박감을 느끼며 아직 때가 아니이라고 판단합니다.\n");
        printf("숲은 다시 처음처럼 아무것도 없었던 것처럼 고요해집니다.\n");
        EnterToContinue();
    }
    player->location = LOC_ROAD;
}

void DemonKingCastle_Entrance(Player* player) {
    printDivider("마왕성 앞");
    printf("성검의 잔광이 길을 이어 붙여 마치 봉인된 좌표를 강제로 드러내듯 굽이진 길 끝에 거대한 성이 모습을 드러냅니다.\n");
    printf("검붉은 첨탑과 부패한 마력의 소용돌이, 그리고 창문마다 흐르는 녹색 번개의 실루엣.\n");
    printf("저 곳 너머에는 세상을 뒤틀어버린 의지가 기다리고 있습니다. 돌아갈 수 없는 마지막 구역입니다.\n");
    printf("당신의 심장은 고요하지만 단단히 뛰고 있습니다. 결전을 앞두고 한 번 더 호흡을 고칩니다.\n");
    player->location = LOC_DEMON_CASTLE;
    printf("\n(엔터를 눌러 마왕성으로 진입합니다...)");
    getchar();
}

/* ROAD 입장 이벤트 */
void RoadEvent_SmallTownEntrance(Player* player) {
    int choice;
    printDivider("마을 발견");
    printf("먼지 쌓인 길 끝, 낮게 피어오른 굴뚝 연기와 목재 울타리가 얇게 둘러진 작은 마을이 시야에 들어옵니다.\n");
    printf("지나가는 행인은 적고, 간헐적으로 들리는 망치질 소리가 생존 의지를 알려줍니다.\n");
    printf("마을로 들어가시겠습니까? (1: 예, 2: 아니오): ");
    scanf("%d",&choice); while(getchar()!='\n');
    if (choice != 1) {
        printf("당신은 잠깐 멈춰 바라보다 다시 길을 선택합니다.\n");
        EnterToContinue(); return;
    }
    printf("낡은 문짝을 밀고 안으로 들어섭니다. 사람들의 시선이 잠깐 모였다가 흩어집니다.\n");
    EnterToContinue();
    player->location = LOC_TOWN;
}

void RoadEvent_CaveEntrance(Player* player) {
    int choice;
    printDivider("동굴 발견");
    printf("지면이 움푹 꺼져 검은 틈이 생겼고, 안쪽에서는 축축한 흙냄새와 오래된 광물의 쇳내가 희미하게 배어 나옵니다.\n");
    printf("안쪽으로 이어지는 바람은 마치 허기진 짐승의 숨처럼 간헐적으로 들립니다.\n");
    printf("동굴로 들어가시겠습니까? (1: 예, 2: 아니오): ");
    scanf("%d",&choice); while(getchar()!='\n');
    if (choice != 1) {
        printf("위험을 뒤로한 채 다시 햇빛 아래 길을 이어 갑니다.\n");
        EnterToContinue(); return;
    }
    printf("어둠에 눈을 적응시키며 조심스럽게 발을 들입니다.\n");
    EnterToContinue();
    player->location = LOC_CAVE;
}

void RoadEvent_HugeCityEntrance(Player* player) {
    int c;
    printDivider("대도시 검문소");

    printf("거대한 성벽이 도시를 감싸고 있으며, 예전보다도 훨씬 많은 경비병들이 문 앞에 배치되어 있습니다.\n");
    printf("최근 마왕의 재림 이후 대도시 전역에 걸쳐 검문이 강화되었다고 들었습니다.\n");
    printf("경비병들은 지나가는 사람들을 하나하나 철저하게 조사하고 있습니다.\n\n");

    if (!player->hasCityPass) {
        printf("경비병: \"마왕군이 변장하고 침투하려 한다는 보고가 있다.\"\n");
        printf("경비병: \"출입증 없는 자는 누구든 도시 안으로 들일 수 없다. 돌아가라.\"\n\n");
        printf("당신은 출입증이 없어 이곳을 통과할 수 없습니다.\n");
        EnterToContinue();
        return;
    }

    printf("경비병이 출입증을 확인합니다...\n");
    printf("경비병: \"출입증 확인됐다. 이 문을 지나도 좋다. 안쪽에서도 검문이 계속되니 주의하라.\"\n\n");

    printf("1) 대도시에 입장한다\n");
    printf("2) 그냥 지나친다\n> ");

    if (scanf("%d", &c) != 1) {
        while (getchar() != '\n');
        EnterToContinue();
        return;
    }
    while (getchar() != '\n');

    if (c == 1) {
        printf("철문이 천천히 열리고, 당신은 강화된 경비 아래의 대도시로 들어갑니다.\n");
        player->location = LOC_BIG_CITY;
    }
    else {
        printf("당신은 다시 길로 발길을 돌립니다.\n");
    }
    EnterToContinue();
}


/* ROAD 기본 랜덤 */
void TreasureEvent(Player* player) {
    printDivider("보물 상자");
    int gold = 100 + rand()%101;
    int mimic = rand()%2;
    printf("잡초 사이 반쯤 묻힌 고풍스러운 상자. 녹슨 금속 테가 희미하게 반사광을 내며 당신의 시선을 붙듭니다.\n");
    printf("상자를 열어보시겠습니까? (1: 예, 2: 아니오): ");
    int choice; scanf("%d",&choice); while(getchar()!='\n');
    if (choice == 1) {
        printf("천천히 뚜껑을 들어올립니다...\n");
        if (mimic) {
            printf("혀와 이빨이 튀어나오며 상자 전체가 뒤틀립니다! 미믹의 급습입니다!\n");
            Enemy mimicEnemy; initEnemy(&mimicEnemy,"미믹",30+rand()%21,5+rand()%6,0);
            battle(player,&mimicEnemy);
        } else {
            printf("안에는 오래된 왕가 문양이 찍힌 주화가 가득합니다. 금화 %d개 획득!\n", gold);
            player->gold += gold;
        }
        EnterToContinue(); return;
    }
    printf("의심스러운 기운을 느껴 손대지 않고 지나갑니다.\n");
    EnterToContinue();
}

void TrapEvent(Player* player) {
    printDivider("함정");
    printf("발 아래 흙이 살짝 꺼지는 순간, ‘철컥’ 소리와 함께 양옆에서 날카로운 금속 장치가 튀어나오려 합니다.\n");
    printf("당신은 재빨리 판단해야 합니다.\n\n");

    printf("1) 빠르게 몸을 피한다 (민첩 판정)\n");
    printf("2) 함정의 구조를 분석해 해제한다 (지능 판정)\n");
    printf("3) 무시하고 지나간다\n> ");

    int c;
    if (scanf("%d", &c) != 1) {
        while (getchar() != '\n');
        printf("잘못된 입력입니다.\n");
        EnterToContinue();
        return;
    }
    while (getchar() != '\n');

    if (c == 1) {
        printf("\n빠르게 뛰어들 준비를 합니다. (민첩 난이도 11)\n");
        if (StatCheck("민첩", player->Dex, 11)) {
            printf("몸을 틀며 날아오는 파편을 전부 피했습니다! 피해 없음.\n");
        }
        else {
            int dmg = 10 + rand() % 6;
            printf("반응이 늦었습니다! 날카로운 금속이 스칩니다. HP -%d\n", dmg);
            player->hp -= dmg;
            if (player->hp < 0) player->hp = 0;
        }
    }
    else if (c == 2) {
        printf("\n함정의 구조를 재빨리 파악하려 합니다. (지능 난이도 12)\n");
        if (StatCheck("지능", player->Int, 12)) {
            printf("기계식 장치의 약점을 찾아 고정핀이 빠지지 않게 고정했습니다. 함정을 완전히 해제!\n");
            printf("보상: 지능 +1\n");
            player->Int += 1;
        }
        else {
            int dmg = 12 + rand() % 9;
            printf("구조 분석 실패! 장치가 폭발적으로 작동합니다. HP -%d\n", dmg);
            player->hp -= dmg;
            if (player->hp < 0) player->hp = 0;
        }
    }
    else {
        printf("\n아무 조치도 하지 않고 지나치려 합니다...\n");
        int dmg = 8 + rand() % 6;
        printf("함정이 당신을 스치며 작동했습니다! HP -%d\n", dmg);
        player->hp -= dmg;
        if (player->hp < 0) player->hp = 0;
    }

    EnterToContinue();
}


void FondEvent(Player* player) {
    printDivider("마법 우물");
    printf("지표 위엔 없을 법한 푸른 빛의 물이 잔잔히 맺혀 있고, 표면에는 성장과 기억의 문양이 살짝 흐르고 있습니다.\n");
    printf("손을 담그면 한 방향의 힘을 끌어올릴 수 있을 것 같습니다.\n");
    printf("1) 근육의 활성(힘 난이도 10)  2) 사고의 정렬(지능 난이도 10) > ");
    int c; scanf("%d",&c); while(getchar()!='\n');
    if (c == 1) {
        if (StatCheck("힘", player->Str, 10)) { printf("힘의 파동이 팔과 어깨를 순환합니다. Str +1\n"); player->Str += 1; }
        else printf("기운이 흩어져 잠깐의 피로만 남습니다.\n");
    } else if (c == 2) {
        if (StatCheck("지능", player->Int, 10)) { printf("차분한 물결이 사고를 재구성합니다. Int +1\n"); player->Int += 1; }
        else printf("잡음만 맴돌다 사라집니다.\n");
    } else {
        printf("우물을 지나칩니다.\n");
    }
    EnterToContinue();
}

void Encounter_goblin(Player* player) {
    printDivider("고블린 조우");
    printf("작은 그린 스킨이 낡은 가죽 조각과 삐뚤어진 칼을 들고 우물거리는 소리를 냅니다. 눈빛은 허기와 탐욕의 중간.\n");
    Enemy goblin; initEnemy(&goblin,"고블린",25+rand()%11,3+rand()%5,20+rand()%31);
    battle(player,&goblin);
    EnterToContinue();
}

void FindFood(Player* player) {
    int choice; int poison = rand()%100;
    printDivider("버섯 발견");
    printf("그늘진 고목 아래 군생하는 버섯 무리. 모양은 식용과 독이 애매하게 섞여 있어 보입니다.\n");
    printf("1. 그냥 먹는다  2. 지식으로 판별(지능 5 이상)\n> ");
    scanf("%d",&choice); while(getchar()!='\n');
    if (choice == 1) {
        if (poison < 50) {
            int dmg = 10 + rand()%11;
            printf("아릿한 쓴맛과 함께 독이 퍼집니다! HP -%d\n", dmg);
            player->hp -= dmg; if (player->hp < 0) player->hp = 0;
        } else {
            printf("부드럽고 향긋한 맛! 체력이 회복됩니다. HP +10\n");
            player->hp += 10; if (player->hp > player->maxHp) player->hp = player->maxHp;
        }
    } else if (choice == 2) {
        if (player->Int >= 5) {
            printf("자잘한 균사 패턴을 분석해 안전함을 확인. 영양분이 풍부합니다! HP +20\n");
            player->hp += 20; if (player->hp > player->maxHp) player->hp = player->maxHp;
        } else {
            printf("판별 실패. 애매한 위험을 피하고 버립니다.\n");
        }
    }
    EnterToContinue();
}

/* ROAD 확장 스탯 이벤트 */
void FallenTreeEvent(Player* player) {
    printDivider("쓰러진 거목");
    printf("폭풍에 뿌리째 뽑힌 듯한 거대한 나무가 길을 완전히 가로막고 있습니다. 이끼와 곰팡이 사이로 오래된 생명의 잔향이 뿌옇게 증발합니다.\n");
    printf("1) 힘으로 굴린다(Str 12) 2) 날렵히 넘는다(Dex 11) 3) 약점 찾는다(Int 11) 4) 우회한다\n> ");
    int c; scanf("%d",&c); while(getchar()!='\n');
    if (c==1) {
        if (StatCheck("힘", player->Str, 12)) { printf("천천히 나무가 굴러가며 길이 열립니다. Str +1\n"); player->Str += 1; }
        else { printf("근육이 경련하며 저항합니다. HP -8\n"); player->hp -= 8; if (player->hp<0) player->hp=0; }
    } else if (c==2) {
        if (StatCheck("민첩", player->Dex, 11)) { printf("균형감각이 돋보이게 매끄럽게 넘었습니다. Dex +1\n"); player->Dex += 1; }
        else { printf("이끼에 미끄러졌습니다. HP -5\n"); player->hp -= 5; if (player->hp<0) player->hp=0; }
    } else if (c==3) {
        if (StatCheck("지능", player->Int, 11)) { printf("부식된 지점 절단 후 통로 확보! Int +1, 골드 +10(마른 수액 판매)\n"); player->Int +=1; player->gold +=10; }
        else printf("구조를 이해하지 못해 시간만 흘렀습니다.\n");
    } else {
        printf("안전한 작은 오솔길을 찾아 우회합니다.\n");
    }
    EnterToContinue();
}

void BanditAmbushEvent(Player* player) {
    printDivider("산적 습격");
    printf("먼저 깔린 돌들 위로 낡은 붉은 천이 걸쳐져 있고 그 틈 사이로 산적들이 반짝이는 눈을 드러냅니다.\n");
    printf("그들의 긴장과 희열이 공기를 눌러옵니다. 선택은 빠르게 내려져야 합니다.\n");
    printf("1) 정면 돌파(Str 13) 2) 방어 유지(Def 12) 3) 이탈(Dex 13) 4) 속임수(Int 12)\n> ");
    int c; scanf("%d",&c); while(getchar()!='\n');
    if (c==1) {
        if (StatCheck("힘", player->Str, 13)) { printf("거칠게 휘몰아쳐 틈을 열었습니다. 골드 +30, Str +1\n"); player->gold +=30; player->Str +=1; }
        else { printf("균형을 잃고 반격을 허용. HP -12\n"); player->hp -=12; if(player->hp<0) player->hp=0; }
    } else if (c==2) {
        if (StatCheck("방어", player->Def, 12)) { printf("산적들의 공격이 무딘 파동처럼 흩어졌습니다. Def +1\n"); player->Def +=1; }
        else { printf("압박이 누적되어 틈이 생겼습니다. HP -10\n"); player->hp -=10; if(player->hp<0) player->hp=0; }
    } else if (c==3) {
        if (StatCheck("민첩", player->Dex, 13)) { printf("매복 구역을 유연하게 이탈. Dex +1, 골드 +15\n"); player->Dex +=1; player->gold +=15; }
        else { int loss=10; if(player->gold<loss) loss=player->gold; printf("퇴로 차단! HP -8, 골드 -%d\n", loss); player->hp -=8; if(player->hp<0) player->hp=0; player->gold -= loss; }
    } else if (c==4) {
        if (StatCheck("지능", player->Int, 12)) { printf("잘못된 증원 위치를 흘려 혼란 유발! Int +1\n"); player->Int +=1; }
        else { int loss=15; if(player->gold<loss) loss=player->gold; printf("거짓말이 들통났습니다. 골드 -%d\n", loss); player->gold -= loss; }
    } else {
        printf("주저하다 빈틈을 내주었습니다. HP -6\n"); player->hp -=6; if(player->hp<0) player->hp=0;
    }
    EnterToContinue();
}

void TravelingSageEvent(Player* player) {
    printDivider("여행하는 현자");
    printf("낡은 회색 외투와 마치 바람이 안쪽에서 도는 듯한 지팡이를 든 자가 길가 바위를 의자 삼아 앉아 있습니다.\n");
    printf("그는 눈을 뜬 채 조용히 당신을 ‘읽고’ 있는 듯한 느낌을 줍니다.\n");
    printf("1) 수수께끼 풀이(Int 13) 2) 체력 수행(Def 13) 3) 예의 있는 인사(Dex 10) 4) 무시\n> ");
    int c; scanf("%d",&c); while(getchar()!='\n');
    if (c==1) {
        if (StatCheck("지능", player->Int, 13)) { printf("그가 미소를 지으며 희미한 룬 단편을 설명해줍니다. Int +2\n"); player->Int +=2; }
        else printf("단어 조각이 서로 맞물리지 않습니다. 그는 고개를 끄덕이며 말없이 책을 다시 폈습니다.\n");
    } else if (c==2) {
        if (StatCheck("방어", player->Def, 13)) { printf("식지 않은 호흡을 조절하며 고된 버티기를 완주했습니다. 최대 HP +10\n"); player->maxHp +=10; player->hp +=10; }
        else { printf("근육이 떨려 더 진행 불가. HP -8\n"); player->hp -=8; if(player->hp<0) player->hp=0; }
    } else if (c==3) {
        if (StatCheck("민첩", player->Dex, 10)) { printf("절제되고 간결한 인사가 호감으로 이어졌습니다. Str +1\n"); player->Str+=1; }
        else printf("어색한 몸짓, 그는 다시 책으로 시선을 돌립니다.\n");
    } else {
        printf("당신은 조용히 지나칩니다. 바람이 살짝 방향을 바꾸는 느낌만 남습니다.\n");
    }
    EnterToContinue();
}

void MeteorShardEvent(Player* player) {
    printDivider("운석 파편");
    printf("타버린 흙덩어리 중앙에 유약처럼 반짝이는 운석 조각이 식어가며 희미한 열기를 냅니다.\n");
    printf("그 주위 공기는 현실과 약간 어긋난 것처럼 울렁거립니다.\n");
    printf("1) 맨손으로 집는다(Def 14) 2) 깨본다(Str 13) 3) 분석(Int 13) 4) 그냥 둔다\n> ");
    int c; scanf("%d",&c); while(getchar()!='\n');
    if (c==1) {
        if (StatCheck("방어", player->Def, 14)) { printf("타오르는 열을 견디며 파편의 에너지를 흡수했습니다. Def +1, 최대HP +5\n"); player->Def +=1; player->maxHp +=5; player->hp +=5; }
        else { printf("뜨거운 충격이 손바닥을 태웁니다. HP -12\n"); player->hp -=12; if(player->hp<0) player->hp=0; }
    } else if (c==2) {
        if (StatCheck("힘", player->Str, 13)) { printf("균열을 내어 내부 코어를 추출했습니다. Str +2\n"); player->Str +=2; }
        else { printf("반동이 손목을 때렸습니다. HP -6\n"); player->hp -=6; if(player->hp<0) player->hp=0; }
    } else if (c==3) {
        if (StatCheck("지능", player->Int, 13)) { printf("미세한 파형을 해독해 구조를 기록했습니다. Int +2\n"); player->Int +=2; }
        else printf("패턴이 흐트러져 의미를 잡지 못했습니다.\n");
    } else {
        printf("당신은 간섭하지 않고 길을 계속합니다.\n");
    }
    EnterToContinue();
}

/* ROAD 디스패처 */
void Event_Road(Player* player) {
    printDivider("길거리 (Road)");
    printf("갈라진 자갈과 발자국 흔적이 뒤섞인 길. 바람은 주변 수풀의 잎을 뒤집으며 작은 짐승들의 경계심을 건드립니다.\n");
    printf("오늘도 이 길 위에서 당신은 성장하거나, 상처받거나, 아무 일도 없이 지나갈 것입니다.\n");

    int townCooldown = 5, caveCooldown = 6;
    int ev, attempts = 0;
    while (1) {
        ev = rand()%12;
        if (ev == player->lastRoadEvent) { attempts++; if (attempts > 50) { ev = 4; break; } continue; }
        if (ev == 5 && player->turnCount - player->lastTownTurn < townCooldown) { attempts++; if (attempts > 50) { ev = 4; break; } continue; }
        if (ev == 3 && player->turnCount - player->lastCaveTurn < caveCooldown) { attempts++; if (attempts > 50) { ev = 4; break; } continue; }
        break;
    }

    switch (ev) {
        case 0: TreasureEvent(player); break;
        case 1: TrapEvent(player); break;
        case 2: FondEvent(player); break;
        case 3: player->lastCaveTurn = player->turnCount; RoadEvent_CaveEntrance(player); break;
        case 4: printDivider("평화로운 길"); printf("잠시 아무 위협도 없는 구간. 잔잔한 호흡이 안쪽 근육을 풀어줍니다.\n"); EnterToContinue(); break;
        case 5: player->lastTownTurn = player->turnCount; RoadEvent_SmallTownEntrance(player); break;
        case 6: FindFood(player); break;
        case 7: if (player->heardLegend && !player->hasHolySword) RoadEvent_ForestEntrance(player); else Encounter_goblin(player); break;
        case 8: FallenTreeEvent(player); break;
        case 9: BanditAmbushEvent(player); break;
        case 10: TravelingSageEvent(player); break;
        case 11: MeteorShardEvent(player); break;
    }
    player->lastRoadEvent = ev;
}

/* 동굴 탐험 */
void Event_Cave(Player* player) {
    printDivider("동굴 탐험");
    printf("동굴 입구 안쪽은 습하고 좁으며, 멀리 물이 떨어지는 규칙적인 반향이 심장박동처럼 공간을 메웁니다.\n");
    printf("석벽의 광물질 결정들이 희미한 잿빛을 띠고, 맹렬한 기운이 아니라 지친 침묵이 감돌고 있습니다.\n");

    int ev = rand() % 9;
    switch (ev) {
    case 0: Cave_BatSwarm(player); break;
    case 1: Cave_ShiningOre(player); break;
    case 2: Cave_PoisonMist(player); break;
    case 3: Cave_DeadAdventurer(player); break;
    case 4: Cave_StoneGolem(player); break;
    case 5: Cave_FallingFloor(player); break;
    case 6: Cave_UndergroundLake(player); break;
    case 7: Cave_AncientCarving(player); break;
    case 8: Cave_DeadGuard(player); break;     // ★ 추가
    }

    printf("\n(엔터를 눌러 동굴을 빠져나갑니다...)");
    EnterToContinue();
    player->location = LOC_ROAD;
}

void Cave_BatSwarm(Player* player) {
    printDivider("박쥐 무리 습격!");
    printf("천장 그늘에서 검은 물결이 부서지듯 쏟아져 내려 소음과 날개 부딪힘이 방향 감각을 찢어 놓습니다.\n");
    int damage = 5 + rand()%6;
    if (player->Dex >= 7) {
        printf("균형을 낮춰 회피하며 공격각을 최소화했습니다. (피해 -5)\n");
        damage -= 5; if (damage < 0) damage = 0;
    }
    player->hp -= damage; if (player->hp < 0) player->hp = 0;
    printf("HP -%d (현재 HP: %d)\n", damage, player->hp);
    EnterToContinue();
}

void Cave_ShiningOre(Player* player) {
    printDivider("반짝이는 광석");
    printf("벽면에 파란-은빛 결정이 박혀 있고 주위는 다른 광물과 달리 매끄러운 연마면처럼 반사합니다.\n");
    printf("채굴을 시도합니다...\n");
    if (player->Str >= 7) {
        int gold = 40 + rand()%31;
        printf("충분한 파쇄력으로 결정을 분리했습니다. 골드 +%d\n", gold);
        player->gold += gold;
    } else {
        printf("근력 부족으로 균열만 만들다 붕괴를 유발했습니다. HP -10\n");
        player->hp -= 10; if (player->hp < 0) player->hp = 0;
    }
    EnterToContinue();
}

void Cave_PoisonMist(Player* player) {
    printDivider("독 안개 지역");
    printf("바닥 틈에서 초록빛 기체가 낮게 흐르고 후각을 마비시키는 자극이 서서히 번집니다.\n");
    if (player->Int >= 7) {
        printf("바람 흐름을 읽어 상대적으로 맑은 라인을 찾아 이동합니다. 피해 없음.\n");
    } else {
        int damage = 12 + rand()%9;
        printf("기관지가 따끔거리고 어지러움이 몰려옵니다. HP -%d\n", damage);
        player->hp -= damage; if (player->hp < 0) player->hp = 0;
    }
    EnterToContinue();
}

void Cave_DeadAdventurer(Player* player) {
    printDivider("버려진 모험가의 시체");
    printf("낡은 가죽 갑옷과 금속 버클이 부식된 채 엎드려 있습니다. 주변엔 긁힌 흔적과 빨리 마른 혈흔.\n");
    printf("1. 조사한다  2. 무시한다\n> ");
    int c; scanf("%d",&c); while(getchar()!='\n');
    if (c == 1) {
        int r = rand()%4;
        if (r == 0) {
            printf("찰나의 마력 잔류가 깨어나 뒤틀린 기운이 형체를 재구성합니다! 언데드가 일어섭니다!\n");
            Enemy e; initEnemy(&e,"언데드 모험가",30,6,40);
            battle(player,&e);
        } else {
            printf("손때 묻은 작은 주머니에서 사용 가능한 회복 물약을 발견했습니다. 'HP 포션' 획득.\n");
            addItem(player,"HP 포션");
        }
    } else {
        printf("불쾌한 잔류 기운을 건드리지 않기로 하고 발길을 돌립니다.\n");
    }
    EnterToContinue();
}

void Cave_StoneGolem(Player* player) {
    printDivider("돌 골렘");
    printf("벽과 바닥이 솟구쳐 한 덩어리로 결합되더니 눈에 해당하는 부분이 희미한 황토빛을 띱니다.\n");
    Enemy golem; initEnemy(&golem,"돌 골렘",60,10,80);
    battle(player,&golem);
    if (player->hp > 0) {
        printf("광물 조직이 붕괴되며 단단한 파편이 남습니다. 방어력 +1\n");
        player->Def += 1;
    }
    EnterToContinue();
}

void Cave_FallingFloor(Player* player) {
    printDivider("바닥 붕괴");
    printf("무의식적으로 밟은 판상 구조가 ‘뚝’ 소리를 내며 하중을 견디지 못하고 꺼집니다.\n");
    if (player->Dex >= 7) {
        printf("순간적으로 가중치를 재분배하며 점프! 낙하를 피했습니다.\n");
    } else {
        int damage = 15 + rand()%6;
        printf("균형을 잃고 심한 낙하 충격을 받았습니다. HP -%d\n", damage);
        player->hp -= damage; if (player->hp < 0) player->hp = 0;
    }
    EnterToContinue();
}

void Cave_UndergroundLake(Player* player) {
    printDivider("지하 호수");
    printf("검푸른 수면이 거의 숨 쉬듯 잔잔하게 맥동하며 별빛 없는 밤처럼 깊은 색을 띱니다.\n");
    printf("물을 마시겠습니까? (1: 예, 2: 아니오) > ");
    int c; scanf("%d",&c); while(getchar()!='\n');
    if (c == 1) {
        int r = rand()%2;
        if (r == 0) {
            printf("맑은 미네랄이 체내 순환을 돕습니다. HP +20\n");
            player->hp +=20; if (player->hp > player->maxHp) player->hp = player->maxHp;
        } else {
            printf("금속성 쓴맛과 함께 위가 뒤틀립니다. HP -10\n");
            player->hp -=10; if (player->hp < 0) player->hp = 0;
        }
    } else {
        printf("잠재된 위험을 피하고 지나갑니다.\n");
    }
    EnterToContinue();
}

void Cave_AncientCarving(Player* player) {
    printDivider("고대 벽화");
    printf("침식된 암벽에 잔흔처럼 남은 각인. 순환 · 결박 · 승화를 나타내는 듯한 도형 배열이 흐릿하게 남았습니다.\n");
    if (player->Int >= 8) {
        printf("상징을 조합해 오래된 의식 구조를 해석했습니다. 지능 +1\n");
        player->Int += 1;
    } else {
        printf("단편이 있지만 의미망을 결합하기엔 정보가 부족합니다.\n");
    }
    EnterToContinue();
}

void Cave_DeadGuard(Player* player) {
    printDivider("사망한 대도시 경비병");
    printf("축축한 암흑 속에서 철제 갑옷이 희미하게 반짝입니다.\n");
    printf("가까이 다가가 보니, 대도시의 문장을 새긴 갑옷을 입은 경비병의 시신입니다.\n");
    printf("아마도 마왕의 봉인 붕괴 이후, 강화된 검문과 순찰 임무 중 동굴에 출몰한 괴물들에게 습격받은 듯합니다.\n\n");

    printf("그의 허리 쪽 주머니가 반쯤 열려 있습니다.\n");
    printf("1) 조사한다\n");
    printf("2) 건드리지 않는다\n> ");

    int c;
    if (scanf("%d", &c) != 1) {
        while (getchar() != '\n');
        printf("입력 오류.\n");
        EnterToContinue();
        return;
    }
    while (getchar() != '\n');

    if (c == 1) {
        printf("\n조심스럽게 주머니를 열어봅니다...\n");

        if (player->hasCityPass) {
            printf("이미 출입증을 가지고 있으므로 가져갈 만한 것은 없습니다.\n");
        }
        else {
            printf("안쪽에서 금속으로 된 작은 카드가 발견됩니다.\n");
            printf("앞면에는 대도시의 문장이, 뒷면에는 ‘정식 출입 허가’라고 새겨져 있습니다.\n");
            printf("당신은 ‘대도시 출입증’을 획득했습니다!\n");
            printf("죽은 경비병의 헌신이 헛되지 않도록 해야겠다는 기분이 듭니다.\n");
            player->hasCityPass = 1;
        }
    }
    else {
        printf("죽은 경비병을 건드리기 꺼림칙해 그냥 지나칩니다.\n");
    }

    EnterToContinue();
}



/* TOWN 허브 */
void Event_Town(Player* player) {
    int choice;
    while (1) {
        ClearScreen();
        printDivider("마을 (Town - 허브)");
        printf("이곳은 아직 버티고 있는 공동체입니다. 매일 조금씩 희망을 ‘유지’하는 사람들이 사는 곳.\n");
        printPlayerStatus(player);
        printf("\n1 여관  2 노상 음식  3 상점  4 장터  5 자원봉사\n");
        printf("6 대장간 7 광장 8 도박장 9 훈련장 10 전설 노인\n");
        printf("11 떠난다 12 도서관 13 투기장 14 연금술 15 도둑 길드\n선택: ");
        if (scanf("%d",&choice)!=1){ while(getchar()!='\n'); printf("잘못된 입력.\n"); EnterToContinue(); continue; }
        while(getchar()!='\n');
        switch(choice){
            case 1: Town_Inn(player); break;
            case 2: Town_StreetFood(player); break;
            case 3: Town_Shop(player); break;
            case 4: Town_MarketStall(player); break;
            case 5: Town_Volunteer(player); break;
            case 6: Town_Blacksmith(player); break;
            case 7: Town_Square(player); break;
            case 8: Town_Gamble(player); break;
            case 9: Town_Training(player); break;
            case 10: Town_Legend(player); break;
            case 11: printf("당신은 다시 길을 선택합니다.\n"); EnterToContinue(); player->location = LOC_ROAD; return;
            case 12: Town_Library(player); break;
            case 13: Town_Arena(player); break;
            case 14: Town_AlchemyLab(player); break;
            case 15: Town_ThievesGuild(player); break;
            default: printf("존재하지 않는 선택입니다.\n"); EnterToContinue(); break;
        }
        if (player->hp <= 0) { printf("당신은 마을에서 의식을 잃었습니다...\n"); EnterToContinue(); return; }
    }
}

/* TOWN 세부 (기존 단순 로직 + 서사 강화) */
void Town_Inn(Player* player) {
    printDivider("여관");
    printf("따뜻한 난로와 마른 허브 냄새가 배어 있는 작은 홀. 지친 여행객과 상인 둘이 낮은 목소리로 무역 얘기를 나눕니다.\n");
    printf("1 묵는다(10골드, HP +40) 2 떠난다 > ");
    int c; if (scanf("%d",&c)!=1){ while(getchar()!='\n'); printf("입력 오류.\n"); EnterToContinue(); return; } while(getchar()!='\n');
    if (c==1){
        if (player->gold < 10) printf("소지 골드가 부족합니다.\n");
        else {
            player->gold -= 10; player->hp += 40; if(player->hp>player->maxHp) player->hp=player->maxHp;
            printf("깊은 수면 뒤 맑은 의식으로 깨어났습니다. HP 회복.\n");
        }
    } else printf("당신은 여관을 나섭니다.\n");
    EnterToContinue();
}

void Town_StreetFood(Player* player) {
    printDivider("노상 음식점");
    printf("휘황찬란하진 않지만 따끈한 국물에서 김이 피어오르고, 허기를 달래는 향신료가 코끝을 자극합니다.\n");
    int cost=10, heal=20;
    if (player->gold >= cost){
        player->gold -= cost; player->hp += heal; if(player->hp>player->maxHp) player->hp=player->maxHp;
        printf("허기를 괜찮은 맛으로 채웠습니다. (HP +%d, 골드 -%d)\n", heal, cost);
    } else printf("주머니는 텅 비어 있고 허기는 잠시 더 지속됩니다.\n");
    EnterToContinue();
}

void Town_Shop(Player* player) {
    printDivider("상점");
    printf("벽에는 모험가들이 남기다 팔고 간 물품들이 정렬되어 있고 상인은 상황을 예의주시하는 눈빛입니다.\n");
    printf("1 포션(20) 2 힘의 반지(50) 3 캠핑 장비(40) 4 나가기 > ");
    int sel; scanf("%d",&sel); while(getchar()!='\n');
    if (sel==1){
        if(player->gold>=20){ player->gold-=20; player->hp+=30; if(player->hp>player->maxHp) player->hp=player->maxHp; printf("붉은 액체가 온몸을 덥힙니다. HP +30\n"); }
        else printf("골드 부족.\n");
    } else if (sel==2){
        if(player->gold>=50){ player->gold-=50; player->Str+=1; printf("손에 끼자 은은한 열이 전해집니다. 힘 +1\n"); }
        else printf("골드 부족.\n");
    } else if (sel==3){
        if(player->gold>=40){ player->gold-=40; addItem(player,"캠핑 장비"); printf("야영 준비가 수월해질 것입니다.\n"); }
        else printf("골드 부족.\n");
    } else printf("거래 없이 상점을 나갑니다.\n");
    EnterToContinue();
}

void Town_MarketStall(Player* player) {
    printDivider("장터 노점");
    printf("열악하지만 생활의 생동감이 남은 장터. 바람에 덜 마른 허브 다발과 손수 만든 장신구가 어설프게 진열되어 있습니다.\n");
    printf("1 회복약(8) 2 낡은 단검(30) 3 허리띠(40) 4 나가기 > ");
    int sel; if (scanf("%d",&sel)!=1){ while(getchar()!='\n'); printf("입력 오류.\n"); EnterToContinue(); return;} while(getchar()!='\n');
    if(sel==1){ if(player->gold>=8){ player->gold-=8; player->hp+=15; if(player->hp>player->maxHp) player->hp=player->maxHp; printf("쓴맛 뒤 안정. HP +15\n"); } else printf("골드 부족.\n"); }
    else if(sel==2){ if(player->gold>=30){ player->gold-=30; player->Str+=1; printf("닳은 손잡이가 손에 익습니다. 힘 +1\n"); } else printf("골드 부족.\n"); }
    else if(sel==3){ if(player->gold>=40){ player->gold-=40; player->Def+=1; printf("허리를 지지하며 호흡이 안정됩니다. 방어 +1\n"); } else printf("골드 부족.\n"); }
    else printf("빈 손으로 자리를 떠납니다.\n");
    EnterToContinue();
}

void Town_Volunteer(Player* player) {
    printDivider("자원봉사 센터");
    printf("허름한 게시판에 손글씨로 적힌 요청들이 붙어 있습니다. 모두 ‘지금 당장’ 필요한 작은 일들.\n");
    printf("1 돕는다 2 떠난다 > ");
    int sel; if (scanf("%d",&sel)!=1){ while(getchar()!='\n'); printf("입력 오류.\n"); EnterToContinue(); return;} while(getchar()!='\n');
    if(sel==1){
        int r=rand()%3;
        if(r==0){ player->Str+=1; printf("반복된 짐 운반이 근섬유를 자극했습니다. 힘 +1\n"); }
        else if(r==1){ int g=15; player->gold+=g; printf("감사 인사와 함께 작은 사례금. 골드 +%d\n", g); }
        else { player->Int+=1; printf("노인과의 대화 속에서 세밀한 표현을 배웠습니다. 지능 +1\n"); }
    } else printf("당신은 도움을 미루고 발길을 돌립니다.\n");
    EnterToContinue();
}

void Town_Blacksmith(Player* player) {
    printDivider("무기 제작소");
    printf("대장간 안은 금속 냄새와 숯의 열기로 가득합니다. 규칙적인 망치질이 일정한 리듬으로 귀를 두드립니다.\n");
    printf("1 무기 강화(50) 2 방어구 강화(50) 3 떠난다 > ");
    int sel; scanf("%d",&sel); while(getchar()!='\n');
    if(sel==1){ if(player->gold>=50){ player->gold-=50; player->Str+=1; printf("대장장이가 당신의 무기를 연마했습니다! (힘 +1)\n"); } else printf("골드가 부족합니다.\n"); }
    else if(sel==2){ if(player->gold>=50){ player->gold-=50; player->Def+=1; printf("대장장이가 당신의 갑옷을 보강했습니다! (방어 +1)\n"); } else printf("골드가 부족합니다.\n"); }
    else printf("대장간을 떠났습니다.\n");
    EnterToContinue();
}

void Town_GuardCheck(Player* player) {
    printDivider("마을 경비대 점검");
    printf("긴장한 경비가 여행자의 신분과 목적을 묻습니다. 주변에는 최근 실종 전단이 몇 장 붙어 있습니다.\n");
    if (player->Dex >= 7) {
        printf("침착하고 분명한 태도가 의심을 누그러뜨립니다. 별일 없이 통과.\n");
    } else {
        int penalty = rand()%2;
        if(penalty==0){ int hpLoss = 5+rand()%6; printf("불필요하게 에너지를 소모했습니다. HP -%d\n", hpLoss); player->hp -= hpLoss; if(player->hp<0) player->hp=0; }
        else { int goldLoss=5; if(player->gold<goldLoss) goldLoss=player->gold; printf("임시 통행료를 요구받았습니다. 골드 -%d\n", goldLoss); player->gold -= goldLoss; }
    }
    EnterToContinue();
}

void Town_Square(Player* player) {
    printDivider("마을 광장");
    printf("소박한 화단과 마른 샘터를 중심으로 아이들이 조심스럽게 공을 굴리고 상인들이 낮은 호객 음성을 냅니다.\n");
    printf("1 인파 속을 걷는다 2 경비대 쪽을 본다 3 쉬어간다 4 떠난다 > ");
    int sel; if (scanf("%d",&sel)!=1){ while(getchar()!='\n'); printf("입력 오류.\n"); EnterToContinue(); return;} while(getchar()!='\n');
    if(sel==1){ Town_Pickpocket(player); return; }
    else if(sel==2){ Town_GuardCheck(player); return; }
    else if(sel==3){
        int r=rand()%2;
        if(r==0){ printf("짧은 휴식으로 미미한 활력이 돌아옵니다. HP +5\n"); player->hp +=5; if(player->hp>player->maxHp) player->hp=player->maxHp; }
        else { int g=10; printf("땅에 떨어진 주머니를 발견. 골드 +%d\n", g); player->gold += g; }
        EnterToContinue();
    } else {
        printf("광장을 벗어납니다.\n");
        EnterToContinue();
    }
}

void Town_Gamble(Player* player) {
    printDivider("도박장");
    printf("희미한 등불과 굴러다니는 주사위, 긴장과 체념이 뒤섞인 공기가 저택 지하의 뒷골목 냄새를 떠올리게 합니다.\n");
    printf("베팅 선택: 1)10 2)20 3)50 4) 떠난다 > ");
    int sel; scanf("%d",&sel); while(getchar()!='\n');
    int bet=0;
    if(sel==1) bet=10; else if(sel==2) bet=20; else if(sel==3) bet=50; else { printf("도박을 포기합니다.\n"); EnterToContinue(); return; }
    if(player->gold < bet){ printf("골드 부족.\n"); EnterToContinue(); return; }
    player->gold -= bet;
    int playerDice = 1+rand()%6, dealerDice = 1+rand()%6;
    printf("당신 주사위: %d / 딜러: %d\n", playerDice, dealerDice);
    if (playerDice > dealerDice){ int win = bet*2; printf("운이 따라줍니다! 골드 +%d\n", win); player->gold += win; }
    else if (playerDice == dealerDice){ printf("무승부. 베팅 환불.\n"); player->gold += bet; }
    else printf("패배... 손실은 그대로입니다.\n");
    EnterToContinue();
}

void Town_Training(Player* player) {
    printDivider("훈련장");
    printf("간이 표적과 모래주머니가 줄지어 놓여 있습니다. 반복은 진보를 만듭니다.\n");
    printf("훈련 비용: 30골드 (관전은 무료)\n");
    printf("1. 힘 훈련 (Str 판정 난이도 10, 성공 시 Str +1)\n");
    printf("2. 민첩 훈련 (Dex 판정 난이도 10, 성공 시 Dex +1)\n");
    printf("3. 지능 훈련 (Int 판정 난이도 10, 성공 시 Int +1)\n");
    printf("4. 관전/분석 (Int 판정 난이도 11, 성공 시 Int +1, 실패 시 페널티 없음)\n");
    printf("5. 떠난다\n> ");

    int sel;
    if (scanf("%d", &sel) != 1) {
        while (getchar() != '\n');
        printf("잘못된 입력입니다.\n");
        EnterToContinue();
        return;
    }
    while (getchar() != '\n');

    if (sel == 5) {
        printf("훈련장을 떠납니다.\n");
        EnterToContinue();
        return;
    }

    if (sel == 4) {
        // 관전: 비용 없음
        if (StatCheck("지능", player->Int, 11)) {
            player->Int += 1;
            printf("전술 흐름을 분석하여 새로운 통찰을 얻었습니다. Int +1\n");
        } else {
            printf("복잡한 패턴을 끝내 이해하지 못했습니다.\n");
        }
        EnterToContinue();
        return;
    }

    if (sel < 1 || sel > 3) {
        printf("유효하지 않은 선택입니다.\n");
        EnterToContinue();
        return;
    }

    int cost = 30;
    if (player->gold < cost) {
        printf("골드가 부족합니다. (필요: %d, 보유: %d)\n", cost, player->gold);
        EnterToContinue();
        return;
    }

    player->gold -= cost;

    if (sel == 1) {
        if (StatCheck("힘", player->Str, 10)) {
            player->Str += 1;
            printf("저항 훈련 성공! Str +1\n");
        } else {
            printf("근육이 피로 누적. 효과 없음.\n");
        }
    } else if (sel == 2) {
        if (StatCheck("민첩", player->Dex, 10)) {
            player->Dex += 1;
            printf("리듬 훈련 성공! Dex +1\n");
        } else {
            printf("타이밍을 끝내 맞추지 못했습니다. 효과 없음.\n");
        }
    } else if (sel == 3) {
        if (StatCheck("지능", player->Int, 10)) {
            player->Int += 1;
            printf("동작 분석 정리 성공! Int +1\n");
        } else {
            printf("집중이 흐트러져 통찰을 얻지 못했습니다.\n");
        }
    }

    EnterToContinue();
}

void Town_Library(Player* player) {
    printDivider("도서관");
    printf("정적이 깔린 넓은 공간. 오래된 두루마리와 마법 서적들이 층층이 차곡하게 쌓여 있습니다.\n");
    printf("사서로 보이는 인물이 조용히 고개를 끄덕이며 당신을 맞이합니다.\n");

    printf("1) 고대 기록을 탐독한다(Int 판정 12)\n");
    printf("2) 전투 기술서를 읽는다(Str 판정 11)\n");
    printf("3) 민첩 교본을 읽는다(Dex 판정 11)\n");
    printf("4) 떠난다\n> ");

    int c;
    if (scanf("%d", &c) != 1) { while (getchar() != '\n'); printf("입력 오류.\n"); EnterToContinue(); return; }
    while (getchar() != '\n');

    if (c == 1) {
        if (StatCheck("지능", player->Int, 12)) {
            printf("고대 문양의 해석에 성공! 심층적 통찰을 얻습니다. Int +2\n");
            player->Int += 2;
        }
        else {
            printf("복잡한 문자가 집중력을 흩뜨립니다. HP -5\n");
            player->hp -= 5; if (player->hp < 0) player->hp = 0;
        }
    }
    else if (c == 2) {
        if (StatCheck("힘", player->Str, 11)) {
            printf("전투 자세와 체중 이동 원리를 익혔습니다! Str +1\n");
            player->Str += 1;
        }
        else {
            printf("무리한 동작 재현으로 근육을 삐끗했습니다. HP -4\n");
            player->hp -= 4; if (player->hp < 0) player->hp = 0;
        }
    }
    else if (c == 3) {
        if (StatCheck("민첩", player->Dex, 11)) {
            printf("균형과 타이밍의 핵심을 파악했습니다. Dex +1\n");
            player->Dex += 1;
        }
        else {
            printf("동작 이해 실패로 책더미에 부딪혔습니다. HP -3\n");
            player->hp -= 3; if (player->hp < 0) player->hp = 0;
        }
    }
    else {
        printf("당신은 도서관을 조용히 빠져나옵니다.\n");
    }

    EnterToContinue();
}

void Town_Arena(Player* player) {
    printDivider("투기장");
    printf("투박한 모래바닥과 거칠게 웃고 있는 전사들. 긴장감이 전장을 감쌉니다.\n");
    printf("관장이 묻습니다: “승부 보러 왔나?”\n");

    printf("전투 참가 비용: 20골드\n");
    printf("1) 결투한다  2) 떠난다\n> ");

    int c;
    if (scanf("%d", &c) != 1) { while (getchar() != '\n'); printf("입력 오류.\n"); EnterToContinue(); return; }
    while (getchar() != '\n');

    if (c != 1) {
        printf("투기장을 떠납니다.\n");
        EnterToContinue();
        return;
    }

    if (player->gold < 20) {
        printf("골드가 부족합니다.\n");
        EnterToContinue();
        return;
    }

    player->gold -= 20;

    Enemy fighter;
    initEnemy(&fighter, "숙련된 전사", 45 + rand() % 11, 8 + rand() % 5, 40);

    printf("상대 전사가 칼날을 세우며 앞으로 나옵니다!\n");
    battle(player, &fighter);

    if (player->hp > 0) {
        printf("결투에서 승리했습니다! 보상: 골드 +70, 힘 +1\n");
        player->gold += 70;
        player->Str += 1;
    }
    else {
        printf("전사는 마지막 일격을 멈추고 고개를 숙입니다.\n");
        printf("“패배를 인정해라.”\n");
    }
    EnterToContinue();
}

void Town_AlchemyLab(Player* player) {
    printDivider("연금술 작업장");
    printf("향긋하면서도 위험한 냄새가 섞인 방. 곳곳에 끓어오르는 약병이 진동을 냅니다.\n");
    printf("연금술사는 웃으며 말합니다: “재료를 섞어 볼 텐가?”\n");

    printf("필요 재료: HP 포션 1개 + 골드 15\n");
    printf("성공 시: 강화 포션 획득 (HP +50)\n");
    printf("1) 합성  2) 떠난다\n> ");

    int c;
    if (scanf("%d", &c) != 1) { while (getchar() != '\n'); printf("입력 오류.\n"); EnterToContinue(); return; }
    while (getchar() != '\n');

    if (c != 1) {
        printf("당신은 조용히 자리를 떠납니다.\n");
        EnterToContinue();
        return;
    }

    /* HP 포션 존재 여부 체크 */
    int index = -1;
    for (int i = 0; i < 100; i++) {
        if (player->inventory[i] && strcmp(player->inventory[i], "HP 포션") == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("HP 포션이 없습니다.\n");
        EnterToContinue();
        return;
    }
    if (player->gold < 15) {
        printf("골드가 부족합니다.\n");
        EnterToContinue();
        return;
    }

    player->gold -= 15;
    free(player->inventory[index]);
    player->inventory[index] = NULL;

    /* 합성 판정 */
    if (StatCheck("지능", player->Int, 12)) {
        printf("비율 조정 성공! 새로운 강화 포션을 획득합니다.\n");
        addItem(player, "강화 포션");
    }
    else {
        int explosion = 10 + rand() % 11;
        printf("합성이 폭발했습니다! HP -%d\n", explosion);
        player->hp -= explosion;
        if (player->hp < 0) player->hp = 0;
    }
    EnterToContinue();
}

void Town_ThievesGuild(Player* player) {
    printDivider("도둑 길드");
    printf("비좁은 골목 뒤 숨겨진 문. 은밀한 기운이 도는 방 안에서 그림자들이 움직입니다.\n");
    printf("길드 우두머리가 말합니다. “기량을 시험받고 싶나?”\n");

    printf("1) 은신 능력 시험(Dex 12)\n");
    printf("2) 함정 해제 시험(Int 12)\n");
    printf("3) 무력 시험(Str 12) → 전투 발생\n");
    printf("4) 그냥 떠난다\n> ");

    int c;
    if (scanf("%d", &c) != 1) { while (getchar() != '\n'); printf("입력 오류.\n"); EnterToContinue(); return; }
    while (getchar() != '\n');

    if (c == 1) {
        if (StatCheck("민첩", player->Dex, 12)) {
            printf("어둠에 녹아 사라지는 발걸음. 보상: Dex +1, 골드 +20\n");
            player->Dex += 1;
            player->gold += 20;
        }
        else {
            printf("발각되었습니다! 어둠 속에서 단검이 날아옵니다. HP -8\n");
            player->hp -= 8; if (player->hp < 0) player->hp = 0;
        }
    }
    else if (c == 2) {
        if (StatCheck("지능", player->Int, 12)) {
            printf("정교한 손놀림으로 함정을 해제했습니다. Int +1\n");
            player->Int += 1;
        }
        else {
            printf("오작동! 폭탄이 터집니다. HP -10\n");
            player->hp -= 10; if (player->hp < 0) player->hp = 0;
        }
    }
    else if (c == 3) {
        printf("길드 전사가 시험 삼아 칼을 뽑습니다!\n");
        Enemy thief;
        initEnemy(&thief, "도둑 길드 전사", 35 + rand() % 11, 7 + rand() % 4, 30);
        battle(player, &thief);

        if (player->hp > 0) {
            printf("무력 시험에서 승리했습니다! Str +1, 골드 +40\n");
            player->Str += 1;
            player->gold += 40;
        }
    }
    else {
        printf("길드는 다시 어둠 속으로 사라집니다.\n");
    }

    EnterToContinue();
}

void Event_BigCity(Player* player) {
    int choice;

    while (1) {
        ClearScreen();
        printDivider("대도시 (Hub)");
        printf("높은 성벽과 수많은 사람들, 상인과 모험가, 학자들이 뒤섞여 움직이는 거대한 도시입니다.\n");
        printf("이곳에서는 더 강력한 장비와 심화된 훈련, 정보와 마법을 얻을 수 있습니다.\n");
        printPlayerStatus(player);

        printf("\n1 대성당(Temple)\n");
        printf("2 상급 상점(Premium Shop)\n");
        printf("3 마법 길드(Mage Guild)\n");
        printf("4 모험가 길드(Adventurer Guild)\n");
        printf("5 공원(Park)\n");
        printf("6 도시에서 나간다(도로로 귀환)\n");
        printf("선택: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("잘못된 입력입니다.\n");
            EnterToContinue();
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
        case 1: BigCity_Temple(player); break;
        case 2: BigCity_PremiumShop(player); break;
        case 3: BigCity_MageGuild(player); break;
        case 4: BigCity_AdventurerGuild(player); break;
        case 5: BigCity_Park(player); break;
        case 6:
            printf("당신은 다시 성문을 나와 길로 돌아갑니다.\n");
            player->location = LOC_ROAD;
            EnterToContinue();
            return;
        default:
            printf("존재하지 않는 선택입니다.\n");
            EnterToContinue();
            break;
        }

        if (player->hp <= 0) {
            printf("당신은 대도시 한복판에서 의식을 잃었습니다...\n");
            EnterToContinue();
            return;
        }
    }
}


/* ====== 마왕성 개별 이벤트 복구/추가 시작 ====== */
void Castle_CursedHall(Player* player) {
    printDivider("저주받은 회랑");
    printf("뒤틀린 문양이 벽을 기어다니며 정신을 긁어냅니다.\n");
    printf("1) 의지로 버틴다(Def 13) 2) 문양을 해독(Int 13) 3) 빠르게 통과(Dex 12) 4) 머뭇거린다\n> ");
    int c;
    if (scanf("%d", &c) != 1) {
        while (getchar() != '\n');
        printf("입력 오류.\n");
        EnterToContinue();
        return;
    }
    while (getchar() != '\n');

    if (c == 1) {
        if (StatCheck("방어", player->Def, 13)) {
            printf("정신적 압박을 견뇌내며 내성을 강화했습니다. Def +1\n");
            player->Def += 1;
        } else {
            printf("속삭임이 마음을 후벼팝니다. HP -12\n");
            player->hp -= 12; if (player->hp < 0) player->hp = 0;
        }
    } else if (c == 2) {
        if (StatCheck("지능", player->Int, 13)) {
            printf("문양의 반복 패턴을 추출, 저주를 부분 중화. Int +1\n");
            player->Int += 1;
        } else {
            printf("위상 해석 실패로 반발을 받았습니다. HP -10\n");
            player->hp -= 10; if (player->hp < 0) player->hp = 0;
        }
    } else if (c == 3) {
        if (StatCheck("민첩", player->Dex, 12)) {
            printf("최소 노출로 회랑을 돌파. Dex +1\n");
            player->Dex += 1;
        } else {
            printf("균형을 잃고 불필요하게 오래 머물렀습니다. HP -8\n");
            player->hp -= 8; if (player->hp < 0) player->hp = 0;
        }
    } else {
        printf("망설이다 저주가 짙어집니다. HP -6\n");
        player->hp -= 6; if (player->hp < 0) player->hp = 0;
    }
    EnterToContinue();
}

void BigCity_Temple(Player* player) {
    printDivider("대성당");
    printf("높은 천장과 스테인드글라스를 통해 들어오는 빛이, 먼지 위로 조용히 흩어집니다.\n");
    printf("사제는 피곤해 보이지만, 아직 사람들을 돕고자 하는 의지를 잃지 않았습니다.\n\n");

    printf("1) 치유 의식 (30 골드, HP 완전 회복)\n");
    printf("2) 축복 의식 (60 골드, 모든 능력치 +1)\n");
    printf("3) 그냥 떠난다\n> ");

    int c;
    if (scanf("%d", &c) != 1) {
        while (getchar() != '\n');
        printf("입력 오류.\n");
        EnterToContinue();
        return;
    }
    while (getchar() != '\n');

    if (c == 1) {
        if (player->gold < 30) {
            printf("골드가 부족합니다.\n");
        }
        else {
            player->gold -= 30;
            player->hp = player->maxHp;
            printf("따뜻한 빛이 몸을 감싸며 상처가 아물어 갑니다. HP가 완전히 회복되었습니다.\n");
        }
    }
    else if (c == 2) {
        if (player->gold < 60) {
            printf("골드가 부족합니다.\n");
        }
        else {
            player->gold -= 60;
            player->Str += 1;
            player->Dex += 1;
            player->Def += 1;
            player->Int += 1;
            printf("짧은 기도와 함께 이마에 새긴 문양이 희미하게 빛납니다. 모든 능력치가 1씩 증가했습니다.\n");
        }
    }
    else {
        printf("당신은 조용히 대성당을 빠져나옵니다.\n");
    }
    EnterToContinue();
}

void BigCity_PremiumShop(Player* player) {
    printDivider("상급 상점");
    printf("정돈된 진열대 위로 고급 장비와 특제 약품들이 가지런히 놓여 있습니다.\n");
    printf("상인은 가격을 말하기 전에 당신의 지갑 두께를 먼저 훑어봅니다.\n\n");

    printf("1) 고급 회복약 (40 골드, HP +50)\n");
    printf("2) 힘의 상급 반지 (70 골드, Str +2)\n");
    printf("3) 지식 서적 (70 골드, Int +2)\n");
    printf("4) 떠난다\n> ");

    int c;
    if (scanf("%d", &c) != 1) {
        while (getchar() != '\n');
        printf("입력 오류.\n");
        EnterToContinue();
        return;
    }
    while (getchar() != '\n');

    if (c == 1) {
        if (player->gold < 40) printf("골드가 부족합니다.\n");
        else {
            player->gold -= 40;
            player->hp += 50;
            if (player->hp > player->maxHp) player->hp = player->maxHp;
            printf("강한 약초 향이 목을 타고 내려갑니다. HP +50\n");
        }
    }
    else if (c == 2) {
        if (player->gold < 70) printf("골드가 부족합니다.\n");
        else {
            player->gold -= 70;
            player->Str += 2;
            printf("손가락에 낀 반지가 묵직한 힘을 전합니다. Str +2\n");
        }
    }
    else if (c == 3) {
        if (player->gold < 70) printf("골드가 부족합니다.\n");
        else {
            player->gold -= 70;
            player->Int += 2;
            printf("두꺼운 책을 읽고 나니 머리가 맑아진 느낌입니다. Int +2\n");
        }
    }
    else {
        printf("당신은 거래를 하지 않고 상점을 떠납니다.\n");
    }
    EnterToContinue();
}

void BigCity_MageGuild(Player* player) {
    printDivider("마법 길드");
    printf("공중에 떠 있는 서적과 빙글도는 룬 조각들이 공간 전체에 흐르고 있습니다.\n");
    printf("마법사는 당신을 흥미로운 눈빛으로 바라봅니다.\n\n");

    printf("1) 마나 구조 강의 (Int 판정 난이도 13, 성공 시 Int +2)\n");
    printf("2) 정신 집중 훈련 (Def 판정 난이도 12, 성공 시 최대 HP +5)\n");
    printf("3) 떠난다\n> ");

    int c;
    if (scanf("%d", &c) != 1) {
        while (getchar() != '\n');
        printf("입력 오류.\n");
        EnterToContinue();
        return;
    }
    while (getchar() != '\n');

    if (c == 1) {
        if (StatCheck("지능", player->Int, 13)) {
            player->Int += 2;
            printf("복잡한 마나 흐름이 조금은 읽히는 것 같습니다. Int +2\n");
        }
        else {
            printf("개념이 뒤엉켜 머리가 지끈거립니다. HP -5\n");
            player->hp -= 5;
            if (player->hp < 0) player->hp = 0;
        }
    }
    else if (c == 2) {
        if (StatCheck("방어", player->Def, 12)) {
            player->maxHp += 5;
            player->hp += 5;
            printf("정신 집중을 통해 신체의 회복력이 향상됩니다. 최대 HP +5\n");
        }
        else {
            printf("집중이 잘 되지 않습니다. 큰 변화는 없었습니다.\n");
        }
    }
    else {
        printf("당신은 마법의 기운을 뒤로하고 길드를 나옵니다.\n");
    }
    EnterToContinue();
}

void BigCity_AdventurerGuild(Player* player) {
    printDivider("모험가 길드");
    printf("벽에는 수많은 의뢰서와 성공/실패 기록이 빽빽하게 붙어 있습니다.\n");
    printf("접수원이 말합니다. \"도시 주변을 괴롭히는 강적이 하나 있소. 처리해볼 텐가?\"\n\n");

    printf("1) 수락한다 (전투)  2) 거절한다\n> ");

    int c;
    if (scanf("%d", &c) != 1) {
        while (getchar() != '\n');
        printf("입력 오류.\n");
        EnterToContinue();
        return;
    }
    while (getchar() != '\n');

    if (c != 1) {
        printf("당신은 아직 때가 아니라 생각하고 의뢰를 거절합니다.\n");
        EnterToContinue();
        return;
    }

    Enemy elite;
    initEnemy(&elite, "도시 주변 위협자", 80 + rand() % 21, 14 + rand() % 5, 120);

    printf("당신은 길드를 나와 목표를 찾아 나섭니다.\n");
    battle(player, &elite);

    if (player->hp > 0) {
        printf("의뢰를 완수했습니다! 보상: 골드 +120, Str +1, Dex +1\n");
        player->gold += 120;
        player->Str += 1;
        player->Dex += 1;
    }
    else {
        printf("전투는 혹독했습니다. 살아 돌아온다면 언젠가 다시 도전할 수 있을 것입니다.\n");
    }
    EnterToContinue();
}

void BigCity_Park(Player* player) {
    printDivider("도시 공원");
    printf("어느 정도 정돈된 나무와 벤치, 사람들의 웃음소리가 들리는 한켠입니다.\n");
    printf("잠시 숨을 고르기에 좋은 장소입니다.\n\n");

    int r = rand() % 3;
    if (r == 0) {
        printf("조용히 벤치에 앉아 쉬었습니다. HP +10\n");
        player->hp += 10;
        if (player->hp > player->maxHp) player->hp = player->maxHp;
    }
    else if (r == 1) {
        printf("길거리 공연을 보며 기분이 좋아졌습니다. Int +1\n");
        player->Int += 1;
    }
    else {
        int g = 15;
        printf("벤치 아래에서 떨어진 주머니를 발견했습니다. 골드 +%d\n", g);
        player->gold += g;
    }
    EnterToContinue();
}


void Castle_ShadowKnight(Player* player) {
    printDivider("그림자 기사");
    printf("검은 갑주는 형체를 이루고 연기가 칼날을 따라 응축됩니다.\n");
    Enemy knight;
    initEnemy(&knight, "그림자 기사", 55 + rand()%11, 12 + rand()%4, 80);
    battle(player, &knight);
    if (player->hp > 0) {
        printf("잔류 에너지가 신체를 강화합니다. Str +1, Def +1\n");
        player->Str += 1; player->Def += 1;
    }
    EnterToContinue();
}

void Castle_DarkAltar(Player* player) {
    printDivider("암흑 제단");
    printf("맥동하는 코어와 흩어진 룬 조각. 선택이 필요합니다.\n");
    printf("1) 물리 파괴(Str 15) 2) 룬 재배열(Int 15) 3) 에너지 흡수(Def 15) 4) 관망\n> ");
    int c; scanf("%d",&c); while(getchar()!='\n');
    if (c == 1) {
        if (StatCheck("힘", player->Str, 15)) {
            printf("균열 확산 성공! Str +2\n");
            player->Str += 2;
        } else {
            printf("반발 폭렬! HP -18\n");
            player->hp -= 18; if (player->hp < 0) player->hp = 0;
        }
    } else if (c == 2) {
        if (StatCheck("지능", player->Int, 15)) {
            printf("위상 재조정 성공! Int +2\n");
            player->Int += 2;
        } else {
            printf("실패한 교정이 충격파를 발생시켰습니다. HP -16\n");
            player->hp -= 16; if (player->hp < 0) player->hp = 0;
        }
    } else if (c == 3) {
        if (StatCheck("방어", player->Def, 15)) {
            printf("안정적 흡수. Def +2, 최대HP +5\n");
            player->Def += 2; player->maxHp += 5; player->hp += 5;
        } else {
            printf("과부하 역류! HP -14\n");
            player->hp -= 14; if (player->hp < 0) player->hp = 0;
        }
    } else {
        printf("코어는 더 밝아질 뿐 아무 것도 얻지 못했습니다. HP -6\n");
        player->hp -= 6; if (player->hp < 0) player->hp = 0;
    }
    EnterToContinue();
}

void Castle_MaliceStorm(Player* player) {
    printDivider("악의 폭풍");
    printf("농밀한 마력 소용돌이가 생체 리듬을 뒤틀려 놓습니다.\n");
    printf("1) 힘으로 돌파(Str 14) 2) 방어로 견딘다(Def 14) 3) 흐름을 읽는다(Int 14) 4) 틈새 통과(Dex 14) 5) 지연\n> ");
    int c; scanf("%d",&c); while(getchar()!='\n');
    int diff = 14;
    const char* statName = NULL;
    int* statPtr = NULL;

    if (c == 1) { statName = "힘"; statPtr = &player->Str; }
    else if (c == 2) { statName = "방어"; statPtr = &player->Def; }
    else if (c == 3) { statName = "지능"; statPtr = &player->Int; }
    else if (c == 4) { statName = "민첩"; statPtr = &player->Dex; }
    else {
        printf("판단 지연으로 폭풍에 휘말립니다. HP -12\n");
        player->hp -= 12; if (player->hp < 0) player->hp = 0;
        EnterToContinue();
        return;
    }

    if (StatCheck(statName, *statPtr, diff)) {
        printf("폭풍을 극복하며 잠재 능력이 고양됩니다. %s +1\n", statName);
        (*statPtr) += 1;
    } else {
        printf("잔류 전하가 신경계를 태웁니다. HP -15\n");
        player->hp -= 15; if (player->hp < 0) player->hp = 0;
    }
    EnterToContinue();
}

void Castle_FinalBattle(Player* player) {
    printDivider("마왕과의 결전");
    printf("뒤틀린 공간과 붉은 파동. 마왕이 천천히 시선을 돌립니다.\n");
    Enemy demonLord;
    initEnemy(&demonLord, "마왕", 200, 18, 500);
    battle(player, &demonLord);
    if (player->hp > 0) {
        printDivider("승리");
        printf("어둠의 실이 붕괴되며 세계는 회복 가능성을 되찾습니다.\n");
        printf("보상: 모든 기본 스탯 +1, 골드 +300\n");
        player->Str +=1; player->Dex +=1; player->Def +=1; player->Int +=1;
        player->gold += 300;
    } else {
        printDivider("패배");
        printf("의지가 분해되어 어둠은 잠시 더 지속됩니다.\n");
    }
    EnterToContinue();
}
/* ====== 마왕성 개별 이벤트 끝 ====== */

void RoadEvent_ForestEntrance(Player* player) {
    printDivider("전설의 숲 입구");
    printf("전설로만 듣던 숲의 경계. 나무들은 정상적인 성장 방향을 잊은 듯 서로 엮여 아치형 통로를 만들고\n");
    printf("희미한 빛의 입자들이 공중에서 숨을 쉬듯 천천히 맥동합니다.\n");
    printf("숲으로 들어가겠습니까? (1: 들어간다 2: 아직 아니다): ");
    int c; scanf("%d",&c); while(getchar()!='\n');
    if (c == 1) {
        printf("당신은 얽힌 뿌리 사이로 발을 들입니다. 공기는 한층 조용하고 또렷합니다.\n");
        player->location = LOC_FOREST;
    } else {
        printf("아직 준비가 더 필요하다고 느끼며 길을 계속 걷습니다.\n");
    }
    EnterToContinue();
}


/* 간단 야영 이벤트 (현재 호출되지 않지만 향후 확장 대비) */
void CampingEvent(Player* player) {
    printDivider("야영");
    printf("휴대 장비를 펼쳐 조용한 밤공기에 호흡을 맞춥니다.\n");
    int heal = 25;
    player->hp += heal;
    if (player->hp > player->maxHp) player->hp = player->maxHp;
    printf("짧은 회복 시간. HP +%d (현재 %d/%d)\n", heal, player->hp, player->maxHp);
    EnterToContinue();
}

/* 소매치기 시도 (Town_Square 경로) */
void Town_Pickpocket(Player* player) {
    printDivider("소매치기 시도");
    printf("혼잡한 틈 사이 노출된 헐거운 주머니 하나가 눈에 들어옵니다.\n");
    if (StatCheck("민첩", player->Dex, 11)) {
        int g = 20 + rand()%21;
        player->gold += g;
        printf("은밀하게 추출 성공! 골드 +%d (현재 %d)\n", g, player->gold);
    } else {
        int loss = 10; if (player->gold < loss) loss = player->gold;
        int dmg = 5 + rand()%6;
        player->gold -= loss;
        player->hp -= dmg; if (player->hp < 0) player->hp = 0;
        printf("발각! 밀쳐집니다. 골드 -%d, HP -%d\n", loss, dmg);
    }
    EnterToContinue();
}

/* 전설 노인 (성검 힌트) */
void Town_Legend(Player* player) {
    printDivider("전설의 노인");
    if (player->heardLegend) {
        printf("그는 조용히 고개만 끄덕입니다. 더 줄 말은 없습니다.\n");
    } else {
        printf("“얽힌 숲 중심의 고요... 빛이 숨을 쉬는 자리. 검은 거기.”\n");
        player->heardLegend = 1;
        printf("전설을 들었습니다. (전설의 숲 진입 이벤트 해금)\n");
    }
    EnterToContinue();
}

/* 숲 지역 디스패처 */
void Event_Forest(Player* player) {
    printDivider("전설의 숲");
    if (!player->hasHolySword) {
        Forest_HolySword(player); /* 성공 시 마왕성 입구로 이동, 실패 시 길 복귀 */
        return;
    }
    printf("성검의 잔광이 비틀린 수목 배열을 정돈해 통로를 형성합니다.\n");
    printf("1 마왕성으로 간다 2 떠난다 > ");
    int c; if (scanf("%d",&c)!=1){ while(getchar()!='\n'); printf("입력 오류.\n"); EnterToContinue(); return; }
    while(getchar()!='\n');
    if (c==1) {
        DemonKingCastle_Entrance(player);
    } else {
        printf("당신은 다시 길로 돌아갑니다.\n");
        player->location = LOC_ROAD;
        EnterToContinue();
    }
}

/* 마왕성 진행 관리 */
static int demonCastleStage = 0;

void Event_DemonCastle(Player* player) {
    printDivider("마왕성 내부");
    if (player->hp <= 0) { printf("움직일 힘이 없습니다.\n"); EnterToContinue(); return; }

    if (demonCastleStage >= 5) {
        Castle_FinalBattle(player);
        /* 최종전 후 초기화 */
        demonCastleStage = 0;
        player->location = LOC_ROAD;
        return;
    }

    printf("정화 진행도: %d / 5\n", demonCastleStage);
    printf("1 전진  2 후퇴(도로) > ");
    int c; if (scanf("%d",&c)!=1){ while(getchar()!='\n'); printf("입력 오류.\n"); EnterToContinue(); return; }
    while(getchar()!='\n');

    if (c==2) {
        printf("강한 압력을 뒤로하고 성을 빠져나옵니다.\n");
        player->location = LOC_ROAD;
        EnterToContinue();
        return;
    }

    int ev = rand()%4;
    switch(ev){
        case 0: Castle_CursedHall(player); break;
        case 1: Castle_ShadowKnight(player); break;
        case 2: Castle_DarkAltar(player); break;
        case 3: Castle_MaliceStorm(player); break;
    }
    if (player->hp > 0) demonCastleStage++;
}