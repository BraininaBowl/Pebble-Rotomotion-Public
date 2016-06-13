#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer_h;
static TextLayer *s_time_layer_m;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
static Layer *s_canvas;
static Layer *s_canvas_line;
static Layer *s_arrows;
static const GPathInfo ARROW_LEFT_PATH_POINTS = {
	3,
	   (GPoint []) {{0, 0}, {0, 14}, {8, 7},}
	};
static const GPathInfo ARROW_RIGHT_PATH_POINTS = {
	3,
	   (GPoint []) {{8, 0}, {8, 14}, {0, 7},}
	};
static int firstrun = 1;

// set variables for shader
		int rowHalf;
		int rowFull;
		int colHalf;
		int colFull;


// Customizations
#define COLORBG GColorBlack
//#define COLORHRFR GColorWhite
//#define COLORHRBG GColorClear
#define COLORHRBG GColorWhite
#define COLORHRFR GColorBlack
#define COLORMNBG GColorClear
#define COLORMNFR GColorWhite
#define COLORTRICL GColorRed
#define COLORTRIBW GColorWhite
static int twelveHour = 1;
static int xOffset = 15;
static int pmSpace = 0;



// Draw border to hide shader noise
static void canvas_update_proc(Layer *layer, GContext *ctx) {
  // Custom drawing happens here!
// Set the line color
graphics_context_set_stroke_color(ctx, COLORBG);

// Set the fill color
graphics_context_set_fill_color(ctx, GColorClear);
	
	// Set the stroke width (must be an odd integer value)

	
	GRect bounds = layer_get_bounds(layer);

	#if defined(PBL_RECT)
	// Rectengular screen: Draw a rectangle
	graphics_context_set_stroke_width(ctx, 14);
	graphics_draw_rect(ctx, bounds);
	#elif defined(PBL_ROUND)
	// Rectengular screen: Draw a rectangle
	graphics_context_set_stroke_width(ctx, 16);
	GPoint center = GPoint(bounds.size.h/2, bounds.size.w/2);
	uint16_t radius = bounds.size.h/2;

	// Draw the outline of a circle
	graphics_draw_circle(ctx, center, radius);
	#endif

}

// Bitmap data manipulation
// **************************************************************************
// **************************************************************************
//
// The bitmap code is based on the Pebble Compass code by orviwan
// https://github.com/orviwan/pebble-compass/blob/master/src/bitmap.c
//
// **************************************************************************
// **************************************************************************
void set_bitmap_pixel_color(GBitmap *bitmap, GBitmapFormat bitmap_format, int y, int x, GColor color) {
  GBitmapDataRowInfo row = gbitmap_get_data_row_info(bitmap, y);
  if ((x >= row.min_x) && (x <= row.max_x)) {
    switch(bitmap_format) {
      case GBitmapFormat1Bit :
        row.data[x / 8] ^= (-(gcolor_equal(color, GColorWhite)? 1 : 0) ^ row.data[x / 8]) & (1 << (x % 8));
        break;
      case GBitmapFormat1BitPalette :
        //TODO
        break;
      case GBitmapFormat2BitPalette :
        //TODO
        break;
      case GBitmapFormat4BitPalette :
        //TODO
        break;
      case GBitmapFormat8BitCircular :
      case GBitmapFormat8Bit :
        row.data[x] = color.argb;
    }
  }
}
GColor get_bitmap_color_from_palette_index(GBitmap *bitmap, uint8_t index) {
  GColor *palette = gbitmap_get_palette(bitmap);
  return palette[index];
}
GColor get_bitmap_pixel_color(GBitmap *bitmap, GBitmapFormat bitmap_format, int y, int x) {
  GBitmapDataRowInfo row = gbitmap_get_data_row_info(bitmap, y);
  switch(bitmap_format) {
    case GBitmapFormat1Bit :
      return ((row.data[x / 8] >> (x % 8)) & 1) == 1 ? GColorWhite : GColorBlack;
    case GBitmapFormat1BitPalette :
      return get_bitmap_color_from_palette_index(bitmap, ((row.data[x / 8] << (x % 8)) & 128) == 128 ? 1 : 0);
    case GBitmapFormat2BitPalette :
      return GColorBlack; //TODO get_color_from_palette_index(bitmap, (row.data[x / 4] >> (x % 4)) & 3);
    case GBitmapFormat4BitPalette :
      return GColorBlack; //TODO
    case GBitmapFormat8BitCircular :
    case GBitmapFormat8Bit :
      return (GColor) {.argb = row.data[x] };
  }
  return GColorClear;
}

