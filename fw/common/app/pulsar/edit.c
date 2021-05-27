#include "app.h"
#include "common/svc/svc.h"

static void menu_exit(void) {
	PRIV(app_current)->edit_menu_state.item_current = 0;
	app_set_view(app_current, 0);
}

static int32_t hbpm_get(void *ud) {
    return svc_pulsar_hbpm_get();
}

static void hbpm_set(uint8_t dig, int8_t dir, void *user_data) {
    int32_t hbpm = svc_pulsar_hbpm_get();
    int32_t inc = 0;
    switch(dig) {
        case 4:  // 100
            inc = 10000;
            break;
		case 3:  // 10
            inc = 1000;
            break;
        case 2:  // 1
            inc = 100;
            break;
        case 1:  // .1
            inc = 10;
            break;
		case 0:  // .01
            inc = 1;
            break;

		default :
			return;
	}
    inc *= dir;
    hbpm = CLAMP(hbpm + inc, 100, 51200);
    svc_pulsar_hbpm_set(hbpm);
}

static void hbpm_draw(svc_menu_state_t *state, svc_menu_item_unknown_t *item, void *user_data) {
    svc_lcd_puts(8,"bt");
	hal_lcd_dig_set_blink_mask(0b11<<8);
    uint16_t hbpm = svc_pulsar_hbpm_get();
    svc_lcd_puti(1, 5, hbpm);
}

static const svc_menu_item_adj_t menu_item_hbpm = {
	.type = SVC_MENU_ITEM_T_ADJ,
	.text = "",
	.header = "bt",
    .digits = 5,
	.offset = 1,
	.handler_get = hbpm_get,
	.handler_set = hbpm_set,
	.handler_draw = hbpm_draw,
};


static void start_stop_draw(svc_menu_state_t *state, svc_menu_item_unknown_t *item, void *user_data) {
	svc_lcd_puts(8,"bt");
	hal_lcd_dig_set_blink_mask(0b11<<8);
    uint8_t pulsar_state = svc_pulsar_state_get();
	if(pulsar_state == SVC_PULSAR_STATE_STOP) {
		svc_lcd_puts(1, "pulse");
		hal_lcd_dig_set_blink_mask(0b111110);
	}
	else {
		svc_lcd_puts(0, "stop");
		hal_lcd_dig_set_blink_mask(0b1111);
	}
}

static void start_stop(void *ud) {
	PRIV(app_current)->edit_menu_state.item_current = 0;
	app_set_view(app_current, 0);
    uint8_t pulsar_state = svc_pulsar_state_get();
	if(pulsar_state == SVC_PULSAR_STATE_STOP) {
        // TODO introduce state variable for current sequence
		svc_pulsar_play_repeat(svc_pulsar_sequence_get(), 255);
	}
	else {
		svc_pulsar_stop();
	}
}

static const svc_menu_item_text_t menu_item_start_stop = {
	.type = SVC_MENU_ITEM_T_TEXT,
	.text = "",
	.handler = start_stop,
	.handler_draw = start_stop_draw,
};


static void sequence_set(uint8_t choice, void *ud) {
	svc_pulsar_sequence_set(choice);
}

static uint8_t sequence_get(void *ud) {
	return svc_pulsar_sequence_get();
}

static void sequence_draw(svc_menu_state_t *state, svc_menu_item_unknown_t *item, void *user_data) {
	svc_lcd_putsn(4, 2, svc_pulsar_seqs[svc_pulsar_sequence_get()].title);
}

static svc_menu_item_choice_t menu_item_sequence = {
	.type = SVC_MENU_ITEM_T_CHOICE,
	.text = " seq",
	.choice_pos = 4,
	// .n_choices = svc_pulsar_seqs_n,
	.n_choices = 0,
	.choices = {""},
	.handler_set = sequence_set,
	.handler_get = sequence_get,
	.handler_draw = sequence_draw,
};


// Metronome
static uint8_t bp_beat_state_get(void *ud) {
    return svc_pulsar_bp_beat_state_get();
}

static void bp_beat_state_set(uint8_t choice, void *ud) {
	svc_pulsar_bp_beat_state_set(choice);
}

static const svc_menu_item_choice_t menu_item_bp_beat_enable = {
	.type = SVC_MENU_ITEM_T_CHOICE,
	.text = " pue",
	.choice_pos = 4,
	.n_choices = 2,
	.choices = {
		"of",
		"on",
	},
	.handler_set = bp_beat_state_set,
	.handler_get = bp_beat_state_get
};


static int32_t bp_beat_freq_get(void *ud) {
    return svc_pulsar_bp_beat_freq_get();
}

static void bp_beat_freq_set(uint8_t dig, int8_t dir, void *user_data) {
	int16_t inc = dir*ipow(10, dig);
	int16_t va = svc_pulsar_bp_beat_freq_get();
	va = CLAMP(va+inc, 400, 20000);
	svc_pulsar_bp_beat_freq_set(va);
}

static const svc_menu_item_adj_t menu_item_bp_beat_freq = {
	.type = SVC_MENU_ITEM_T_ADJ,
	.header = "bt",
	.text = " puf",
	.digits = 5,
	.handler_get = bp_beat_freq_get,
	.handler_set = bp_beat_freq_set
};


static int32_t bp_beat_dur_get(void *ud) {
    return svc_pulsar_bp_beat_dur_get();
}

