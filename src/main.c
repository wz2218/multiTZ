#include <pebble.h>

#define DATE_Y 83
#define TIME_Y 108
	
#define TIMEZONE_ONE_Y 11
#define TIMEZONE_TWO_Y 45
	
#define HOUR_VIBRATION_START 8
#define HOUR_VIBRATION_END 23
	
#define INIT_TZ1_NAME "CHN"
#define INIT_TZ2_NAME "GER"
#define INIT_LOCAL_OFFSET (-7)
#define INIT_TZ1_OFFSET (+8)
#define INIT_TZ2_OFFSET (+2)
		
#define TOTAL_DATE_DIGITS 6
static GBitmap *date_digits_images[TOTAL_DATE_DIGITS];
static BitmapLayer *date_digits_layers[TOTAL_DATE_DIGITS];

const int BIG_DIGIT_IMAGE_RESOURCE_IDS[] = {
	RESOURCE_ID_IMAGE_NUM_0,
	RESOURCE_ID_IMAGE_NUM_1,
	RESOURCE_ID_IMAGE_NUM_2,
	RESOURCE_ID_IMAGE_NUM_3,
	RESOURCE_ID_IMAGE_NUM_4,
	RESOURCE_ID_IMAGE_NUM_5,
	RESOURCE_ID_IMAGE_NUM_6,
	RESOURCE_ID_IMAGE_NUM_7,
	RESOURCE_ID_IMAGE_NUM_8,
	RESOURCE_ID_IMAGE_NUM_9
};

#define TOTAL_TIME_DIGITS 4
static GBitmap *time_digits_images[TOTAL_TIME_DIGITS];
static BitmapLayer *time_digits_layers[TOTAL_TIME_DIGITS];

static GBitmap *tz_one_digits_images[TOTAL_TIME_DIGITS];
static BitmapLayer *tz_one_digits_layers[TOTAL_TIME_DIGITS];

static GBitmap *tz_two_digits_images[TOTAL_TIME_DIGITS];
static BitmapLayer *tz_two_digits_layers[TOTAL_TIME_DIGITS];

const int DATENUM_IMAGE_RESOURCE_IDS[] = {
	RESOURCE_ID_IMAGE_DATENUM_0,
	RESOURCE_ID_IMAGE_DATENUM_1,
	RESOURCE_ID_IMAGE_DATENUM_2,
	RESOURCE_ID_IMAGE_DATENUM_3,
	RESOURCE_ID_IMAGE_DATENUM_4,
	RESOURCE_ID_IMAGE_DATENUM_5,
	RESOURCE_ID_IMAGE_DATENUM_6,
	RESOURCE_ID_IMAGE_DATENUM_7,
	RESOURCE_ID_IMAGE_DATENUM_8,
	RESOURCE_ID_IMAGE_DATENUM_9
};


#ifndef CONFIG_H
#define CONFIG_H

const int day_month_x[] = {
	87,
	55,
	115
};

const char *DAY_NAME_ENGLISH[] = {
	"SUN",
	"MON",
	"TUE",
	"WED",
	"THU",
	"FRI",
	"SAT"
};

#endif
	
static int valueRead, valueWritten;

#define SETTINGS_KEY 77
	
typedef struct persist {
	char tz_one_name[4];
	int tz_one_offset;
	char tz_two_name[4];
	int tz_two_offset;
	int local_offset;
	int hourlyVibe;
} __attribute__((__packed__)) persist;

persist settings = {
	.tz_one_name = INIT_TZ1_NAME,
	.tz_one_offset = INIT_TZ1_OFFSET,
	.tz_two_name = INIT_TZ2_NAME,
	.tz_two_offset = INIT_TZ2_OFFSET,
	.local_offset = INIT_LOCAL_OFFSET,
	.hourlyVibe = 1
};
	
Window *my_window;

static GBitmap *background_image;
static BitmapLayer *background_layer;
// TODO: Handle 12/24 mode preference when it's exposed.
static GBitmap *time_format_image;
static BitmapLayer *time_format_layer;

static TextLayer *day_text_layer;
static TextLayer *tz_one_text_layer;
static TextLayer *tz_two_text_layer;

