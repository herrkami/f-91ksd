#include "pulsar.h"
#include "common/hal/hal.h"
#include "aux_timer.h"
#include "platform.h"
#include "leds.h"
#include "util.h"

static const svc_pulsar_frame_t *frame_cur = 0;
static const svc_pulsar_frame_t *frame_start = 0;
static uint64_t pulsar_phi;
static uint8_t repeat;
// static uint64_t pulsar_dphi = 120/60*SVC_PULSAR_PHI_MAX/SVC_PULSAR_F_SAMPLE;
static uint64_t pulsar_dphi = 120/60*SVC_PULSAR_PHI_MAX/SVC_PULSAR_F_SAMPLE;

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

void svc_aux_timer_pulsar_handler(void) {
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
    			svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_PULSAR, 0);
    		}
    	}
    	else {
            pulsar_phi += pulsar_dphi;
            if (pulsar_phi >= SVC_PULSAR_PHI_MAX) {

                // handle pulse events here
                svc_flash_rightled_timed(frame_cur->led0);
                svc_flash_caseled_timed(frame_cur->led1);

                pulsar_phi -= SVC_PULSAR_PHI_MAX;
                frame_cur++;
            }
    	}
	}
	else {
		svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_MELODY, 0);
	}
}

void svc_pulsar_play_repeat(uint8_t seq, uint8_t rep){
    frame_cur = svc_pulsar_seqs[seq].frames;
    frame_start = svc_pulsar_seqs[seq].frames;
    pulsar_phi = SVC_PULSAR_PHI_MAX;
    repeat = (!rep)?255:rep;
	svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_PULSAR, 1);
}

void svc_pulsar_play(uint8_t seq){
    svc_pulsar_play_repeat(seq, 1);
}

void svc_pulsar_stop(void){
    svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_PULSAR, 0);
}

uint8_t svc_pulsar_state_get(void){
    svc_aux_timer_required_t aux_timer = svc_aux_timer_get_required();
    if (aux_timer & SVC_AUX_TIMER_REQUIRED_PULSAR) {
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
