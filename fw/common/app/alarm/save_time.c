#include "app.h"


void app_app_alarm_save_time_main(uint8_t view, const app_t *app, svc_main_proc_event_t event) {
	// svc_alarm_t al;
	// svc_alarm_get(PRIV(app_current)->alarm_current, &al);

	hal_rtc_timedate_t dt;
	hal_rtc_get(&dt);

	hal_lcd_clear();
	if(event & SVC_MAIN_PROC_EVENT_KEY_ENTER) {
		// Set the alarm to current time and day
		for(uint8_t dow = 0; dow < 7; dow++){
			svc_alarm_set_day(PRIV(app_current)->alarm_current, dow, 0);
		}
		svc_alarm_set_day(PRIV(app_current)->alarm_current, dt.dow, 1);

		svc_alarm_set_time(PRIV(app_current)->alarm_current, dt.h, dt.m);
		app_set_view(app, 0);
	}
	else if (event & SVC_MAIN_PROC_EVENT_KEY_DOWN_LONG) {
		app_set_view(app, 0);
	}

	svc_lcd_puts(8, "al");
	hal_lcd_dig_set_blink_mask(0b11<<8);
	svc_lcd_puti(6, 2, PRIV(app_current)->alarm_current);

	svc_lcd_puti(0, 2, dt.h);
	svc_lcd_puti(2, 2, dt.m);
	svc_lcd_puts(4, svc_dow_to_string(dt.dow, SVC_LANG_EN));
	hal_lcd_dig_set_blink_mask(0b111111);
	hal_lcd_seg_set(HAL_LCD_SEG_COLON, 1);
}
