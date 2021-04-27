#include "leds.h"
#include "common/hal/hal.h"
#include "aux_timer.h"
#include <string.h>
#include "platform.h"

#define BRIGHTNESS_LEVELS_N 4
// Three brightness levels: full, 1/2, 1/4, or 0

static uint8_t rightled_start;
static uint16_t rightled_duration;
static uint8_t rightled_brightness;

static uint8_t caseled_start;
static uint16_t caseled_duration;
static uint8_t caseled_brightness;

static uint8_t backled_start;
static uint16_t backled_duration;
static uint8_t backled_brightness;

static svc_led_timer_required_t led_timer_req = 0;


void svc_led_timer_set_required(svc_led_timer_required_t which, uint8_t state) {
    if(state) {
        led_timer_req |= which;
    }
    else {
        led_timer_req &= ~which;
    }
    svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_LEDS, !!led_timer_req);
}

uint8_t svc_led_timer_get_required(svc_led_timer_required_t which) {
    return !!(led_timer_req & which);
}

void svc_flash_rightled_timed(uint16_t duration, uint8_t brightness) {
    // svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_RIGHTLED, 1);
    svc_led_timer_set_required(SVC_LEDS_RIGHTLED, 1);
	rightled_start = 1;
	rightled_duration = duration;
    rightled_brightness = 1<<brightness;
    rightled_brightness  = rightled_brightness>>1;
}

void svc_aux_timer_rightled_handler(void) {
    static uint8_t counter;
	if(rightled_duration) {
		if (rightled_start) {
            counter = 0;
			rightled_start = 0;
		}
        if (counter < rightled_brightness) {
            hal_rightled(1);
        }
        else {
            hal_rightled(0);
        }

		rightled_duration--;
	}
	else {
		hal_rightled(0);
		// svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_RIGHTLED, 0);
        svc_led_timer_set_required(SVC_LEDS_RIGHTLED, 0);
	}
    counter++;
    counter %= 4;
}

void svc_flash_caseled_timed(uint16_t duration, uint8_t brightness) {
    // svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_CASELED, 1);
    svc_led_timer_set_required(SVC_LEDS_CASELED, 1);
	caseled_start = 1;
	caseled_duration = duration;
    caseled_brightness = 1<<brightness;
    caseled_brightness  = caseled_brightness>>1;
}

void svc_aux_timer_caseled_handler(void) {
    static uint8_t counter;
	if(caseled_duration) {
		if (caseled_start) {
            counter = 0;
			caseled_start = 0;
		}
        if (counter < caseled_brightness) {
            hal_caseled(1);
        }
        else {
            hal_caseled(0);
        }

		caseled_duration--;
	}
	else {
		hal_caseled(0);
		// svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_CASELED, 0);
        svc_led_timer_set_required(SVC_LEDS_CASELED, 0);
	}
    counter++;
    counter %= 4;
}

void svc_flash_backled_timed(uint16_t duration, uint8_t brightness) {
    // svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_RIGHTLED, 1);
    svc_led_timer_set_required(SVC_LEDS_BACKLED, 1);
	backled_start = 1;
	backled_duration = duration;
    backled_brightness = 1<<brightness;
    backled_brightness  = backled_brightness>>1;
}

void svc_aux_timer_backled_handler(void) {
    static uint8_t counter;
	if(backled_duration) {
		if (backled_start) {
            counter = 0;
			backled_start = 0;
		}
        if (counter < backled_brightness) {
            hal_backled(1);
        }
        else {
            hal_backled(0);
        }

		backled_duration--;
	}
	else {
		hal_backled(0);
		// svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_RIGHTLED, 0);
        svc_led_timer_set_required(SVC_LEDS_BACKLED, 0);
	}
    counter++;
    counter %= 4;
}

void svc_aux_timer_leds_handler(void) {
    if (led_timer_req & SVC_LEDS_RIGHTLED) {
        svc_aux_timer_rightled_handler();
    }
    if (led_timer_req & SVC_LEDS_CASELED) {
        svc_aux_timer_caseled_handler();
    }
    if (led_timer_req & SVC_LEDS_BACKLED) {
        svc_aux_timer_backled_handler();
    }
}

// Right LED key press

static uint8_t  SECTION_INFOMEM rightled_key_enable = 1;
static uint16_t SECTION_INFOMEM rightled_key_duration = 32;

void svc_flash_rightled_key(void) {
    if(rightled_key_enable) {
        svc_flash_rightled_timed(rightled_key_duration, 2);
    }
}

uint16_t svc_flash_rightled_key_get_duration(void) {
    return rightled_key_duration;
}

void svc_flash_rightled_key_set_duration(uint16_t d) {
    rightled_key_duration = d;
}

uint8_t svc_flash_rightled_key_get_enable(void) {
    return rightled_key_enable;
}

void svc_flash_rightled_key_set_enable(uint8_t e) {
    rightled_key_enable = e;
}

// Case LED minute

static uint8_t  SECTION_INFOMEM caseled_minute_enable = 0;
static uint16_t SECTION_INFOMEM caseled_minute_duration = 3;

void svc_flash_caseled_minute(void) {
	static uint8_t minute_last = 255;

    hal_rtc_timedate_t td;
	hal_rtc_get(&td);

    if(td.m == minute_last)
        return;
    minute_last = td.m;

    if(caseled_minute_enable) {
        svc_flash_caseled_timed(caseled_minute_duration, 4);
    }
}

uint16_t svc_flash_caseled_minute_get_duration(void) {
    return caseled_minute_duration;
}

void svc_flash_caseled_minute_set_duration(uint16_t d) {
    caseled_minute_duration = d;
}

uint8_t svc_flash_caseled_minute_get_enable(void) {
    return caseled_minute_enable;
}

void svc_flash_caseled_minute_set_enable(uint8_t e) {
    caseled_minute_enable = e;
}
