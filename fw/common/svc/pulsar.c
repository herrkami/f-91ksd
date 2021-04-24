#include "pulsar.h"
#include "common/hal/hal.h"
#include "aux_timer.h"
#include "platform.h"
#include "leds.h"
#include "util.h"

static const svc_pulsar_frame_t *frame_cur = 0;
static const svc_pulsar_frame_t *frame_start = 0;
static uint64_t pulsar_phi;
static uint64_t pulsar_phinext;
static uint64_t pulsar_seq = 2;
static uint8_t pulsar_metronome;
static uint8_t repeat;
static uint64_t pulsar_dphi = 120/60*SVC_PULSAR_PHI_MAX/SVC_PULSAR_F_SAMPLE;
static const uint16_t pulsar_clk_div = 16;

static uint16_t clk_counter;
static uint16_t clk_counter_max;
static uint32_t clk_counter_total;
static uint16_t tap_counter;
static uint32_t interval_avg;

uint64_t svc_pulsar_hbpm_to_dphi(uint16_t hbpm) {
    // 1 hbpm = 1BPM*100
    // f = hbpm/6000
    // f = dphi/phi_max*f_sample
    // dphi = hbpm/6000*phi_max/f_sample
    uint64_t dphi = ((uint64_t)hbpm*SVC_PULSAR_PHI_MAX*16)/6000/SVC_PULSAR_F_SAMPLE;
    uint16_t mod = dphi%16;
    dphi /= 16;
    if (mod > 7) {
        dphi += 1;
    }
    return dphi;
}

uint16_t svc_pulsar_dphi_to_hbpm(uint64_t dphi) {
    // 1 hbpm = 1BPM*100
    // f = hbpm/6000
    // f = dphi/phi_max*f_sample
    // hbpm = dphi*6000*f_sample/phi_max
    uint32_t hbpm = (uint64_t)(dphi*6000*SVC_PULSAR_F_SAMPLE*16)/SVC_PULSAR_PHI_MAX;
    uint16_t mod = hbpm%16;
    hbpm /= 16;
    if (mod > 7) {
        hbpm += 1;
    }
    return (uint16_t)hbpm;
}

void svc_pulsar_hbpm_set(uint16_t hbpm) {
    uint16_t _hbpm = CLAMP(hbpm, 100, 51200);
    pulsar_dphi = svc_pulsar_hbpm_to_dphi(_hbpm);
}

uint16_t svc_pulsar_hbpm_get(void) {
    uint16_t hbpm = svc_pulsar_dphi_to_hbpm(pulsar_dphi);
    return hbpm;
}

void svc_aux_timer_pulsar_pulse_handler(void) {
    if(frame_cur) {
        // Duration of the last frame is always 0
        if(frame_cur->duration == 0) {
    		if(repeat != 255) { /* repeat==255 -> repeat infinite */
    			repeat--;
    		}
    		if(repeat) {
    			frame_cur = frame_start;
    		}
    		else {
    			svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_PULSAR_PULSE, 0);
    		}
    	}
    	else {
            pulsar_phi += pulsar_dphi;
            if (pulsar_phi >= pulsar_phinext) {
                svc_flash_rightled_timed(frame_cur->led0);
                svc_flash_caseled_timed(frame_cur->led1);
                uint8_t div = frame_cur->duration ? frame_cur->duration : 1;
                pulsar_phinext += SVC_PULSAR_PHI_MAX/div;
                frame_cur++;
            }
            if (pulsar_phi >= SVC_PULSAR_PHI_MAX) {
                pulsar_phi -= SVC_PULSAR_PHI_MAX;
                if (pulsar_phinext >= SVC_PULSAR_PHI_MAX) {
                    pulsar_phinext -= SVC_PULSAR_PHI_MAX;
                }
            }
    	}
	}
	else {
		svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_MELODY, 0);
	}
}

