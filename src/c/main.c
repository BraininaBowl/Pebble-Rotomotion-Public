#include <pebble.h>
#include "main.h"

static Window *s_window;
static TextLayer *s_time_layer_h;
static TextLayer *s_time_layer_m;
static Layer *s_canvas;
static Layer *s_canvas_line;
static Layer *s_arrows;

// set variables for drawing
static int rowHalf;
static int rowFull;
static int colHalf;
static int colFull;

static char s_buffer_hour[3];
static char s_buffer_m[3];
int delay_ms_h = 0;
int duration_ms_h = 500;
int delay_ms_m = 0;
int duration_ms_m = 300;
static int s_hour;
static int s_minute;
static int firstrun;

int xGrid;
int yGrid;
int xToUse;
int xToGet;
int yToUse;
int yToGet;
int yToSet;
GColor colorToSet;


// A struct for our specific settings (see main.h)
ClaySettings settings;

// Initialize the default settings
static void prv_default_settings() {
  settings.BackgroundColor = GColorBlack;
  settings.HourColor = GColorWhite;
  settings.MinColor = GColorWhite;
  settings.HourBgColor = GColorBlack;
  settings.MinBgColor = GColorBlack; 
#if defined(PBL_COLOR)
	settings.ArrowColor = GColorRed;
#else
	settings.ArrowColor = GColorWhite;
#endif
  settings.twelveHour = false;
  settings.shaderMode = 1;
  settings.dropShadow = true;
}

