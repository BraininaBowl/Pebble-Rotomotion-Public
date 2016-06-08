#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer_h;
static TextLayer *s_time_layer_m;
static GFont s_time_font_h;
static GFont s_time_font_m;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
   static char s_buffer_hour[8];
   strftime(s_buffer_hour, sizeof(s_buffer_hour),"%H", tick_time);
   int s_hour = ((s_buffer_hour[0] - '0')*10)+s_buffer_hour[1] - '0';

   // The start and end frames - move the Layer 40 pixels to the right
GRect start = GRect(30,-13-((s_hour-1)*36), 43, 1016);
GRect finish = GRect(30,-13-(s_hour*36), 43, 1016);

// Animate the Layer
PropertyAnimation *prop_anim = property_animation_create_layer_frame(s_time_layer_h, &start, &finish);

// Get the Animation
Animation *anim = property_animation_get_animation(prop_anim);

// Choose parameters
const int delay_ms = 0;
const int duration_ms = 500;

// Configure the Animation's curve, delay, and duration
animation_set_curve(anim, AnimationCurveEaseIn);
animation_set_delay(anim, delay_ms);
animation_set_duration(anim, duration_ms);

// Play the animation
animation_schedule(anim);


  static char s_buffer_m[8];
  strftime(s_buffer_m, sizeof(s_buffer_m), "%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer_m, s_buffer_m);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
	
  // Create the TextLayer with specific bounds
  s_time_layer_h = text_layer_create(GRect(30, 0, 43, 1016));
  s_time_layer_m = text_layer_create(GRect(bounds.size.w - 57, (bounds.size.h-20)/2, 57, 20));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer_h, GColorClear);
  text_layer_set_text_color(s_time_layer_h, GColorWhite);
  text_layer_set_text(s_time_layer_h, "22\n23\n00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n00\n01\n02\n03\n04");
  text_layer_set_text_alignment(s_time_layer_h, GTextAlignmentCenter);

 text_layer_set_background_color(s_time_layer_m, GColorClear);
  text_layer_set_text_color(s_time_layer_m, GColorWhite);
  text_layer_set_text(s_time_layer_m, "00");
  text_layer_set_text_alignment(s_time_layer_m, GTextAlignmentLeft);
	
  // Create GFont
  s_time_font_h = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_HOUR_36));
  s_time_font_m = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_MIN_16));

  // Apply to TextLayer
  text_layer_set_font(s_time_layer_h, s_time_font_h);
  text_layer_set_font(s_time_layer_m, s_time_font_m);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer_h));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer_m));

  // Create GBitmap
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_S_PATTERN_LARGE);

  // Create BitmapLayer to display the GBitmap
  s_background_layer = bitmap_layer_create(
  			GRect(0, 0, bounds.size.w, bounds.size.h));

  // Set the bitmap onto the layer and add to the window
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  bitmap_layer_set_compositing_mode(s_background_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
}

	
static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer_h);
  text_layer_destroy(s_time_layer_m);

  // Unload GFont
  fonts_unload_custom_font(s_time_font_h);
  fonts_unload_custom_font(s_time_font_m);


  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);

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
  update_time();

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