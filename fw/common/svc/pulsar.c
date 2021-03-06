#include "pulsar.h"
#include "common/hal/hal.h"
#include "aux_timer.h"
#include "platform.h"
#include "beep.h"
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

static volatile uint16_t clk_counter;
static uint16_t clk_counter_max;
static uint32_t clk_counter_total;
static uint16_t tap_counter;
static uint32_t interval_avg;

// Metronome beat
static uint8_t SECTION_INFOMEM bp_beat_state = 1;
static uint16_t SECTION_INFOMEM bp_beat_freq = 8000;
static uint8_t SECTION_INFOMEM bp_beat_dur = 4;

static uint8_t SECTION_INFOMEM bp_bar_state = 1;
static uint16_t SECTION_INFOMEM bp_bar_signature = 4;
static uint16_t SECTION_INFOMEM bp_bar_freq = 9000;
static uint8_t SECTION_INFOMEM bp_bar_dur = 4;
static uint8_t SECTION_INFOMEM bp_metronome_en = 0;
static uint8_t SECTION_INFOMEM bp_beat_multishift = 0;
static uint8_t bp_bar_cnt = 0;
static uint64_t bp_phinext = 0;


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


static uint8_t SECTION_INFOMEM default_pulsar = 0;

uint8_t svc_default_pulsar_get(void) {
    return default_pulsar;
}

void svc_default_pulsar_set(uint8_t pulsar) {
    default_pulsar = pulsar;
}



void svc_aux_timer_pulsar_pulse_handler(void) {
    if(frame_cur) {
        // Duration of the last frame is always 0
        if(frame_cur->duration == 0) {
    		if(repeat != 255) {
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
                // Call flashes
                svc_flash_rightled_timed(frame_cur->led0, 4);
                svc_flash_caseled_timed(frame_cur->led1, 4);

                // Pulsar state keeping
                uint8_t div = frame_cur->duration ? frame_cur->duration : 1;
                pulsar_phinext += SVC_PULSAR_PHI_MAX/div;
                frame_cur++;
            }
            if(pulsar_phi >= bp_phinext) {
                if(bp_metronome_en) {
                    // Metronome beep
                    // if(bp_bar_state) {
                    //     if(++bp_bar_cnt >= bp_bar_signature) {
                    //         svc_beep_timed(bp_bar_freq, bp_bar_dur);
                    //         bp_bar_cnt = 0;
                    //     }
                    // }
                    // if(bp_beat_state) {
                    //     svc_beep_timed(bp_beat_freq, bp_beat_dur);
                    // }
                    if(++bp_bar_cnt >= bp_bar_signature) {
                        if(bp_bar_state) {
                            svc_beep_timed(bp_bar_freq, bp_bar_dur);
                        }
                        else {
                            svc_beep_timed(bp_beat_freq, bp_beat_dur);
                        }
                        bp_bar_cnt = 0;
                    }
                    else if (bp_beat_state) {
                        svc_beep_timed(bp_beat_freq, bp_beat_dur);
                    }
                }
                bp_phinext += (SVC_PULSAR_PHI_MAX >> bp_beat_multishift);
            }
            if (pulsar_phi >= SVC_PULSAR_PHI_MAX) {
                pulsar_phi -= SVC_PULSAR_PHI_MAX;
                if (pulsar_phinext >= SVC_PULSAR_PHI_MAX) {
                    pulsar_phinext -= SVC_PULSAR_PHI_MAX;
                }
                if (bp_phinext >= SVC_PULSAR_PHI_MAX) {
                    bp_phinext -= SVC_PULSAR_PHI_MAX;
                }
            }
    	}
	}
	else {
		svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_PULSAR_PULSE, 0);
	}
}

void svc_pulsar_reset_phase(void) {
    frame_cur = frame_start;
    pulsar_phi = SVC_PULSAR_PHI_MAX;
}


void _svc_pulsar_play_repeat(uint8_t rep) {
    frame_start = svc_pulsar_seqs[pulsar_seq].frames;
    frame_cur = svc_pulsar_seqs[pulsar_seq].frames;
    pulsar_phi = SVC_PULSAR_PHI_MAX;
    uint8_t div = frame_cur->duration ? frame_cur->duration : 1;
    pulsar_phinext = SVC_PULSAR_PHI_MAX/div;
    bp_phinext = SVC_PULSAR_PHI_MAX;
    bp_bar_cnt = 0;
    repeat = (!rep)?255:rep;
	svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_PULSAR_PULSE, 1);
}

void svc_pulsar_play_repeat(uint8_t pulsar, uint8_t rep) {
    svc_pulsar_sequence_set(pulsar);
    _svc_pulsar_play_repeat(rep);
}

void svc_pulsar_play(void) {
    svc_pulsar_play_repeat(svc_pulsar_sequence_get(), 1);
}