// bitmap manipulation
void set_bitmap_pixel_color(GBitmap *bitmap, GBitmapFormat bitmap_format, int y, int x, GColor color) {
			// Bitmap data manipulation
	// **************************************************************************
	// **************************************************************************
	//
	// All bitmap code is based on the Pebble Compass code by orviwan
	// https://github.com/orviwan/pebble-compass/blob/master/src/bitmap.c
	//
	// **************************************************************************
	// **************************************************************************
	
	
	
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

	
// Iterate over all rows
	for(int y = 0; y < colFull; y++) {
	  
	
if (settings.shaderMode == 1) {
// draw as cylinder	  
	
	if (y < colHalf) {
			// top half
			yToUse = colHalf - y;
			yToSet = colHalf - y - 1;
			yToGet = yToSet - (colHalf/(yToUse));
		} else {
			// bottom half
			yToUse = colFull - y;
			yToSet = y;
			yToGet = yToSet + (colHalf/(yToUse));
		} 
		
	// filter only edge pixels, to improve readability and performance
	if (yToSet < (colHalf - 26) || yToSet > (colHalf + 26)){

	  // Iterate over all visible columns
		  for(int x = 0; x < (rowHalf + 40); x++) {
		   if (x == 40){
		     x = rowHalf;
		   }
			  // Split in left and right halves
			  if (x < rowHalf) {
				  // left half: Work from right to left
				  xToUse = rowHalf - x;
				  xToGet = xToUse - ((x*3)/yToUse)+1;
			  } else {
				  // right half: Work from left to right
				  xToUse = x;
				  xToGet = x + (((xToUse - rowHalf)*3)/yToUse)-1;
			  }
			  // is the target pixel inside the area?
			  if (xToGet < 0 || xToGet >= rowFull || yToGet < 0 || yToGet > colFull ){
				  // No, so we'll use the background color
				  colorToSet = settings.BackgroundColor;
			  } else {
				  // Yes, so get the target pixel color
				  colorToSet = get_bitmap_pixel_color(fb, fb_format, yToGet, xToGet);
			  }
		 		
		#if defined(PBL_COLOR)
			  // Apply shadows
		  if(settings.dropShadow) {
					if( yToSet < 19 || yToSet > (colFull - 19)) {
						colorToSet = GColorFromRGB(
						  (colorToSet.r + settings.BackgroundColor.r * 2)*85/3, 
						  (colorToSet.g + settings.BackgroundColor.g * 2)*85/3,
						  (colorToSet.b + settings.BackgroundColor.b * 2)*85/3);
					} else if( yToSet < 25 || yToSet > (colFull - 25)) {
						colorToSet = GColorFromRGB(
						  (colorToSet.r * 3 + settings.BackgroundColor.r)*85/4, 
						  (colorToSet.g * 3 + settings.BackgroundColor.g)*85/4, 
						  (colorToSet.b * 3 + settings.BackgroundColor.b)*85/4);
					}				
				}  	
			  #endif
						
		 		  
		 		// Now we set the pixel to the right color
		 		set_bitmap_pixel_color(fb, fb_format, yToSet, xToUse, colorToSet);
			  }
	  	}	
	
		// zwarte balk	
			#if defined(PBL_COLOR)
	  //	if (yToSet < 14 || yToSet > (colFull - 13)){
	  if (yToSet < 3 || yToSet > (colFull - 8)){
			for(int x = 0; x < rowFull; x++) {
				// Now we set the pixel to the right color
		 		set_bitmap_pixel_color(fb, fb_format, yToSet, x, settings.BackgroundColor);
			}
		}
	#endif
	  	
	  	} else if (settings.shaderMode == 2) {
// draw as inverted cylinder	  	
	
		if (y < colHalf) {
			// top half
			yToUse = colHalf - y;
			yToSet = colHalf - y - 1;
yToGet = yToSet - (colHalf/(yToUse));
		} else {
			// bottom half
			yToUse = colFull - y;
			yToSet = y;
yToGet = yToSet + (colHalf/(yToUse));
		} 
		
	// filter only edge pixels, to improve readability and performance
	if (yToSet < (colHalf - 28) || yToSet > (colHalf + 28)){

	  // Iterate over all visible columns
		  for(int x = 0; x < (rowHalf + 45); x++) {
		   if (x == 45){
		     x = rowHalf;
		   }			  // Split in left and right halves
			  if (x < rowHalf) {
				  // left half: Work from right to left
				  xToUse = rowHalf - x;
				  xToGet = xToUse + ((x*3)/yToUse)-1;
			  } else {
				  // right half: Work from left to right
				  xToUse = x;
				  xToGet = x - (((xToUse - rowHalf)*3)/yToUse)+1;
			  }
			  // is the target pixel inside the area?
			  if (xToGet < 0 || xToGet >= rowFull || yToGet < 0 || yToGet > colFull ){
				  // No, so we'll use the background color
				  colorToSet = settings.BackgroundColor;
			  } else {
				  // Yes, so get the target pixel color
				  colorToSet = get_bitmap_pixel_color(fb, fb_format, yToGet, xToGet);
			  }
			  

			// Apply shadows
			  		#if defined(PBL_COLOR)
		  	if(settings.dropShadow) {
					if( yToSet < 19 || yToSet > (colFull - 19)) {
						colorToSet = GColorFromRGB(
						  (colorToSet.r + settings.BackgroundColor.r * 2)*85/3, 
						  (colorToSet.g + settings.BackgroundColor.g * 2)*85/3,
						  (colorToSet.b + settings.BackgroundColor.b * 2)*85/3);
					} else if( yToSet < 25 || yToSet > (colFull - 25)) {
						colorToSet = GColorFromRGB(
						  (colorToSet.r * 3 + settings.BackgroundColor.r)*85/4, 
						  (colorToSet.g * 3 + settings.BackgroundColor.g)*85/4, 
						  (colorToSet.b * 3 + settings.BackgroundColor.b)*85/4);
					}				
				}  
			  #endif
			  
			  
			  // Now we set the pixel to the right color
		 		set_bitmap_pixel_color(fb, fb_format, yToSet, xToUse, colorToSet);
			  }
	  	}
	  	
		// zwarte balk	
				#if defined(PBL_COLOR)
	  	if (yToSet < 14 || yToSet > (colFull - 13)){
			for(int x = 0; x < rowFull; x++) {
				// Now we set the pixel to the right color
		 		set_bitmap_pixel_color(fb, fb_format, yToSet, x, settings.BackgroundColor);
			}
		}
		#endif
	  	
	  	} 	else if (settings.shaderMode == 3) {
	  	// draw as banner
	  	
	  	
	  		
		if (y < colHalf) {
			// top half
			yToUse = -1*(colHalf - y);
			yToSet = colHalf - y - 1;
yToGet = yToSet - (colHalf/(yToSet));
		} else {
			// bottom half
			yToUse = colFull - y;
			yToSet = y;
yToGet = yToSet + (colHalf/(yToUse));
			
		} 
		
	// filter only edge pixels, to improve readability and performance
	if (yToSet < (colHalf - 32) || yToSet > (colHalf + 32)){

	  // Iterate over all visible columns
		  for(int x = 0; x < (rowHalf + 45); x++) {
		   if (x == 45){
		     x = rowHalf;
		   }			  
		   // Split in left and right halves
			  if (x < rowHalf) {
				  // left half: Work from right to left
				  xToUse = rowHalf - x;
				  xToGet = xToUse + ((x*3)/yToUse);
			  } else {
				  // right half: Work from left to right
				  xToUse = x;
				  xToGet = x - (((xToUse - rowHalf)*3)/yToUse);
			  }
			  // is the target pixel inside the area?
			  if (xToGet < 0 || xToGet >= rowFull || yToGet < 0 || yToGet > colFull ){
				  // No, so we'll use the background color
				  colorToSet = settings.BackgroundColor;
			  } else {
				  // Yes, so get the target pixel color
				  colorToSet = get_bitmap_pixel_color(fb, fb_format, yToGet, xToGet);
			  }
			  
			// Apply shadows
			  		#if defined(PBL_COLOR)
		  	if(settings.dropShadow) {
					if( yToSet < 19 || yToSet > (colFull - 19)) {
						colorToSet = GColorFromRGB(
						  (colorToSet.r + settings.BackgroundColor.r * 2)*85/3, 
						  (colorToSet.g + settings.BackgroundColor.g * 2)*85/3,
						  (colorToSet.b + settings.BackgroundColor.b * 2)*85/3);
					} else if( yToSet < 25 || yToSet > (colFull - 25)) {
						colorToSet = GColorFromRGB(
						  (colorToSet.r * 3 + settings.BackgroundColor.r)*85/4, 
						  (colorToSet.g * 3 + settings.BackgroundColor.g)*85/4, 
						  (colorToSet.b * 3 + settings.BackgroundColor.b)*85/4);
					}				
				}  	
			  #endif
			  
			  
			  
			  // Now we set the pixel to the right color
		 		set_bitmap_pixel_color(fb, fb_format, yToSet, xToUse, colorToSet);
			  }
	  	}

		// zwarte balk	
			#if defined(PBL_COLOR)
	  	if (yToSet < 14 || yToSet > (colFull - 13)){
			for(int x = 0; x < rowFull; x++) {
				// Now we set the pixel to the right color
		 		set_bitmap_pixel_color(fb, fb_format, yToSet, x, settings.BackgroundColor);
			}
		}
		#endif
	
}   else if (settings.shaderMode == 4) {
	  	// draw as Frosted
	  	if(yGrid==1){
	  	  yGrid = 0;
	  	  xGrid = 0;
	  	} else {
	  	  yGrid = 1;
	  	  xGrid = 1;
	  	}
	  	
	  	   
	   // filter only edge pixels, to improve readability and performance
	   if (y < (colHalf - 28) || y > (colHalf + 26)){

			
			
		#if defined(PBL_COLOR)
	   for(int x = 0; x < (rowHalf + 40); x++) {
	      if (x == 40){
          x = rowHalf;
	      }
		
		   // Split in left and right halves
			  if (x < rowHalf) {
				  // left half: Work from right to left
				  xToUse = rowHalf - x;
			  } else {
				  // right half: Work from left to right
				  xToUse = x;
			  }
		     
		    GColor color1;
		    GColor color2;
		    GColor color3;
		    GColor color4;
		    GColor color5;
		    
		   // is the target pixel inside the area?
			  if (xToUse < 0 || xToUse >= rowFull || y < 0 || y > colFull ){
				  // No, so we'll use the background color
				  color1 = settings.BackgroundColor;
			  } else {
				  // Yes, so get the target pixel color
				  color1 = get_bitmap_pixel_color(fb, fb_format, y, xToUse);
			  }
		    
		   // is the target pixel inside the area?
			  if (xToUse < 0 || xToUse >= rowFull || y - 2 < 0 || y - 2 > colFull ){
				  // No, so we'll use the background color
				  color2 = settings.BackgroundColor;
			  } else {
				  // Yes, so get the target pixel color
				  color2 = get_bitmap_pixel_color(fb, fb_format, y - 2, xToUse);
			  }
		    
		   // is the target pixel inside the area?
			  if (xToUse - 2 < 0 || xToUse - 2 >= rowFull || y < 0 || y > colFull ){
				  // No, so we'll use the background color
				  color3 = settings.BackgroundColor;
			  } else {
				  // Yes, so get the target pixel color
				  color3 = get_bitmap_pixel_color(fb, fb_format, y, xToUse - 2);
			  }
		    
		   // is the target pixel inside the area?
			  if (xToUse < 0 || xToUse >= rowFull || y + 2 < 0 || y + 2 > colFull ){
				  // No, so we'll use the background color
				  color4 = settings.BackgroundColor;
			  } else {
				  // Yes, so get the target pixel color
				  color4 = get_bitmap_pixel_color(fb, fb_format, y + 2, xToUse);
			  }
		    
		   // is the target pixel inside the area?
			  if (xToUse + 2 < 0 || xToUse + 2 >= rowFull || y < 0 || y > colFull ){
				  // No, so we'll use the background color
				  color5 = settings.BackgroundColor;
			  } else {
				  // Yes, so get the target pixel color
				  color5 = get_bitmap_pixel_color(fb, fb_format, y, xToUse + 2);
			  }
		     	  
		     GColor colorToSet = GColorFromRGB(
		     (color1.r + color2.r + color3.r + color4.r + color5.r)*85/5, 
		     (color1.g + color2.g + color3.g + color4.g + color5.g)*85/5,		     
		     (color1.b + color2.b + color3.b + color4.b + color5.b)*85/5);
		     
		     if (xGrid == 1) {
	         colorToSet = GColorFromRGB((colorToSet.r + settings.BackgroundColor.r)*85/2, (colorToSet.g + settings.BackgroundColor.g)*85/2, (colorToSet.b + settings.BackgroundColor.b)*85/2);
	         xGrid = 0;
	       } else {
	         xGrid = 1;
	       }
		       
		       
		  // Apply shadows
					#if defined(PBL_COLOR)
		  if(settings.dropShadow) {
					if( y < 19 || y > (colFull - 19)) {
						colorToSet = GColorFromRGB(
						  (colorToSet.r + settings.BackgroundColor.r * 2)*85/3, 
						  (colorToSet.g + settings.BackgroundColor.g * 2)*85/3,
						  (colorToSet.b + settings.BackgroundColor.b * 2)*85/3);
					} else if( y < 25 || y > (colFull - 25)) {
						colorToSet = GColorFromRGB(
						  (colorToSet.r + settings.BackgroundColor.r)*85/2, 
						  (colorToSet.g + settings.BackgroundColor.g)*85/2, 
						  (colorToSet.b + settings.BackgroundColor.b)*85/2);
					}				
				}   
			#endif
			
				// Now we set the pixel to the right color
				set_bitmap_pixel_color(fb, fb_format, y, xToUse, colorToSet); 
		           
		   }
			
			 #elif defined(PBL_BW)
			
			for(int x = 0; x < rowFull; x++) {
				
				if (xGrid == 1) {
	         	colorToSet = settings.BackgroundColor;
					
					// Now we set the pixel to the right color
					set_bitmap_pixel_color(fb, fb_format, y, x, colorToSet);

					xGrid = 0;
	       	} else {
	         	xGrid = 1;
	       	}
				 
			}
			
			#endif
			
		   }
	   
	   
	#if defined(PBL_COLOR)
	
	   // full width overlay
	   if (y == (colHalf - 30) || y == (colHalf + 27)){
	     for(int x = 0; x < rowFull; x++) {
	       GColor currentColor = get_bitmap_pixel_color(fb, fb_format, y, x);
	       GColor colorToSet = GColorFromRGB((currentColor.r + 3)*85/2, (currentColor.g + 3)*85/2, (currentColor.b + 3)*85/2);
	     
	       set_bitmap_pixel_color(fb, fb_format, y, x, colorToSet);
	     }
		   } else if (y == (colHalf + 28)){
		     for(int x = 0; x < rowFull; x++) {
		       GColor currentColor = get_bitmap_pixel_color(fb, fb_format, y, x);
		       GColor colorToSet = GColorFromRGB((currentColor.r)*85/2, (currentColor.g)*85/2, (currentColor.b)*85/2);
		       
		       set_bitmap_pixel_color(fb, fb_format, y, x, colorToSet);
		   } 
		 }	else if (y == (colHalf - 29)){
		     for(int x = 0; x < rowFull; x++) {
		       GColor currentColor = get_bitmap_pixel_color(fb, fb_format, y, x);
		       GColor colorToSet = GColorFromRGB((currentColor.r*2)*85/3, (currentColor.g*2)*85/3, (currentColor.b*2)*85/3);
		       
		       set_bitmap_pixel_color(fb, fb_format, y, x, colorToSet);
		   } 
		 }	else if (y == (colHalf - 28)){
		     for(int x = 0; x < rowFull; x++) {
		       GColor currentColor = get_bitmap_pixel_color(fb, fb_format, y, x);
		       GColor colorToSet = GColorFromRGB((currentColor.r*3)*85/4, (currentColor.g*3)*85/4, (currentColor.b*3)*85/4);
		       
		       set_bitmap_pixel_color(fb, fb_format, y, x, colorToSet);
		   } 
		 }	else if (y == (colHalf - 27)){
		     for(int x = 0; x < rowFull; x++) {
		       GColor currentColor = get_bitmap_pixel_color(fb, fb_format, y, x);
		       GColor colorToSet = GColorFromRGB((currentColor.r*4)*85/5, (currentColor.g*4)*85/5, (currentColor.b*4)*85/5);
		       
		       set_bitmap_pixel_color(fb, fb_format, y, x, colorToSet);
		   } 
		 }	
	#endif

}
		
		// ANTIALIAS
		if(settings.shaderMode != 4){
		#if defined(PBL_COLOR)
		for(int x = 0; x < rowFull; x++) {
		     GColor currentColor = get_bitmap_pixel_color(fb, fb_format, y, x);
		     GColor nextColor = get_bitmap_pixel_color(fb, fb_format, y, x+1);
		     GColor colorToSet = GColorFromRGB((currentColor.r + nextColor.r)*85/2, (currentColor.g + nextColor.g)*85/2, (currentColor.b + nextColor.b)*85/2);  

			  // Now we set the pixel to the right color
		 	  set_bitmap_pixel_color(fb, fb_format, y, x, colorToSet);
			  }
		#endif
		}
	 }

	graphics_release_frame_buffer(ctx, fb);
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "End Shader. Mem %d", heap_bytes_free());
	
}

