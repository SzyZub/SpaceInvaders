#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>

#define BUFFER_W 640
#define BUFFER_H 480
#define DISP_TO_BUFFER_SCALE 2
#define DISP_W (BUFFER_W * DISP_TO_BUFFER_SCALE)
#define DISP_H (BUFFER_H * DISP_TO_BUFFER_SCALE)

#define KEY_USED   1
#define KEY_RELEASED 2

void init(bool test, const char* description);
void init_all();
void destroyall();
void disp_pre_draw();
void disp_post_draw(long *frames);
void keyboard_update(ALLEGRO_EVENT* event);
bool collide(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2);
void placeenemies(int enemycount, int enemyperrow, struct enemystruct list[]);
void initplayer(struct playerstruct* player);

enum status {
    dead = 0,
    alive
};

typedef struct playerstruct {
    int x, y;
} playerstruct;

typedef struct enemystruct {
    int x, y;
    unsigned int id;
    enum status status; 
} enemystruct;

ALLEGRO_DISPLAY* display;
ALLEGRO_BITMAP* buffer;
ALLEGRO_FONT* font;
ALLEGRO_TIMER* timer;
ALLEGRO_EVENT_QUEUE* queue;
ALLEGRO_BITMAP* playerimg;
ALLEGRO_BITMAP* enemybullet;
ALLEGRO_BITMAP* playerbullet;
ALLEGRO_BITMAP* enemyimg;
unsigned char key[ALLEGRO_KEY_MAX];

int main() {
    init_all();
    ALLEGRO_EVENT event;
    memset(key, 0, sizeof(key));
    bool quitprog = false;
	bool redraw = true;
    long score = 0;
    long frames = 0;
    int enemycount = 72;
    int enemyperrow = 12;
    enemystruct *enemylist = calloc(enemycount, sizeof(enemystruct));
    playerstruct player;
    placeenemies(enemycount, enemyperrow, enemylist);
    initplayer(&player);

    int enemymovement = 1;
    al_start_timer(timer);

	while (true) {
		al_wait_for_event(queue, &event);
        switch (event.type) {
            case ALLEGRO_EVENT_TIMER:
                redraw = true;
                /*for (int i = 0; i < enemycount; i++) {
                    enemylist[i].x += 1 * enemymovement;
                }
                if (enemylist[enemycount - 1].x > 900 || enemylist[0].x < 10) {
                    enemymovement *= -1;
                    for (int i = 0; i < enemycount; i++) {
                        enemylist[i].y += 50;
                    }
                }
                */
                if (key[ALLEGRO_KEY_LEFT])
                    player.x--;
                if (key[ALLEGRO_KEY_RIGHT])
                    player.x++;
                if (key[ALLEGRO_KEY_ESCAPE])
                    quitprog = true;
                for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
                    key[i] &= KEY_USED;
                redraw = true;
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
        
		if (redraw && al_is_event_queue_empty(queue))
		{
            disp_pre_draw();
            al_clear_to_color(al_map_rgb(0, 0, 0));
            for (int i = 0; i < enemycount; i++) {
                al_draw_bitmap(enemyimg, enemylist[i].x, enemylist[i].y, 0);
            }
            al_draw_bitmap(playerimg, player.x, player.y, 0);
            disp_post_draw(&frames);
			redraw = false;
		}
        
	}
    free(enemylist);
    destroyall();
}


void init(bool test, const char* description)
{
    if (test) return;
    printf("couldn't initialize %s\n", description);
    exit(1);
}

void init_all()
{
    init(al_init(), "allegro");
    init(al_install_keyboard(), "keyboard");
    init(al_init_font_addon(), "font addon");
    init(al_init_ttf_addon(), "ttf addon");
    init(al_init_image_addon(), "image addon");
    display = al_create_display(DISP_W, DISP_H);
    init(display, "display");
    buffer = al_create_bitmap(BUFFER_W, BUFFER_H);
    init(buffer, "bitmap buffer");
    font = al_load_ttf_font("font.ttf", 24, 0);
    init(font, "font.ttf");
    timer = al_create_timer(1.0 / 60.0);
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
    memset(key, 0, sizeof(key));
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

void disp_pre_draw()
{
    al_set_target_bitmap(buffer);
}

void disp_post_draw(long* frames)
{
    al_set_target_backbuffer(display);
    al_draw_scaled_bitmap(buffer, 0, 0, BUFFER_W, BUFFER_H, 0, 0, DISP_W, DISP_H, 0);

    al_flip_display();
    (*frames)++;
}

void keyboard_update(ALLEGRO_EVENT* event)
{
    switch (event->type)
    {
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

void placeenemies(int enemycount, int enemyperrow, struct enemystruct list[]) {
    int tempy = -10;
    for (int i = 0; i < enemycount; i++) {
        if (i % enemyperrow == 0) {
            tempy += 25;
        }
        enemystruct enemy;
        enemy.x = 50 + i % enemyperrow * 30;
        enemy.y = tempy;
        enemy.status = 1;
        list[i] = enemy;
    }
}

void initplayer(struct playerstruct *player) {
    player->x = 320;
    player->y = 400;
}

//TODO change to sprite based system
//include "waves of enemies"