// Shader stuff goes here
void layer_update_proc(Layer *layer, GContext *ctx) {

  // Get the framebuffer
	GBitmap *fb = graphics_capture_frame_buffer(ctx);
	GBitmapFormat fb_format = gbitmap_get_format(fb);

	//set variables for calculations
		int xToUse;
		int xToGet;
		int yToUse;
		int yToGet;
		int yToSet;
		GColor colorToSet;

	
// Iterate over all rows
	for(int y = 0; y < colFull; y++) {
	  	// Get this row's range and data
	  	GBitmapDataRowInfo info = gbitmap_get_data_row_info(fb, y);

		if (y < colHalf) {
			// top half
			yToUse = colHalf - y;
			yToSet = colHalf - y;
		} else {
			// bottom half
			yToUse = colFull - y;
			yToSet = y;
		} 
yToGet = yToSet + (64/(yToUse));

	  // Iterate over all visible columns
		  for(int x = 0; x <= rowFull; x++) {
			  // Split in left and right halves
			  if (x < rowHalf) {
				  // left half: Work from right to left
				  xToUse = rowHalf - x;
				  xToGet = xToUse - (x*3)/(yToUse);
			  } else {
				  // right half: Work from left to right
				  xToUse = x;
				  xToGet = x + ((xToUse - rowHalf)*3)/(yToUse);
			  }
			  // is the target pixel inside the area?
			  if (xToGet < 0 || xToGet > rowFull || yToGet < 0 || yToGet > colFull ){
				  // No, so we'll use the background color
				  colorToSet = COLORBG;
			  } else {
				  // Yes, so get the target pixel color
				  colorToSet = get_bitmap_pixel_color(fb, fb_format, yToGet, xToGet);
			  }
			  // Now we set the pixel to the right color
		 		set_bitmap_pixel_color(fb, fb_format, yToSet, xToUse, colorToSet);
			  }
	  	}
  // Finally, release the framebuffer
  graphics_release_frame_buffer(ctx, fb);
}

// layer for arrows
void s_arrows_update_proc(Layer *s_arrows, GContext* ctx) {
	static GPath *s_arrow_left_path = NULL;
	static GPath *s_arrow_right_path = NULL;

	// Fill the path:
	#if defined(PBL_COLOR)
  	graphics_context_set_fill_color(ctx, COLORTRICL);
	#elif defined(PBL_BW)
  	graphics_context_set_fill_color(ctx, COLORTRIBW);
	#endif
	
	gpath_draw_filled(ctx, s_arrow_left_path);
	gpath_draw_filled(ctx, s_arrow_right_path);

	s_arrow_left_path = gpath_create(&ARROW_LEFT_PATH_POINTS);
	s_arrow_right_path = gpath_create(&ARROW_RIGHT_PATH_POINTS);
	gpath_move_to(s_arrow_right_path, GPoint(102+(pmSpace*2), 0));
}