// Read settings from persistent storage
static void prv_load_settings() {
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// Save the settings to persistent storage
static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  // Update the display based on new settings
  prv_update_display();
}

// Update the display elements
static void prv_update_display() {
  // Background color
  window_set_background_color(s_window, settings.BackgroundColor);

  // Foreground Colors
	text_layer_set_background_color(s_time_layer_h, settings.HourBgColor);
	text_layer_set_text_color(s_time_layer_h, settings.HourColor);
	text_layer_set_background_color(s_time_layer_m, settings.MinBgColor);
	text_layer_set_text_color(s_time_layer_m, settings.MinColor);
	
  
  if (settings.twelveHour)
  		{
			text_layer_set_text(s_time_layer_h, "09\n10\n11\n12\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n01\n02\n03\n04");
		} else {
			text_layer_set_text(s_time_layer_h, "21\n22\n23\n00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n00\n01\n02\n03\n04");
		}
}

// Handle the response from AppMessage
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  // Background Color
	Tuple *bg_color_t = dict_find(iter, MESSAGE_KEY_BackgroundColor);
  if (bg_color_t) {
    settings.BackgroundColor = GColorFromHEX(bg_color_t->value->int32);
  }

  // Hours Color  
	Tuple *HourColor_t = dict_find(iter, MESSAGE_KEY_HourColor);
  if (HourColor_t) {
    settings.HourColor = GColorFromHEX(HourColor_t->value->int32);
  }
	
  // Minutes Color  
	Tuple *MinColor_t = dict_find(iter, MESSAGE_KEY_MinColor);
  if (MinColor_t) {
    settings.MinColor = GColorFromHEX(MinColor_t->value->int32);
  }
	
  // Hours Bg Color  
	Tuple *HourBgColor_t = dict_find(iter, MESSAGE_KEY_HourBgColor);
  if (HourBgColor_t) {
    settings.HourBgColor = GColorFromHEX(HourBgColor_t->value->int32);
  }
	
  // Minutes Bg Color  
	Tuple *MinBgColor_t = dict_find(iter, MESSAGE_KEY_MinBgColor);
  if (MinBgColor_t) {
    settings.MinBgColor = GColorFromHEX(MinBgColor_t->value->int32);
  }
	
	// Arrow Color
  Tuple *ar_color_t = dict_find(iter, MESSAGE_KEY_ArrowColor);
  if (ar_color_t) {
    settings.ArrowColor = GColorFromHEX(ar_color_t->value->int32);
  }
	
	// Twelve Hour Mode
  Tuple *twelveHour_t = dict_find(iter, MESSAGE_KEY_twelveHour);
  if (twelveHour_t) {
    settings.twelveHour = twelveHour_t->value->int32 == 1;
  } 
	
	// Drop shadow
  Tuple *dropShadow_t = dict_find(iter, MESSAGE_KEY_dropShadow);
  if (dropShadow_t) {
    settings.dropShadow = dropShadow_t->value->int32 == 1;
  } 
	
	// Shader Mode
  Tuple *shaderMode_t = dict_find(iter, MESSAGE_KEY_shaderMode);
  if (shaderMode_t) {
    settings.shaderMode = (shaderMode_t->value->uint16)-48;
  } 
  
  // Save the new settings to persistent storage
  prv_save_settings();
}

