#include "app.h"
#include "common/hal/lcd_segments.h"

static void main(uint8_t view, const app_t *app, svc_main_proc_event_t event) {
    hal_lcd_clear();
    if (event & SVC_MAIN_PROC_EVENT_KEY_ENTER) {
        app_set_view(app, 1);
    }
    else if (event & SVC_MAIN_PROC_EVENT_KEY_DOWN) {
        app_launch(&app_app_time);
    }
    else if(event & SVC_MAIN_PROC_EVENT_KEY_DOWN_LONG) {
		app_launch(&app_app_time);
	}
    else if (event & SVC_MAIN_PROC_EVENT_KEY_UP) {
        svc_pulsar_measure_tap_handler();
    }
    else if (event & SVC_MAIN_PROC_EVENT_KEY_ENTER_LONG) {
        svc_pulsar_bp_metronome_set_enable(!svc_pulsar_bp_metronome_get_enable());
    }
    uint16_t hbpm = svc_pulsar_hbpm_get();
    svc_lcd_puti(1, 5, hbpm);
    // svc_lcd_puti(1, 5, svc_pulsar_interval_avg_get());
    // svc_lcd_puti(0, 6, svc_pulsar_clk_counter_max_get());
    svc_lcd_puts(8, "bt");
    hal_lcd_seg_set(HAL_LCD_SEG_LAP, svc_pulsar_metronome_get());
}

static app_view_t views[] = {
	{
		.main = main
	},
	{
		.main = app_app_pulsar_edit_main,
	},
};

static priv_t priv = {0, 0};

const app_t app_app_pulsar = {
	.n_views = ARRAY_SIZE(views),
	.priv = (app_priv_t*)(&priv),
	.views = views
};
