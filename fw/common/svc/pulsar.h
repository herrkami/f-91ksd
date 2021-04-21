#pragma once
#include <stdint.h>

#define SVC_PULSAR_PHI_MAX 0xffffff
#define SVC_PULSAR_F_SAMPLE 128
#define SVC_PULSAR_NR_LEDS 2

typedef struct {
    uint16_t led0;
    uint16_t led1;
    uint16_t duration;
} svc_pulsar_frame_t;

typedef struct {
    char title[2];
    uint16_t dphi;
    const svc_pulsar_frame_t *frames;
} svc_pulsar_sequence_t;

extern const svc_pulsar_sequence_t svc_pulsar_seqs[];
extern const uint8_t svc_pulsar_seqs_n;

uint32_t svc_pulsar_bbpm_to_dphi(uint16_t bbpm);
uint16_t svc_pulsar_dphi_to_bbpm(uint16_t dphi);
void svc_pulsar_bbpm_set(uint16_t bbpm);
uint16_t svc_pulsar_bbpm_get(void);

void svc_aux_timer_pulsar_handler(void);
void svc_pulsar_play_repeat(uint8_t seq, uint8_t rep);
void svc_pulsar_play(uint8_t seq);
void svc_pulsar_stop(void);
uint8_t svc_pulsar_alarm_repetitions_get(void);
void svc_pulsar_alarm_repetitions_set(uint8_t repetitions);

// void svc_pulsar_default_set(uint8_t pulsar_idx);
// uint8_t svc_pulsar_default_get(void);
