#pragma once
#include <stdint.h>

#define SVC_WEEKDAY_MO (1<<0)
#define SVC_WEEKDAY_TU (1<<1)
#define SVC_WEEKDAY_WE (1<<2)
#define SVC_WEEKDAY_TH (1<<3)
#define SVC_WEEKDAY_FR (1<<4)
#define SVC_WEEKDAY_SA (1<<5)
#define SVC_WEEKDAY_SU (1<<6)

typedef struct {
	uint8_t h;
	uint8_t m;
	uint8_t enable;
	uint8_t days;
	uint8_t melody;
	uint8_t pulsar;
	uint8_t metronome;
} svc_alarm_t;

extern const uint8_t svc_alarms_n;
void svc_alarm_get(uint8_t index, svc_alarm_t *out);
void svc_alarm_set_time(uint8_t index, uint8_t h, uint8_t m);
void svc_alarm_set_enable(uint8_t index, uint8_t enable);
void svc_alarm_set_day(uint8_t index, uint8_t day, uint8_t enable);
void svc_alarm_set_melody(uint8_t index, uint8_t melody);
void svc_alarm_set_pulsar(uint8_t index, uint8_t pulsar);
void svc_alarm_set_metronome(uint8_t index, uint8_t state);

uint8_t svc_alarm_get_any_enabled(void);
uint8_t svc_alarm_get_pending(void);
void svc_alarm_clear_pending(void);
void svc_alarm_process(void);
void svc_alarm_init(void);
void svc_alarm_draw_popup(void);
