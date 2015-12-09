#include <pebble.h>

Window *my_window;

//Default Colors
GColor COLOR_OUTER_EMPTY;// = GColorDarkGray;
GColor COLOR_OUTER_FILLED;// = GColorLightGray;
GColor COLOR_TICK_EMPTY;// = GColorDarkGray;
GColor COLOR_TICK_FILLED;// = GColorTiffanyBlue;
GColor COLOR_BACKGROUND;// = GColorBlack;
GColor COLOR_TEXT;// = GColorWhite;

int hour = 0, minute = 0, battery = 100;

char time_buffer[16], day_buffer[32], date_buffer[32];

bool overlay = true;

GFont roboto_bold_big, roboto_bold_small, roboto_regular;

void draw_layer(Layer *layer, GContext *ctx){
	graphics_context_set_fill_color(ctx, COLOR_BACKGROUND);
	graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
	
	
	/* OUTER RINGS */
	graphics_context_set_fill_color(ctx, COLOR_OUTER_EMPTY);
	
	//Right Arc
	graphics_fill_radial(ctx, layer_get_bounds(layer), GOvalScaleModeFitCircle, 6, DEG_TO_TRIGANGLE(0 + 3), DEG_TO_TRIGANGLE(180 - 3));
	
	//Left Arc
	graphics_fill_radial(ctx, layer_get_bounds(layer), GOvalScaleModeFitCircle, 6, DEG_TO_TRIGANGLE(180 + 3), DEG_TO_TRIGANGLE(360 - 3));

	graphics_context_set_fill_color(ctx, COLOR_OUTER_FILLED);
	
	//Right Fill
	graphics_fill_radial(ctx, layer_get_bounds(layer), GOvalScaleModeFitCircle, 6, DEG_TO_TRIGANGLE(180 - ( (180 - 3 ) * battery ) / 100 ), DEG_TO_TRIGANGLE(180 - 3));
	
	//Left Fill
	graphics_fill_radial(ctx, layer_get_bounds(layer), GOvalScaleModeFitCircle, 6, DEG_TO_TRIGANGLE(180  + 3 + ( ( 180 - 3 ) * ( 100 - battery ) / 100 )), DEG_TO_TRIGANGLE(360 - 3));

	
	/* INNER TICK MARKS */
	graphics_context_set_fill_color(ctx, COLOR_TICK_EMPTY);
	
	//Minute Ticks
	for(int i = 0; i < 60; i++){
		graphics_fill_radial(ctx, GRect(8,8,164,164), GOvalScaleModeFitCircle, overlay ? 27 : 82, DEG_TO_TRIGANGLE(3 + 2*i + 5*(i/5)), DEG_TO_TRIGANGLE(3 + 2*i + 1 + 5*(i/5)));
	}
	
	//Hour Ticks
	for(int i = 12; i < 24; i++){
		graphics_fill_radial(ctx, GRect(8,8,164,164), GOvalScaleModeFitCircle, overlay ? 27 : 82, DEG_TO_TRIGANGLE(3 + 15*i), DEG_TO_TRIGANGLE(3 + 9 + 15*i));
	}
	
	graphics_context_set_fill_color(ctx, COLOR_TICK_FILLED);
	
	//Minute Fill
	for(int i = 0; i < minute; i++){
		graphics_fill_radial(ctx, GRect(8,8,164,164), GOvalScaleModeFitCircle, overlay ? 27 : 82, DEG_TO_TRIGANGLE(3 + 2*i + 5*(i/5)), DEG_TO_TRIGANGLE(3 + 2*i + 1 + 5*(i/5)));
	}
	
	//Hour Fill
	for(int i = 0; i < hour; i++){
		graphics_fill_radial(ctx, GRect(8,8,164,164), GOvalScaleModeFitCircle, overlay ? 27 : 82, DEG_TO_TRIGANGLE(180 + 3 + 15*i), DEG_TO_TRIGANGLE(180 + 3 + 15*i + 9));
	}
	
	
	/* INNER TEXT */
	graphics_context_set_text_color(ctx, COLOR_TEXT);
	
	//Time
	graphics_draw_text(ctx, time_buffer, roboto_bold_big, GRect(0, (180-33)/2 - 4, 180, 33), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
	
	//Day
	graphics_draw_text(ctx, day_buffer, roboto_regular, GRect(0, (180-33)/2 + 34, 180, 10), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
	
	//Date
	graphics_draw_text(ctx, date_buffer, roboto_bold_small, GRect(0, (180-33)/2 + 34 + 11, 180, 10), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}

char* get_day(int day){
	switch(day){
		case MONDAY: return "MONDAY";
		case TUESDAY: return "TUESDAY";
		case WEDNESDAY: return "WEDNESDAY";
		case THURSDAY: return "THURSDAY";
		case FRIDAY: return "FRIDAY";
		case SATURDAY: return "SATURDAY";
		case SUNDAY: return "SUNDAY";
	}
	
	return "";
}

char* get_date(int month, int mday){
	static char combined[32];
	static char* month_buffer;
	
	switch(month){
		case 0: month_buffer = "JAN"; break;
		case 1: month_buffer = "FEB"; break;
		case 2: month_buffer = "MAR"; break;
		case 3: month_buffer = "APR"; break;
		case 4: month_buffer = "MAY"; break;
		case 5: month_buffer = "JUN"; break;
		case 6: month_buffer = "JUL"; break;
		case 7: month_buffer = "AUG"; break;
		case 8: month_buffer = "SEP"; break;
		case 9: month_buffer = "OCT"; break;
		case 10: month_buffer = "NOV"; break;
		case 11: month_buffer = "DEC"; break;
		default: month_buffer = "";
	}
	
	snprintf(combined, sizeof(combined), "%s %02d", month_buffer, mday);
	
	return combined;
}

static void tick(struct tm *tick_time, TimeUnits units){
	hour = tick_time->tm_hour%12;
	minute = tick_time->tm_min;
	
	snprintf(time_buffer, sizeof(time_buffer), "%02d:%02d", clock_is_24h_style() ? tick_time->tm_hour : tick_time->tm_hour%12, tick_time->tm_min);
	
	snprintf(day_buffer, sizeof(day_buffer), "%s", get_day(tick_time->tm_wday + 1));
	
	snprintf(date_buffer, sizeof(date_buffer), "%s", get_date(tick_time->tm_mon, tick_time->tm_mday));
	
	layer_mark_dirty(window_get_root_layer(my_window));
}

static void battery_event(BatteryChargeState state){
	battery = state.charge_percent;
	layer_mark_dirty(window_get_root_layer(my_window));
}

void handle_init(void) {
  my_window = window_create();
	
	COLOR_OUTER_EMPTY = GColorDarkGray;
	COLOR_OUTER_FILLED = GColorLightGray;
	COLOR_BACKGROUND = GColorBlack;
	COLOR_TEXT = GColorWhite;
	COLOR_TICK_EMPTY = GColorDarkGray;
	COLOR_TICK_FILLED = GColorTiffanyBlue;
	
	roboto_bold_big = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROBOTO_BOLD_33));
	roboto_bold_small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROBOTO_BOLD_12));
	roboto_regular = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROBOTO_10));

	layer_set_update_proc(window_get_root_layer(my_window), draw_layer);

  window_stack_push(my_window, true);
	
	time_t now = time(NULL);
	struct tm *ltime = localtime(&now);
	tick(ltime, MINUTE_UNIT);
	
	tick_timer_service_subscribe(MINUTE_UNIT, tick);
	
	battery_event(battery_state_service_peek());
	
	battery_state_service_subscribe(battery_event);
}

void handle_deinit(void) {
  window_destroy(my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
