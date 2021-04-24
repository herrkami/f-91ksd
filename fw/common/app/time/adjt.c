#include "app.h"

static uint8_t digit = 5;

void app_app_time_adjt_main(uint8_t view, const app_t *app, svc_main_proc_event_t event) {
	hal_lcd_clear();
	hal_rtc_timedate_t td;
	hal_rtc_get(&td);
	int8_t dir = 0;
	if(event & SVC_MAIN_PROC_EVENT_KEY_UP) {
		dir = 1;
	}
	else if (event & SVC_MAIN_PROC_EVENT_KEY_DOWN) {
		dir = -1;
	}
	else if (event & SVC_MAIN_PROC_EVENT_KEY_ENTER) {
		digit = digit > 1 ? digit - 1 : 5;
		// digit = !digit ? 5 : digit;
	}
	else if (event & SVC_MAIN_PROC_EVENT_KEY_ENTER_LONG) {
		app_set_view(app_current, 1);
	}
	switch(digit) {
		case 5:
		case 3:
			dir *= 10;
		break;
	}
	switch(digit) {
		case 5 :
		case 4 :
			td.h = CLAMP(td.h+dir, 0, 23);
		break ;

		case 3 :
		case 2 :
			td.m = CLAMP(td.m+dir, 0, 59);
		break ;

		case 1 :
		case 0 :
			if((dir > 0) | ((dir < 0) & (!td.s))) {
				td.m = CLAMP(td.m+dir, 0, 59);
			}
			
			td.s = dir ? 0 : td.s;
			// td.s = CLAMP(td.s+dir, 0, 59);
			hal_rtc_reset_second();
		break ;
	}
	if(dir != 0) hal_rtc_set_time(&td);
	svc_lcd_puts(8, "ti");
	// hal_lcd_dig_set_blink_mask(0b11<8);
	hal_lcd_seg_set(HAL_LCD_SEG_COLON, 1);
	hal_lcd_seg_set_blink(HAL_LCD_SEG_COLON, 1);
	if (digit > 1)
		hal_lcd_dig_set_blink(5-digit, 1);
	else
		hal_lcd_dig_set_blink_mask(0b11<<4);
	svc_lcd_puti_fast(0, 2, td.h);
	svc_lcd_puti_fast(2, 2, td.m);
	svc_lcd_puti_fast(4, 2, td.s);
}
