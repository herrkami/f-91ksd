#pragma once
#include <stdint.h>
#include "common/hal/hal.h"

void svc_bat_sample(void);
uint16_t svc_bat_mv_get(void);
uint16_t svc_bat_adc_get(void);
uint16_t svc_bat_percentage_get(void);
uint16_t svc_bat_days_running_get(void);
void svc_bat_days_running_set(uint16_t days);
uint16_t svc_bat_days_remaining_get(void);
