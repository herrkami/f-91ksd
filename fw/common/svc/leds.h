#pragma once
#include <stdint.h>

void svc_flash_rightled_timed(uint16_t duration);
void svc_aux_timer_rightled_handler(void);

void svc_flash_caseled_timed(uint16_t duration);
void svc_aux_timer_caseled_handler(void);

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
