#pragma once
#include <stdint.h>

#define SVC_COUNTDOWN_NR 11
#define SVC_COUNTDOWN_SUCCESSOR_NONE (SVC_COUNTDOWN_NR)

typedef enum {
	SVC_COUNTDOWN_STATE_STOP = 0,
	SVC_COUNTDOWN_STATE_RUN,
	SVC_COUNTDOWN_STATE_PAUSE,
} svc_countdown_state_t;

#define SVC_COUNTDOWN_COMMON \
	int8_t h; \
	int8_t m; \
	int8_t s; \
	svc_countdown_state_t state; \
	uint8_t melody; \
	uint8_t pulsar; \
	uint8_t successor; \
	uint8_t metronome; \

typedef struct {
	SVC_COUNTDOWN_COMMON
} svc_countdown_t;

extern const uint8_t svc_countdowns_n;

void svc_countdown_init(void);
void svc_countdown_get(uint8_t index, svc_countdown_t *out);
void svc_countdown_set_time(uint8_t index, uint8_t h, uint8_t m, uint8_t s);
void svc_countdown_start(uint8_t index);
void svc_countdown_stop(uint8_t index);
void svc_countdown_play_pause(uint8_t index);
void svc_countdown_process(void);
uint8_t svc_countdown_get_n_running(void);
uint8_t svc_coundown_get_running(void);
uint8_t svc_countdowns_are_running(void);
uint8_t svc_countdown_get_pending(void);
void svc_countdown_clear_pending(void);
void svc_countdown_set_melody(uint8_t index, uint8_t melody);
void svc_countdown_set_pulsar(uint8_t index, uint8_t pulsar);
void svc_countdown_set_successor(uint8_t index, uint8_t successor);
void svc_countdown_set_metronome(uint8_t index, uint8_t state);
void svc_countdown_draw_popup(void);
