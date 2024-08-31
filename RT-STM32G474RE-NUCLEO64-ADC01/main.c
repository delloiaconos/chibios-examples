/*
    NeaPolis Innovation Summer Campus Examples
    Copyright (C) 2020-2024 Salvatore Dello Iacono [delloiaconos@gmail.com]
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
        http://www.apache.org/licenses/LICENSE-2.0
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/*
 * [ADC01] Using ADC Peripherals - Example 01
 * Project with asynchronous acquisition on a single channel.
 * This example acquires ADC_GRP_BUF_DEPTH samples from the ADC_IN1 channel
 * (regular sequence with only one channel) and uses a callback to evalutate
 * the sampling interval from a single channel.
 * The trigger pin (LINE_TRIG) is used to measure execution time and demonstrate
 * the callback period and the Sampling Period.
 */

#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#define VOLTAGE_RES            ((float)3.3/4096)

#define ADC_GRP_NUM_CHANNELS   1
#define ADC_GRP_BUF_DEPTH      32


#define LINE_TRIG              PAL_LINE( GPIOA, 1U )

/* Toggles the PIN LINE_TRIGGER in order to externally measure the
 * Sampling Period.
 * The expected period can be calculated as:
 * ADC_GRP_NUM_CHANNELS * ADC_GRP_BUF_DEPTH * ( 0.5 + SMPR + 12 ) / ADC_CLK
 *
 * 1 * 32 * ( 0.5 + 247.5 + 0.5 ) / ( 42.5 [MHz] ) = 195.7 [us]
 */
static void adccallback(ADCDriver *adcp) {
  if (adcIsBufferComplete(adcp)) {
    palClearLine( LINE_TRIG );
  } else {
    palSetLine( LINE_TRIG );
  }
}

static adcsample_t adc_buffer[ADC_GRP_NUM_CHANNELS * ADC_GRP_BUF_DEPTH];

/*
 * ADC conversion group 1.
 * Mode:        Circular buffer, ADC_GRP_NUM_CHANNELS samples of 1 channel, SW triggered.
 * Channels:    ADC_IN1.
 */
static const ADCConversionGroup adcgrpcfg = {
          .circular     = true,
          .num_channels = ADC_GRP_NUM_CHANNELS,
          .end_cb       = adccallback,
          .error_cb     = NULL,
          .cfgr         = ADC_CFGR_CONT,
          .cfgr2        = 0U,
          .tr1          = ADC_TR_DISABLED,
          .tr2          = ADC_TR_DISABLED,
          .tr3          = ADC_TR_DISABLED,
          .awd2cr       = 0U,
          .awd3cr       = 0U,
          .smpr         = {
            ADC_SMPR1_SMP_AN1(ADC_SMPR_SMP_247P5),
            0U
          },
          .sqr          = {
            ADC_SQR1_SQ1_N(ADC_CHANNEL_IN1),
            0U,
            0U,
            0U
          }
        };

/*
 * Application entry point.
 */
int main(void) {

  halInit();
  chSysInit();

  palSetLineMode( LINE_TRIG, PAL_MODE_OUTPUT_PUSHPULL );

  /* ADC input:
   * GPIOA 0 -> A0
   */
  palSetPadMode(GPIOA, 0U, PAL_MODE_INPUT_ANALOG);

  /*
   * Starting ADCD1 driver and an Asynchronous Continuous Conversion.
   */
  adcStart(&ADCD1, NULL);
  adcSTM32EnableVREF(&ADCD1);

  adcStartConversion(&ADCD1, &adcgrpcfg, adc_buffer, ADC_GRP_NUM_CHANNELS * ADC_GRP_BUF_DEPTH);

  /*
   * Normal main() thread activity, in this demo it does nothing.
   */
  while (true) {
    palToggleLine( LINE_LED_GREEN );
    chThdSleepMilliseconds( 500 );
  }

  adcStop(&ADCD1);
}
