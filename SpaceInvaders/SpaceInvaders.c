#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <time.h>

#define DISP_W 1280
#define DISP_H 960
#define KEY_USED   1
#define KEY_RELEASED 2

enum screenflag {
    start = 0,
    play,
    countdown,
    initiation,
    gameover
};

typedef struct playerstruct {
    int x, y, wait, bulletspeed, movementspeed;
} playerstruct;

typedef struct liststruct {
    int x, y;
    struct liststruct* next;
} liststruct;

typedef struct enemyhead {
    int enemycount, enemyperrow, enemyspeed, bulletspeed;
    struct liststruct* start;
} enemyhead;

typedef struct bullethead {
    struct liststruct* start;
} bullethead;

typedef struct boundingbox {
    int x1, x2, y1, y2;
} boundingbox;

typedef struct gamestate {
    bool quitprog, redraw;
    long score, timing, frames;
    unsigned char round;
    int flag;
} gamestate;

void destroyall();
void pbullets_collision(bullethead* head, enemyhead* enemyhead, gamestate* state, boundingbox enemycol);
bool ebullets_collision(bullethead* ebhead, playerstruct player);
bool checkcollide(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2);
void checkptrnull(liststruct* head);
void state_handle_event(gamestate* state, playerstruct* player, enemyhead* ehead, boundingbox* enemycol, bullethead* bhead, bullethead* ebhead);
void state_exit(enemyhead* ehead, bullethead* bhead, bullethead* ebhead);
void state_draw(gamestate* state, playerstruct* player, enemyhead* ehead, boundingbox* enemycol, bullethead* bhead, bullethead* ebhead);
void state_draw_gameover(gamestate* state, enemyhead* head);
void state_draw_start(gamestate* state);
void state_draw_countdown(gamestate* state);
void state_draw_play(gamestate* state, playerstruct* player, enemyhead* ehead, boundingbox* enemycol, bullethead* bhead, bullethead* ebhead);
void state_init_initiation(gamestate* state, playerstruct* player, enemyhead* ehead, boundingbox* enemycol, bullethead* bhead, bullethead *ebhead);
void state_init_play(gamestate* state);
void state_gamelogic(gamestate* state, playerstruct* player, enemyhead* ehead, boundingbox* enemycol, bullethead* bhead, bullethead* ebhead);
void state_keyboard_update();
void bullet_movement(bullethead* ptr, int bulletspeed);
void bullet_deletefirst(bullethead* ptr);
void bullet_freeptrlist(bullethead* ptr);
void bullet_draw(bullethead bhead, bullethead ebhead);
void init_test(bool test, const char* description);;
void init_all();
void init_enemy(enemyhead* ptr, boundingbox* enemycol, gamestate state);
void init_state(gamestate* state);
void init_player(playerstruct* player);
void enemy_deletefirst(enemyhead* ptr);
void enemy_draw(enemyhead ptr);
void enemy_movement(boundingbox* box, enemyhead* ptr, bullethead* ebhead);
void enemy_shoot(bullethead* ebhead, int x, int y);
void enemy_freeptrlist(enemyhead* ptr);
void enemy_enemycol_check(boundingbox* enemycol, enemyhead headenemylist);
void player_movement(playerstruct* player, bullethead* ptr);
void player_draw(playerstruct* playerstruct);
void player_shoot(bullethead* ptr, playerstruct player);

ALLEGRO_DISPLAY* display;
ALLEGRO_BITMAP* buffer;
ALLEGRO_FONT* font;
ALLEGRO_FONT* smallfont;
ALLEGRO_TIMER* timer;
ALLEGRO_EVENT_QUEUE* queue;
ALLEGRO_BITMAP* playerimg;
ALLEGRO_BITMAP* enemybullet;
ALLEGRO_BITMAP* playerbullet;
ALLEGRO_BITMAP* enemyimg;
ALLEGRO_EVENT event;
unsigned char key[ALLEGRO_KEY_MAX];

