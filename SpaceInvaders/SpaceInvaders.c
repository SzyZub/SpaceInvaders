#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
//testing purposes
#include <allegro5/allegro_primitives.h>

#define BUFFER_W 640
#define BUFFER_H 480
#define DISP_TO_BUFFER_SCALE 2
#define DISP_W (BUFFER_W * DISP_TO_BUFFER_SCALE)
#define DISP_H (BUFFER_H * DISP_TO_BUFFER_SCALE)
#define KEY_USED   1
#define KEY_RELEASED 2

enum screenflag {
    startorplay = 0,
    countdown,
    initiation
};

typedef struct playerstruct {
    int x, y, wait, bulletspeed;
} playerstruct;

typedef struct liststruct {
    int x, y;
    struct liststruct* next;
} liststruct;

typedef struct enemyhead {
    int enemycount, enemyperrow, enemyspeed;
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

void bulletmovement(bullethead* ptr, int bulletspeed);
bool checkcollide(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2);
void checkptrnull(liststruct* head);
void collision(bullethead* head, enemyhead* enemyhead);
void bullet_deletefirst(bullethead* ptr);
void enemy_deletefirst(enemyhead* ptr);
void destroyall();
void disp_post_draw(gamestate* state);
void drawbackground();
void drawbullets(bullethead ptr);
void drawenemies(enemyhead ptr);
void drawplayer(playerstruct* playerstruct);
void enemymovement(boundingbox* box, enemyhead* ptr);
void bullet_freeptrlist(bullethead* ptr);
void enemy_freeptrlist(enemyhead* ptr);
void init(bool test, const char* description);
void init_all();
void initbox(int x1, int x2, int y1, int y2, boundingbox* box);
void initenemylist(enemyhead* ptr, boundingbox* enemycol);
void initgamestate(gamestate* state);
void initplayer(playerstruct* player);
void keyboard_update();
void playermovement(playerstruct* player, bullethead* ptr);
void playershoot(bullethead* ptr, playerstruct player);

ALLEGRO_DISPLAY* display;
ALLEGRO_BITMAP* buffer;
ALLEGRO_FONT* font;
ALLEGRO_TIMER* timer;
ALLEGRO_EVENT_QUEUE* queue;
ALLEGRO_BITMAP* playerimg;
ALLEGRO_BITMAP* enemybullet;
ALLEGRO_BITMAP* playerbullet;
ALLEGRO_BITMAP* enemyimg;
ALLEGRO_EVENT event;
unsigned char key[ALLEGRO_KEY_MAX];


int main() {
    enemyhead headenemylist; 
    headenemylist.start = NULL;
    bullethead playerbulletlist;
    playerbulletlist.start = NULL;
    boundingbox enemycol;
    playerstruct player;
    gamestate state;
    memset(key, 0, sizeof(key));
    initgamestate(&state);
    init_all();
    initplayer(&player);
    al_start_timer(timer);
    while (true) {
        if (headenemylist.start == NULL) {
            if (state.flag == startorplay) {
                state.timing = state.frames;
                state.flag = countdown;
            }
            if (state.flag == initiation) {
                enemy_freeptrlist(&headenemylist);
                bullet_freeptrlist(&playerbulletlist);
                initenemylist(&headenemylist, &enemycol);
                initplayer(&player);
                state.flag = startorplay;
                headenemylist.enemyspeed++;
                state.round++;
            }
        }
        if (state.flag == startorplay && enemycol.y2 >= player.y) {
            puts("gameover");
            return;
        }
        al_wait_for_event(queue, &event);
        switch (event.type) {
            case ALLEGRO_EVENT_TIMER:
                state.redraw = true;
                if (state.flag == startorplay) {
                    player.wait--;
                    bulletmovement(&playerbulletlist, player.bulletspeed);
                    enemymovement(&enemycol, &headenemylist);
                    playermovement(&player, &playerbulletlist);
                    collision(&playerbulletlist, &headenemylist);
                }
                break;
            case ALLEGRO_EVENT_KEY_DOWN:
                key[event.keyboard.keycode] = KEY_USED | KEY_RELEASED;
                break;
            case ALLEGRO_EVENT_KEY_UP:
                key[event.keyboard.keycode] &= KEY_RELEASED;
                break;
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                state.quitprog = true;
                break;
        }
        if (key[ALLEGRO_KEY_ESCAPE])
            break;
        if (state.quitprog) {
            break;
        }
        keyboard_update(&event, key);
		if (state.redraw && al_is_event_queue_empty(queue))
		{
            al_set_target_bitmap(buffer);
            drawbackground();
            if (state.flag == startorplay) {
                //testing purposes
                al_draw_rectangle(enemycol.x1, enemycol.y1, enemycol.x2, enemycol.y2, al_map_rgb(120, 120, 120), 10);
                //
                drawplayer(&player);
                drawenemies(headenemylist);
                drawbullets(playerbulletlist);
            }
            else if (state.flag == countdown) {
                if (state.frames - state.timing > 90) {
                    state.flag = initiation;
                } else if (state.frames - state.timing > 60) {
                    al_draw_text(font, al_map_rgb(255, 255, 255), 320, 220, 0, "1");
                    al_draw_textf(font, al_map_rgb(255, 255, 255), 260, 120, 0, "runda %d", state.round);
                } else if (state.frames - state.timing > 30) {
                    al_draw_text(font, al_map_rgb(255, 255, 255), 320, 220, 0, "2");
                    al_draw_textf(font, al_map_rgb(255, 255, 255), 260, 120, 0, "runda %d", state.round);
                } else if (state.frames - state.timing > 0) {
                    al_draw_text(font, al_map_rgb(255, 255, 255), 320, 220, 0, "3");
                    al_draw_textf(font, al_map_rgb(255, 255, 255), 260, 120, 0, "runda %d", state.round);
                }   
            }
            disp_post_draw(&state);
            state.redraw = false;
		}
	}
    destroyall();
}

void bulletmovement(bullethead* ptr, int bulletspeed) {
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

void collision(bullethead* head, enemyhead* enemyhead) {
    liststruct* enemy = (*enemyhead).start, * tempenemy = NULL, * prevenemy = NULL;
    liststruct* tempbullet = (*head).start, * prev = NULL;
    int flag = 0;
    while (tempbullet != NULL) {
        tempenemy = (*enemyhead).start;
        while (enemy != NULL) {
            if (checkcollide(tempbullet->x, tempbullet->y, tempbullet->x + 8, tempbullet->y + 16, enemy->x, enemy->y, enemy->x + 16, enemy->y + 16)) {
                if (tempbullet == (*head).start) {
                    bullet_deletefirst(head);
                    tempbullet = (*head).start;
                    flag = 1;
                    if (enemy == (*enemyhead).start) {
                        enemy_deletefirst(enemyhead);
                        enemy = (*enemyhead).start;
                    }
                    else {
                        prevenemy->next = enemy->next;
                        free(enemy);
                        enemy = prevenemy->next;
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
                    }
                    else {
                        prevenemy->next = enemy->next;
                        free(enemy);
                        enemy = prevenemy->next;
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

void enemy_deletefirst(enemyhead* ptr) {
    liststruct* temp = (*ptr).start;
    (*ptr).start = (*ptr).start->next;
    free(temp);
}

void bullet_deletefirst(bullethead* ptr) {
    liststruct* temp = (*ptr).start;
    (*ptr).start = (*ptr).start->next;
    free(temp);
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

void disp_post_draw(gamestate* state)
{
    al_set_target_backbuffer(display);
    al_draw_scaled_bitmap(buffer, 0, 0, BUFFER_W, BUFFER_H, 0, 0, DISP_W, DISP_H, 0);
    al_flip_display();
    state->frames++;
}

void drawbackground() {
    al_clear_to_color(al_map_rgb(0, 0, 0));
}

void drawbullets(bullethead ptr) {
    if (!(ptr.start == NULL)) {
        while (ptr.start != NULL) {
            al_draw_bitmap(playerbullet, ptr.start->x, ptr.start->y, 0);
            ptr.start = ptr.start->next;
        }
    }
}

void drawenemies(enemyhead ptr) {
    while (ptr.start != NULL) {
        al_draw_bitmap(enemyimg, ptr.start->x, ptr.start->y, 0);
        ptr.start = ptr.start->next;
    }
}

void drawplayer(playerstruct* playerstruct) {
    al_draw_bitmap(playerimg, playerstruct->x, playerstruct->y, 0);
}

void enemymovement(boundingbox* box, enemyhead* ptr) {
    liststruct* temp = (*ptr).start;
    liststruct* copy = (*ptr).start;
    while (copy != NULL) {
        copy->x += (*ptr).enemyspeed;
        copy = copy->next;
    }
    box->x1 += (*ptr).enemyspeed;
    box->x2 += (*ptr).enemyspeed;
    if (box->x1 < BUFFER_W/10 || box->x2 > BUFFER_W*9/10) {
        while (temp != NULL) {
            temp->y += BUFFER_H/15;
            temp = temp->next;
        }
        box->y1 += BUFFER_H/15;
        box->y2 += BUFFER_H/15;
        (*ptr).enemyspeed *= -1;
    }
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

void enemy_freeptrlist(enemyhead* ptr) {
    struct liststruct* nextp = (*ptr).start;
    struct liststruct* prev = NULL;
    while (nextp != NULL)
    {
        prev = nextp;
        nextp = nextp->next;
        free(prev);
    }
}

void init(bool test, const char* description)
{
    if (test) return;
    fprintf(stderr, "couldn't initialize %s\n", description);
    exit(1);
}

void init_all()
{
    init(al_init(), "allegro");
    init(al_install_keyboard(), "keyboard");
    init(al_init_font_addon(), "font addon");
    init(al_init_ttf_addon(), "ttf addon");
    init(al_init_image_addon(), "image addon");
    //testing purposes
    init(al_init_primitives_addon(), "primitives addon");
    //
    display = al_create_display(DISP_W, DISP_H);
    init(display, "display");
    buffer = al_create_bitmap(BUFFER_W, BUFFER_H);
    init(buffer, "bitmap buffer");
    font = al_load_ttf_font("font.ttf", BUFFER_W/10, 0);
    init(font, "font.ttf");
    timer = al_create_timer(1.0 / 30.0);
    init(timer, "timer");
    queue = al_create_event_queue();
    init(queue, "queue");
    playerimg = al_load_bitmap("player.png");
    init(playerimg, "playerimg");
    enemybullet = al_load_bitmap("bullet.png");
    init(enemybullet, "bullet");
    playerbullet = al_load_bitmap("bullet_me.png");
    init(playerbullet, "bullet_me");
    enemyimg = al_load_bitmap("enemy.png");
    init(enemyimg, "enemies");
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
}

void initbox(int x1, int x2, int y1, int y2, boundingbox* box) {
    box->x1 = x1;
    box->x2 = x2;
    box->y1 = y1;
    box->y2 = y2;
}

void initenemylist(enemyhead* ptr, boundingbox* enemycol) {
    int tempy = BUFFER_H/32;
    (*ptr).enemycount = 72;
    (*ptr).enemyperrow = 12;
    (*ptr).enemyspeed = 0;
    (*ptr).start = (liststruct*)malloc(sizeof(liststruct));
    initbox(BUFFER_W/10, BUFFER_W/20 + (*ptr).enemyperrow * BUFFER_W/20, BUFFER_H/32, (*ptr).enemycount / (*ptr).enemyperrow * BUFFER_H/19, enemycol);
    checkptrnull((*ptr).start);
    liststruct* temp = (*ptr).start;
    temp->x = BUFFER_W/10;
    temp->y = tempy;
    temp->next = NULL;
    for (int i = 1; i < (*ptr).enemycount; i++) {
        temp->next = (liststruct*)malloc(sizeof(liststruct));
        checkptrnull(temp->next);
        if (i % (*ptr).enemyperrow == 0) {
            tempy += BUFFER_H/20;
        }
        temp->next->x = BUFFER_W/10 + i % (*ptr).enemyperrow * BUFFER_W/21;
        temp->next->y = tempy;
        temp = temp->next;
    }
    temp->next = NULL;
}

void initgamestate(gamestate* state) {
    (*state).quitprog = false;
    (*state).redraw = true;
    (*state).score = 0;
    (*state).frames = 0;
    (*state).round = 1;
    (*state).flag = startorplay;
}

void initplayer(playerstruct* player) {
    player->x = BUFFER_W/2;
    player->y = BUFFER_H*8/10;
    player->wait = 5;
    player->bulletspeed = BUFFER_H/60;
}

void keyboard_update(ALLEGRO_EVENT* event, unsigned char key[]){
    switch (event->type){
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

void playermovement(playerstruct* player, bullethead* ptr) {
    if (key[ALLEGRO_KEY_LEFT]) {
        if (player->x < -15) {
            player->x = BUFFER_W+5;
        }
        player->x += -4;
    }
    if (key[ALLEGRO_KEY_RIGHT]) {
        player->x += 4;
        if (player->x > BUFFER_W + 5) {
            player->x = -5;
        }
    }
    if (key[ALLEGRO_KEY_SPACE] && player->wait < 1) {
        playershoot(ptr, *player);
        player->wait = 4;
    }
    for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
        key[i] &= KEY_USED;
}

void playershoot(bullethead* ptr, playerstruct player) {
    if (!((*ptr).start == NULL)) {
        liststruct* start = (*ptr).start;
        while (start->next != NULL) {
            start = start->next;
        }
        start->next = (liststruct*)malloc(sizeof(liststruct));
        checkptrnull(start->next);
        start->next->x = player.x + 12;
        start->next->y = player.y - 18;
        start->next->next = NULL;
    }
    else {
        (*ptr).start = malloc(sizeof(liststruct));
        checkptrnull((*ptr).start);
        (*ptr).start->x = player.x + 12;
        (*ptr).start->y = player.y - 18;
        (*ptr).start->next = NULL;
    }
}