void svc_pulsar_sequence_set(uint8_t seq) {
    pulsar_seq = seq;
}

uint8_t svc_pulsar_sequence_get(void) {
    return pulsar_seq;
}

void svc_pulsar_stop(void) {
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
    svc_aux_timer_set_call_main();

    if (clk_counter > 4*SVC_PULSAR_F_SAMPLE) {
        // Timeout after 4 seconds
        clk_counter = 0;
        clk_counter_total = 0;
        tap_counter = 0;
        svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_PULSAR_MEAS, 0);
        pulsar_metronome = 0;
    }
}

// uint16_t svc_pulsar_get_rtc_clk_count(void) {
//     // Reliable until excluding 4 seconds
//     static uint16_t clk_sample = 0;
//     static uint16_t clk_sample_prev = 0;
//     // Majority vote because of asynchronious RTC clock
//     uint16_t _clk_sample[4];
//     for (uint8_t i=0; i++; i<4) {
//         _clk_sample[i] = (uint16_t)(RT1PS<<8) | RT0PS;
//     }
//     if (_clk_sample[0] == _clk_sample[1]) {
//         clk_sample =_clk_sample[0] & 0x7fff;
//     }
//     else if (_clk_sample[1] == _clk_sample[2]) {
//         clk_sample = _clk_sample[1] & 0x7fff;
//     }
//     else if (_clk_sample[2] == _clk_sample[3]) {
//         clk_sample = _clk_sample[2] & 0x7fff;
//     }
//     else {
//         return 0;
//     }
//     // So far, bit 14 contains the 1 second bit
//     // We fill 15 with the 2 seconds bits
//     hal_rtc_timedate_t td;
//     hal_rtc_get(&td);
//     clk_sample = (clk_sample >> 1) | (uint16_t)((td.s&0b10)<<14);
//     int32_t _clk_counter_rtc = clk_sample_prev - clk_sample;
//     clk_sample_prev = clk_sample;
//     // Handle wrap
//     if (_clk_counter_rtc < 0) {
//         _clk_counter_rtc += 0xffff;
//     }
//     return (uint16_t)_clk_counter_rtc;
// }

void svc_pulsar_measure_tap_handler(void) {

    // Gets called when BPM measure button gets tapped
    tap_counter++;
    svc_pulsar_reset_phase();
    if (tap_counter == 1) {
        // Start measurement
        clk_counter = 0;  // delay until aux timer becomes active
        clk_counter_total = 0;
        svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_PULSAR_MEAS, 1);
        return;
    }

    // clk_counter = svc_pulsar_get_rtc_clk_count();
    // clk_counter  = clk_counter >> 8;

    // Catch accidentally missing taps
    if ((pulsar_clk_div*clk_counter > (3*interval_avg)/2) & (tap_counter > 3)) {
        clk_counter = interval_avg/pulsar_clk_div;
        svc_flash_caseled_timed(3, 4);
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


uint8_t svc_pulsar_bp_beat_state_get(void) {
    return bp_beat_state;
}

void svc_pulsar_bp_beat_state_set(uint8_t state) {
    bp_beat_state = state;
}

uint16_t svc_pulsar_bp_beat_freq_get(void) {
    return bp_beat_freq;
}

void svc_pulsar_bp_beat_freq_set(uint16_t freq) {
    bp_beat_freq = freq;
}

uint8_t svc_pulsar_bp_beat_dur_get(void) {
    return bp_beat_dur;
}

void svc_pulsar_bp_beat_dur_set(uint8_t dur) {
    bp_beat_dur = dur;
}

uint8_t svc_pulsar_bp_bar_state_get(void) {
    return bp_bar_state;
}

void svc_pulsar_bp_bar_state_set(uint8_t state) {
    bp_bar_state = state;
}

uint16_t svc_pulsar_bp_bar_signature_get(void) {
    return bp_bar_signature;
}

void svc_pulsar_bp_bar_signature_set(uint16_t signature) {
    bp_bar_signature = signature;
}

uint8_t svc_pulsar_bp_multishift_get(void) {
    return bp_beat_multishift;
}

void svc_pulsar_bp_multishift_set(uint8_t multishift) {
    bp_beat_multishift = multishift;
}

uint16_t svc_pulsar_bp_bar_freq_get(void) {
    return bp_bar_freq;
}

void svc_pulsar_bp_bar_freq_set(uint16_t freq) {
    bp_bar_freq = freq;
}

uint8_t svc_pulsar_bp_bar_dur_get(void) {
    return bp_bar_dur;
}

void svc_pulsar_bp_bar_dur_set(uint8_t dur) {
    bp_bar_dur = dur;
}

void svc_pulsar_bp_metronome_set_enable(uint8_t state) {
    bp_metronome_en = state;
}

uint8_t svc_pulsar_bp_metronome_get_enable(void) {
    return bp_metronome_en;
};
