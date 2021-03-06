#pragma once
#include <stdint.h>

// #define SVC_PULSAR_PHI_MAX 0xffffff
#define SVC_PULSAR_PHI_MAX 0x1000000ULL
#define SVC_PULSAR_F_SAMPLE 128UL
#define SVC_PULSAR_NR_LEDS 2

typedef enum {
	SVC_PULSAR_STATE_STOP = 0,
	SVC_PULSAR_STATE_PULSE
} svc_pulsar_state_t;

typedef struct {
    uint16_t led0;
    uint16_t led1;
    uint16_t duration;
} svc_pulsar_frame_t;

typedef struct {
    char title[2];
    uint16_t hbpm;
    const svc_pulsar_frame_t *frames;
} svc_pulsar_sequence_t;

extern const svc_pulsar_sequence_t svc_pulsar_seqs[];
extern const uint8_t svc_pulsar_seqs_n;

uint64_t svc_pulsar_hbpm_to_dphi(uint16_t hbpm);
uint16_t svc_pulsar_dphi_to_hbpm(uint64_t dphi);
void svc_pulsar_hbpm_set(uint16_t hbpm);
uint16_t svc_pulsar_hbpm_get(void);

uint8_t svc_default_pulsar_get(void);

void svc_aux_timer_pulsar_pulse_handler(void);
void svc_pulsar_reset_phase(void);
void _svc_pulsar_play_repeat(uint8_t rep);
void svc_pulsar_play_repeat(uint8_t pulsar, uint8_t rep);
void svc_pulsar_play(void);
void svc_pulsar_stop(void);
void svc_pulsar_sequence_set(uint8_t seq);
uint8_t svc_pulsar_sequence_get(void);
uint8_t svc_pulsar_state_get(void);
uint8_t svc_pulsar_alarm_repetitions_get(void);
void svc_pulsar_alarm_repetitions_set(uint8_t repetitions);
uint8_t svc_pulsar_metronome_get(void);

void svc_aux_timer_pulsar_measure_handler(void);
void svc_pulsar_measure_tap_handler(void);

uint16_t svc_pulsar_interval_avg_get(void);
uint32_t svc_pulsar_clk_counter_total_get(void);
uint16_t svc_pulsar_clk_counter_max_get(void);

uint8_t svc_pulsar_bp_beat_state_get(void);
void svc_pulsar_bp_beat_state_set(uint8_t state);
uint16_t svc_pulsar_bp_beat_freq_get(void);
void svc_pulsar_bp_beat_freq_set(uint16_t freq);
uint8_t svc_pulsar_bp_beat_dur_get(void);
void svc_pulsar_bp_beat_dur_set(uint8_t dur);
uint8_t svc_pulsar_bp_bar_state_get(void);
void svc_pulsar_bp_bar_state_set(uint8_t state);
uint16_t svc_pulsar_bp_bar_signature_get(void);
void svc_pulsar_bp_bar_signature_set(uint16_t signature);
uint8_t svc_pulsar_bp_multishift_get(void);
void svc_pulsar_bp_multishift_set(uint8_t multishift);
uint16_t svc_pulsar_bp_bar_freq_get(void);
void svc_pulsar_bp_bar_freq_set(uint16_t freq);
uint8_t svc_pulsar_bp_bar_dur_get(void);
void svc_pulsar_bp_bar_dur_set(uint8_t dur);
void svc_pulsar_bp_metronome_set_enable(uint8_t state);
uint8_t svc_pulsar_bp_metronome_get_enable(void);

// void svc_pulsar_default_set(uint8_t pulsar_idx);
// uint8_t svc_pulsar_default_get(void);