static void bp_beat_dur_set(uint8_t dig, int8_t dir, void *user_data) {
	int16_t inc = dir*ipow(10, dig);
	int16_t va = svc_pulsar_bp_beat_dur_get();
	va = CLAMP(va+inc, 1, 100);
	svc_pulsar_bp_beat_dur_set(va);
}

static const svc_menu_item_adj_t menu_item_bp_beat_dur = {
	.type = SVC_MENU_ITEM_T_ADJ,
	.header = "bt",
	.text = " pud",
	.digits = 3,
	.handler_get = bp_beat_dur_get,
	.handler_set = bp_beat_dur_set
};


static uint8_t bp_bar_state_get(void *ud) {
    return svc_pulsar_bp_bar_state_get();
}

static void bp_bar_state_set(uint8_t choice, void *ud) {
	svc_pulsar_bp_bar_state_set(choice);
}

static const svc_menu_item_choice_t menu_item_bp_bar_enable = {
	.type = SVC_MENU_ITEM_T_CHOICE,
	.text = " bae",
	.choice_pos = 4,
	.n_choices = 2,
	.choices = {
		"of",
		"on",
	},
	.handler_set = bp_bar_state_set,
	.handler_get = bp_bar_state_get
};


static int32_t bp_bar_freq_get(void *ud) {
    return svc_pulsar_bp_bar_freq_get();
}

static void bp_bar_freq_set(uint8_t dig, int8_t dir, void *user_data) {
	int16_t inc = dir*ipow(10, dig);
	int16_t va = svc_pulsar_bp_bar_freq_get();
	va = CLAMP(va+inc, 400, 20000);
	svc_pulsar_bp_bar_freq_set(va);
}

static const svc_menu_item_adj_t menu_item_bp_bar_freq = {
	.type = SVC_MENU_ITEM_T_ADJ,
	.header = "bt",
	.text = " baf",
	.digits = 5,
	.handler_get = bp_bar_freq_get,
	.handler_set = bp_bar_freq_set
};


static int32_t bp_bar_dur_get(void *ud) {
    return svc_pulsar_bp_bar_dur_get();
}

static void bp_bar_dur_set(uint8_t dig, int8_t dir, void *user_data) {
	int16_t inc = dir*ipow(10, dig);
	int16_t va = svc_pulsar_bp_bar_dur_get();
	va = CLAMP(va+inc, 1, 100);
	svc_pulsar_bp_bar_dur_set(va);
}

static const svc_menu_item_adj_t menu_item_bp_bar_dur = {
	.type = SVC_MENU_ITEM_T_ADJ,
	.header = "bt",
	.text = " bad",
	.digits = 3,
	.handler_get = bp_bar_dur_get,
	.handler_set = bp_bar_dur_set
};


static int32_t bp_signature_get(void *ud) {
    return svc_pulsar_bp_bar_signature_get();
}

static void bp_signature_set(uint8_t dig, int8_t dir, void *user_data) {
	int16_t inc = dir*ipow(10, dig);
	int16_t va = svc_pulsar_bp_bar_signature_get();
	va = CLAMP(va+inc, 1, 1024);
	svc_pulsar_bp_bar_signature_set(va);
}

static const svc_menu_item_adj_t menu_item_bp_signature = {
	.type = SVC_MENU_ITEM_T_ADJ,
	.header = "bt",
	.text = " sig",
	.digits = 4,
	.handler_get = bp_signature_get,
	.handler_set = bp_signature_set
};


static int32_t bp_multiply_get(void *ud) {
	uint8_t multi = (1 << svc_pulsar_bp_multishift_get());
    return multi;
}

static void bp_multiply_set(uint8_t dig, int8_t dir, void *user_data) {
	uint8_t shift = svc_pulsar_bp_multishift_get();
	shift = CLAMP(shift + dir, 0, 3);
	svc_pulsar_bp_multishift_set(shift);
}

static const svc_menu_item_adj_t menu_item_bp_multiply = {
	.type = SVC_MENU_ITEM_T_ADJ,
	.header = "bt",
	.text = " mul",
	.digits = 1,
	.handler_get = bp_multiply_get,
	.handler_set = bp_multiply_set
};


static const svc_menu_item_unknown_t *menu_items[] = {
	(void*)&menu_item_start_stop,
	(void*)&menu_item_hbpm,
	(void*)&menu_item_sequence,
	(void*)&menu_item_bp_beat_enable,
	(void*)&menu_item_bp_beat_freq,
	(void*)&menu_item_bp_beat_dur,
	(void*)&menu_item_bp_bar_enable,
	(void*)&menu_item_bp_bar_freq,
	(void*)&menu_item_bp_bar_dur,
	(void*)&menu_item_bp_signature,
	(void*)&menu_item_bp_multiply,
};

static const svc_menu_t menu = {
	.n_items = ARRAY_SIZE(menu_items),
	.items = (void*)menu_items,
	.handler_exit = menu_exit,
	.header = "bt",
	.header_pos = 8
};

void app_app_pulsar_edit_main(uint8_t view, const app_t *app, svc_main_proc_event_t event) {
	menu_item_sequence.n_choices = svc_pulsar_seqs_n;
	svc_menu_run(&menu, &(PRIV(app)->edit_menu_state), event);
}
