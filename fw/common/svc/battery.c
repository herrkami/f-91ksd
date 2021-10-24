#include "battery.h"
#include "common/hal/hal.h"
#include "platform.h"
#include "util.h"

#define ADC_MV_REF 2000L
#define ADC_MAX 0xFFFL

static uint16_t bat_adc_initial = (3100/2*ADC_MAX/ADC_MV_REF);  // battery starts with 3.1 V
static uint16_t bat_adc_final = (2750/2*ADC_MAX/ADC_MV_REF);  // battery ends with 2.75 V

static uint16_t bat_mv;
static uint16_t bat_adc;
static uint16_t SECTION_INFOMEM bat_days_running = 1;

uint16_t svc_bat_adc_to_mv(uint16_t adc) {
	uint16_t mv = ((uint32_t)adc*ADC_MV_REF*2)/ADC_MAX;
	return mv;
}

void svc_bat_sample(void) {
	bat_adc = hal_adc_sample_battery();
	bat_mv = svc_bat_adc_to_mv(bat_adc);
}

uint16_t svc_bat_mv_get(void) {
	return bat_mv;
}

uint16_t svc_bat_adc_get(void) {
	return bat_adc;
}

uint16_t svc_bat_percentage_get(void) {
	int16_t percentage = (100*(bat_adc - bat_adc_final))/(bat_adc_initial - bat_adc_final);
	percentage = percentage < 0 ? 0 : percentage;
	return percentage;
}

void svc_bat_adc_initial_set(uint16_t adc) {
	bat_adc_initial = adc;
}

uint16_t svc_bat_days_running_get(void) {
	return bat_days_running;
}

void svc_bat_days_running_set(uint16_t days) {
	bat_days_running = days;
}

uint16_t svc_bat_days_remaining_get(void) {
	const uint16_t multi = 32;

	int32_t m = (multi*(bat_adc_initial - bat_adc))/bat_days_running;
	uint16_t tr = (multi*(bat_adc_final - bat_adc))/m - bat_days_running;
	return tr;
}