static void set_container_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id, GPoint origin) {
	GBitmap *old_image = *bmp_image;

	*bmp_image = gbitmap_create_with_resource(resource_id);
	GRect frame = (GRect) {
		.origin = origin,
		.size = (*bmp_image)->bounds.size
	};
	bitmap_layer_set_bitmap(bmp_layer, *bmp_image);
	layer_set_frame(bitmap_layer_get_layer(bmp_layer), frame);

	if (old_image != NULL) {
		gbitmap_destroy(old_image);
	}
}

static unsigned short get_display_hour(unsigned short hour) {
	if (clock_is_24h_style()) {
		return hour;
	}

	unsigned short display_hour = hour % 12;

	// Converts "0" to "12"
	return display_hour ? display_hour : 12;
}

unsigned short the_last_hour = 25;

static void update_display(struct tm *current_time) {
  
	unsigned short display_hour = get_display_hour(current_time->tm_hour);
	short tzOne_hour = current_time->tm_hour + (settings.tz_one_offset - settings.local_offset);
	short tzTwo_hour = current_time->tm_hour + (settings.tz_two_offset - settings.local_offset);
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "update_display load local offset %d", settings.local_offset);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "update_display load tz1 offset %d", settings.tz_one_offset);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "update_display load tz2 offset %d", settings.tz_two_offset);

	if (tzOne_hour >= 24)	tzOne_hour -= 24;
	if (tzOne_hour <   0)	tzOne_hour += 24;
	if (tzTwo_hour >= 24)	tzTwo_hour -= 24;
	if (tzTwo_hour <   0)	tzTwo_hour += 24;	
  
	//Hour
	if (display_hour/10 != 0) {
		layer_set_hidden(bitmap_layer_get_layer(time_digits_layers[0]), false);
		set_container_image(&time_digits_images[0], time_digits_layers[0], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour/10], GPoint(4, TIME_Y));
	} else {
		layer_set_hidden(bitmap_layer_get_layer(time_digits_layers[0]), true);
	}
	set_container_image(&time_digits_images[1], time_digits_layers[1], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour%10], GPoint(37, TIME_Y));

	if(current_time->tm_min == 0
	  && current_time->tm_hour >= HOUR_VIBRATION_START
	  && current_time->tm_hour <= HOUR_VIBRATION_END
	  && settings.hourlyVibe)
	{
		vibes_double_pulse();
	}

	if(current_time->tm_min == 0 && current_time->tm_hour == 0)
	{
		vibes_double_pulse();
	}

	//Minute
	set_container_image(&time_digits_images[2], time_digits_layers[2], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min/10], GPoint(80, TIME_Y));
	set_container_image(&time_digits_images[3], time_digits_layers[3], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min%10], GPoint(111, TIME_Y));

	// More Timezones
	if (tzOne_hour/10 != 0) {
		layer_set_hidden(bitmap_layer_get_layer(tz_one_digits_layers[0]), false);
		set_container_image(&tz_one_digits_images[0], tz_one_digits_layers[0], DATENUM_IMAGE_RESOURCE_IDS[tzOne_hour/10], GPoint(71, TIMEZONE_ONE_Y));
	} else {
		layer_set_hidden(bitmap_layer_get_layer(tz_one_digits_layers[0]), true);
	}
	set_container_image(&tz_one_digits_images[1], tz_one_digits_layers[1], DATENUM_IMAGE_RESOURCE_IDS[tzOne_hour%10], GPoint(83, TIMEZONE_ONE_Y));
	
	set_container_image(&tz_one_digits_images[2], tz_one_digits_layers[2], DATENUM_IMAGE_RESOURCE_IDS[current_time->tm_min/10], GPoint(103, TIMEZONE_ONE_Y));
	set_container_image(&tz_one_digits_images[3], tz_one_digits_layers[3], DATENUM_IMAGE_RESOURCE_IDS[current_time->tm_min%10], GPoint(115, TIMEZONE_ONE_Y));
		
	if (tzTwo_hour/10 != 0) {
		layer_set_hidden(bitmap_layer_get_layer(tz_two_digits_layers[0]), false);
		set_container_image(&tz_two_digits_images[0], tz_two_digits_layers[0], DATENUM_IMAGE_RESOURCE_IDS[tzTwo_hour/10], GPoint(71, TIMEZONE_TWO_Y));
	} else {
		layer_set_hidden(bitmap_layer_get_layer(tz_two_digits_layers[0]), true);
	}
	set_container_image(&tz_two_digits_images[1], tz_two_digits_layers[1], DATENUM_IMAGE_RESOURCE_IDS[tzTwo_hour%10], GPoint(83, TIMEZONE_TWO_Y));
	
	set_container_image(&tz_two_digits_images[2], tz_two_digits_layers[2], DATENUM_IMAGE_RESOURCE_IDS[current_time->tm_min/10], GPoint(103, TIMEZONE_TWO_Y));
	set_container_image(&tz_two_digits_images[3], tz_two_digits_layers[3], DATENUM_IMAGE_RESOURCE_IDS[current_time->tm_min%10], GPoint(115, TIMEZONE_TWO_Y));
	
	if (the_last_hour != display_hour){
	  
	// Day of week
	text_layer_set_text(day_text_layer, DAY_NAME_ENGLISH[current_time->tm_wday]); 

	// Day
	if (current_time->tm_mday/10 != 0) {
		layer_set_hidden(bitmap_layer_get_layer(date_digits_layers[0]), false);
		set_container_image(&date_digits_images[0], date_digits_layers[0], DATENUM_IMAGE_RESOURCE_IDS[current_time->tm_mday/10], GPoint(day_month_x[0], DATE_Y));
	} else {
		layer_set_hidden(bitmap_layer_get_layer(date_digits_layers[0]), true);
	}
	set_container_image(&date_digits_images[1], date_digits_layers[1], DATENUM_IMAGE_RESOURCE_IDS[current_time->tm_mday%10], GPoint(day_month_x[0] + 13, DATE_Y));

	// Month
	if ((current_time->tm_mon+1)/10 != 0) {
		layer_set_hidden(bitmap_layer_get_layer(date_digits_layers[2]), false);
		set_container_image(&date_digits_images[2], date_digits_layers[2], DATENUM_IMAGE_RESOURCE_IDS[(current_time->tm_mon+1)/10], GPoint(day_month_x[1], DATE_Y));
	} else {
		layer_set_hidden(bitmap_layer_get_layer(date_digits_layers[2]), true);
	}
	set_container_image(&date_digits_images[3], date_digits_layers[3], DATENUM_IMAGE_RESOURCE_IDS[(current_time->tm_mon+1)%10], GPoint(day_month_x[1] + 13, DATE_Y));

	// Year
	set_container_image(&date_digits_images[4], date_digits_layers[4], DATENUM_IMAGE_RESOURCE_IDS[((1900+current_time->tm_year)%1000)/10], GPoint(day_month_x[2], DATE_Y));
	set_container_image(&date_digits_images[5], date_digits_layers[5], DATENUM_IMAGE_RESOURCE_IDS[((1900+current_time->tm_year)%1000)%10], GPoint(day_month_x[2] + 13, DATE_Y));

	// Time format
	if (!clock_is_24h_style()) {
		if (current_time->tm_hour >= 12) {
			set_container_image(&time_format_image, time_format_layer, RESOURCE_ID_IMAGE_PM_MODE, GPoint(120, 154));
		} else {
			set_container_image(&time_format_image, time_format_layer, RESOURCE_ID_IMAGE_AM_MODE, GPoint(120, 154));
		}
	} else {
		set_container_image(&time_format_image, time_format_layer, RESOURCE_ID_IMAGE_24_HOUR_MODE, GPoint(120, 154));
	}

	the_last_hour = display_hour;
	}
	
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
	update_display(tick_time);
}

