#include "app.h"

static void main(uint8_t view, const app_t *app, svc_main_proc_event_t event) {
    hal_lcd_clear();
    if (event & SVC_MAIN_PROC_EVENT_KEY_ENTER) {
        app_set_view(app, 1);
    }
    else if (event & SVC_MAIN_PROC_EVENT_KEY_DOWN) {
        app_launch(&app_app_time);
    }
    else if (event & SVC_MAIN_PROC_EVENT_KEY_UP) {
        // Handle BPM capture
    }
    uint16_t hbpm = svc_pulsar_hbpm_get();
    // uint16_t bpm = hbpm/100;
    // uint16_t bpm_hundredth = hbpm%100;
    // svc_lcd_puti(1, 3, bpm);
    // svc_lcd_puti(4, 2, bpm_hundredth);
    // svc_lcd_puti(1, 5, hbpm);
    svc_lcd_puti(1, 5, hbpm);
    svc_lcd_puts(8, "bt");
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
