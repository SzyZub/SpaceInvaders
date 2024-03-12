#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

int main() {
	al_init();
	al_init_font_addon();
	al_init_ttf_addon();

	ALLEGRO_DISPLAY* display = al_create_display(1024, 768);
	if (!display) return -1;

	ALLEGRO_FONT* font = al_load_font("font.ttf", 64, 0);

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_text(font, al_map_rgb(255, 255, 255), 20, 20, 0, "Hello World");
	al_flip_display();
	al_rest(10);

	al_destroy_font(font);
	al_destroy_display(display);
}