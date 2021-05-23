#include "backlight.h"
#include "leds.h"
#include "common/hal/hal.h"
#include "platform.h"

static uint8_t SECTION_INFOMEM bl_brightness = 1;
static uint8_t SECTION_INFOMEM bl_timeout = 3;
static uint8_t SECTION_INFOMEM fl_brightness = 1;
static uint8_t SECTION_INFOMEM fl_timeout = 3;
static uint8_t timer;
static uint8_t state;

#define STATE_OFF			0
#define STATE_FLASHLIGHT	(1<<1)
#define STATE_BACKLIGHT		(1<<2)



void svc_backlight_brightness_set(uint8_t brightness) {
	bl_brightness = brightness;
}
uint8_t svc_backlight_brightness_get(void) {
	return bl_brightness;
}

void svc_backlight_timeout_set(uint8_t timeout) {
	bl_timeout = timeout;
}

uint8_t svc_backlight_timeout_get(void) {
	return bl_timeout;
}

void svc_flashlight_brightness_set(uint8_t brightness) {
	fl_brightness = brightness;
}
uint8_t svc_flashlight_brightness_get(void) {
	return fl_brightness;
}

void svc_flashlight_timeout_set(uint8_t timeout) {
	fl_timeout = timeout;
}

uint8_t svc_flashlight_timeout_get(void) {
	return fl_timeout;
}

// void svc_backlight_process(svc_main_proc_event_t ev) {
// 	if(ev & SVC_MAIN_PROC_EVENT_KEY_UP_LONG) {
// 		timer = bl_timeout*4;
// 		hal_backlight_set(bl_brightness);
// 	}
// 	if((ev & (SVC_MAIN_PROC_EVENT_KEY_ANY | SVC_MAIN_PROC_EVENT_KEY_ANY_LONG)) && timer) {
// 		timer = bl_timeout*4;
// 	}
//
// 	if(ev & SVC_MAIN_PROC_EVENT_TICK) { //decrement
// 		if(timer) {
// 			timer--;
// 		}
// 		else {
// 			hal_backlight_set(0);
// 		}
// 	}
// }
uint8_t svc_backlight_process(svc_main_proc_event_t ev) {
	if(ev & SVC_MAIN_PROC_EVENT_KEY_UP_LONG) {
		if(state == STATE_OFF) {
			timer = bl_timeout*4;
			svc_flash_backled_timed(bl_timeout*128, bl_brightness);
			state = STATE_BACKLIGHT;
		}
		else if(state == STATE_BACKLIGHT) {
			timer = bl_timeout*4;
			svc_flash_backled_timed(0, bl_brightness);
			svc_flash_caseled_timed(fl_timeout*128, fl_brightness);
			state = STATE_FLASHLIGHT;
		}
		else if(state == STATE_FLASHLIGHT) {
			// timer = bl_timeout*4;
			// svc_flash_caseled_timed(0, fl_brightness);
			// svc_flash_backled_timed(bl_timeout*128, bl_brightness);
			// state = STATE_BACKLIGHT;
			timer = 0;
			svc_flash_caseled_timed(0, fl_brightness);
			state = STATE_OFF;
			return 1;
		}
	}
	else if((ev & (SVC_MAIN_PROC_EVENT_KEY_ANY | SVC_MAIN_PROC_EVENT_KEY_ANY_LONG)) && timer) {
		switch (state) {
			case STATE_BACKLIGHT:
				timer = bl_timeout*4;
				svc_flash_backled_timed(bl_timeout*128, bl_brightness);
			break;
			case STATE_FLASHLIGHT:
				timer = fl_timeout*4;
				svc_flash_caseled_timed(fl_timeout*128, fl_brightness);
				// timer = 0;
				// svc_flash_caseled_timed(0, fl_brightness);
				// state = STATE_OFF;
				return 1;
			break;
		}
	}

	if(ev & SVC_MAIN_PROC_EVENT_TICK) { //decrement
		if(timer) {
			timer--;
			if(timer == 0) {
				state = STATE_OFF;
				// This prevent artifacts 
				svc_flash_caseled_timed(0, fl_brightness);
				svc_flash_backled_timed(0, bl_brightness);
			}
		}
	}
	return 0;
}

void svc_backlight_start(void) {

}