void svc_pulsar_reset_phase(void) {
    frame_cur = frame_start;
    pulsar_phi = SVC_PULSAR_PHI_MAX;
}


void svc_pulsar_play_repeat(uint8_t rep){
    frame_cur = svc_pulsar_seqs[pulsar_seq].frames;
    frame_start = svc_pulsar_seqs[pulsar_seq].frames;
    pulsar_phi = SVC_PULSAR_PHI_MAX;
    uint8_t div = frame_cur->duration ? frame_cur->duration : 1;
    pulsar_phinext = SVC_PULSAR_PHI_MAX/div;
    repeat = (!rep)?255:rep;
	svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_PULSAR_PULSE, 1);
}

void svc_pulsar_play(void){
    svc_pulsar_play_repeat(1);
}

void svc_pulsar_sequence_set(uint8_t seq) {
    pulsar_seq = seq;
}

uint8_t svc_pulsar_sequence_get(void) {
    return pulsar_seq;
}

void svc_pulsar_stop(void){
    svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_PULSAR_PULSE, 0);
}

uint8_t svc_pulsar_state_get(void){
    svc_aux_timer_required_t aux_timer = svc_aux_timer_get_required();
    if (aux_timer & SVC_AUX_TIMER_REQUIRED_PULSAR_PULSE) {
        return SVC_PULSAR_STATE_PULSE;
    }
    else {
        return SVC_PULSAR_STATE_STOP;
    }
}

static uint8_t SECTION_INFOMEM pulsar_repetitions = 10;

uint8_t svc_pulsar_alarm_repetitions_get(void){
    return pulsar_repetitions;
}

void svc_pulsar_alarm_repetitions_set(uint8_t repetitions){
    pulsar_repetitions = repetitions;
}

uint8_t svc_pulsar_metronome_get(void) {
    return pulsar_metronome;
}

void svc_aux_timer_pulsar_measure_handler(void) {
    clk_counter++;

    uint16_t ia = interval_avg/pulsar_clk_div;
    ia = ia > 0? ia : 1;
    // if (clk_counter%ia < ia/2) {
    if ((2*clk_counter)/ia) {
        pulsar_metronome = 1;
    }
    else {
        pulsar_metronome = 0;
    }

    if (clk_counter > 4*SVC_PULSAR_F_SAMPLE) {
        // Timeout after 4 seconds
        // clk_counter = 0;
        tap_counter = 0;
        svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_PULSAR_MEAS, 0);
        pulsar_metronome = 0;
    }
}

void svc_pulsar_measure_tap_handler(void) {
    // Gets called when BPM measure button gets tapped
    tap_counter++;
    svc_pulsar_reset_phase();
    if (tap_counter == 1) {
        // Start measurement
        clk_counter = 0;
        clk_counter_total = 0;
        svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_PULSAR_MEAS, 1);
        return;
    }

    // Catch missing taps (that's due to a bug..)
    if ((pulsar_clk_div*clk_counter > (3*interval_avg)/2) & (tap_counter > 3)) {
        clk_counter = interval_avg/pulsar_clk_div;
    }

    // Factor div increases averaging precision
    clk_counter_total += (pulsar_clk_div*(uint32_t)clk_counter);
    clk_counter_max = clk_counter;
    clk_counter = 0;

    interval_avg = clk_counter_total/(tap_counter-1);

    // Turn interval into hbpm = 6000*f
    // hbpm = 6000*128*4/interval_avg
    uint32_t hbpm = (6000*SVC_PULSAR_F_SAMPLE*pulsar_clk_div)/interval_avg;
    svc_pulsar_hbpm_set(hbpm);
}

uint16_t svc_pulsar_interval_avg_get(void) {
    return interval_avg;
}

uint32_t svc_pulsar_clk_counter_total_get(void) {
    return clk_counter_total;
}

uint16_t svc_pulsar_clk_counter_max_get(void) {
    return clk_counter_max;
}
