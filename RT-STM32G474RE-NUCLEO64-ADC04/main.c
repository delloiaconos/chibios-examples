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
 * [ADC04] Using ADC Peripherals - Example 04
 * ADC Acquisition triggered by a Timer.
 * This example makes use of ARDUINO A0 and A1 pins. ADC is configured
 * to execute a continuous acquisition on a circula buffer triggerd by
 * STM32 TIM4. The ADC Thread waits for the message to arrive and then
 * calculates and sends over serial the elapsed time.
 */

#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#define MSG_ADC_OK               0x1337
#define MSG_ADC_KO               0x7331
static thread_reference_t trp = NULL;

BaseSequentialStream * chp = (BaseSequentialStream *) &SD2;

/*
 * GPT4 configuration. This timer is used as trigger for the ADC.
 */
const GPTConfig gpt4cfg = {
  .frequency    =  1000000U,
  .callback     =  NULL,
  .cr2          =  TIM_CR2_MMS_1,   /* MMS = 010 = TRGO on Update Event.    */
  .dier         =  0U
};


#define ADC_GRP_NUM_CHANNELS        2
#define ADC_GRP_BUF_DEPTH           20

static adcsample_t samples[ADC_GRP_NUM_CHANNELS * ADC_GRP_BUF_DEPTH];
static float converted[ADC_GRP_NUM_CHANNELS] = { 0.0f };


static void adccallback(ADCDriver *adcp) {
  int i;

  if (adcIsBufferComplete(adcp)) {
    for( i = 0; i < ADC_GRP_NUM_CHANNELS; i++ ) {
      converted[i] = 0.0f;
    }

    for( i = 0; i < ADC_GRP_NUM_CHANNELS * ADC_GRP_BUF_DEPTH; i++ ) {
      converted[ i % ADC_GRP_NUM_CHANNELS] += (float) samples[i] * 100 / 4096;
    }

    for( i = 0; i < ADC_GRP_NUM_CHANNELS; i++ ) {
      converted[i] /= ADC_GRP_BUF_DEPTH;
    }

    chSysLockFromISR();
    chThdResumeI(&trp, (msg_t) MSG_ADC_OK );
    chSysUnlockFromISR();
  }
}


static void adcerrorcallback(ADCDriver *adcp, adcerror_t err) {
  (void)adcp;
  (void)err;
  chSysLockFromISR();
  chThdResumeI(&trp, (msg_t) MSG_ADC_KO );
  chSysUnlockFromISR();
}

const ADCConversionGroup adcgrpcfg = {
  .circular     = true,
  .num_channels = ADC_GRP_NUM_CHANNELS,
  .end_cb       = adccallback,
  .error_cb     = adcerrorcallback,
  .cfgr         = ADC_CFGR_EXTEN_RISING |
                  ADC_CFGR_EXTSEL_SRC(12),  /* TIM4_TRGO */
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


static THD_WORKING_AREA( waAdc, 256 );
static THD_FUNCTION( thdAdc, arg ) {
  systime_t start, stop;
  sysinterval_t delta;

  (void) arg;

  /*
   * Activates the serial driver 2 using the A2 and A3 pins.
   */
  palSetPadMode( GPIOA, 2, PAL_MODE_ALTERNATE(7) );
  palSetPadMode( GPIOA, 3, PAL_MODE_ALTERNATE(7) );

  sdStart( &SD2, NULL );

  /*
   * ADC input:
   *    PORTA PIN 0 -> ADC1_CH1
   *    PORTA PIN 1 -> ADC1_CH2
   */
  palSetPadMode(GPIOA, 0U, PAL_MODE_INPUT_ANALOG);
  palSetPadMode(GPIOA, 1U, PAL_MODE_INPUT_ANALOG);

  /*
   * Starting GPT4 driver, it is used for triggering the ADC.
   * Starting the ADC1 driver.
   */
  gptStart(&GPTD4, &gpt4cfg);
  adcStart(&ADCD1, NULL);

  adcStartConversion(&ADCD1, &adcgrpcfg, samples, ADC_GRP_BUF_DEPTH);

  /*
   * Start the GPT4 driver with a period of 10000 cycles and a
   * frequency of 1000000 Hz
   */
  gptStartContinuous(&GPTD4, 10000);
  start = chVTGetSystemTime();

  /* Expected time:
   * 20 * 10000 [cnt] / 1000000 [cnt/s] = 200/1000 [s] = 200 [ms]
   */
  while( true ) {
    msg_t msg;
    float chX, chY;

    chSysLock();
    msg = chThdSuspendS(&trp);
    stop = chVTGetSystemTime();
    chSysUnlock();

    /*
     * Copy converted values into local variables.
     */
    chX = converted[0];
    chY = converted[1];
    delta = chTimeDiffX( start, stop );

    /*
     * Check the message and inform the user
     */
    if( (uint32_t) msg == MSG_ADC_OK ) {
      chprintf( chp, "Elapsed = %d ms\n\r", TIME_I2MS( delta ) );
      chprintf( chp, "X channel = %4.1f %%\n\r", chX );
      chprintf( chp, "Y channel = %4.1f %%\n\r", chY );
    } else {
      chprintf( chp, "Elapsed = %d ms\r\n" , TIME_I2MS( delta ) );
      chprintf( chp, "Error!\r\n" );
    }

    start = stop;
  }
  adcStop(&ADCD1);
}

/*
 * Application entry point.
 */
int main(void) {

  halInit();
  chSysInit();

  chThdCreateStatic( waAdc, sizeof(waAdc), NORMALPRIO + 5, thdAdc, (void*) NULL );

  while (true) {
    palToggleLine( LINE_LED_GREEN );
    chThdSleepMilliseconds(500);
  }

}