static void loadPersistentSettings() {	
	valueRead = persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

static void savePersistentSettings() {
	valueWritten = persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
}

void handle_init(void) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "init'ing");
 	my_window = window_create();

 	window_stack_push(my_window, true);
	
	//loadPersistentSettings();
	
  	Layer *window_layer = window_get_root_layer(my_window);
  	background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  	background_layer = bitmap_layer_create(layer_get_frame(window_layer));
  	bitmap_layer_set_bitmap(background_layer, background_image);
  	layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));

	// Create time format layer
	time_format_layer = bitmap_layer_create(GRect(120, 154, 19 /* width */, 8 /* height */));
	layer_add_child(window_layer, bitmap_layer_get_layer(time_format_layer));
	
	 // Create day layers
    day_text_layer = text_layer_create(GRect(5, 74, 130 /* width */, 30 /* height */));
	layer_add_child(window_layer, text_layer_get_layer(day_text_layer));
	text_layer_set_background_color(day_text_layer, GColorClear);
	text_layer_set_text_color(day_text_layer, GColorWhite);
	text_layer_set_font(day_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	
	// Create time zone 1 text layers
    tz_one_text_layer = text_layer_create(GRect(15, 0, 55 /* width */, 30 /* height */));
	layer_add_child(window_layer, text_layer_get_layer(tz_one_text_layer));
	text_layer_set_background_color(tz_one_text_layer, GColorClear);
	text_layer_set_text_color(tz_one_text_layer, GColorWhite);
	text_layer_set_font(tz_one_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD)); 
	
	// Create time zone 2 text layers
    tz_two_text_layer = text_layer_create(GRect(15, 34, 55 /* width */, 30 /* height */));
	layer_add_child(window_layer, text_layer_get_layer(tz_two_text_layer));
	text_layer_set_background_color(tz_two_text_layer, GColorClear);
	text_layer_set_text_color(tz_two_text_layer, GColorWhite);
	text_layer_set_font(tz_two_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	
	 // Create time and date layers
  	GRect dummy_frame = { {0, 0}, {0, 0} };
 	for (int i = 0; i < TOTAL_TIME_DIGITS; ++i) {
    	time_digits_layers[i] = bitmap_layer_create(dummy_frame);
    	layer_add_child(window_layer, bitmap_layer_get_layer(time_digits_layers[i]));
    }
    for (int i = 0; i < TOTAL_DATE_DIGITS; ++i) {
        date_digits_layers[i] = bitmap_layer_create(dummy_frame);
        layer_add_child(window_layer, bitmap_layer_get_layer(date_digits_layers[i]));
    }
	for (int i = 0; i < TOTAL_TIME_DIGITS; ++i) {
		tz_one_digits_layers[i] = bitmap_layer_create(dummy_frame);
		layer_add_child(window_layer, bitmap_layer_get_layer(tz_one_digits_layers[i]));
	}
	for (int i = 0; i < TOTAL_TIME_DIGITS; ++i) {
		tz_two_digits_layers[i] = bitmap_layer_create(dummy_frame);
		layer_add_child(window_layer, bitmap_layer_get_layer(tz_two_digits_layers[i]));
	}
	
	//Avoid blank screen
	text_layer_set_text(tz_one_text_layer, settings.tz_one_name);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Init load tz1 name %s", settings.tz_one_name);
	text_layer_set_text(tz_two_text_layer, settings.tz_two_name);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Init load tz2 name %s", settings.tz_two_name);
	time_t now = time(NULL);
    struct tm *tick_time = localtime(&now);

    update_display(tick_time);

    tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

void handle_deinit(void) {
	savePersistentSettings();
	
	layer_remove_from_parent(bitmap_layer_get_layer(background_layer));
	bitmap_layer_destroy(background_layer);
	gbitmap_destroy(background_image);
	
	layer_remove_from_parent(bitmap_layer_get_layer(time_format_layer));
	bitmap_layer_destroy(time_format_layer);
	gbitmap_destroy(time_format_image);

	text_layer_destroy(day_text_layer);
	text_layer_destroy(tz_one_text_layer);
	text_layer_destroy(tz_two_text_layer);
		
	for (int i = 0; i < TOTAL_DATE_DIGITS; i++) {
		layer_remove_from_parent(bitmap_layer_get_layer(date_digits_layers[i]));
		gbitmap_destroy(date_digits_images[i]);
		bitmap_layer_destroy(date_digits_layers[i]);
	}

	for (int i = 0; i < TOTAL_TIME_DIGITS; i++) {
		layer_remove_from_parent(bitmap_layer_get_layer(time_digits_layers[i]));
		gbitmap_destroy(time_digits_images[i]);
		bitmap_layer_destroy(time_digits_layers[i]);
	}	

	for (int i = 0; i < TOTAL_TIME_DIGITS; i++) {
		layer_remove_from_parent(bitmap_layer_get_layer(tz_one_digits_layers[i]));
		gbitmap_destroy(tz_one_digits_images[i]);
		bitmap_layer_destroy(tz_one_digits_layers[i]);
	}

	for (int i = 0; i < TOTAL_TIME_DIGITS; i++) {
		layer_remove_from_parent(bitmap_layer_get_layer(tz_two_digits_layers[i]));
		gbitmap_destroy(tz_two_digits_images[i]);
		bitmap_layer_destroy(tz_two_digits_layers[i]);
	}

	window_destroy(my_window);
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}
