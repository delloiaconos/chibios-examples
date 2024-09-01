/*
    NeaPolis Innovation Summer Campus 2023 Examples
    Copyright (C) 2020-2023 Salvatore Dello Iacono [delloiaconos@gmail.com]

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
 * [ADC03] Using ADC Peripherals - Example 03
 * Asynchronous sampling on multiple channels and serial communication!
 * The ADC acquires multiple samples from ADC_IN1 and ADC_IN2 channels
 * within a regular sequence and and uses a callback to resume the thread when
 * the samples have been elaborated. The thread computes the mean value and
 * sends the converted values over serial.
 */

#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#define VOLTAGE_RES            ((float)3.3/4096)

#define MSG_ADC_OK               0x1337
#define MSG_ADC_KO               0x7331
static thread_reference_t trp = NULL;

BaseSequentialStream * chp = (BaseSequentialStream *) &SD2;

/*
 * ADC streaming callback.
 */
static void adccallback(ADCDriver *adcp) {
  if (adcIsBufferComplete(adcp)) {
    chSysLockFromISR();
    chThdResumeI(&trp, (msg_t) MSG_ADC_OK );   /* Resuming the thread.*/
    chSysUnlockFromISR();
  }
}

/*
 * ADC error callback.
 */
static void adcerrorcallback(ADCDriver *adcp, adcerror_t err) {
  (void)adcp;
  (void)err;
  chSysLockFromISR();
  chThdResumeI(&trp, (msg_t) MSG_ADC_KO );  /* Resuming the thread with Error Message.*/
  chSysUnlockFromISR();
}

#define ADC_GRP_NUM_CHANNELS        2
#define ADC_GRP_BUF_DEPTH           16
static adcsample_t samples[ADC_GRP_NUM_CHANNELS * ADC_GRP_BUF_DEPTH];

/*
 * ADC conversion group.
 * Mode:        Continuous on 2 channels, SW triggered.
 * Channels:    IN1 (GPIOA0), IN2 (GPIOA1)
 */
static const ADCConversionGroup adcgrpcfg = {
          .circular     = false,
          .num_channels = ADC_GRP_NUM_CHANNELS,
          .end_cb       = adccallback,
          .error_cb     = adcerrorcallback,
          .cfgr         = ADC_CFGR_CONT,
          .cfgr2        = 0U,
          .tr1          = ADC_TR_DISABLED,
          .tr2          = ADC_TR_DISABLED,
          .tr3          = ADC_TR_DISABLED,
          .awd2cr       = 0U,
          .awd3cr       = 0U,
          .smpr         = {
            ADC_SMPR1_SMP_AN1(ADC_SMPR_SMP_247P5) |
            ADC_SMPR1_SMP_AN2(ADC_SMPR_SMP_247P5),
            0U
          },
          .sqr          = {
            ADC_SQR1_SQ1_N(ADC_CHANNEL_IN1) | ADC_SQR1_SQ2_N(ADC_CHANNEL_IN2),
            0U,
            0U,
            0U
          }
        };


static THD_WORKING_AREA( waAdc, 1024 + sizeof(float) * ADC_GRP_NUM_CHANNELS );
static THD_FUNCTION( thdAdc, arg ) {
  (void) arg;

  /*
   * Group setting as analog input:
   *    PORTA PIN 0 -> ADC1_CH1
   *    PORTA PIN 1 -> ADC1_CH2
   */

  /* ADC inputs.*/
  palSetPadMode(GPIOA, 0U, PAL_MODE_INPUT_ANALOG);
  palSetPadMode(GPIOA, 1U, PAL_MODE_INPUT_ANALOG);

  adcStart(&ADCD1, NULL);

  while( true ) {
    float converted[ADC_GRP_NUM_CHANNELS] = { 0.0f };
    msg_t msg;
    int i;

    chSysLock();
    adcStartConversionI(&ADCD1, &adcgrpcfg, samples, ADC_GRP_BUF_DEPTH);
    msg = chThdSuspendS(&trp);
    chSysUnlock();

    /*
     * Check if acquisition is OK or KO
     */
    if( (uint32_t) msg == MSG_ADC_KO ) {
      continue;
    }

    /*
     * Clean the buffer and then calculate the average value.
     */
    for( i = 0; i < ADC_GRP_NUM_CHANNELS; i++ ) {
      converted[i] = 0.0f;
    }

    for( i = 0; i < ADC_GRP_NUM_CHANNELS * ADC_GRP_BUF_DEPTH; i++ ) {
      converted[ i % ADC_GRP_NUM_CHANNELS] += (float) samples[i] * 100 / 4096;
    }

    for( i = 0; i < ADC_GRP_NUM_CHANNELS; i++ ) {
      converted[i] /= ADC_GRP_BUF_DEPTH;
    }

    /*
     * Print converted values. (Set CHPRINTF_USE_FLOAT to TRUE)
     */
    chprintf( chp, "X channel = %4.1f %%\n\r", converted[0] );
    chprintf( chp, "Y channel = %4.1f %%\n\r", converted[1] );
  }

  adcStop(&ADCD1);

}

/*
 * Application entry point.
 */
int main(void) {

  halInit();
  chSysInit();
  /*
   * Activates the serial driver 2 using the A2 and A3 pins.
   */
  palSetPadMode( GPIOA, 2, PAL_MODE_ALTERNATE(7) );
  palSetPadMode( GPIOA, 3, PAL_MODE_ALTERNATE(7) );

  sdStart( &SD2, NULL );

  chThdCreateStatic( waAdc, sizeof( waAdc ), NORMALPRIO + 5, thdAdc, (void*) NULL );

  while (true) {
    palToggleLine( LINE_LED_GREEN );
    chThdSleepMilliseconds(500);
  }

}
