#include "common/app/app.h"
#include "common/app/apps.h"
#include "common/svc/svc.h"
#include "common/hal/hal.h"
#include "common/hal/lcd_segments.h"

extern void app_app_alarm_edit_main(uint8_t view, const app_t *app, svc_main_proc_event_t event);
extern void app_app_alarm_days_main(uint8_t view, const app_t *app, svc_main_proc_event_t event);
extern void app_app_alarm_save_time_main(uint8_t view, const app_t *app, svc_main_proc_event_t event);

typedef struct {
	APP_PRIV_COMMON
	uint8_t alarm_current;
	uint8_t day_current;
	svc_menu_state_t edit_menu_state;
} priv_t;

#define PRIV(a) ((priv_t*)((a)->priv))
