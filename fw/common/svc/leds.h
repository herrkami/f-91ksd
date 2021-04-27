#pragma once
#include <stdint.h>

typedef enum {
	SVC_LEDS_RIGHTLED     = (1<<0),
    SVC_LEDS_CASELED      = (1<<1),
	SVC_LEDS_BACKLED      = (1<<2),
} svc_led_timer_required_t;

void svc_led_timer_set_required(svc_led_timer_required_t which, uint8_t state);
uint8_t svc_led_timer_get_required(svc_led_timer_required_t which);

void svc_flash_rightled_timed(uint16_t duration, uint8_t brightness);
void svc_aux_timer_rightled_handler(void);

void svc_flash_caseled_timed(uint16_t duration, uint8_t brightness);
void svc_aux_timer_caseled_handler(void);

void svc_flash_backled_timed(uint16_t duration, uint8_t brightness);
void svc_aux_timer_backled_handler(void);

void svc_aux_timer_leds_handler(void);

uint16_t svc_flash_rightled_key_get_duration(void);
void svc_flash_rightled_key_set_duration(uint16_t d);
uint8_t svc_flash_rightled_key_get_enable(void);
void svc_flash_rightled_key_set_enable(uint8_t e);
void svc_flash_rightled_key(void);

uint16_t svc_flash_caseled_minute_get_duration(void);
void svc_flash_caseled_minute_set_duration(uint16_t d);
uint8_t svc_flash_caseled_minute_get_enable(void);
void svc_flash_caseled_minute_set_enable(uint8_t e);
void svc_flash_caseled_minute(void);
