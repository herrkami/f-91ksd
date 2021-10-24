#include "app.h"


void percentage_draw(svc_menu_state_t *state, svc_menu_item_unknown_t *item, void *user_data) {
	svc_bat_sample();
    int16_t bat = svc_bat_percentage_get();

	hal_lcd_seg_set(HAL_LCD_SEG(0, 4), 1);
	hal_lcd_seg_set(HAL_LCD_SEG(0, 2), 1);
	hal_lcd_seg_set(HAL_LCD_SEG(1, 4), 1);
	hal_lcd_seg_set(HAL_LCD_SEG(1, 2), 1);
	hal_lcd_seg_set(HAL_LCD_SEG(2, 4), 1);
	hal_lcd_seg_set(HAL_LCD_SEG(2, 2), 1);
	hal_lcd_seg_set(HAL_LCD_SEG(3, 4), 1);
	hal_lcd_seg_set(HAL_LCD_SEG(3, 2), 1);


	hal_lcd_seg_set(HAL_LCD_SEG(0, 5), (bat > 88));
	hal_lcd_seg_set(HAL_LCD_SEG(0, 1), (bat > 75));
	hal_lcd_seg_set(HAL_LCD_SEG(1, 5), (bat > 63));
	hal_lcd_seg_set(HAL_LCD_SEG(1, 1), (bat > 50));
	hal_lcd_seg_set(HAL_LCD_SEG(2, 5), (bat > 38));
	hal_lcd_seg_set(HAL_LCD_SEG(2, 1), (bat > 25));
	hal_lcd_seg_set(HAL_LCD_SEG(3, 1), (bat > 13));
	hal_lcd_seg_set(HAL_LCD_SEG(3, 5), (bat > 0));

	svc_lcd_puti(4, 2, bat);
	// svc_lcd_puts(8, "ba");
}

static const svc_menu_item_text_t menu_item_percentage = {
	.type = SVC_MENU_ITEM_T_TEXT,
	.text = "",
	// .handler = start_stop,
	.handler_draw = percentage_draw,
};


void adc_draw(svc_menu_state_t *state, svc_menu_item_unknown_t *item, void *user_data) {
	svc_bat_sample();
	// uint16_t adc = svc_bat_adc_get();
    uint16_t adc = hal_adc_sample_battery();

	svc_lcd_puti(0, 6, adc);
	// svc_lcd_puts(8, "ba");
}

static const svc_menu_item_text_t menu_item_adc = {
	.type = SVC_MENU_ITEM_T_TEXT,
	.text = "",
	// .handler = start_stop,
	.handler_draw = adc_draw,
};


static void menu_exit(void) {
	app_set_view(app_current, 0);
}

static const svc_menu_item_unknown_t *menu_items[] = {
	(void*)&menu_item_percentage,
	(void*)&menu_item_adc,
};

static const svc_menu_t menu = {
	.n_items = ARRAY_SIZE(menu_items),
	.items = (void*)menu_items,
	.handler_exit = menu_exit,
	.header = "ba",
	.header_pos = 8
};

void app_app_battery_main(uint8_t view, const app_t *app, svc_main_proc_event_t event) {
	svc_menu_run(&menu, &(PRIV(app)->bat_menu_state), event);
}
