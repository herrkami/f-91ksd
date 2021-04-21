#include "pulsar.h"
#include "common/hal/hal.h"
#include "aux_timer.h"
#include "platform.h"
#include "leds.h"

static const svc_pulsar_frame_t *frame_cur = 0;
static const svc_pulsar_frame_t *frame_start = 0;
static uint32_t pulsar_phi;
static uint32_t pulsar_dphi;
static uint8_t repeat;
static uint8_t repeat_delay;

uint32_t svc_pulsar_bbpm_to_dphi(uint16_t bbpm) {
    // 1 bbpm = 1BPM*128
    // f = bbpm/128/60
    // f = dphi/phi_max*f_sample
    // dphi = bbpm/128/60*phi_max/f_sample
    uint16_t dphi = ((uint64_t)bbpm*SVC_PULSAR_PHI_MAX)/60/128/SVC_PULSAR_F_SAMPLE;
    return dphi;
}

uint16_t svc_pulsar_dphi_to_bbpm(uint16_t dphi) {
    // 1 bbpm = 1BPM*128
    // f = bbpm/60/128
    // f = dphi/phi_max*f_sample
    // bbpm = dphi*60*128*f_sample/phi_max
    uint16_t bbpm = ((uint64_t)dphi*60*128*SVC_PULSAR_F_SAMPLE)/SVC_PULSAR_PHI_MAX;
    return bbpm;
}

void svc_pulsar_bbpm_set(uint16_t bbpm) {
    pulsar_dphi = svc_pulsar_bbpm_to_dphi(bbpm);
}

uint16_t svc_pulsar_bbpm_get(void) {
    uint16_t bbpm = svc_pulsar_dphi_to_bbpm(pulsar_dphi);
    return bbpm;
}

void svc_aux_timer_pulsar_handler(void) {
    if(repeat_delay) {
		repeat_delay--;
		return;
	}

    if(frame_cur) {
        // Duration of the last frame is always 0
        if(frame_cur->duration == 0) {
    		if(repeat != 255) { /* repeat==255 -> repeat infinite */
    			repeat--;
    		}
    		if(repeat) {
    			frame_cur = frame_start;
    			repeat_delay = 255;
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
	repeat_delay = 0;
	svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_PULSAR, 1);
}

void svc_pulsar_play(uint8_t seq){
    svc_pulsar_play_repeat(seq, 1);
}

void svc_pulsar_stop(void){
    svc_aux_timer_set_required(SVC_AUX_TIMER_REQUIRED_PULSAR, 0);
}

static uint8_t SECTION_INFOMEM pulsar_repetitions = 10;

uint8_t svc_pulsar_alarm_repetitions_get(void){
    return pulsar_repetitions;
}

void svc_pulsar_alarm_repetitions_set(uint8_t repetitions){
    pulsar_repetitions = repetitions;
}