// *** DRAW STUFF ***
// Draw text
static void drawText(Layer *window_layer) {
//	 APP_LOG(APP_LOG_LEVEL_DEBUG, "Start drawText. Mem %d", heap_bytes_used());
	 
   // Create the TextLayer with specific bounds
	s_time_layer_h = text_layer_create(GRect(rowHalf-49+12, colHalf-84, 47, 1216));
  	s_time_layer_m = text_layer_create(GRect(rowHalf+12, colHalf-84, 27, 1488));

  // TextLayer options
	text_layer_set_text_alignment(s_time_layer_h, GTextAlignmentCenter);
	text_layer_set_text(s_time_layer_m, "55\n56\n57\n58\n59\n00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59\n00\n01\n02\n03\n04");
  	text_layer_set_text_alignment(s_time_layer_m, GTextAlignmentCenter);

	// add fonts
	text_layer_set_font(s_time_layer_h, fonts_get_system_font(FONT_KEY_LECO_36_BOLD_NUMBERS));
	text_layer_set_font(s_time_layer_m, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));

	// Add it as a child layer to the Window's root layer
	layer_add_child(window_layer, text_layer_get_layer(s_time_layer_h));
  	layer_add_child(window_layer, text_layer_get_layer(s_time_layer_m));
	
}

