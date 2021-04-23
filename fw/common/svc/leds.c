#include "leds.h"
#include "common/hal/hal.h"
#include "aux_timer.h"
#include <string.h>
#include "platform.h"

static uint8_t rightled_start;
static uint16_t rightled_duration;

static uint8_t caseled_start;
static uint16_t caseled_duration;

void svc_flash_rightled_timed(uint16_t duration) {
    svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_RIGHTLED, 1);
	rightled_start = 1;
	rightled_duration = duration;
}

void svc_aux_timer_rightled_handler(void) {
	if(rightled_duration) {
		if(rightled_start) {
			hal_rightled(1);
			rightled_start = 0;
		}
		rightled_duration--;
	}
	else {
		hal_rightled(0);
		svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_RIGHTLED, 0);
	}
}

void svc_flash_caseled_timed(uint16_t duration) {
    svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_CASELED, 1);
	caseled_start = 1;
	caseled_duration = duration;
}

void svc_aux_timer_caseled_handler(void) {
	if(caseled_duration) {
		if(caseled_start) {
			hal_caseled(1);
			caseled_start = 0;
		}
		caseled_duration--;
	}
	else {
		hal_caseled(0);
		svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_CASELED, 0);
	}
}


// Right LED key press

static uint8_t  SECTION_INFOMEM rightled_key_enable = 1;
static uint16_t SECTION_INFOMEM rightled_key_duration = 3;

void svc_flash_rightled_key(void) {
    if(rightled_key_enable) {
        svc_flash_rightled_timed(rightled_key_duration);
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
        svc_flash_caseled_timed(caseled_minute_duration);
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