int main() {
    srand(time(0));
    enemyhead headenemylist; 
    bullethead playerbulletlist;
    bullethead enemybulletlist;
    boundingbox enemycol;
    playerstruct player;
    gamestate state;
    headenemylist.start = NULL; 
    playerbulletlist.start = NULL; 
    enemybulletlist.start = NULL;
    memset(key, 0, sizeof(key));
    init_all();
    init_state(&state);
    while (true) {
        if (headenemylist.start == NULL) {
            if (state.flag == initiation) {
                state_init_initiation(&state, &player, &headenemylist, &enemycol, &playerbulletlist, &enemybulletlist);
            }
            else if (state.flag == play) {
                state_init_play(&state);
            }
        }
        state_handle_event(&state, &player, &headenemylist, &enemycol, &playerbulletlist, &enemybulletlist);
        if (key[ALLEGRO_KEY_ESCAPE] || state.quitprog) {
            state_exit(&headenemylist, &playerbulletlist, &enemybulletlist);
            break;
        }
        state_keyboard_update(&event, key);
		if (state.redraw && al_is_event_queue_empty(queue))
		{
            state_draw(&state, &player, &headenemylist, &enemycol, &playerbulletlist, &enemybulletlist);
        }       
    }
    destroyall();
}

void state_handle_event(gamestate* state, playerstruct* player, enemyhead* ehead, boundingbox* enemycol, bullethead* bhead, bullethead* ebhead) {
    al_wait_for_event(queue, &event);
    switch (event.type) {
        case ALLEGRO_EVENT_TIMER:
            (*state).redraw = true;
            if ((*state).flag == play) {
                state_gamelogic(state, player, ehead, enemycol, bhead, ebhead);
            }
            break;
        case ALLEGRO_EVENT_KEY_DOWN:
            key[event.keyboard.keycode] = KEY_USED | KEY_RELEASED;
            break;
        case ALLEGRO_EVENT_KEY_UP:
            key[event.keyboard.keycode] &= KEY_RELEASED;
            break;
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            (*state).quitprog = true;
            break;
    }
}

void destroyall() {
    al_destroy_bitmap(playerimg);
    al_destroy_bitmap(enemyimg);
    al_destroy_bitmap(enemybullet);
    al_destroy_bitmap(playerbullet);
    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_event_queue(queue);
    al_destroy_timer(timer);
    al_destroy_bitmap(buffer);
}

void pbullets_collision(bullethead* head, enemyhead* enemyhead, gamestate* state, boundingbox enemycol) {
    liststruct* enemy = (*enemyhead).start, * tempenemy = NULL, * prevenemy = NULL;
    liststruct* tempbullet = (*head).start, * prev = NULL;
    int flag = 0;
    while (tempbullet != NULL) {
        tempenemy = (*enemyhead).start;
        if (!(checkcollide(tempbullet->x, tempbullet->y, tempbullet->x + 16, tempbullet->y + 32, enemycol.x1, enemycol.y1, enemycol.x2, enemycol.y2))) {
            prev = tempbullet;
            tempbullet = tempbullet->next;
            continue;
        }
        while (enemy != NULL) {
            if (checkcollide(tempbullet->x, tempbullet->y, tempbullet->x + 16, tempbullet->y + 32, enemy->x, enemy->y, enemy->x + 32, enemy->y + 32)) {
                if (tempbullet == (*head).start) {
                    bullet_deletefirst(head);
                    tempbullet = (*head).start;
                    flag = 1;
                    if (enemy == (*enemyhead).start) {
                        enemy_deletefirst(enemyhead);
                        enemy = (*enemyhead).start;
                        (*state).score += 15;
                    }
                    else {
                        prevenemy->next = enemy->next;
                        free(enemy);
                        enemy = prevenemy->next;
                        (*state).score += 15;
                    }
                    break;
                }
                else {
                    prev->next = tempbullet->next;
                    free(tempbullet);
                    tempbullet = prev->next;
                    flag = 1;
                    if (enemy == (*enemyhead).start) {
                        enemy_deletefirst(enemyhead);
                        enemy = (*enemyhead).start;
                        (*state).score += 15;
                    }
                    else {
                        prevenemy->next = enemy->next;
                        free(enemy);
                        enemy = prevenemy->next;
                        (*state).score += 15;
                    }
                    break;
                }
            }
            prevenemy = enemy;
            enemy = enemy->next;
        }
        if (flag == 1) {
            flag = 0;
            continue;
        }
        enemy = tempenemy;
        prev = tempbullet;
        tempbullet = tempbullet->next;
    }
}