static void update_time() {
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
Layer *root_layer = window_get_root_layer(s_main_window);
  GRect bounds = layer_get_bounds(root_layer);


	GRect start_h;
	if(s_minute == 00 || firstrun == 1) {
	  	start_h = GRect((bounds.size.w/2)-49+xOffset,(bounds.size.h/2)-132-((s_hour-1)*36), 47, 1400);
		firstrun = 0;
	} else {
		start_h = GRect((bounds.size.w/2)-49+xOffset,(bounds.size.h/2)-132-(s_hour*36), 47, 1400);
	}
	GRect finish_h = GRect((bounds.size.w/2)-49+xOffset,(bounds.size.h/2)-132-(s_hour*36), 47, 1400);
	
	GRect start_m = GRect((bounds.size.w/2)+xOffset,(bounds.size.h/2)-114-((s_minute-1)*20), 27, 1888);
	GRect finish_m = GRect((bounds.size.w/2)+xOffset,(bounds.size.h/2)-114-(s_minute*20), 27, 1888);
	

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
  update_time();
}

static void main_window_load(Window *window) {
	
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

// set variables for shader
		rowHalf = bounds.size.w/2;
		rowFull = bounds.size.w;
		colHalf = bounds.size.h/2;
		colFull = bounds.size.h;
	
	
  // Create the TextLayer with specific bounds
  s_time_layer_h = text_layer_create(GRect((bounds.size.w/2)-49+xOffset, (bounds.size.h/2)-84, 47, 1216));
  s_time_layer_m = text_layer_create(GRect((bounds.size.w/2)+xOffset, (bounds.size.h/2)-84, 27, 1488));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer_h, COLORHRBG);
  text_layer_set_text_color(s_time_layer_h, COLORHRFR);
  if (twelveHour == 1) {
		text_layer_set_text(s_time_layer_h, "09\n10\n11\n12\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n01\n02\n03\n04");
	} else {
		text_layer_set_text(s_time_layer_h, "21\n22\n23\n00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n00\n01\n02\n03\n04");
	}
	text_layer_set_text_alignment(s_time_layer_h, GTextAlignmentCenter);

	text_layer_set_background_color(s_time_layer_m, COLORMNBG);
  	text_layer_set_text_color(s_time_layer_m, COLORMNFR);
	text_layer_set_text(s_time_layer_m, "55\n56\n57\n58\n59\n00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59\n00\n01\n02\n03\n04");
  	text_layer_set_text_alignment(s_time_layer_m, GTextAlignmentCenter);
	

	// add fonts
	text_layer_set_font(s_time_layer_h, fonts_get_system_font(FONT_KEY_LECO_36_BOLD_NUMBERS));
	text_layer_set_font(s_time_layer_m, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer_h));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer_m));
	

	
	// set canvas for shader
	s_canvas = layer_create(bounds);
  	layer_set_update_proc(s_canvas, layer_update_proc);
  	layer_add_child(window_layer, s_canvas);
	
	// ************************************************
	// ** Drop shadow
	// ************************************************

 // Create GBitmap
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_DROP);

  // Create BitmapLayer to display the GBitmap
  s_background_layer = bitmap_layer_create(
  			GRect(0, 0, bounds.size.w, bounds.size.h));

  // Set the bitmap onto the layer and add to the window
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  bitmap_layer_set_compositing_mode(s_background_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));

	
	// Create canvas line layer
	s_canvas_line = layer_create(bounds);
	// Assign the custom drawing procedure
	layer_set_update_proc(s_canvas_line, canvas_update_proc);

	// Add to Window
	layer_add_child(window_get_root_layer(window), s_canvas_line);


	// ************************************************
	// ** Arrows
	// ************************************************
	
	s_arrows = layer_create(GRect(((bounds.size.w/2)-55)+pmSpace, ((bounds.size.h/2)-7), 110+(pmSpace*2), 14));
  	layer_set_update_proc(s_arrows, s_arrows_update_proc);
  //---add the layer to the Window layer---
  	layer_add_child(window_layer, s_arrows);
	
	
	

}
	
static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer_h);
  text_layer_destroy(s_time_layer_m);

  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);

	// Destroy Canvas
	layer_destroy(s_canvas);
	
	// Destroy Canvas line
	layer_destroy(s_canvas_line);
	
	//destroy arrows
	layer_destroy(s_arrows);
	
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set the background color
  window_set_background_color(s_main_window, COLORBG);

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