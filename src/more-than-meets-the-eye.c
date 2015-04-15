//RESOURCE_ID_IMAGE_AUTOBOTS
//RESOURCE_ID_IMAGE_DECEPTICONS
//RESOURCE_ID_FONT_AUTOBOTS_28

#include <pebble.h>

static Window *window;
static int tap_count = 0;
TextLayer *text_date_layer;
TextLayer *text_time_layer;
TextLayer *text_parking_layer;
BitmapLayer *logo_layer;
Layer *line_layer;
GBitmap *autobots_bmp;
GBitmap *decepticons_bmp;

void handle_tap(AccelAxisType axis, int32_t direction) {
  if (axis == 2) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Z TAP");
  } else if (axis == 1) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Y TAP");
  }

  if(axis == 0) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "X TAP");

    switch(tap_count) {
      case 0:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "I think I'm setting the decepticon logo");
        bitmap_layer_set_bitmap(logo_layer, decepticons_bmp);
        tap_count = 1;
        break;
      case 1:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "I think I'm setting the autobot logo");
        bitmap_layer_set_bitmap(logo_layer, autobots_bmp);
        tap_count = 0;
        break;
    }
  }
}

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  static char time_text[] = "00:00";
  static char date_text[] = "Xxxxxxxxx 00";
  static char parking_text[] = "Xxxxxx Xxx";
  static char* weeks[] = {NULL, NULL, NULL, NULL, NULL};
  weeks[0] = "1st ";
  weeks[1] = "2nd ";
  weeks[2] = "3rd ";
  weeks[3] = "4th ";
  weeks[4] = "5th ";

  char *time_format;

  //  bool day_changed = units_changed->DAY_UNIT == 1;
  //  if (day_changed) {
  strftime(date_text, sizeof(date_text), "%B %e", tick_time);
  text_layer_set_text(text_date_layer, date_text);

  int week_num =  (tick_time->tm_mday - 1) / 7;
  char *week_num_text = weeks[week_num];

  char *day_name = "Xxxxxxxxx";
  memset(day_name, 0, sizeof(day_name));
  strftime(day_name, sizeof(day_name), "%a", tick_time);

  memset(parking_text, 0, sizeof(parking_text));
  strcat(parking_text, week_num_text);
  strcat(parking_text, day_name);
  text_layer_set_text(text_parking_layer, parking_text);
  //  }

  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  strftime(time_text, sizeof(time_text), time_format, tick_time);

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  text_layer_set_text(text_time_layer, time_text);
}

void window_load(Window *window) {
  window_set_background_color(window, GColorWhite);

  Layer *window_layer = window_get_root_layer(window);

  text_date_layer = text_layer_create(GRect(8, 98, 144-8, 168-68));
  text_layer_set_text_color(text_date_layer, GColorBlack);
  text_layer_set_background_color(text_date_layer, GColorClear);
  text_layer_set_font(text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TRANSFORMERS_21)));
  layer_add_child(window_layer, text_layer_get_layer(text_date_layer));

  text_time_layer = text_layer_create(GRect(7, 122, 144-7, 168-92));
  text_layer_set_text_color(text_time_layer, GColorBlack);
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_font(text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TRANSFORMERS_38)));
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));

  text_parking_layer = text_layer_create(GRect(8,70, 144-6, 168-68));
  text_layer_set_text_color(text_parking_layer, GColorBlack);
  text_layer_set_background_color(text_parking_layer, GColorClear);
  text_layer_set_font(text_parking_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TRANSFORMERS_21)));
  layer_add_child(window_layer, text_layer_get_layer(text_parking_layer));

  logo_layer = bitmap_layer_create(GRect(24,2,96,74));
  autobots_bmp = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_AUTOBOTS);
  decepticons_bmp = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DECEPTICONS);
  bitmap_layer_set_bitmap(logo_layer, autobots_bmp);
  layer_add_child(window_layer, bitmap_layer_get_layer(logo_layer));

};

void window_unload(Window * win) {
  tick_timer_service_unsubscribe();
  accel_tap_service_unsubscribe();

  gbitmap_destroy(autobots_bmp);
  gbitmap_destroy(decepticons_bmp);

  bitmap_layer_destroy(logo_layer);
  text_layer_destroy(text_parking_layer);
  text_layer_destroy(text_time_layer);
  text_layer_destroy(text_date_layer);
};

static void init(void) {
  accel_tap_service_subscribe(&handle_tap);

  window = window_create();
  window_set_window_handlers(window, (WindowHandlers)
      {
      .load = window_load,
      .unload = window_unload
    });
  const bool animated = true;
  window_stack_push(window, animated);
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