bool ebullets_collision(bullethead* ebhead, playerstruct player) {
    liststruct* head = (*ebhead).start;
    if ((*ebhead).start != NULL) {
        while (head != NULL) {
            if (checkcollide(player.x +5, player.y + 10, player.x + 59, player.y + 64, head->x + 1, head->y + 2, head->x + 15, head->y + 30)) {
                return true;
            }
            head = head->next;
        }
    }
    return false;
}

bool checkcollide(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2)
{
    if (ax1 > bx2) return false;
    if (ax2 < bx1) return false;
    if (ay1 > by2) return false;
    if (ay2 < by1) return false;
    return true;
}

void checkptrnull(liststruct* head) {
    if (head == NULL) {
        perror("Was assigned null as a pointer");
        exit(-1);
    }
}

void state_exit(enemyhead* ehead, bullethead* bhead, bullethead* ebhead) {
    enemy_freeptrlist(ehead);
    bullet_freeptrlist(bhead);
    bullet_freeptrlist(ebhead);
}

void state_draw(gamestate* state, playerstruct* player, enemyhead* ehead, boundingbox* enemycol, bullethead* bhead, bullethead* ebhead) {
    al_set_target_bitmap(buffer);
    al_clear_to_color(al_map_rgb(0, 0, 0));
    switch ((*state).flag) {
    case play:
        state_draw_play(state, player, ehead, enemycol, bhead, ebhead);
        break;
    case countdown:
        state_draw_countdown(state);
        break;
    case start:
        state_draw_start(state);
        break;
    case gameover:
        state_draw_gameover(state, ehead);
        break;
    }
    al_set_target_backbuffer(display);
    al_draw_scaled_bitmap(buffer, 0, 0, DISP_W, DISP_H, 0, 0, DISP_W, DISP_H, 0);
    al_flip_display();
    state->frames++;
    (*state).redraw = false;
}

void state_draw_gameover(gamestate* state, enemyhead* head) {
    al_draw_text(font, al_map_rgb(255, 255, 255), DISP_W / 8, DISP_H / 10, 0, "Nacisnij spacje zeby");
    al_draw_text(font, al_map_rgb(255, 255, 255), DISP_W / 8, DISP_H * 5 / 20, 0, "rozpoczac ponownie");
    al_draw_textf(font, al_map_rgb(255, 255, 255), DISP_W / 8, DISP_H * 7 / 10, 0, "Liczba punktow: %d", (*state).score);
    if ((*state).frames - (*state).timing > 60) {
        if (key[ALLEGRO_KEY_SPACE]) {
            enemy_freeptrlist(head);
            init_state(state);
            (*state).flag = countdown;
            (*state).timing = (*state).frames;
        }
    }
}

void state_draw_start(gamestate* state) {
    al_draw_text(font, al_map_rgb(255, 255, 255), DISP_W / 8, DISP_H / 3, 0, "Nacisnij spacje zeby");
    al_draw_text(font, al_map_rgb(255, 255, 255), DISP_W / 8, DISP_H / 2, 0, "rozpoczac");
    if (key[ALLEGRO_KEY_SPACE]) {
        (*state).flag = countdown;
        (*state).timing = (*state).frames;
    }
}

void state_draw_countdown(gamestate* state) {
    if ((*state).frames - (*state).timing > 90) {
        (*state).flag = initiation;
    }
    else if ((*state).frames - (*state).timing > 60) {
        al_draw_text(font, al_map_rgb(255, 255, 255), DISP_W / 2, DISP_H / 2, 0, "1");
        al_draw_textf(font, al_map_rgb(255, 255, 255), DISP_W * 4 / 10, DISP_H / 3, 0, "Runda %d", (*state).round);
    }
    else if ((*state).frames - (*state).timing > 30) {
        al_draw_text(font, al_map_rgb(255, 255, 255), DISP_W / 2, DISP_H / 2, 0, "2");
        al_draw_textf(font, al_map_rgb(255, 255, 255), DISP_W * 4 / 10, DISP_H / 3, 0, "Runda %d", (*state).round);
    }
    else if ((*state).frames - (*state).timing > 0) {
        al_draw_text(font, al_map_rgb(255, 255, 255), DISP_W / 2, DISP_H / 2, 0, "3");
        al_draw_textf(font, al_map_rgb(255, 255, 255), DISP_W * 4 / 10, DISP_H / 3, 0, "Runda %d", (*state).round);
    }
}

