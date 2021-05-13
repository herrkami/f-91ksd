#include "app.h"
#include "../apps.h"
#include "../../svc/svc.h"
#include <string.h>

void app_app_time_display_enter(uint8_t view, const app_t *app) {
	PRIV(app)->needs_clear = 1;
}

void app_app_time_display_main(uint8_t view, const app_t *app, svc_main_proc_event_t event) {
	hal_lcd_set_mode(HAL_LCD_MODE_IMMEDIATE);
	hal_rtc_timedate_t td;
	hal_rtc_get(&td);
	hal_rtc_timedate_t *td_last = &(PRIV(app)->td_last);
	if(svc_lcd_get_force_redraw()) {
		PRIV(app)->needs_clear = 1;
	}
	if(PRIV(app)->needs_clear) {
		PRIV(app)->needs_clear = 1;
		memset(td_last, 0xff, sizeof(hal_rtc_timedate_t)); //force update of all digits
	}
	if(event & SVC_MAIN_PROC_EVENT_KEY_ENTER_LONG) {
		app_set_view(app, 1);
	}
	else if(event & SVC_MAIN_PROC_EVENT_KEY_UP) {
		PRIV(app)->display_date = !PRIV(app)->display_date;
		PRIV(app)->needs_clear = 1;
		memset(td_last, 0xff, sizeof(hal_rtc_timedate_t)); //force update of all digits
	}
	// else if(event & SVC_MAIN_PROC_EVENT_KEY_UP) {
	else if(event & SVC_MAIN_PROC_EVENT_KEY_DOWN) {
		// app_exit();
		app_launch(&app_app_countdown);
	}
	else if(event & SVC_MAIN_PROC_EVENT_KEY_DOWN_LONG) {
		// app_exit();
		app_launch(&app_app_conf);
	}
	if(PRIV(app)->needs_clear) {
		hal_lcd_clear();
	}


	static uint8_t ctr_idx = 0;
	static uint8_t second_next = 0;
	uint8_t countdowns_running = svc_coundown_get_running();

	if(PRIV(app)->display_date) {
		// Display date
		hal_lcd_seg_set(HAL_LCD_SEG_COLON, 0);
		svc_lcd_puti(0, 4, td.year);
		svc_lcd_puti(4, 2, td.month);
		svc_lcd_puti(6, 2, td.dom);
	}
	else {
		// Display time
		switch(*(PRIV(app)->base)) {
			case BASE_DEC :
				hal_lcd_seg_set(HAL_LCD_SEG_COLON, 1);
				hal_lcd_seg_set_blink(HAL_LCD_SEG_COLON, 1);
				if(td.h != td_last->h)
					svc_lcd_puti_fast(0, 2, td.h);
				if(td.m != td_last->m)
					svc_lcd_puti_fast(2, 2, td.m);
				if(td.s != td_last->s)
					svc_lcd_puti_fast(4, 2, td.s);
				if(td.dom != td_last->dom)
					svc_lcd_puti_fast(6, 2, td.dom);
			break;

			case BASE_HEX :
				hal_lcd_seg_set(HAL_LCD_SEG_COLON, 1);
				hal_lcd_seg_set_blink(HAL_LCD_SEG_COLON, 1);
				if(td.h != td_last->h)
					svc_lcd_putix(0, 2, td.h);
				if(td.m != td_last->m)
					svc_lcd_putix(2, 2, td.m);
				if(td.s != td_last->s)
					svc_lcd_putix(4, 2, td.s);
				if(td.dom != td_last->dom)
					svc_lcd_putix(6, 2, td.dom);
			break;

			case BASE_BIN :
				hal_lcd_seg_set(HAL_LCD_SEG(1, 6), 1);
				hal_lcd_seg_set(HAL_LCD_SEG(2, 6), 1);
				hal_lcd_seg_set(HAL_LCD_SEG(3, 6), 1);

				if(td.h != td_last->h) {
					hal_lcd_seg_set(HAL_LCD_SEG(3, 1), td.h&(1<<0));
					hal_lcd_seg_set(HAL_LCD_SEG(3, 5), td.h&(1<<1));
					hal_lcd_seg_set(HAL_LCD_SEG(2, 1), td.h&(1<<2));
					hal_lcd_seg_set(HAL_LCD_SEG(2, 5), td.h&(1<<3));
					hal_lcd_seg_set(HAL_LCD_SEG(1, 1), td.h&(1<<4));
				}

				if(td.m != td_last->m) {
					hal_lcd_seg_set(HAL_LCD_SEG(3, 2), td.m&(1<<0));
					hal_lcd_seg_set(HAL_LCD_SEG(3, 4), td.m&(1<<1));
					hal_lcd_seg_set(HAL_LCD_SEG(2, 2), td.m&(1<<2));
					hal_lcd_seg_set(HAL_LCD_SEG(2, 4), td.m&(1<<3));
					hal_lcd_seg_set(HAL_LCD_SEG(1, 2), td.m&(1<<4));
					hal_lcd_seg_set(HAL_LCD_SEG(1, 4), td.m&(1<<5));
				}

				if(td.s != td_last->s) {
					hal_lcd_seg_set(HAL_LCD_SEG(5, 0), td.s>(60/8));
					hal_lcd_seg_set(HAL_LCD_SEG(5, 1), td.s>((60/8)*2));
					hal_lcd_seg_set(HAL_LCD_SEG(5, 2), td.s>((60/8)*3));
					hal_lcd_seg_set(HAL_LCD_SEG(5, 3), td.s>((60/8)*4));
					hal_lcd_seg_set(HAL_LCD_SEG(4, 3), td.s>((60/8)*5));
					hal_lcd_seg_set(HAL_LCD_SEG(4, 4), td.s>((60/8)*6));
					hal_lcd_seg_set(HAL_LCD_SEG(4, 5), td.s>((60/8)*7));
					hal_lcd_seg_set(HAL_LCD_SEG(4, 0), td.s>((60/8)*8));
				}
				if(td.dom != td_last->dom)
					svc_lcd_puti_fast(6, 2, td.dom);
			break;
		}
	}

	if(td.dow != td_last->dow)
		svc_lcd_puts(8, svc_dow_to_string(td.dow, *(PRIV(app)->lang)));
	*td_last = td;

	// Countdown state
	if(countdowns_running) {
		uint8_t nr_running = countdowns_running ? svc_countdown_get_n_running() : 1;
		uint8_t counters[nr_running];
		uint8_t idx = 0;
		for(uint8_t i = 0; i < SVC_COUNTDOWN_NR; i++) {
			if(countdowns_running & (1 << i)) {
				counters[idx] = i;
				idx++;
			}
		}
		// if(td.s >= second_next) {
		if(!(td.s%4)) {
			// if((int8_t)(second_next - td.s) < 0)
			svc_countdown_t cd;
			svc_countdown_get(counters[ctr_idx], &cd);
			svc_lcd_puti_fast(0, 2, cd.h);
			svc_lcd_puti_fast(2, 2, cd.m);
			svc_lcd_puti_fast(4, 2, cd.s);
			hal_lcd_seg_set(HAL_LCD_SEG_COLON, 1);
			svc_lcd_puts(8, "cd");
			svc_lcd_puti_fast(6, 2, counters[ctr_idx]);
			// svc_lcd_force_redraw();
			ctr_idx = (ctr_idx+1)%nr_running;
			// second_next = (td.s+3)%60;
		}
		// memset(td_last, 0xff, sizeof(hal_rtc_timedate_t)); //force update of all digits
		PRIV(app)->needs_clear = 1;
	}
	else {
		PRIV(app)->needs_clear = 0;
	}

}
