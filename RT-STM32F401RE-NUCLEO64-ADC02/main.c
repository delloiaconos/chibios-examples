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
 * [ADC02] Using ADC Peripherals - Example 02
 * How to user asynchronous conversion call.
 *
 */
#include "ch.h"
#include "hal.h"


#define VOLTAGE_RES            ((float)3.3/4096)

#define LEDX_LINE               PAL_LINE( GPIOB, 10 )
#define LEDY_LINE               PAL_LINE( GPIOA, 8 )

#define MSG_ADC_OK               0x1337
#define MSG_ADC_KO               0x7331
static thread_reference_t trp = NULL;

/*
 * ADC streaming callback.
 */
static void adccallback(ADCDriver *adcp) {
  if (adcIsBufferComplete(adcp)) {
    chSysLockFromISR();
    chThdResumeI(&trp, (msg_t) MSG_ADC_OK );  /* Resuming the thread with message 0x1337.*/
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
  chThdResumeI(&trp, (msg_t) MSG_ADC_KO );  /* Resuming the thread with message 0x1337.*/
  chSysUnlockFromISR();
}

#define ADC_GRP_NUM_CHANNELS        2
#define ADC_GRP_BUF_DEPTH           16
static adcsample_t samples[ADC_GRP_NUM_CHANNELS * ADC_GRP_BUF_DEPTH];
/*
 * ADC conversion group.
 * Mode:        Continuous on 2 channels, SW triggered.
 * Channels:    IN10 (GPIOC0), IN11 (GPIOC1)
 */
static const ADCConversionGroup adcgrpcfg = {
  FALSE,
  ADC_GRP_NUM_CHANNELS,
  adccallback,
  adcerrorcallback,
  0,                        /* CR1 */
  ADC_CR2_SWSTART,          /* CR2 */
  ADC_SMPR1_SMP_AN10(ADC_SAMPLE_144) | ADC_SMPR1_SMP_AN11(ADC_SAMPLE_144),  /* SMPR1 */
  0,                        /* SMPR2 */
  0,                        /* HTR */
  0,                        /* LTR */
  ADC_SQR1_NUM_CH(ADC_GRP_NUM_CHANNELS),                                    /* SQR1 */
  0,                        /* SQR2 */
  ADC_SQR3_SQ2_N(ADC_CHANNEL_IN11) | ADC_SQR3_SQ1_N(ADC_CHANNEL_IN10)       /* SQR3 */
};


static float converted[ADC_GRP_NUM_CHANNELS];
static THD_WORKING_AREA( waLed, 128 );
static THD_FUNCTION( thdLed, arg ) {
  (void) arg;

  palSetLineMode( LEDX_LINE, PAL_MODE_OUTPUT_PUSHPULL );
  palSetLineMode( LEDY_LINE, PAL_MODE_OUTPUT_PUSHPULL );

  /*
   * Group setting as analog input:
   *    PORTC PIN 0 -> ADC1_CH10
   *    PORTC PIN 1 -> ADC1_CH11
   */
  palSetGroupMode(GPIOC, PAL_PORT_BIT(0) | PAL_PORT_BIT(1),
                  0, PAL_MODE_INPUT_ANALOG);

  adcStart(&ADCD1, NULL);

  while( true ) {
    msg_t msg;
    float chX, chY;
    int i;

    chSysLock();
    adcStartConversionI(&ADCD1, &adcgrpcfg, samples, ADC_GRP_BUF_DEPTH);
    msg = chThdSuspendS(&trp);
    chSysUnlock();

    /* Check if acquisition is OK or KO */
    if( (uint32_t) msg == MSG_ADC_KO ) {
      continue;
    }

    /*
     * Clean the buffer
     */
    for( i = 0; i < ADC_GRP_NUM_CHANNELS; i++ ) {
      converted[i] = 0.0f;
    }

    for( i = 0; i < ADC_GRP_NUM_CHANNELS * ADC_GRP_BUF_DEPTH; i++ ) {
      converted[ i % ADC_GRP_NUM_CHANNELS] += (float) samples[i] * VOLTAGE_RES;
    }

    for( i = 0; i < ADC_GRP_NUM_CHANNELS; i++ ) {
      converted[i] /= ADC_GRP_BUF_DEPTH;
    }

    /* Copy converted values into a new variable */
    chX = converted[0];
    chY = converted[1];

    /* Update output! */
    if( chX > 2.0f ) {
      palSetLine( LEDX_LINE );
    } else {
      palClearLine( LEDX_LINE );
    }

    if( chY > 2.0f ) {
      palSetLine( LEDY_LINE );
    } else {
      palClearLine( LEDY_LINE );
    }

  }

}
/*
 * Application entry point.
 */
int main(void) {

  halInit();
  chSysInit();


  chThdCreateStatic( waLed, sizeof( waLed), NORMALPRIO + 5, thdLed, (void*) NULL );

  while (true) {
    palToggleLine( LINE_LED_GREEN );
    chThdSleepMilliseconds(500);
  }

  adcStop(&ADCD1);
}