void state_draw_play(gamestate* state, playerstruct* player, enemyhead* ehead, boundingbox* enemycol, bullethead* bhead, bullethead* ebhead) {
    player_draw(player);
    enemy_draw(*ehead);
    bullet_draw(*bhead, *ebhead);
    if (!((*state).frames % 6)) {
        enemy_enemycol_check(enemycol, *ehead);
    }
    al_draw_rectangle((*enemycol).x1, (*enemycol).y1, (*enemycol).x2, (*enemycol).y2, al_map_rgb(255, 255, 255), 5);
    al_draw_textf(smallfont, al_map_rgb(255, 255, 255), DISP_W / 30, 0, 0, "Punkty: %d", (*state).score);
    al_draw_textf(smallfont, al_map_rgb(255, 255, 255), DISP_W * 13 / 15, 0, 0, "Runda: %d", (*state).round);
}

void state_init_initiation(gamestate* state, playerstruct* player, enemyhead* ehead, boundingbox* enemycol, bullethead* bhead, bullethead* ebhead) {
    enemy_freeptrlist(ehead);
    bullet_freeptrlist(bhead);
    bullet_freeptrlist(ebhead);
    init_enemy(ehead, enemycol, *state);
    init_player(player);
    enemy_enemycol_check(enemycol, *ehead);
    (*state).flag = play;
}

void state_init_play(gamestate* state) {
    (*state).round++;
    (*state).score += 500;
    (*state).flag = countdown;
    (*state).timing = (*state).frames;
}

void state_gamelogic(gamestate* state, playerstruct* player, enemyhead* ehead, boundingbox* enemycol, bullethead* bhead, bullethead* ebhead) {
    bullet_movement(bhead, (*player).bulletspeed);
    bullet_movement(ebhead, (*ehead).bulletspeed);
    enemy_movement(enemycol, ehead, ebhead);
    player_movement(player, bhead);
    pbullets_collision(bhead, ehead, state, *enemycol);
    (*player).wait--;
    if ((*enemycol).y2 >= (*player).y || ebullets_collision(ebhead, *player)) {
        (*state).flag = gameover;
        (*state).timing = (*state).frames;
    }
}

void state_keyboard_update(ALLEGRO_EVENT* event, unsigned char key[]) {
    switch (event->type) {
    case ALLEGRO_EVENT_TIMER:
        for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
            key[i] &= KEY_USED;
        break;
    case ALLEGRO_EVENT_KEY_DOWN:
        key[event->keyboard.keycode] = KEY_USED | KEY_RELEASED;
        break;
    case ALLEGRO_EVENT_KEY_UP:
        key[event->keyboard.keycode] &= KEY_RELEASED;
        break;
    }
}

void bullet_movement(bullethead* ptr, int bulletspeed) {
    if (!((*ptr).start == NULL)) {
        if ((*ptr).start->y < 5) {
            bullet_deletefirst(ptr);
        }
        liststruct* temp = (*ptr).start;
        while (temp != NULL) {
            temp->y -= bulletspeed;
            temp = temp->next;
        }
    }
}

void bullet_deletefirst(bullethead* ptr) {
    liststruct* temp = (*ptr).start;
    (*ptr).start = (*ptr).start->next;
    free(temp);
}

void bullet_freeptrlist(bullethead* ptr) {
    struct liststruct* nextp = (*ptr).start;
    struct liststruct* prev = NULL;
    while (nextp != NULL)
    {
        prev = nextp;
        nextp = nextp->next;
        free(prev);

    }
    (*ptr).start = NULL;
}

