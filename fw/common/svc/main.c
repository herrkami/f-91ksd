#include "svc.h"
#include "aux_timer.h"
#include "common/app/app.h"
#include "common/hal/hal.h"
#include "common/hal/lcd_segments.h"
#include <unistd.h>
#include <stdio.h>

void svc_main_proc(svc_main_proc_event_t event) {
	static uint8_t keypress;
	if(event & (SVC_MAIN_PROC_EVENT_KEY_ANY | SVC_MAIN_PROC_EVENT_KEY_ANY_LONG)) {
		if(svc_alarm_get_pending() || svc_countdown_get_pending()) {
			// eat event if alarm/countdown shall be disabled
			event &= ~(SVC_MAIN_PROC_EVENT_KEY_ANY | SVC_MAIN_PROC_EVENT_KEY_ANY_LONG);
			svc_pulsar_stop();
		}
		svc_alarm_clear_pending();
		svc_countdown_clear_pending();
		keypress = 1;
		svc_beep_key();
		svc_flash_rightled_key();
	}
	if(event & SVC_MAIN_PROC_EVENT_AUX_TIMER) {
		svc_aux_timer_handler();
	}
	if(svc_backlight_process(event)) {
		// eat event if flashlight shall be disabled
		event &= ~(SVC_MAIN_PROC_EVENT_KEY_ANY | SVC_MAIN_PROC_EVENT_KEY_ANY_LONG);
	}
	if(event & SVC_MAIN_PROC_EVENT_TICK) {
		static uint8_t div;
		if(!div) { /* prescale to one second */
			svc_countdown_process();
			svc_seconds_since_last_set_process();
			svc_menu_process_timetohome(keypress);
			keypress = 0;
		}
		div = (div+1)%4;

		svc_lcd_blink_process();
		svc_rtc_adj_process();
	}
	if(!(event & SVC_MAIN_PROC_EVENT_AUX_TIMER) || svc_aux_timer_get_call_main()) {
		app_current_update();
		hal_lcd_set_mode(HAL_LCD_MODE_BUFFERED);
		if(app_view_changed) {
			if(app_current->views[app_current->priv->view_current].enter) {
				app_current->views[app_current->priv->view_current].enter(app_current->priv->view_current, app_current);
				app_view_changed = 0;
			}
		}
		app_current->views[app_current->priv->view_current].main(app_current->priv->view_current, app_current, event);
	}

	if(svc_chro_get_any_running()) {
		hal_lcd_seg_set(HAL_LCD_SEG_LAP, 1);
		hal_lcd_seg_set_blink(HAL_LCD_SEG_LAP, 1);
	}
	svc_alarm_process();
	if(svc_alarm_get_any_enabled()) {
		hal_lcd_seg_set(HAL_LCD_SEG_BELL, 1);
		hal_lcd_seg_set_blink(HAL_LCD_SEG_BELL, svc_alarm_get_pending());
	}
	else {
		hal_lcd_seg_set(HAL_LCD_SEG_BELL, 0);
	}

	if(svc_countdowns_are_running() || svc_countdown_get_pending()) {
		hal_lcd_seg_set(HAL_LCD_SEG_BARS, 1);
		hal_lcd_seg_set_blink(HAL_LCD_SEG_BARS, svc_countdown_get_pending());
		// hal_lcd_seg_set_blink(HAL_LCD_SEG_BARS, 1);
	}
	else {
		hal_lcd_seg_set(HAL_LCD_SEG_BARS, 0);
	}

	if(svc_rtc_adj_get_pending()) {
		hal_lcd_seg_set(HAL_LCD_SEG_PM, 1);
		hal_lcd_seg_set_blink(HAL_LCD_SEG_PM, 1);
	}
	else {
		hal_lcd_seg_set(HAL_LCD_SEG_PM, 0);
	}

	if(event & SVC_MAIN_PROC_EVENT_TICK) {
		svc_countdown_draw_popup();
		svc_alarm_draw_popup();
	}

	svc_beep_hour();
	svc_flash_caseled_minute();
}
