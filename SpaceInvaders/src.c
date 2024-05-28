#include "heading.h"
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>

void _initTest(bool testRes, char* name)
{
    if (testRes) {
        return;
    }
    fprintf(stderr, "couldn't initialize %s\n", name);
    exit(-2);
}
void _initAllegroVars()
{
    _initTest(al_init(), "allegro");
    _initTest(al_install_keyboard(), "keyboard");
    _initTest(al_init_font_addon(), "font addon");
    _initTest(al_init_ttf_addon(), "ttf addon");
    _initTest(al_init_image_addon(), "image addon");
    _initTest(al_init_primitives_addon(), "primitives addon");
    _initTest(al_install_audio(), "audio addon");
    _initTest(al_init_acodec_addon(), "acodec addon");
    allDisplay = al_create_display(DISP_W, DISP_H);
    _initTest(allDisplay, "allDisplay");
    allBuffer = al_create_bitmap(DISP_W, DISP_H);
    _initTest(allBuffer, "bitmap allBuffer");
    font = al_load_ttf_font("font.ttf", DISP_W / 13, 0);
    _initTest(font, "font.ttf");
    smallFont = al_load_ttf_font("font.ttf", DISP_W / 20, 0);
    _initTest(smallFont, "font.ttf - small");
    allTimer = al_create_timer(1.0 / FRAMERATE);
    _initTest(allTimer, "allTimer");
    allQueue = al_create_event_queue();
    _initTest(allQueue, "allQueue");
    bmapSpriteMap = al_load_bitmap("SpriteMap.png");
    _initTest(bmapSpriteMap, "bmapSpriteMap");
    bmapBackground = al_load_bitmap("Background.png");
    _initTest(bmapBackground, "bmapBackground");
    al_reserve_samples(10);
    sndLaser = al_load_sample("LaserShoot.wav");
    _initTest(sndLaser, "sndLaser");
    sndEnemyLaser = al_load_sample("EnemyLaserShoot.wav");
    _initTest(sndEnemyLaser, "sndEnemyLaser");
    sndHitHurt = al_load_sample("HitHurt.wav");
    _initTest(sndHitHurt, "sndHitHurt");
    sndMenuSelect = al_load_sample("Select.wav");
    _initTest(sndMenuSelect, "sndMenuSelect");
    sndMenuChange = al_load_sample("MenuChange.wav");
    _initTest(sndMenuChange, "sndMenuChange");
    al_register_event_source(allQueue, al_get_keyboard_event_source());
    al_register_event_source(allQueue, al_get_display_event_source(allDisplay));
    al_register_event_source(allQueue, al_get_timer_event_source(allTimer));
    al_start_timer(allTimer);
}
StrGaState _initGameState() {
    StrGaState retStruct;
    retStruct.quitProg = false;
    retStruct.reDraw = true;
    retStruct.currentScore = 0;
    retStruct.roundNum = 1;
    retStruct.timer = 10;
    retStruct.screenFlag = start;
    retStruct.menuCount = 0;
    retStruct.pauseMenuFlag = 0;
    return retStruct;
}
StrRect* _initWallArr(int wallAmnt) {
    StrRect* retWallArr = (StrRect*)calloc(wallAmnt, sizeof(StrRect));
    _ptrCheckNull(retWallArr);
    for (int i = 0; i < wallAmnt; i++) {
        retWallArr[i].x1 = rand() % (DISP_W / wallAmnt - WALLWIDTH) + i * (DISP_W / wallAmnt);
        retWallArr[i].x2 = retWallArr[i].x1 + WALLWIDTH;
        retWallArr[i].y1 = DISP_H * 2 / 5 + rand() % 9 * DISP_H / 20 + ENEMYHEIGHT;
        retWallArr[i].y2 = retWallArr[i].y1 + WALLHEIGHT;
    }
    return retWallArr;
}
StrList* _initEnemyList() {
    int height = DISP_H * 2 / 5;
    StrList* tempEnList = (StrList*)malloc(sizeof(StrList));
    _ptrCheckNull(tempEnList);
    StrList* retEnList = tempEnList;
    tempEnList->x = DISP_W / 4;
    tempEnList->y = height;
    tempEnList->next = NULL;
    for (int i = 1; i < ENEMYCOUNT; i++) {
        tempEnList->next = (StrList*)malloc(sizeof(StrList));
        _ptrCheckNull(tempEnList->next);
        if (i % ENEMYPERROW == 0) height -= DISP_H / 20;
        tempEnList->next->x = DISP_W / 4 + i % ENEMYPERROW * ENEMYWIDTH * 5 / 4;
        tempEnList->next->y = height;
        tempEnList = tempEnList->next;
    }
    tempEnList->next = NULL;
    return retEnList;
}
StrEntStats _initEntStats(int roundNum, StrFileVars fiVars) {
    StrEntStats retEntStats;
    retEntStats.enMovSpd = DISP_W / 1280 + roundNum * DISP_W / 1280;
    retEntStats.enBullSpd = -DISP_H / 240;
    retEntStats.enFireWaitMax = 32 - roundNum;
    retEntStats.enFireWaitNow = retEntStats.enFireWaitMax;
    retEntStats.plMovSpd = DISP_H / 240 + fiVars.movSpdUpg * DISP_W / 1280;
    retEntStats.plBullSpd = DISP_H / 120 + fiVars.bullSpdUpg * DISP_H / 960;
    retEntStats.plFireWaitMax = 19 - fiVars.attSpdUpg;
    retEntStats.plFireWaitNow = retEntStats.plFireWaitMax;
    retEntStats.plX = DISP_W / 2;
    retEntStats.plY = DISP_H * 9 / 10;
    retEntStats.wallAmnt = roundNum + 2;
    return retEntStats;
}
void _initFileVars(StrFileVars* fiVarsP, int rwFlag) {
    FILE* fileTest = NULL;
    fopen_s(&fileTest, "data.txt", "r");
    if (fileTest == NULL) {
        _initFileVarsWriteFirst();
        fiVarsP->attSpdUpg = 0, fiVarsP->bullSpdUpg = 0, fiVarsP->movSpdUpg = 0, fiVarsP->keyBindFlag = 0;
        fiVarsP->totalScore = 0;
        for (int i = 0; i < 8; i++) {
            fiVarsP->scoreTable[i] = 111 * (8 - i);
            for (int j = 0; j < 8; j++) {
                fiVarsP->nameTable[8 * i + j] = 'A' + i;
            }
        }
    }
    else if (rwFlag == 1) {
        fclose(fileTest);
        _initFileVarsWrite(*fiVarsP);
    }
    else {
        fclose(fileTest);
        _initFileVarsRead(fiVarsP);
    }
}
void _initFileVarsWrite(StrFileVars fiVars) {
    FILE* fileWrite = NULL;
    _initTest(!(fopen_s(&fileWrite, "data.txt", "w")), "data text");
    fprintf(fileWrite, "%d ", fiVars.movSpdUpg);
    fprintf(fileWrite, "%d ", fiVars.bullSpdUpg);
    fprintf(fileWrite, "%d ", fiVars.attSpdUpg);
    fprintf(fileWrite, "%ld ", fiVars.totalScore);
    fprintf(fileWrite, "%d ", fiVars.keyBindFlag);
    for (int i = 0; i < 8; i++) {
        fprintf(fileWrite, "%d ", fiVars.scoreTable[i]);
    }
    fprintf(fileWrite, "%s", fiVars.nameTable);
    fclose(fileWrite);
}
void _initFileVarsWriteFirst() {
    FILE* fileWrite = NULL;
    _initTest(!(fopen_s(&fileWrite, "data.txt", "w")), "data text");
    fprintf(fileWrite, "%d %d %d %ld %d ", 0, 0, 0, 0, 0);
    fprintf(fileWrite, "%d %d %d %d %d %d %d %d ", 888, 777, 666, 555, 444, 333, 222, 111);
    fprintf(fileWrite, "%s", "AAAAAAAABBBBBBBBCCCCCCCCDDDDDDDDEEEEEEEEFFFFFFFFGGGGGGGGHHHHHHHH");
    fclose(fileWrite);
}
void _initFileVarsRead(StrFileVars* fiVarsP) {
    FILE* fileRead = NULL;
    _initTest(!(fopen_s(&fileRead, "data.txt", "r")), "data text");
    fscanf_s(fileRead, "%d", &fiVarsP->movSpdUpg);
    fscanf_s(fileRead, "%d", &fiVarsP->bullSpdUpg);
    fscanf_s(fileRead, "%d", &fiVarsP->attSpdUpg);
    fscanf_s(fileRead, "%ld", &fiVarsP->totalScore);
    fscanf_s(fileRead, "%d", &fiVarsP->keyBindFlag);
    for (int i = 0; i < 8; i++) {
        fscanf_s(fileRead, "%d", &fiVarsP->scoreTable[i]);
    }
    fscanf_s(fileRead, "%s", &fiVarsP->nameTable, 65);
    fclose(fileRead);
}
EnScreenFlag _firstRound(StrEntStats* entStatsP, StrList** enListPP, StrList** plBullListPP, StrList** enBullListPP, StrRect** wallArrayP, StrRect* collBoxP, StrFileVars fiVars) {
    *enListPP = _freeList(*enListPP);
    *plBullListPP = _freeList(*plBullListPP);
    *enBullListPP = _freeList(*enBullListPP);
    *wallArrayP = _freeWallArr(*wallArrayP);
    *entStatsP = _initEntStats(1, fiVars);
    *wallArrayP = _initWallArr(entStatsP->wallAmnt);
    *enListPP = _initEnemyList();
    _collReposition(collBoxP, *enListPP);
    return play;
}
EnScreenFlag _nextRound(StrGaState* gaStateP, StrFileVars* fiVarsP) {
    gaStateP->roundNum++;
    gaStateP->currentScore += 500;
    gaStateP->timer = FRAMERATE * 3;
    fiVarsP->totalScore += gaStateP->currentScore;
    if (fiVarsP->totalScore > 999999999) {
        fiVarsP->totalScore = 999999999;
    }
    _initFileVars(fiVarsP, 1);
    return countdown;
}
StrList* _freeList(StrList* listP) {
    struct StrList* nextp = listP;
    struct StrList* prev = NULL;
    while (nextp != NULL)
    {
        prev = nextp;
        free(nextp);
        nextp = prev->next;
    }
    return NULL;
}
StrRect* _freeWallArr(StrRect* wallArrayP) {
    if (wallArrayP != NULL) {
        free(wallArrayP);
    }
    return NULL;
}
void _listDelete(StrList** prevListPP, StrList** currListPP) {
    StrList* temp = (*currListPP)->next;
    if (*prevListPP == NULL) {
        free(*currListPP);
        *currListPP = temp;
    }
    else {
        (*prevListPP)->next = temp;
        free(*currListPP);
        (*currListPP) = (*prevListPP)->next;
    }
}
void _ptrCheckNull(void* listP) {
    if (listP == NULL) {
        perror("Was assigned null as a pointer");
        exit(-1);
    }
}
void _logicPlayDraw(StrEntStats entStats, StrList* enListP, StrList* plBullListP, StrList* enBullListP, StrRect* wallArrayP, StrRect collBox, int score, int roundNum) {
    al_draw_bitmap(bmapBackground, 0, 0, 0);
    al_draw_textf(smallFont, WHITE, DISP_W / 25, DISP_H / 80, 0, "Score: %06d", score);
    al_draw_textf(smallFont, WHITE, DISP_W * 4 / 5, DISP_H / 80, 0, "Round: %d", roundNum);
    al_draw_bitmap_region(bmapSpriteMap, 0, 0, PLAYERWIDTH, PLAYERHEIGHT, entStats.plX, entStats.plY, 0);
    while (enListP != NULL) {
        al_draw_bitmap_region(bmapSpriteMap, PLAYERWIDTH, BULLETHEIGHT, ENEMYWIDTH, ENEMYHEIGHT, enListP->x, enListP->y, 0);
        enListP = enListP->next;
    }
    while (plBullListP != NULL) {
        al_draw_bitmap_region(bmapSpriteMap, PLAYERWIDTH, 0, BULLETWIDTH, BULLETHEIGHT, plBullListP->x, plBullListP->y, 0);
        plBullListP = plBullListP->next;
    }
    while (enBullListP != NULL) {
        al_draw_bitmap_region(bmapSpriteMap, PLAYERWIDTH + BULLETWIDTH, 0, BULLETWIDTH, BULLETHEIGHT, enBullListP->x, enBullListP->y, 0);
        enBullListP = enBullListP->next;
    }
    for (int i = 0; i < entStats.wallAmnt; i++) {
        al_draw_bitmap_region(bmapSpriteMap, 0, PLAYERHEIGHT, WALLWIDTH, WALLHEIGHT, wallArrayP[i].x1, wallArrayP[i].y1, 0);
    }
}
void _logicPlayMec(StrGaState* gaStateP, StrEntStats* entStatsP, StrList** enListPP, StrList** plBullListPP, StrList** enBullListPP, StrRect* wallArray, StrRect* collBoxP, StrFileVars* fiVarsP) {
    _bulletMovement(*plBullListPP, entStatsP->plBullSpd);
    _bulletMovement(*enBullListPP, entStatsP->enBullSpd);
    _enemyBehaviour(*enListPP, enBullListPP, collBoxP, entStatsP);
    if (gaStateP->timer == 0) {
        _playerBehaviour(entStatsP, plBullListPP, fiVarsP->keyBindFlag);
    }
    _playerBulletsCollision(enListPP, plBullListPP, wallArray, *collBoxP, entStatsP->wallAmnt, &(gaStateP->currentScore));
    if (gaStateP->currentScore > 999999) {
        gaStateP->currentScore = 999999;
    }
    _collReposition(collBoxP, *enListPP);
    entStatsP->plFireWaitNow--;
    entStatsP->enFireWaitNow--;
    if (collBoxP->y2 >= entStatsP->plY || _enemyBulletsCollision(*entStatsP, enBullListPP, wallArray)) {
        fiVarsP->totalScore += gaStateP->currentScore;
        if (fiVarsP->totalScore > 999999999) {
            fiVarsP->totalScore = 999999999;
        }
        *enListPP = _freeList(*enListPP);
        gaStateP->screenFlag = scores;
    }
    if (keyPressed[ALLEGRO_KEY_ESCAPE] && gaStateP->timer == 0) {
        gaStateP->pauseMenuFlag = 1;
    }
}
void _enemyBehaviour(StrList* enListP, StrList** enBullListPP, StrRect* collBoxP, StrEntStats* entStatsP) {
    StrList* tempEnList = enListP;
    StrList* copyEnList = enListP;
    while (copyEnList != NULL) {
        copyEnList->x += entStatsP->enMovSpd;
        if (!(rand() % 144) && entStatsP->enFireWaitNow < 0) {
            _bulletCreate(enBullListPP, copyEnList->x + (ENEMYWIDTH - BULLETWIDTH) / 2, copyEnList->y + ENEMYHEIGHT);
            al_play_sample(sndEnemyLaser, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
            entStatsP->enFireWaitNow = entStatsP->enFireWaitMax;
        }
        copyEnList = copyEnList->next;
    }
    collBoxP->x1 += entStatsP->enMovSpd;
    collBoxP->x2 += entStatsP->enMovSpd;
    if (collBoxP->x1 < DISP_W / 40 || collBoxP->x2 > DISP_W * 39 / 40) {
        while (tempEnList != NULL) {
            tempEnList->y += DISP_H / 20;
            tempEnList = tempEnList->next;
        }
        collBoxP->y1 += DISP_H / 20;
        collBoxP->y2 += DISP_H / 20;
        entStatsP->enMovSpd *= -1;
    }
}
void _playerBehaviour(StrEntStats* entStatsP, StrList** plBullListPP, int kBindFlag) {
    if ((keyPressed[ALLEGRO_KEY_LEFT]) && kBindFlag || (keyPressed[ALLEGRO_KEY_A] && !kBindFlag)) {
        if (entStatsP->plX < -DISP_W / 20) {
            entStatsP->plX = DISP_W + DISP_W / 256;
        }
        entStatsP->plX -= entStatsP->plMovSpd;
    }
    if ((keyPressed[ALLEGRO_KEY_RIGHT] && kBindFlag) || (keyPressed[ALLEGRO_KEY_D] && !kBindFlag)) {
        if (entStatsP->plX > DISP_W + DISP_W / 128) {
            entStatsP->plX = -DISP_W / 20 + DISP_W / 256;
        }
        entStatsP->plX += entStatsP->plMovSpd;
    }
    if (keyPressed[ALLEGRO_KEY_SPACE] && entStatsP->plFireWaitNow < 1) {
        _bulletCreate(plBullListPP, entStatsP->plX + (PLAYERWIDTH - BULLETWIDTH) / 2, entStatsP->plY - BULLETHEIGHT);
        al_play_sample(sndLaser, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        entStatsP->plFireWaitNow = entStatsP->plFireWaitMax;
    }
}
void _bulletCreate(StrList** BullListPP, int x, int y) {
    StrList* temp = *BullListPP;
    StrList* prev = NULL;
    while (temp != NULL) {
        prev = temp;
        temp = temp->next;
    }
    temp = (StrList*)malloc(sizeof(StrList));
    _ptrCheckNull(temp);
    temp->x = x;
    temp->y = y;
    temp->next = NULL;
    if (*BullListPP == NULL) {
        *BullListPP = temp;
    }
    else {
        prev->next = temp;
    }
}
void _bulletMovement(StrList* BullListP, int bulletSpeed) {
    while (BullListP != NULL) {
        BullListP->y -= bulletSpeed;
        BullListP = BullListP->next;
    }
}
void _playerBulletsCollision(StrList** enListPP, StrList** plBullListPP, StrRect* wallArray, StrRect collBox, int wallAmnt, int* scoreP) {
    StrList* tempBull = *plBullListPP, * prevBull = NULL;
    StrList* tempEnemy = *enListPP;
    while (tempBull != NULL) {
        if (_playerBulletsCollisionCheck(&tempBull, &prevBull, wallArray, &tempEnemy, collBox, wallAmnt, scoreP) == NULL) {
            *enListPP = tempEnemy;
        }
        if (prevBull == NULL) {
            *plBullListPP = tempBull;
        }
    }
}
StrList* _playerBulletsCollisionCheck(StrList** tempBullPP, StrList** prevBullPP, StrRect* wallArray, StrList** tempEnemyPP, StrRect collBox, int wallAmnt, int* scoreP) {
    StrList* prevEnemy = NULL;
    if ((*tempBullPP)->y < DISP_H / 192) {
        _listDelete(prevBullPP, tempBullPP);
        return;
    }
    for (int i = 0; i < wallAmnt; i++) {
        if (_checkCollision((*tempBullPP)->x, (*tempBullPP)->y, (*tempBullPP)->x + BULLETWIDTH, (*tempBullPP)->y + BULLETHEIGHT, wallArray[i].x1, wallArray[i].y1, wallArray[i].x2, wallArray[i].y2)) {
            al_play_sample(sndHitHurt, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
            _listDelete(prevBullPP, tempBullPP);
            return;
        }
    }
    if (!(_checkCollision((*tempBullPP)->x, (*tempBullPP)->y, (*tempBullPP)->x + BULLETWIDTH, (*tempBullPP)->y + BULLETHEIGHT, collBox.x1, collBox.y1, collBox.x2, collBox.y2))) {
        *prevBullPP = *tempBullPP;
        *tempBullPP = (*tempBullPP)->next;
        return;
    }
    while (*tempEnemyPP != NULL) {
        if (_checkCollision((*tempBullPP)->x, (*tempBullPP)->y, (*tempBullPP)->x + BULLETWIDTH, (*tempBullPP)->y + BULLETHEIGHT, (*tempEnemyPP)->x, (*tempEnemyPP)->y, (*tempEnemyPP)->x + ENEMYWIDTH, (*tempEnemyPP)->y + ENEMYHEIGHT)) {
            al_play_sample(sndHitHurt, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
            _listDelete(&prevEnemy, tempEnemyPP);
            _listDelete(prevBullPP, tempBullPP);
            *scoreP += 15;
            return prevEnemy;
        }
        prevEnemy = *tempEnemyPP;
        *tempEnemyPP = (*tempEnemyPP)->next;
    }
    *prevBullPP = *tempBullPP;
    *tempBullPP = (*tempBullPP)->next;
}
bool _enemyBulletsCollision(StrEntStats entStats, StrList** enBullListPP, StrRect* wallArray) {
    StrList* tempBull = *enBullListPP, * prevBull = NULL;
    while (tempBull != NULL) {
        if (_enemyBulletsCollisionCheck(entStats, &tempBull, &prevBull, wallArray)) {
            return true;
        }
        if (prevBull == NULL) {
            *enBullListPP = tempBull;
        }
    }
    return false;
}
bool _enemyBulletsCollisionCheck(StrEntStats entStats, StrList** tempBullPP, StrList** prevBullPP, StrRect* wallArray) {
    if ((*tempBullPP)->y > entStats.plY - PLAYERHEIGHT) {
        if (_checkCollision(entStats.plX, entStats.plY, entStats.plX + PLAYERWIDTH, entStats.plY + PLAYERHEIGHT, (*tempBullPP)->x, (*tempBullPP)->y, (*tempBullPP)->x + BULLETWIDTH, (*tempBullPP)->y + BULLETHEIGHT)) {
            return true;
        }
    }
    if ((*tempBullPP)->y > DISP_H - DISP_H / 64) {
        _listDelete(prevBullPP, tempBullPP);
        return false;
    }
    for (int i = 0; i < entStats.wallAmnt; i++) {
        if (_checkCollision((*tempBullPP)->x, (*tempBullPP)->y, (*tempBullPP)->x + BULLETWIDTH, (*tempBullPP)->y + BULLETHEIGHT, wallArray[i].x1, wallArray[i].y1, wallArray[i].x2, wallArray[i].y2)) {
            _listDelete(prevBullPP, tempBullPP);
            al_play_sample(sndHitHurt, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
            return false;
        }
    }
    *prevBullPP = *tempBullPP;
    *tempBullPP = (*tempBullPP)->next;
    return false;
}
bool _checkCollision(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2) {
    if (ax1 > bx2) {
        return false;
    }
    if (ax2 < bx1) {
        return false;
    }
    if (ay1 > by2) {
        return false;
    }
    if (ay2 < by1) {
        return false;
    }
    return true;
}
void _collReposition(StrRect* collBoxP, StrList* enListP) {
    struct StrList* tempColl = enListP;
    if (tempColl == NULL) {
        return;
    }
    int minx = tempColl->x, maxx = tempColl->x + ENEMYWIDTH, miny = tempColl->y, maxy = tempColl->y + ENEMYHEIGHT;
    while (tempColl != NULL) {
        if (minx > tempColl->x) {
            minx = tempColl->x;
        }
        else if (maxx < tempColl->x + ENEMYWIDTH) {
            maxx = tempColl->x + ENEMYWIDTH;
        }
        if (miny > tempColl->y) {
            miny = tempColl->y;
        }
        else if (maxy < tempColl->y + ENEMYHEIGHT) {
            maxy = tempColl->y + ENEMYHEIGHT;
        }
        tempColl = tempColl->next;
    }
    collBoxP->x1 = minx;
    collBoxP->x2 = maxx;
    collBoxP->y1 = miny;
    collBoxP->y2 = maxy;
}
void _logicCountdown(StrGaState* gaStateP) {
    al_draw_textf(font, WHITE, DISP_W / 2, DISP_H / 3, ALLEGRO_ALIGN_CENTRE, "Round %d", gaStateP->roundNum);
    if (gaStateP->timer == 0) {
        gaStateP->screenFlag = initiation;
        return;
    }
    int num = gaStateP->timer / FRAMERATE + 1;
    al_draw_textf(font, WHITE, DISP_W / 2, DISP_H / 2, ALLEGRO_ALIGN_CENTRE, "%d", num);
}
void _logicGameover(StrGaState* gaStateP) {
    _drawButtons(3, gaStateP->menuCount, 0);
    al_draw_text(font, WHITE, DISP_W / 2, BASEBOXY, ALLEGRO_ALIGN_CENTRE, "Restart");
    al_draw_text(font, WHITE, DISP_W / 2, BASEBOXY + BOXOFFSETY, ALLEGRO_ALIGN_CENTRE, "Menu");
    al_draw_text(font, WHITE, DISP_W / 2, BASEBOXY + BOXOFFSETY * 2, ALLEGRO_ALIGN_CENTRE, "Exit");
    if (gaStateP->timer == 0) {
        _menuScroll(gaStateP, 3);
        switch (gaStateP->menuCount) {
        case 0:
            if (keyPressed[ALLEGRO_KEY_ENTER]) {
                al_play_sample(sndMenuSelect, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                *gaStateP = _initGameState();
                gaStateP->screenFlag = countdown;
                gaStateP->timer = FRAMERATE * 3;
            }
            break;
        case 1:
            if (keyPressed[ALLEGRO_KEY_ENTER]) {
                al_play_sample(sndMenuSelect, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                *gaStateP = _initGameState();
            }
            break;
        case 2:
            if (keyPressed[ALLEGRO_KEY_ENTER]) {
                gaStateP->quitProg = true;
            }
            break;
        }
    }
}
void _logicPause(StrGaState* gaStateP, StrList** enListPP) {
    _drawButtons(3, gaStateP->menuCount, 0);
    al_draw_textf(font, WHITE, DISP_W / 2, BASEBOXY / 10, ALLEGRO_ALIGN_CENTRE, "Paused");
    al_draw_text(font, WHITE, DISP_W / 2, BASEBOXY, ALLEGRO_ALIGN_CENTRE, "Unpause");
    al_draw_text(font, WHITE, DISP_W / 2, BASEBOXY + BOXOFFSETY, ALLEGRO_ALIGN_CENTRE, "Menu");
    al_draw_text(font, WHITE, DISP_W / 2, BASEBOXY + BOXOFFSETY * 2, ALLEGRO_ALIGN_CENTRE, "Exit");
    if (gaStateP->timer == 0) {
        _menuScroll(gaStateP, 3);
        switch (gaStateP->menuCount) {
        case 0:
            if (keyPressed[ALLEGRO_KEY_ENTER]) {
                al_play_sample(sndMenuSelect, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                gaStateP->pauseMenuFlag = 0;
                gaStateP->timer = 3;
            }
            break;
        case 1:
            if (keyPressed[ALLEGRO_KEY_ENTER]) {
                al_play_sample(sndMenuSelect, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                *enListPP = _freeList(*enListPP);
                *gaStateP = _initGameState();
            }
            break;
        case 2:
            if (keyPressed[ALLEGRO_KEY_ENTER]) {
                gaStateP->quitProg = true;
            }
            break;
        }
    }
}
void _logicStart(StrGaState* gaStateP) {
    _drawButtons(4, gaStateP->menuCount, 0);
    al_draw_text(font, WHITE, DISP_W / 2, BASEBOXY, ALLEGRO_ALIGN_CENTRE, "Start");
    al_draw_text(font, WHITE, DISP_W / 2, BASEBOXY + BOXOFFSETY, ALLEGRO_ALIGN_CENTRE, "Shop");
    al_draw_text(font, WHITE, DISP_W / 2, BASEBOXY + BOXOFFSETY * 2, ALLEGRO_ALIGN_CENTRE, "Other");
    al_draw_text(font, WHITE, DISP_W / 2, BASEBOXY + BOXOFFSETY * 3, ALLEGRO_ALIGN_CENTRE, "Exit");
    if (gaStateP->timer == 0) {
        _menuScroll(gaStateP, 4);
        switch (gaStateP->menuCount) {
        case 0:
            if (keyPressed[ALLEGRO_KEY_ENTER]) {
                al_play_sample(sndMenuSelect, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                gaStateP->screenFlag = countdown;
                gaStateP->timer = FRAMERATE * 3;
            }
            break;
        case 1:
            if (keyPressed[ALLEGRO_KEY_ENTER]) {
                al_play_sample(sndMenuSelect, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                gaStateP->screenFlag = shop;
                gaStateP->timer = 5;
                gaStateP->menuCount = 0;
            }
            break;
        case 2:
            if (keyPressed[ALLEGRO_KEY_ENTER]) {
                al_play_sample(sndMenuSelect, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                gaStateP->screenFlag = other;
                gaStateP->timer = 20;
                gaStateP->menuCount = 0;
            }
            break;
        case 3:
            if (keyPressed[ALLEGRO_KEY_ENTER]) {
                gaStateP->quitProg = true;
            }
            break;
        }
    }
}
void _logicShop(StrFileVars* fiVarsP, StrGaState* gaStateP) {
    al_draw_textf(smallFont, WHITE, DISP_W / 32, 0, 0, "Currency left: %ld", fiVarsP->totalScore);
    _drawButtons(4, gaStateP->menuCount, 100);
    al_draw_text(font, WHITE, DISP_W / 2, BASEBOXY, ALLEGRO_ALIGN_CENTRE, "Move Speed");
    al_draw_text(font, WHITE, DISP_W / 2, BASEBOXY + BOXOFFSETY, ALLEGRO_ALIGN_CENTRE, "Bullet Speed");
    al_draw_text(font, WHITE, DISP_W / 2, BASEBOXY + BOXOFFSETY * 2, ALLEGRO_ALIGN_CENTRE, "Attack Speed");
    al_draw_text(font, WHITE, DISP_W / 2, BASEBOXY + BOXOFFSETY * 3, ALLEGRO_ALIGN_CENTRE, "Menu");
    switch (gaStateP->menuCount) {
    case 0:
        if (fiVarsP->movSpdUpg < 3) {
            al_draw_textf(smallFont, WHITE, DISP_W * 13 / 16, 0, ALLEGRO_ALIGN_CENTER, "Upgrade Cost: %ld", 500 + fiVarsP->movSpdUpg * 250);
        }
        else if (fiVarsP->movSpdUpg == 3) {
            al_draw_text(smallFont, WHITE, DISP_W * 13 / 16, 0, ALLEGRO_ALIGN_CENTER, "Fully Upgraded");
        }
        al_draw_textf(smallFont, WHITE, DISP_W / 2, DISP_W / 32, ALLEGRO_ALIGN_CENTRE, "%d/3", fiVarsP->movSpdUpg);
        break;
    case 1:
        if (fiVarsP->bullSpdUpg < 3) {
            al_draw_textf(smallFont, WHITE, DISP_W * 13 / 16, 0, ALLEGRO_ALIGN_CENTER, "Upgrade Cost: %ld", 500 + fiVarsP->bullSpdUpg * 250);
        }
        else if (fiVarsP->bullSpdUpg == 3) {
            al_draw_text(smallFont, WHITE, DISP_W * 13 / 16, 0, ALLEGRO_ALIGN_CENTER, "Fully Upgraded");
        }
        al_draw_textf(smallFont, WHITE, DISP_W / 2, DISP_W / 32, ALLEGRO_ALIGN_CENTRE, "%d/3", fiVarsP->bullSpdUpg);
        break;
    case 2:
        if (fiVarsP->attSpdUpg < 3) {
            al_draw_textf(smallFont, WHITE, DISP_W * 13 / 16, 0, ALLEGRO_ALIGN_CENTER, "Upgrade Cost: %ld", 500 + fiVarsP->attSpdUpg * 250);
        }
        else if (fiVarsP->attSpdUpg == 3) {
            al_draw_text(smallFont, WHITE, DISP_W * 13 / 16, 0, ALLEGRO_ALIGN_CENTER, "Fully Upgraded");
        }
        al_draw_textf(smallFont, WHITE, DISP_W / 2, DISP_W / 32, ALLEGRO_ALIGN_CENTRE, "%d/3", fiVarsP->attSpdUpg);
        break;
    }
    if (gaStateP->timer == 0) {
        _menuScroll(gaStateP, 4);
        switch (gaStateP->menuCount) {
        case 0:
            if (keyPressed[ALLEGRO_KEY_ENTER] && fiVarsP->movSpdUpg < 3) {
                al_play_sample(sndMenuSelect, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                if (fiVarsP->totalScore >= 500 + fiVarsP->movSpdUpg * 250) {
                    fiVarsP->totalScore -= 500 + fiVarsP->movSpdUpg * 250;
                    fiVarsP->movSpdUpg++;
                }
                gaStateP->timer = 10;
            }
            break;
        case 1:
            if (keyPressed[ALLEGRO_KEY_ENTER] && fiVarsP->bullSpdUpg < 3) {
                al_play_sample(sndMenuSelect, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                if (fiVarsP->totalScore >= 500 + fiVarsP->bullSpdUpg * 250) {
                    fiVarsP->totalScore -= 500 + fiVarsP->bullSpdUpg * 250;
                    fiVarsP->bullSpdUpg++;
                }
                gaStateP->timer = 10;
            }
            break;
        case 2:
            if (keyPressed[ALLEGRO_KEY_ENTER] && fiVarsP->attSpdUpg < 3) {
                al_play_sample(sndMenuSelect, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                if (fiVarsP->totalScore >= 500 + fiVarsP->attSpdUpg * 250) {
                    fiVarsP->totalScore -= 500 + fiVarsP->attSpdUpg * 250;
                    fiVarsP->attSpdUpg++;
                }
                gaStateP->timer = 10;
            }
            break;
        case 3:
            if (keyPressed[ALLEGRO_KEY_ENTER]) {
                al_play_sample(sndMenuSelect, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                *gaStateP = _initGameState();
                _initFileVars(fiVarsP, 1);
            }
            break;
        }
    }
}
void _logicOther(StrFileVars* fiVarsP, StrGaState* gaStateP) {
    al_draw_text(smallFont, WHITE, DISP_W * 3 / 4, 0, ALLEGRO_ALIGN_CENTRE, "Score table:");
    for (int i = 0; i < 8; i++) {
        al_draw_textf(smallFont, WHITE, (DISP_W - al_get_text_width(smallFont, "Score table")) * 3 / 4 + al_get_text_width(smallFont, "AAAAAAAA"), DISP_H / 10 + DISP_H / 12 * i, 0, ":%06d", fiVarsP->scoreTable[i]);
        for (int j = 0; j < 8; j++) {
            al_draw_textf(smallFont, WHITE, (DISP_W - al_get_text_width(smallFont, "Score table")) * 3 / 4 + al_get_text_width(smallFont, "A") * j, DISP_H / 10 + DISP_H / 12 * i, 0, "%c", fiVarsP->nameTable[i * 8 + j]);
        }
    }
    al_draw_text(smallFont, WHITE, DISP_W / 4, 0, ALLEGRO_ALIGN_CENTRE, "Change keybindings:");
    al_draw_text(smallFont, WHITE, DISP_W / 4, DISP_H / 10, ALLEGRO_ALIGN_CENTRE, "A+D+Space");
    al_draw_text(smallFont, WHITE, DISP_W / 4, DISP_H / 10 * 2, ALLEGRO_ALIGN_CENTRE, "Arrow keys + Space");
    al_draw_text(smallFont, WHITE, DISP_W / 2, DISP_H / 10 * (fiVarsP->keyBindFlag + 1), ALLEGRO_ALIGN_CENTRE, "X");
    al_draw_rectangle(BASEBOXX, DISP_H * 13 / 16, BASEBOXX + BOXOFFSETX, DISP_H * 13 / 16 + FONTOFFSETY, WHITE, BOXTHICKNESS);
    al_draw_text(font, WHITE, DISP_W / 2, DISP_H * 13 / 16, ALLEGRO_ALIGN_CENTRE, "Menu");
    if (gaStateP->timer == 0) {
        _menuScroll(gaStateP, 3);
        switch (gaStateP->menuCount) {
        case 0:
            if (keyPressed[ALLEGRO_KEY_ENTER]) {
                al_play_sample(sndMenuSelect, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                fiVarsP->keyBindFlag = 0;
                gaStateP->timer = 20;
            }
            al_draw_line(DISP_W / 4 - al_get_text_width(smallFont, "Arrow keys + Space") / 2, DISP_H / 10 + DISP_H / 11, DISP_W / 4 + al_get_text_width(smallFont, "Arrow keys + Space") / 2, DISP_H / 10 + DISP_H / 11, WHITE, BOXTHICKNESS);
            break;
        case 1:
            if (keyPressed[ALLEGRO_KEY_ENTER]) {
                al_play_sample(sndMenuSelect, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                fiVarsP->keyBindFlag = 1;
                gaStateP->timer = 20;
            }
            al_draw_line(DISP_W / 4 - al_get_text_width(smallFont, "Arrow keys + Space") / 2, DISP_H * 2 / 10 + DISP_H / 11, DISP_W / 4 + al_get_text_width(smallFont, "Arrow keys + Space") / 2, DISP_H * 2 / 10 + DISP_H / 11, WHITE, BOXTHICKNESS);
            break;
        case 2:
            if (keyPressed[ALLEGRO_KEY_ENTER]) {
                al_play_sample(sndMenuSelect, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                *gaStateP = _initGameState();
                _initFileVars(fiVarsP, 1);
            }
            al_draw_rectangle(BASEBOXX, DISP_H * 13 / 16, BASEBOXX + BOXOFFSETX, DISP_H * 13 / 16 + FONTOFFSETY, YELLOW, BOXTHICKNESS);
            break;
        }
    }
}
void _logicScores(StrFileVars* fiVarsP, StrGaState* gaStateP) {
    static int place = -1;
    static int index = 0;
    al_draw_text(smallFont, WHITE, DISP_W / 2, 0, ALLEGRO_ALIGN_CENTER, "Score Table:");
    for (int i = 0; i < 8; i++) {
        al_draw_textf(smallFont, WHITE, (DISP_W - al_get_text_width(smallFont, "Score table:") - al_get_text_width(smallFont, "AAAAAAAA") / 2) / 2 + al_get_text_width(smallFont, "AAAAAAAA"), DISP_H / 12 * (i + 1), 0, ":%06d", fiVarsP->scoreTable[i]);
        for (int j = 0; j < 8; j++) {
            al_draw_textf(smallFont, WHITE, (DISP_W - al_get_text_width(smallFont, "Score table:") - al_get_text_width(smallFont, "AAAAAAAA") / 2) / 2 + al_get_text_width(smallFont, "A") * j, DISP_H / 12 * (i + 1), 0, "%c", fiVarsP->nameTable[i * 8 + j]);
        }
        if ((gaStateP->currentScore >= fiVarsP->scoreTable[i]) && place == -1) {
            place = i;
        }
    }
    if (place != -1) {
        fiVarsP->scoreTable[place] = gaStateP->currentScore;
        if (!(gaStateP->timer)) {
            if (keyPressed[ALLEGRO_KEY_DOWN] && fiVarsP->nameTable[place * 8 + index] > 'A') {
                al_play_sample(sndMenuChange, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                fiVarsP->nameTable[place * 8 + index]--;
                gaStateP->timer = 5;
            }
            if (keyPressed[ALLEGRO_KEY_UP] && fiVarsP->nameTable[place * 8 + index] < 'Z') {
                al_play_sample(sndMenuChange, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                fiVarsP->nameTable[place * 8 + index]++;
                gaStateP->timer = 5;
            }
            if (keyPressed[ALLEGRO_KEY_LEFT] && index > 0) {
                al_play_sample(sndMenuChange, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                index--;
                gaStateP->timer = 10;
            }
            if (keyPressed[ALLEGRO_KEY_RIGHT] && index < 7) {
                al_play_sample(sndMenuChange, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
                index++;
                gaStateP->timer = 10;
            }
        }
        al_draw_line((DISP_W - al_get_text_width(smallFont, "Score table:") - al_get_text_width(smallFont, "AAAAAAAA") / 2) / 2 + al_get_text_width(smallFont, "A") * index, DISP_H / 12 * (place + 2), (DISP_W - al_get_text_width(smallFont, "Score table:") - al_get_text_width(smallFont, "AAAAAAAA") / 2) / 2 + al_get_text_width(smallFont, "A") * (index + 1), DISP_H / 12 * (place + 2), WHITE, BOXTHICKNESS);
        al_draw_text(smallFont, WHITE, DISP_W / 2, DISP_H * 4 / 5, ALLEGRO_ALIGN_CENTER, "Congratulations!");
        al_draw_text(smallFont, WHITE, DISP_W / 2, DISP_H * 13 / 15, ALLEGRO_ALIGN_CENTER, "Enter your name and then press enter");
    }
    else {
        al_draw_text(smallFont, WHITE, DISP_W / 2, DISP_H * 13 / 15, ALLEGRO_ALIGN_CENTER, "Press Enter");
    }
    if (keyPressed[ALLEGRO_KEY_ENTER]) {
        al_play_sample(sndMenuSelect, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        place = -1;
        gaStateP->screenFlag = gameover;
        gaStateP->timer = 20;
        _initFileVars(fiVarsP, 1);
    }
}
void _menuScroll(StrGaState* gaStateP, int menu_length) {
    if (keyPressed[ALLEGRO_KEY_DOWN]) {
        al_play_sample(sndMenuChange, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        gaStateP->menuCount++;
        gaStateP->menuCount %= menu_length;
        gaStateP->timer = 6;
    }
    if (keyPressed[ALLEGRO_KEY_UP]) {
        al_play_sample(sndMenuChange, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        gaStateP->menuCount--;
        gaStateP->menuCount %= menu_length;
        if (gaStateP->menuCount == -1) {
            gaStateP->menuCount = menu_length - 1;
        }
        gaStateP->timer = 6;
    }
}
void _drawButtons(int number, int menuCount, int addSize) {
    for (int i = 0; i < number; i++) {
        al_draw_rectangle(BASEBOXX - addSize, BASEBOXY + BOXOFFSETY * i, BASEBOXX + BOXOFFSETX + addSize, BASEBOXY + FONTOFFSETY + BOXOFFSETY * i, WHITE, BOXTHICKNESS);
    }
    al_draw_rectangle(BASEBOXX - addSize, BASEBOXY + menuCount * BOXOFFSETY, BASEBOXX + BOXOFFSETX + addSize, BASEBOXY + menuCount * BOXOFFSETY + FONTOFFSETY, YELLOW, BOXTHICKNESS);
}
void _eventHandler(bool* reDraw, bool* quitProg) {
    al_wait_for_event(allQueue, &allEvent);
    switch (allEvent.type) {
    case ALLEGRO_EVENT_TIMER:
        *reDraw = true;
        for (int i = 0; i < ALLEGRO_KEY_MAX; i++) {
            keyPressed[i] &= KEY_USED;
        }
        break;
    case ALLEGRO_EVENT_KEY_DOWN:
        keyPressed[allEvent.keyboard.keycode] = KEY_USED | KEY_RELEASED;
        break;
    case ALLEGRO_EVENT_KEY_UP:
        keyPressed[allEvent.keyboard.keycode] &= KEY_RELEASED;
        break;
    case ALLEGRO_EVENT_DISPLAY_CLOSE:
        *quitProg = true;
        break;
    }
}
void _logic(StrGaState* gaStateP, StrEntStats* entStatsP, StrList** enListPP, StrList** plBullListPP, StrList** enBullListPP, StrRect** wallArrayP, StrRect* collBoxP, StrFileVars* fiVarsP) {
    al_set_target_bitmap(allBuffer);
    al_clear_to_color(al_map_rgb(0, 0, 0));
    if (gaStateP->pauseMenuFlag != 1) {
        switch (gaStateP->screenFlag) {
        case play:
            _logicPlayDraw(*entStatsP, *enListPP, *plBullListPP, *enBullListPP, *wallArrayP, *collBoxP, gaStateP->currentScore, gaStateP->roundNum);
            _logicPlayMec(gaStateP, entStatsP, enListPP, plBullListPP, enBullListPP, *wallArrayP, collBoxP, fiVarsP);
            break;
        case countdown:
            _logicCountdown(gaStateP);
            break;
        case start:
            _logicStart(gaStateP);
            break;
        case gameover:
            _logicGameover(gaStateP, enListPP);
            break;
        case shop:
            _logicShop(fiVarsP, gaStateP);
            break;
        case other:
            _logicOther(fiVarsP, gaStateP);
            break;
        case scores:
            _logicScores(fiVarsP, gaStateP);
        }
    }
    else {
        _logicPause(gaStateP, enListPP);
    }
    if (gaStateP->timer > 0) {
        gaStateP->timer--;
    }
    al_set_target_backbuffer(allDisplay);
    al_draw_scaled_bitmap(allBuffer, 0, 0, DISP_W, DISP_H, 0, 0, DISP_W, DISP_H, 0);
    al_flip_display();
    gaStateP->reDraw = false;
}

















