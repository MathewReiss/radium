#include <pebble.h>

Window *my_window;

#define COLOR_OUTER_EMPTY GColorDarkGray
#define COLOR_OUTER_FILLED GColorLightGray
#define COLOR_TICK_EMPTY GColorDarkGray
#define COLOR_TICK_FILLED GColorRed
#define COLOR_BACKGROUND GColorBlack
#define COLOR_TEXT GColorWhite

int hour = 6, minute = 35, battery = 70;

void draw_layer(Layer *layer, GContext *ctx){
	graphics_context_set_fill_color(ctx, COLOR_BACKGROUND);
	graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
	
	/* OUTER RINGS */
	graphics_context_set_fill_color(ctx, COLOR_OUTER_EMPTY);
	
	//Right Arc
	graphics_fill_radial(ctx, layer_get_bounds(layer), GOvalScaleModeFitCircle, 4, DEG_TO_TRIGANGLE(0 + 3), DEG_TO_TRIGANGLE(180 - 3));
	
	//Left Arc
	graphics_fill_radial(ctx, layer_get_bounds(layer), GOvalScaleModeFitCircle, 4, DEG_TO_TRIGANGLE(180 + 3), DEG_TO_TRIGANGLE(360 - 3));
	
	/* INNER TICK MARKS */
	graphics_context_set_fill_color(ctx, COLOR_OUTER_FILLED);
	
	//Right Fill
	graphics_fill_radial(ctx, layer_get_bounds(layer), GOvalScaleModeFitCircle, 4, DEG_TO_TRIGANGLE(180 - 3 - ( (180 - 3 ) * battery ) / 100 ), DEG_TO_TRIGANGLE(180 - 3));
	
	//Left Fill
	// ???
	
	graphics_context_set_fill_color(ctx, COLOR_TICK_EMPTY);
	
	for(int i = 0; i < 24; i++){
		graphics_fill_radial(ctx, GRect(6,6,168,168), GOvalScaleModeFitCircle, 20, DEG_TO_TRIGANGLE(3 + 15*i), DEG_TO_TRIGANGLE(3 + 9 + 15*i));
	}
	
	graphics_context_set_fill_color(ctx, COLOR_TICK_FILLED);
	
	for(int i = 0; i < minute; i++){
		graphics_fill_radial(ctx, GRect(6,6,168,168), GOvalScaleModeFitCircle, 20, DEG_TO_TRIGANGLE(3 + 2*i + 5*(i/5)), DEG_TO_TRIGANGLE(3 + 2*i + 1 + 5*(i/5)));
	}
	
	for(int i = 0; i < hour; i++){
		graphics_fill_radial(ctx, GRect(6,6,168,168), GOvalScaleModeFitCircle, 20, DEG_TO_TRIGANGLE(180 + 3 + 15*i), DEG_TO_TRIGANGLE(180 + 3 + 15*i + 9));
	}
}

void handle_init(void) {
  my_window = window_create();

	layer_set_update_proc(window_get_root_layer(my_window), draw_layer);

  window_stack_push(my_window, true);
}

void handle_deinit(void) {
  window_destroy(my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
