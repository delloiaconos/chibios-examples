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
 * [ADC01] Using ADC Peripherals - Example 04
 * ADC Acquisition with Timer as trigger.
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
static const GPTConfig gpt3cfg = {
  1000000U,         /* frequency */
  NULL,             /* callback */
  TIM_CR2_MMS_1,    /* cr2: MMS = 010 = TRGO on Update Event. */
  0U                /* dier */
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


static const ADCConversionGroup adcgrpcfg = {
  TRUE,
  ADC_GRP_NUM_CHANNELS,
  adccallback,
  adcerrorcallback,
  0,                                    /* CR1 */
  ADC_CR2_EXTEN_0 | ADC_CR2_EXTSEL_3,   /* CR2 */
  ADC_SMPR1_SMP_AN10(ADC_SAMPLE_3) | ADC_SMPR1_SMP_AN11(ADC_SAMPLE_3) , /* SMPR1 */
  0,                        /* SMPR2 */
  0,                        /* HTR */
  0,                        /* LTR */
  ADC_SQR1_NUM_CH(ADC_GRP_NUM_CHANNELS),  /* SQR1 */
  0,                        /* SQR2 */
  ADC_SQR3_SQ2_N(ADC_CHANNEL_IN11) | ADC_SQR3_SQ1_N(ADC_CHANNEL_IN10) /* SQR3 */
};


static THD_WORKING_AREA( waLed, 256 );
static THD_FUNCTION( thdLed, arg ) {
  systime_t start, stop;
  sysinterval_t delta;

  (void) arg;
  /*
   * Setting as analog input:
   *    PORTC PIN 0 -> ADC1_CH10
   *    PORTC PIN 1 -> ADC1_CH11
   */
  palSetGroupMode(GPIOC, PAL_PORT_BIT(0) | PAL_PORT_BIT(1),
                  0, PAL_MODE_INPUT_ANALOG);

  /*
   * Starting GPT3 driver, it is used for triggering the ADC.
   * Starting the ADC1 driver.
   */
  gptStart(&GPTD3, &gpt3cfg);
  adcStart(&ADCD1, NULL);

  adcStartConversion(&ADCD1, &adcgrpcfg, samples, ADC_GRP_BUF_DEPTH);

  /*
   * Start the GPT3 driver with a period of 10000 cycles and a
   * frequency of 1000000 Hz
   */
  gptStartContinuous(&GPTD3, 10000);
  start = chVTGetSystemTime();

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

  sdStart( &SD2, NULL );

  chThdCreateStatic( waLed, sizeof( waLed), NORMALPRIO + 5, thdLed, (void*) NULL );

  while (true) {
    palToggleLine( LINE_LED_GREEN );
    chThdSleepMilliseconds(500);
  }

}