// Draw arrows
void s_arrows_update_proc(Layer *s_arrows, GContext* ctx) {
	GPathInfo ARROW_LEFT_PATH_POINTS = {
	3,
	   (GPoint []) {{0, 0}, {0, 15}, {8, 7},}
	};
	
	GPathInfo ARROW_RIGHT_PATH_POINTS = {
	3,
	   (GPoint []) {{8, 0}, {8, 15}, {0, 7},}
	};
	
	
		static GPath *s_arrow_left_path = NULL;
		static GPath *s_arrow_right_path = NULL;

		// Fill the path:
		graphics_context_set_fill_color(ctx, settings.ArrowColor);
		gpath_draw_filled(ctx, s_arrow_left_path);
		gpath_draw_filled(ctx, s_arrow_right_path);

		s_arrow_left_path = gpath_create(&ARROW_LEFT_PATH_POINTS);
		s_arrow_right_path = gpath_create(&ARROW_RIGHT_PATH_POINTS);
		gpath_move_to(s_arrow_right_path, GPoint(102, 0));
	}
// Draw arrows
static void drawArrows(Layer *window_layer){
	s_arrows = layer_create(GRect((rowHalf-55), (colHalf-8), 110, 15));
  	layer_set_update_proc(s_arrows, s_arrows_update_proc);
  //---add the layer to the Window layer---
	layer_add_child(window_layer, s_arrows);
}
// Draw shader
static void drawShader(Layer * window_layer){
	if (settings.shaderMode > 0) {
	// set canvas for shader
	s_canvas = layer_create(GRect(0,0, rowFull, colFull));
  	layer_set_update_proc(s_canvas, layer_update_proc);
  	layer_add_child(window_layer, s_canvas);
  	}
}

