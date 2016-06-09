#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer_h;
static TextLayer *s_time_layer_m;

static void update_time(Window *window) {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

	//*****************
	//**   ANIMATE   ** 
	//*****************
	
	// Write the current hours into a buffer
   static char s_buffer_hour[8];
   strftime(s_buffer_hour, sizeof(s_buffer_hour),"%H", tick_time);
   int s_hour = ((s_buffer_hour[0] - '0')*10)+s_buffer_hour[1] - '0';
	
	// Write the current minutes into a buffer
  	static char s_buffer_m[8];
  	strftime(s_buffer_m, sizeof(s_buffer_m), "%M", tick_time);
   int s_minute = ((s_buffer_m[0] - '0')*10)+s_buffer_m[1] - '0';

	
   // Setup animation layer
	Layer *layer_h = text_layer_get_layer(s_time_layer_h);
	Layer *layer_m = text_layer_get_layer(s_time_layer_m);
	

   // The start and end frames
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

	GRect start_h = GRect((bounds.size.w/2)-43,0-12-((s_hour-1)*36), 55, 1200);
	if(s_minute < 1) {
	  GRect start_h = GRect((bounds.size.w/2)-43,0-12-(s_hour*36), 55, 1200);
		}
	GRect finish_h = GRect((bounds.size.w/2)-43,0-12-(s_hour*36), 55, 1200);
	
	GRect start_m = GRect((bounds.size.w/2),0-10-((s_minute-1)*20), 30, 1888);
	GRect finish_m = GRect((bounds.size.w/2),0-10-(s_minute*20), 30, 1888);
	

	// Animate the Layer
	PropertyAnimation *prop_anim_h = property_animation_create_layer_frame(layer_h, &start_h, &finish_h);
	PropertyAnimation *prop_anim_m = property_animation_create_layer_frame(layer_m, &start_m, &finish_m);
	
	
	// Get the Animation
	Animation *anim_h = property_animation_get_animation(prop_anim_h);
	Animation *anim_m = property_animation_get_animation(prop_anim_m);

	
	// Choose parameters
	const int delay_ms_h = 0;
	const int duration_ms_h = 600;

	const int delay_ms_m = 0;
	const int duration_ms_m = 400;

	
	// Configure the Animation's curve, delay, and duration
	animation_set_curve(anim_h, AnimationCurveEaseIn);
	animation_set_delay(anim_h, delay_ms_h);
	animation_set_duration(anim_h, duration_ms_h);

	animation_set_curve(anim_m, AnimationCurveEaseIn);
	animation_set_delay(anim_m, delay_ms_m);
	animation_set_duration(anim_m, duration_ms_m);
	

	// Play the animation
	animation_schedule(anim_h);
	animation_schedule(anim_m);

}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time(Window *window);
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
	
  // Create the TextLayer with specific bounds
  s_time_layer_h = text_layer_create(GRect((bounds.size.w/2)-43, 0, 43, 1216));
  s_time_layer_m = text_layer_create(GRect((bounds.size.w/2), 0, 30, 1488));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer_h, GColorClear);
  text_layer_set_text_color(s_time_layer_h, GColorWhite);
  text_layer_set_text(s_time_layer_h, "22\n23\n00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n00\n01\n02\n03\n04");
  text_layer_set_text_alignment(s_time_layer_h, GTextAlignmentCenter);

 text_layer_set_background_color(s_time_layer_m, GColorClear);
  text_layer_set_text_color(s_time_layer_m, GColorWhite);
  text_layer_set_text(s_time_layer_m, "56\n57\n58\n59\n00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59\n00\n01\n02\n03\n04");
  text_layer_set_text_alignment(s_time_layer_m, GTextAlignmentCenter);
	

	// add fonts
	text_layer_set_font(s_time_layer_h, fonts_get_system_font(FONT_KEY_LECO_36_BOLD_NUMBERS));
	text_layer_set_font(s_time_layer_m, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer_h));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer_m));

}

	
static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer_h);
  text_layer_destroy(s_time_layer_m);
}


static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set the background color
  window_set_background_color(s_main_window, GColorBlack);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Make sure the time is displayed from the start
  update_time(Window *window);

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}