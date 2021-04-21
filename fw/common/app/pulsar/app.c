include "app.h"

static void main(uint8_t view, const app_t *app, svc_main_proc_event_t event) {
    hal_lcd_clear();
    if (event & SVC_MAIN_PROC_EVENT_KEY_ENTER) {
        app_set_view(app, 1);
    }
    if (event & SVC_MAIN_PROC_EVENT_KEY_DOWN) {
        app_launch(&app_app_time);
    }

}
