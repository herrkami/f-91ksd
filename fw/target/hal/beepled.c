#include <msp430.h>
#include <stdint.h>
#include "beepled.h"
#include "io.h"
#include "common/hal/hal.h"

static uint16_t g_freq;
static uint8_t g_brightness;
static uint8_t g_beep_enabled=1;

void beep_init(uint8_t beep_enabled)
{
	g_beep_enabled = beep_enabled;

	// Piezo pin
	PCONF(7, 3, (FUNC1 | OUL)); /* BEEP timer ctrl */

	// LED left PWM and pin
	PCONF(1, 0, (FUNC1 | OUL)); /* LED1 timer ctrl */
	PCONF(J, 1, OUL); /* LED2 to GND */

	// LED case
	// PCONF(7, 2, (FUNC1 | OUL)); /* LED case timer ctrl */
	PCONF(7, 2, OUL); /* LED case timer ctrl */

	// LED right
	PCONF(J, 7, OUL); /* LED case timer ctrl */

	TA0CTL = TASSEL__SMCLK;  // SM as clock
	TA0CCR2 = 1; /* beep */
}

uint8_t hal_beep_get_enabled()
{
	return g_beep_enabled;
}

void hal_beep_set_enabled(uint8_t enabled)
{
	g_beep_enabled = enabled;
}


static void beep_backlight(uint16_t freq, uint8_t brightness) {
	if(!freq && !brightness) { //no beep, no light
		TA0CCTL2 = 0;
		TA0CCTL1 = 0;
		TA0CTL &= ~MC_3;
		PCONF(J, 1, OUL); //light off
	}
	else if(freq && !brightness) { //beep, no light
		TA0CCTL2 = OUTMOD_4;
		TA0CCR0 = 2000000/freq;
		TA0CTL |= MC__UP;  // Timer counts up to TAxCCR0
		PCONF(J, 1, OUL); //light off
	}
	else if(!freq && brightness) { //no beep, light
		TA0CCTL2 = 0;
		TA0CCR0 = 30000; // PWM freq: 100 Hz
		TA0CTL |= MC__UP;
		TA0CCR1 = brightness * 3000;
		TA0CCTL1 = OUTMOD_3;
		PCONF(J, 1, OUH); //light on
	}
	else if(freq && brightness) { //beep, light
		TA0CCTL2 = OUTMOD_4;
		TA0CCR0 = 2000000/freq;
		TA0CCR1 = (TA0CCR0/10)*brightness;
		TA0CCTL1 = OUTMOD_3;
		TA0CTL |= MC__UP;

		PCONF(J, 1, OUH); //light on
	}
}

void hal_beep(uint16_t freq) {
	if(!g_beep_enabled)
		return;
	g_freq = freq;
	beep_backlight(g_freq, g_brightness);
}

void hal_rightled(uint8_t state) {
	if(state) {
		PCONF(J, 7, OUH); //light on
	}
	else {
		PCONF(J, 7, OUL); //light off
	}
}

void hal_caseled(uint8_t state) {
	// TODO
	// Use PWM instead
	if(state) {
		PCONF(7, 2, OUH); //light on
	}
	else {
		PCONF(7, 2, OUL); //light off
	}
}

void hal_backled(uint8_t state) {
	// TODO
	// Use PWM instead
	if(state) {
		PCONF(J, 1, OUH); //light on
	}
	else {
		PCONF(J, 1, OUL); //light off
	}
}

void hal_backlight_set(uint8_t brightness) {
	g_brightness = brightness;
	beep_backlight(g_freq, g_brightness);
}