void bullet_draw(bullethead bhead, bullethead ebhead) {
    if (!(bhead.start == NULL)) {
        while (bhead.start != NULL) {
            al_draw_bitmap(playerbullet, bhead.start->x, bhead.start->y, 0);
            bhead.start = bhead.start->next;
        }
    }
    if (!(ebhead.start == NULL)) {
        while (ebhead.start != NULL) {
            al_draw_bitmap(enemybullet, ebhead.start->x, ebhead.start->y, 0);
            ebhead.start = ebhead.start->next;
        }
    }
}

void init_test(bool test, const char* description)
{
    if (test) return;
    fprintf(stderr, "couldn't initialize %s\n", description);
    exit(1);
}

void init_all()
{
    init_test(al_init(), "allegro");
    init_test(al_install_keyboard(), "keyboard");
    init_test(al_init_font_addon(), "font addon");
    init_test(al_init_ttf_addon(), "ttf addon");
    init_test(al_init_image_addon(), "image addon");
    init_test(al_init_primitives_addon(), "primitives addon");
    display = al_create_display(DISP_W, DISP_H);
    init_test(display, "display");
    buffer = al_create_bitmap(DISP_W, DISP_H);
    init_test(buffer, "bitmap buffer");
    font = al_load_ttf_font("font.ttf", DISP_W / 10, 0);
    init_test(font, "font.ttf");
    smallfont = al_load_ttf_font("font.ttf", DISP_W / 30, 0);
    init_test(smallfont, "font.ttf - small");
    timer = al_create_timer(1.0 / 30.0);
    init_test(timer, "timer");
    queue = al_create_event_queue();
    init_test(queue, "queue");
    playerimg = al_load_bitmap("player.png");
    init_test(playerimg, "playerimg");
    enemybullet = al_load_bitmap("bullet.png");
    init_test(enemybullet, "bullet");
    playerbullet = al_load_bitmap("bullet_me.png");
    init_test(playerbullet, "bullet_me");
    enemyimg = al_load_bitmap("enemy.png");
    init_test(enemyimg, "enemies");
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_start_timer(timer);
}

void init_enemy(enemyhead* ptr, boundingbox* enemycol, gamestate state) {
    int tempy = DISP_H * 7 / 20;
    if (state.round == 1) {
        (*ptr).enemycount = 72;
        (*ptr).enemyperrow = 12;
    }
    (*ptr).enemyspeed = 10 + state.round;
    (*ptr).bulletspeed = -4;
    (*ptr).start = (liststruct*)malloc(sizeof(liststruct));
    checkptrnull((*ptr).start);
    liststruct* temp = (*ptr).start;
    temp->x = DISP_W / 5;
    temp->y = tempy;
    temp->next = NULL;
    for (int i = 1; i < (*ptr).enemycount; i++) {
        temp->next = (liststruct*)malloc(sizeof(liststruct));
        checkptrnull(temp->next);
        if (i % (*ptr).enemyperrow == 0) {
            tempy -= DISP_H / 20;
        }
        temp->next->x = DISP_W / 5 + i % (*ptr).enemyperrow * DISP_W / 30;
        temp->next->y = tempy;
        temp = temp->next;
    }
    temp->next = NULL;
}

void init_state(gamestate* state) {
    (*state).quitprog = false;
    (*state).redraw = true;
    (*state).score = 0;
    (*state).frames = 0;
    (*state).round = 1;
    (*state).flag = start;
}

void init_player(playerstruct* player) {
    player->x = DISP_W / 2;
    player->y = DISP_H * 9 / 10;
    player->wait = 8;
    player->bulletspeed = DISP_H / 60;
    player->movementspeed = 8;
}

void enemy_deletefirst(enemyhead* ptr) {
    liststruct* temp = (*ptr).start;
    (*ptr).start = (*ptr).start->next;
    free(temp);
}

void enemy_freeptrlist(enemyhead* ptr) {
    struct liststruct* nextp = (*ptr).start;
    struct liststruct* prev = NULL;
    while (nextp != NULL)
    {
        prev = nextp;
        nextp = nextp->next;
        free(prev);
    }
    (*ptr).start = NULL;
}

