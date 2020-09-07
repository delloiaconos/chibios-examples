/*
    NeaPolis Innovation Summer Campus 2020 Examples
    Copyright (C) 2020 Salvatore Dello Iacono [delloiaconos@gmail.com]

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
 * Simplest project with single acquisition, acquires ADC_GRP_BUF_DEPTH samples
 * from a single channel (regular sequence with only one channel) and calculates
 * the average value to turn on a LED when this is greater than a threshold.
 */

#include "ch.h"
#include "hal.h"

#define VOLTAGE_RES            ((float)3.3/4096)

#define ADC_GRP_NUM_CHANNELS   1
#define ADC_GRP_BUF_DEPTH      16

static adcsample_t samples[ADC_GRP_NUM_CHANNELS * ADC_GRP_BUF_DEPTH];

/*
 * ADC streaming callback.
 */
size_t nx = 0;
static void adccallback(ADCDriver *adcp) {
  if (adcIsBufferComplete(adcp)) {
    /* Second Half of the buffer is completed */
    nx += 1;
  }
}

/*
 * ADC error callback.
 */
static void adcerrorcallback(ADCDriver *adcp, adcerror_t err) {
  (void)adcp;
  (void)err;
}

/*
 * ADC conversion group.
 * Mode:        Linear buffer, 8 samples of 1 channel, SW triggered.
 * Channels:    IN11.
 */
static const ADCConversionGroup adcgrpcfg = {
  FALSE,                            /* circular     : Circular buffer mode  */
  ADC_GRP_NUM_CHANNELS,             /* num_channels : Number of channels    */
  adccallback,                      /* end_cb       : End Callback          */
  adcerrorcallback,                 /* error_cb     : Error Callback        */
  0,                                /* CR1      */
  ADC_CR2_SWSTART,                  /* CR2      */
  ADC_SMPR1_SMP_AN11(ADC_SAMPLE_3), /* SMPR1    */
  0,                                /* SMPR2    */
  0,                                /* HTR      */
  0,                                /* LTR      */
  0,                                /* SQR1     */
  0,                                /* SQR2     */
  ADC_SQR3_SQ1_N(ADC_CHANNEL_IN11)  /* SQR3     */
};

/*
 * Application entry point.
 */
int main(void) {

  halInit();
  chSysInit();

  /*
   * Setting up as analog inputs GPIOC PIN 1.
   * Arduino A4 (Nucleo64 Board) -> PC1 (STM32F401RE) -> ADC1_CH11 (ADC)
   */
  palSetPadMode(GPIOC, 1, PAL_MODE_INPUT_ANALOG);

  /*
   * Activates the ADC1 driver and the temperature sensor.
   */
  adcStart(&ADCD1, NULL);

  /*
   * Normal main() thread activity, in this demo it does nothing.
   */
  while (true) {
    if (palReadLine( LINE_BUTTON ) == PAL_LOW) {
      float avg = 0.0f;

      /*
       * Capture, Convert samples and then make the average value!
       */
      adcConvert(&ADCD1, &adcgrpcfg, samples, ADC_GRP_BUF_DEPTH);
      for( int i = 0; i < ADC_GRP_BUF_DEPTH; i++ ) {
        avg += (float) samples[i] * VOLTAGE_RES;
      }
      avg /= ADC_GRP_BUF_DEPTH;

      if( avg > 2.0f ) {
        palSetLine( LINE_LED_GREEN );
      } else {
        palClearLine( LINE_LED_GREEN );
      }
    }

    chThdSleepMilliseconds(50);
  }

  adcStop(&ADCD1);
}
