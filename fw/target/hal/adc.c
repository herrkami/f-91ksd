#include "adc.h"
#include "io.h"
#include <msp430.h>


uint8_t hal_adc_conversion_running(void) {
	return !!(ADC12CTL1 & ADC12BUSY);
}

uint16_t hal_adc_sample_battery(void) {
	// Config and enable reference voltage
	// Config ADC MUX
	// Configure and enable ADC (single ended, )
	// Read sample
	// Disable ADC
	// Config ADC MUX
	// Config and disable reference voltage

	ADC12CTL1 |= ADC12PDIV__1;  // clock div to 1
	ADC12CTL1 |= ADC12SHP; // get sample-and-hold time from the sampling timer
	// ADC12CTL1 |= ADC12SSEL__MCLK; // set MCLK as clock source

	ADC12CTL2 |= ADC12PWRMD;  // set into low power mode

	ADC12CTL3 |= ADC12BATMAP;  // select VDDA/2 for input channel A31

	REFCTL0 |= REFON + REFVSEL_1 + REFTCOFF;  // set internal reference voltage to 2V0

	ADC12MCTL0 |= ADC12VRSEL3;  // VR+ = VeREF+, VR- = AVSS, not buffered

	ADC12CTL0 |= ADC12ON;  // switch on
	ADC12CTL0 |= ADC12ENC;  // enable conversion
	ADC12CTL0 |= ADC12SC;  // start conversion

	while(hal_adc_conversion_running());
	uint16_t result = ADC12MEM0;

	ADC12CTL0 &= ~ADC12ENC;  // disable conversion
	ADC12CTL0 &= ~ADC12ON;  // switch off

	return result;
	// return 3071;
}
