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

enum status {
    dead = 0,
    alive
};

typedef struct playerstruct {
    int x, y;
} playerstruct;

typedef struct liststruct {
    int x, y;
    struct liststruct* next;
} liststruct;

typedef struct boundingbox {
    int x1, x2, y1, y2;
} boundingbox;

void checkptrnull(liststruct* head);
void freeptrlist(liststruct** head);
void init(bool test, const char* description);
void init_all();
void destroyall();
void disp_post_draw(long *frames);
void keyboard_update(ALLEGRO_EVENT* event, unsigned char key[]);
bool collide(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2);
void initplayer(playerstruct *player);
void initbox(int x1, int x2, int y1, int y2, boundingbox* box);
void initenemylist(liststruct *head, int count);
void placeenemies(int enemycount, int enemyperrow, liststruct* head);
void drawenemies(liststruct* head);
void drawbullets(liststruct* head);
void drawbackground();
void drawplayer(playerstruct* playerstruct);
void enemymovement(boundingbox* box, liststruct* head, int* enemymovement);
void playermovement(playerstruct* player, bool* quitprog, unsigned char key[]);
void playershoot(liststruct* head, playerstruct player);

ALLEGRO_DISPLAY* display;
ALLEGRO_BITMAP* buffer;
ALLEGRO_FONT* font;
ALLEGRO_TIMER* timer;
ALLEGRO_EVENT_QUEUE* queue;
ALLEGRO_BITMAP* playerimg;
ALLEGRO_BITMAP* enemybullet;
ALLEGRO_BITMAP* playerbullet;
ALLEGRO_BITMAP* enemyimg;

int main() {
    init_all();
    ALLEGRO_EVENT event;
    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));
    bool quitprog = false;
    bool redraw = true;
    long score = 0;
    long frames = 0;
    int enemycount = 72;
    int enemyperrow = 12;
    liststruct *headenemylist = (liststruct*) malloc(sizeof(liststruct)); 
    checkptrnull(headenemylist);
    headenemylist->next = NULL;
    liststruct *playerbulletlist = (liststruct*) malloc(sizeof(liststruct));
    checkptrnull(playerbulletlist);
    playerbulletlist->next = NULL;
    boundingbox enemycol;
    playerstruct player;
    initenemylist(headenemylist, enemycount);
	placeenemies(enemycount, enemyperrow, headenemylist);
    initbox(50, 36 + enemyperrow * 30, 15, 4 + enemycount / enemyperrow * 25, &enemycol);
    initplayer(&player);
    int enemyspeed = 2;
    al_start_timer(timer);
    playershoot(playerbulletlist, player);
    while (true) {
		al_wait_for_event(queue, &event);
        switch (event.type) {
            case ALLEGRO_EVENT_TIMER:
                redraw = true;
                enemymovement(&enemycol, headenemylist, &enemyspeed);
                playermovement(&player, &quitprog, key);
                break;
            case ALLEGRO_EVENT_KEY_DOWN:
                key[event.keyboard.keycode] = KEY_USED | KEY_RELEASED;
                break;
            case ALLEGRO_EVENT_KEY_UP:
                key[event.keyboard.keycode] &= KEY_RELEASED;
                break;
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                quitprog = true;
                break;
        }
        if (quitprog) {
            break;
        }
        keyboard_update(&event, key);
		if (redraw && al_is_event_queue_empty(queue))
		{
            al_set_target_bitmap(buffer);
            drawbackground();
            //testing purposes
            al_draw_rectangle(enemycol.x1, enemycol.y1, enemycol.x2, enemycol.y2, al_map_rgb(120, 120, 120), 10);
            //
            drawplayer(&player);
            drawenemies(headenemylist);
            drawbullets(playerbulletlist);
            disp_post_draw(&frames);
            redraw = false;
		}
	}
    destroyall();
    freeptrlist(&headenemylist);
}