// *** ANIMATE ***

	// Clear animation memory
	void on_animation_stopped_h(Animation *anim_h, bool finished, void *context)
	{
    //Free the memory used by the Animation
    	property_animation_destroy((PropertyAnimation*) anim_h);
	}
	void on_animation_stopped_m(Animation *anim_m, bool finished, void *context)
	{
    //Free the memory used by the Animation
		property_animation_destroy((PropertyAnimation*) anim_m);
	}


// animate time change
static void update_time() {
	
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

	//*****************
	//**   ANIMATE   ** 
	//*****************

	// Write the current hours into a buffer
   strftime(s_buffer_hour, sizeof(s_buffer_hour),"%H", tick_time);
   s_hour = ((s_buffer_hour[0] - '0')*10)+s_buffer_hour[1] - '0';
	
	// Write the current minutes into a buffer
  	strftime(s_buffer_m, sizeof(s_buffer_m), "%M", tick_time);
   s_minute = ((s_buffer_m[0] - '0')*10)+s_buffer_m[1] - '0';

	
   // Setup animation layer
	Layer *layer_h = text_layer_get_layer(s_time_layer_h);
	Layer *layer_m = text_layer_get_layer(s_time_layer_m);

	GRect start_h;
	if(s_minute == 00 || firstrun == 1) {
	  	start_h = GRect(rowHalf-49+12,colHalf-132-((s_hour-1)*36), 47, 1400);
		firstrun = 0;
	} else {
		start_h = GRect(rowHalf-49+12,colHalf-132-(s_hour*36), 47, 1400);
	}
	GRect finish_h = GRect(rowHalf-49+12,colHalf-132-(s_hour*36), 47, 1400);
	GRect start_m = GRect(rowHalf+12,colHalf-114-((s_minute-1)*20), 27, 1888);
	GRect finish_m = GRect(rowHalf+12,colHalf-114-(s_minute*20), 27, 1888);
	

	// Animate the Layer
	PropertyAnimation *prop_anim_h = property_animation_create_layer_frame(layer_h, &start_h, &finish_h);
	PropertyAnimation *prop_anim_m = property_animation_create_layer_frame(layer_m, &start_m, &finish_m);
	
	
	// Get the Animation
	Animation *anim_h = property_animation_get_animation(prop_anim_h);
	Animation *anim_m = property_animation_get_animation(prop_anim_m);

	// Configure the Animation's curve, delay, and duration
	animation_set_curve(anim_h, AnimationCurveLinear);
	animation_set_delay(anim_h, delay_ms_h);
	animation_set_duration(anim_h, duration_ms_h);
		//Set stopped handler to free memory
    AnimationHandlers handlers_h = {
        //The reference to the stopped handler is the only one in the array
        .stopped = (AnimationStoppedHandler) on_animation_stopped_h
    };
	animation_set_handlers((Animation*) anim_h, handlers_h, NULL);
	

	animation_set_curve(anim_m, AnimationCurveLinear);
	animation_set_delay(anim_m, delay_ms_m);
	animation_set_duration(anim_m, duration_ms_m);
		//Set stopped handler to free memory
    AnimationHandlers handlers_m = {
        //The reference to the stopped handler is the only one in the array
        .stopped = (AnimationStoppedHandler) on_animation_stopped_m
    };
   animation_set_handlers((Animation*) anim_m, handlers_m, NULL);
	

	// Play the animation
	animation_schedule(anim_h);
	animation_schedule(anim_m);
	

}


// *** EVENTS ***
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

// Window Load event
static void prv_window_load(Window *window) {
	firstrun = 1;
	
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

	// set variables for drawing
	rowHalf = bounds.size.w/2;
	rowFull = bounds.size.w;
	colHalf = bounds.size.h/2;
	colFull = bounds.size.h;

	// create textlayer
	drawText(window_layer);
	// create shader layer
	drawShader(window_layer);
	// create date layer
	drawArrows(window_layer);

	prv_update_display();
}

// Window Unload event
static void prv_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer_h);
  text_layer_destroy(s_time_layer_m);

	// Destroy Canvas
	layer_destroy(s_canvas);
	
	// Destroy Canvas line
	layer_destroy(s_canvas_line);
	
	//destroy arrows
	layer_destroy(s_arrows);
	
	// Unsubscribe from TickTimerService
 tick_timer_service_unsubscribe();
}

static void prv_init(void) {
  prv_load_settings();

  // Listen for AppMessages
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(128, 128);

  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });

  window_stack_push(s_window, true);
	
	
	 // Make sure the time is displayed from the start
  update_time();

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	
}

static void prv_deinit(void) {
  if (s_window) {
    window_destroy(s_window);
  }
}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}