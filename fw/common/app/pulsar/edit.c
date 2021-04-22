#include "app.h"

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
		svc_pulsar_play_repeat(2, 255);
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

static const svc_menu_item_unknown_t *menu_items[] = {
	(void*)&menu_item_start_stop,
	(void*)&menu_item_hbpm,
};

static const svc_menu_t menu = {
	.n_items = ARRAY_SIZE(menu_items),
	.items = (void*)menu_items,
	.handler_exit = menu_exit,
	.header = "bt",
	.header_pos = 8
};

void app_app_pulsar_edit_main(uint8_t view, const app_t *app, svc_main_proc_event_t event) {
	svc_menu_run(&menu, &(PRIV(app)->edit_menu_state), event);
}
