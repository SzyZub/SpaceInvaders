#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>


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

void init(bool test, const char* description)
{
    if (test) return;
    printf("couldn't initialize %s\n", description);
    exit(1);
}

int main() {
    init(al_init(), "allegro");
    init(al_install_keyboard(), "keyboard");
    init(al_init_font_addon(), "font addon");
    init(al_init_ttf_addon(), "ttf addon");
    init(al_init_primitives_addon, "primitives addon");
    ALLEGRO_FONT* font = al_load_ttf_font("font.ttf", 24, 0);
    init(font, "font.ttf");
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    init(timer, "timer");
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    init(queue, "queue");
    ALLEGRO_DISPLAY* display = al_create_display(1024, 768);
    init(display, "display");
    init(al_init_image_addon(), "image addon");
    ALLEGRO_BITMAP* player = al_load_bitmap("player.png");
    init(player, "player");
    ALLEGRO_BITMAP* bullet = al_load_bitmap("bullet.png");
    init(player, "bullet");
    ALLEGRO_BITMAP* bullet_me = al_load_bitmap("bullet_me.png");
    init(player, "bullet_me");
    ALLEGRO_BITMAP* enemies = al_load_bitmap("enemy.png");
    init(player, "enemies");

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

    int movement = 1;

	while (true) {
		al_wait_for_event(queue, &event);
        switch (event.type) {
            case ALLEGRO_EVENT_TIMER:
                redraw = true;
                for (int i = 0; i < enemycount; i++) {
                    enemylist[i].x += 1 * movement;
                }
                if (enemylist[enemycount - 1].x > 900 || enemylist[0].x < 100) {
                    movement *= -1;
                    for (int i = 0; i < enemycount; i++) {
                        enemylist[i].y += 50;
                    }
                }
                break;
            case ALLEGRO_EVENT_KEY_DOWN:
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
			al_flip_display();
			redraw = false;
		}
	}

    al_destroy_bitmap(player);
    al_destroy_bitmap(enemies);
    al_destroy_bitmap(bullet);
    al_destroy_bitmap(bullet_me);
	al_destroy_font(font);
	al_destroy_display(display);
	al_destroy_event_queue(queue);
	al_destroy_timer(timer);
}