void checkptrnull(liststruct* head) {
    if (head == NULL) {
        perror("Was assigned null as a pointer");
        exit(-1);
    }
}

void freeptrlist(liststruct** head) {
    struct liststruct* nextp = *head;
    struct liststruct* prev = NULL;
    while (nextp != NULL)
    {
        prev = nextp;
        nextp = nextp->next;
        free(prev);
        
    }
    *head = NULL;
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
    font = al_load_ttf_font("font.ttf", 24, 0);
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

void disp_post_draw(long* frames)
{
    al_set_target_backbuffer(display);
    al_draw_scaled_bitmap(buffer, 0, 0, BUFFER_W, BUFFER_H, 0, 0, DISP_W, DISP_H, 0);
    al_flip_display();
    (*frames)++;
}

void keyboard_update(ALLEGRO_EVENT* event, unsigned char key[])
{
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

bool collide(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2)
{
    if (ax1 > bx2) return false;
    if (ax2 < bx1) return false;
    if (ay1 > by2) return false;
    if (ay2 < by1) return false;
    return true;
}

void initplayer(playerstruct *player) {
    player->x = 320;
    player->y = 400;
}

void initbox(int x1, int x2, int y1, int y2, boundingbox* box) {
    box->x1 = x1;
    box->x2 = x2;
    box->y1 = y1;
    box->y2 = y2;
}

void initenemylist(liststruct* head, int count) {
    for (int i = 0; i < count; i++) {
        head->next = (liststruct*) malloc(sizeof(liststruct));
        checkptrnull(head->next);
        head = head->next;
    }
    checkptrnull(head);
    head->next = NULL;
}

void placeenemies(int enemycount, int enemyperrow, liststruct* head) {
    int tempy = -10;
    int i = 0;
    while(head->next != NULL) {
        if (i % enemyperrow == 0) {
            tempy += 25;
        }
        head->x = 50 + i % enemyperrow * 30;
        head->y = tempy;
        head = head->next;
        i++;
    }
}

void drawenemies(liststruct* head) {
    while(head != NULL) {
        al_draw_bitmap(enemyimg, head->x, head->y, 0);
        head = head->next;
    }
}

void drawbullets(liststruct* head) {
    while (head != NULL) {
        al_draw_bitmap(playerbullet, head->x, head->y, 0);
        head = head->next;
    }
}

void drawbackground() {
    al_clear_to_color(al_map_rgb(0, 0, 0));
}

void drawplayer(playerstruct* playerstruct) {
    al_draw_bitmap(playerimg, playerstruct->x, playerstruct->y, 0);
}

void enemymovement(boundingbox* box, liststruct* head, int* enemyspeed) {
    liststruct* temp = head;
    while (head->next != NULL) {
        head->x += *enemyspeed;
        head = head->next;
    }
    box->x1 += *enemyspeed;
    box->x2 += *enemyspeed;
    if (box->x1 < 40 || box->x2 > 600) {
        while (temp->next != NULL) {
            temp->y += 30;
            temp = temp->next;
        }
        box->y1 += 30;
        box->y2 += 30;
        *enemyspeed *= -1;
    }
}

void playershoot(liststruct* head, playerstruct player) {
    while (head->next != NULL) {
        head = head->next;
    }
    head->next = (liststruct*) malloc(sizeof(liststruct));
    checkptrnull(head->next);
    head->x = player.x + 12;
    head->y = player.y + 20;
    head->next = NULL;
}

void playermovement(playerstruct *player, bool* quitprog, unsigned char key[]) {
    if (key[ALLEGRO_KEY_LEFT])
        player->x--;
    if (key[ALLEGRO_KEY_RIGHT])
        player->x++;
    if (key[ALLEGRO_KEY_DOWN]);

    if (key[ALLEGRO_KEY_ESCAPE])
        *quitprog = true;
    for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
        key[i] &= KEY_USED;
}

