/*
    ChibiOS Examples 
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
 * [ADC00] Using ADC Peripherals - Example 00
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
 * ADC conversion group 1.
 * Mode:        Linear buffer, 16 samples of 1 channel, SW triggered.
 * Channels:    IN1.
 */
static const ADCConversionGroup adcgrpcfg1 = {
          .circular     = false,
          .num_channels = ADC_GRP_NUM_CHANNELS,
          .end_cb       = NULL,
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

  /* ADC input.*/
  palSetPadMode(GPIOA, 0U, PAL_MODE_INPUT_ANALOG);

  /*
   * Starting ADCD1 driver and the temperature sensor.
   */
  adcStart(&ADCD1, NULL);
  adcSTM32EnableVREF(&ADCD1);

  /*
   * Normal main() thread activity, in this demo it does nothing.
   */
  while (true) {
    if (palReadLine( LINE_BUTTON ) == PAL_HIGH) {
      float avg = 0.0f;
      /*
       * Capture, Convert samples and then make the average value!
       */
      adcConvert(&ADCD1, &adcgrpcfg1, samples, ADC_GRP_BUF_DEPTH);

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