void enemy_draw(enemyhead ptr) {
    while (ptr.start != NULL) {
        al_draw_bitmap(enemyimg, ptr.start->x, ptr.start->y, 0);
        ptr.start = ptr.start->next;
    }
}

void enemy_movement(boundingbox* box, enemyhead* ptr, bullethead* ebhead) {
    liststruct* temp = (*ptr).start;
    liststruct* copy = (*ptr).start;
    while (copy != NULL) {
        copy->x += (*ptr).enemyspeed;
        if (!(rand() % 1200)) {
            enemy_shoot(ebhead, copy->x, copy->y);
        }  
        copy = copy->next;
    }
    box->x1 += (*ptr).enemyspeed;
    box->x2 += (*ptr).enemyspeed;
    if (box->x1 < DISP_W / 40 || box->x2 > DISP_W * 39 / 40) {
        while (temp != NULL) {
            temp->y += DISP_H / 20;
            temp = temp->next;
        }
        box->y1 += DISP_H / 20;
        box->y2 += DISP_H / 20;
        (*ptr).enemyspeed *= -1;
    }
}

void enemy_shoot(bullethead* ebhead, int x, int y) {
    if (!((*ebhead).start == NULL)) {
        liststruct* start = (*ebhead).start;
        while (start->next != NULL) {
            start = start->next;
        }
        start->next = (liststruct*)malloc(sizeof(liststruct));
        checkptrnull(start->next);
        start->next->x = x + 16;
        start->next->y = y + 32;
        start->next->next = NULL;
    }
    else {
        (*ebhead).start = malloc(sizeof(liststruct));
        checkptrnull((*ebhead).start);
        (*ebhead).start->x = x + 16;
        (*ebhead).start->y = y + 32;
        (*ebhead).start->next = NULL;
    }
}

void enemy_enemycol_check(boundingbox* enemycol, enemyhead headenemylist) {
    struct liststruct* temp = headenemylist.start;
    if (temp == NULL) {
        return;
    }
    int minx = temp->x, maxx = temp->x, miny = temp->y, maxy = temp->y;
    while (temp != NULL) {
        if (minx > temp->x) {
            minx = temp->x;
        }
        if (maxx < temp->x) {
            maxx = temp->x;
        }
        if (miny > temp->y) {
            miny = temp->y;
        }
        if (maxy < temp->y) {
            maxy = temp->y;
        }
        temp = temp->next;
    }
    enemycol->x1 = minx;
    enemycol->x2 = maxx + 32;
    enemycol->y1 = miny;
    enemycol->y2 = maxy + 32;
}

void player_draw(playerstruct* playerstruct) {
    al_draw_bitmap(playerimg, playerstruct->x, playerstruct->y, 0);
}

void player_movement(playerstruct* player, bullethead* ptr) {
    if (key[ALLEGRO_KEY_LEFT]) {
        if (player->x < -15) {
            player->x = DISP_W + 5;
        }
        player->x += -player->movementspeed;
    }
    if (key[ALLEGRO_KEY_RIGHT]) {
        if (player->x > DISP_W + 5) {
            player->x = -5;
        }
        player->x += player->movementspeed;
    }
    if (key[ALLEGRO_KEY_SPACE] && player->wait < 1) {
        player_shoot(ptr, *player);
        player->wait = 8;
    }
    for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
        key[i] &= KEY_USED;
}

void player_shoot(bullethead* ptr, playerstruct player) {
    if (!((*ptr).start == NULL)) {
        liststruct* start = (*ptr).start;
        while (start->next != NULL) {
            start = start->next;
        }
        start->next = (liststruct*)malloc(sizeof(liststruct));
        checkptrnull(start->next);
        start->next->x = player.x + 24;
        start->next->y = player.y - 36;
        start->next->next = NULL;
    }
    else {
        (*ptr).start = malloc(sizeof(liststruct));
        checkptrnull((*ptr).start);
        (*ptr).start->x = player.x + 24;
        (*ptr).start->y = player.y - 36;
        (*ptr).start->next = NULL;
    }
}


/*
IMPLEMENT IT SO ENEMY BULLETS DISSAPEAR WHEN HITING THE GROUND




*/










