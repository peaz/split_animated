/*
   ** SplitText Watch.
   Inspired by TextWatch
   With Date, 24H display and Week #
   Hour and Min has inversed background
   Pluses on new hours
   Now with Animation!
 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "english_time.h"

#define MY_UUID { 0xFF, 0xD6, 0x03, 0x73, 0x4F, 0xA0, 0x4D, 0x2A, 0x91, 0xFC, 0xF1, 0x15, 0x95, 0x95, 0x90, 0x71 }
PBL_APP_INFO(MY_UUID,
             "SplitText2", "atpeaz.com",
             2, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

#define TIME_SLOT_ANIMATION_DURATION 250

Window window;

TextLayer hourLayer;
TextLayer minLayer;
TextLayer dateLayer;
TextLayer day24weekLayer;

static char str_hour[20];
static char str_min[20];
static char str_date[30];
static char str_day24week[30];
static bool init_flag = true;
static PropertyAnimation hour_animation;
static PropertyAnimation min_animation;
static bool busy_animating = false;

void slideout_hour() {
  busy_animating = true;
  GRect hour_outgoing = GRect(-144, 16, 144, 44);
  property_animation_init_layer_frame(&hour_animation, &hourLayer.layer, NULL, &hour_outgoing);
  animation_set_duration(&hour_animation.animation, TIME_SLOT_ANIMATION_DURATION);
  animation_set_curve(&hour_animation.animation, AnimationCurveLinear);
  animation_schedule(&hour_animation.animation);
}

void slidein_hour() {
  busy_animating = true;
  GRect hour_frame = GRect(0, 16, 144, 44);
  GRect hour_incoming = GRect(144, 16, 144, 44);
  property_animation_init_layer_frame(&hour_animation, &hourLayer.layer, &hour_incoming, &hour_frame);
  animation_set_duration(&hour_animation.animation, TIME_SLOT_ANIMATION_DURATION);
  animation_set_curve(&hour_animation.animation, AnimationCurveLinear);
  animation_schedule(&hour_animation.animation);
}

void slideout_min() {
  busy_animating = true;
  GRect min_outgoing = GRect(-144, 56, 144, 168-56);
  property_animation_init_layer_frame(&min_animation, &minLayer.layer, NULL, &min_outgoing);
  animation_set_duration(&min_animation.animation, TIME_SLOT_ANIMATION_DURATION);
  animation_set_curve(&min_animation.animation, AnimationCurveLinear);
  animation_schedule(&min_animation.animation);
}

void slidein_min() {
  busy_animating = true;
  GRect min_frame = GRect(0, 56, 144, 168-56);
  GRect min_incoming = GRect(144, 56, 144, 168-56);
  property_animation_init_layer_frame(&min_animation, &minLayer.layer, &min_incoming, &min_frame);
  animation_set_duration(&min_animation.animation, TIME_SLOT_ANIMATION_DURATION);
  animation_set_curve(&min_animation.animation, AnimationCurveLinear);
  animation_schedule(&min_animation.animation);
}

void hour_animation_stopped(Animation *animation, void *data) {
  text_layer_set_text(&hourLayer, str_hour);
  slidein_hour();
  busy_animating = false;
}

void min_animation_stopped(Animation *animation, void *data) {
  text_layer_set_text(&minLayer, str_min);
  slidein_min();
  busy_animating = false;
}


static void update_watch(PblTm* t) {

  char cur_hour[20];
  strcpy(cur_hour, str_hour);

  english_time(t->tm_hour, t->tm_min, str_hour, str_min);
  string_format_time(str_date, sizeof(str_date), "%e %B %Y", t);
  string_format_time(str_day24week, sizeof(str_day24week), "%A | %H%M | Week %W", t);
  text_layer_set_text(&dateLayer, str_date);
  text_layer_set_text(&day24weekLayer, str_day24week);

  if(init_flag){
    text_layer_set_text(&hourLayer, str_hour);
    text_layer_set_text(&minLayer, str_min);
  }
  else {
    slideout_min();
    animation_set_handlers(&min_animation.animation, (AnimationHandlers){
      .stopped = (AnimationStoppedHandler)min_animation_stopped
      }, NULL);
    if(strcmp(cur_hour,str_hour) != 0) {
      slideout_hour();
      animation_set_handlers(&hour_animation.animation, (AnimationHandlers){
        .stopped = (AnimationStoppedHandler)hour_animation_stopped
      }, NULL);      
      vibes_short_pulse();  
    }
  }
}

// Called once per minute
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  if(busy_animating) return;
  update_watch(t->tick_time);  
}

// Handle the start-up of the app
void handle_init_app(AppContextRef app_ctx) {

  // Create our app's base window
  window_init(&window, "SplitText2");
  window_stack_push(&window, true);
  window_set_background_color(&window, GColorBlack);

  // Init the text layers used to show the time
  // min
  text_layer_init(&minLayer, GRect(0, 56, 144, 168-56));
  text_layer_set_text_color(&minLayer, GColorWhite);
  text_layer_set_background_color(&minLayer, GColorClear);
  text_layer_set_font(&minLayer, fonts_get_system_font(FONT_KEY_GOTHAM_42_LIGHT));
  text_layer_set_text_alignment(&minLayer, GTextAlignmentRight);
  layer_add_child(&window.layer, &minLayer.layer);

   // hour
  text_layer_init(&hourLayer, GRect(0, 16, 144, 44));
  text_layer_set_text_color(&hourLayer, GColorBlack);
  text_layer_set_background_color(&hourLayer, GColorWhite);
  text_layer_set_font(&hourLayer, fonts_get_system_font(FONT_KEY_GOTHAM_42_BOLD));
  text_layer_set_text_alignment(&hourLayer, GTextAlignmentRight);
  layer_add_child(&window.layer, &hourLayer.layer);

  // date
  text_layer_init(&dateLayer, GRect(0, 0, 144, 18));
  text_layer_set_text_color(&dateLayer, GColorWhite);
  text_layer_set_background_color(&dateLayer, GColorClear);
  text_layer_set_font(&dateLayer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(&dateLayer, GTextAlignmentCenter);
  layer_add_child(&window.layer, &dateLayer.layer);

  // day24week
  text_layer_init(&day24weekLayer, GRect(0, 168-18, 144, 18));
  text_layer_set_text_color(&day24weekLayer, GColorBlack);
  text_layer_set_background_color(&day24weekLayer, GColorWhite);
  text_layer_set_font(&day24weekLayer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(&day24weekLayer, GTextAlignmentCenter);
  layer_add_child(&window.layer, &day24weekLayer.layer); 
  
  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  PblTm t;
  get_time(&t);
  update_watch(&t);

  init_flag = false;
}


// The main event/run loop for our app
void pbl_main(void *params) {
  PebbleAppHandlers handlers = {

    // Handle app start
    .init_handler = &handle_init_app,

    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }

  };
  app_event_loop(params, &handlers);
}
