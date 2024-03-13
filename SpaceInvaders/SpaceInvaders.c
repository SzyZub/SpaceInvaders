#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#define BUFFER_W 320
#define BUFFER_H 240
#define DISP_SCALE 3
#define DISP_W (BUFFER_W * DISP_SCALE)
#define DISP_H (BUFFER_H * DISP_SCALE)

#define KEY_SEEN     1
#define KEY_RELEASED 2

bool collide(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2);
void init(bool test, const char* description);
void disp_init();
void disp_deinit();
void disp_pre_draw();
void disp_post_draw();

enum obj_type {
    t_player = 0,
    t_enemy,
    t_bullet,
    t_enemybullet
};

typedef struct obj {
    int x, y;
    int type;
} obj;

ALLEGRO_DISPLAY* display;
ALLEGRO_BITMAP* buffer;

int main() {
    init(al_init(), "allegro");
    init(al_install_keyboard(), "keyboard");
    init(al_init_font_addon(), "font addon");
    init(al_init_ttf_addon(), "ttf addon");
    init(al_init_primitives_addon(), "primitives addon");
    ALLEGRO_FONT* font = al_load_ttf_font("font.ttf", 24, 0);
    init(font, "font.ttf");
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    init(timer, "timer");
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    init(queue, "queue");
    ALLEGRO_DISPLAY* display = al_create_display(1024, 768);
    init(display, "display");
    init(al_init_image_addon(), "image addon");
    ALLEGRO_BITMAP* playerimg = al_load_bitmap("player.png");
    init(playerimg, "playerimg");
    ALLEGRO_BITMAP* bullet = al_load_bitmap("bullet.png");
    init(bullet, "bullet");
    ALLEGRO_BITMAP* bullet_me = al_load_bitmap("bullet_me.png");
    init(bullet_me, "bullet_me");
    ALLEGRO_BITMAP* enemies = al_load_bitmap("enemy.png");
    init(enemies, "enemies");

	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));
	al_register_event_source(queue, al_get_timer_event_source(timer));

    bool finished = false;
	bool redraw = true;
	ALLEGRO_EVENT event;
	al_start_timer(timer);


    int enemycount = 40;
    int enemyperrow = 8;
    obj enemylist[40] = {NULL};
    int tempy = -50;
    for (int i = 0; i < enemycount; i++) {
        if (i % enemyperrow == 0) {
            tempy += 50;
        }
        obj* enemy = &enemylist[i];
        enemy->x = 300 + i % enemyperrow * 50;
        enemy->y = 100 + tempy;
        enemy->type = 2;
    }

    obj player;
    player.x = 562;
    player.y = 650;
    player.type = 0;

    int enemymovement = 1;

    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));

    long score = 0;

	while (true) {
		al_wait_for_event(queue, &event);
        switch (event.type) {
            case ALLEGRO_EVENT_TIMER:
                redraw = true;
                for (int i = 0; i < enemycount; i++) {
                    enemylist[i].x += 1 * enemymovement;
                }
                if (enemylist[enemycount - 1].x > 900 || enemylist[0].x < 100) {
                    enemymovement *= -1;
                    for (int i = 0; i < enemycount; i++) {
                        enemylist[i].y += 50;
                    }
                }
                if (key[ALLEGRO_KEY_LEFT])
                    player.x--;
                if (key[ALLEGRO_KEY_RIGHT])
                    player.x++;
                if (key[ALLEGRO_KEY_ESCAPE])
                    finished = true;
                for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
                    key[i] &= KEY_SEEN;
                redraw = true;
                break;
            case ALLEGRO_EVENT_KEY_DOWN:
                key[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
                break;
            case ALLEGRO_EVENT_KEY_UP:
                key[event.keyboard.keycode] &= KEY_RELEASED;
                break;
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                finished = true;
                break;
        }
		
        if (finished) {
            break;
        }

		if (redraw && al_is_event_queue_empty(queue))
		{
            al_clear_to_color(al_map_rgb(0, 0, 0));
            for (int i = 0; i < enemycount; i++) {
                al_draw_bitmap(enemies, enemylist[i].x, enemylist[i].y, 0);
            }
            al_draw_bitmap(playerimg, player.x, player.y, 0);
			al_flip_display();
			redraw = false;
		}
	}

    al_destroy_bitmap(playerimg);
    al_destroy_bitmap(enemies);
    al_destroy_bitmap(bullet);
    al_destroy_bitmap(bullet_me);
	al_destroy_font(font);
	al_destroy_display(display);
	al_destroy_event_queue(queue);
	al_destroy_timer(timer);
}

bool collide(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2)
{
    if (ax1 > bx2) return false;
    if (ax2 < bx1) return false;
    if (ay1 > by2) return false;
    if (ay2 < by1) return false;

    return true;
}

void init(bool test, const char* description)
{
    if (test) return;
    printf("couldn't initialize %s\n", description);
    exit(1);
}

void disp_init()
{
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);

    display = al_create_display(DISP_W, DISP_H);
    init(display, "display");

    buffer = al_create_bitmap(BUFFER_W, BUFFER_H);
    init(buffer, "bitmap buffer");
}

void disp_deinit()
{
    al_destroy_bitmap(buffer);
    al_destroy_display(display);
}

void disp_pre_draw()
{
    al_set_target_bitmap(buffer);
}

void disp_post_draw()
{
    al_set_target_backbuffer(display);
    al_draw_scaled_bitmap(buffer, 0, 0, BUFFER_W, BUFFER_H, 0, 0, DISP_W, DISP_H, 0);

    al_flip_display();
}