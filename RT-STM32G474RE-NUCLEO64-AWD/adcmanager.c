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
 * adcmanager.c -
*/
#include "ch.h"
#include "hal.h"

#include "adcmanager.h"

/* ADC Constants and module variables. */
#define VOLTAGE_RES            ((float)3.3/4096)
#define ADCMGR_BUF_DEPTH        8

static adcsample_t adcmgs_buf[ADCMGR_BUF_DEPTH];
static thread_reference_t   thdrAdcManager;

/* ADC error callback. */
static void adcerrorcb(ADCDriver *adcp, adcerror_t err) {
  (void)adcp;

  chSysLockFromISR();
  chThdResumeI( &thdrAdcManager, err && ADC_ERR_AWD1 ? ADCMGR_MSG_ADW1 : ADCMGR_MSG_KO );
  chSysUnlockFromISR();

}

/*            ADC conversion group
 * Continuous Mode on 1 channels, SW triggered.
 * Channels:    IN1 (GPIOA0)
 * ADW1 Enabled with filter: 8 consecutive points.
 */
static const ADCConversionGroup adcgrpcfg = {
          .circular     = true,
          .num_channels = 1,
          .end_cb       = NULL,
          .error_cb     = adcerrorcb,
          .cfgr         = ADC_CFGR_CONT | ADC_CFGR_AWD1EN | ADC_CFGR_AWD1CH_0,
          .cfgr2        = 0U,
          .tr1          = ADC_TR( ADCMGR_ADW1_LTH, ADCMGR_ADW1_HTH ) | (0111 << 12),
          .tr2          = ADC_TR_DISABLED,
          .tr3          = ADC_TR_DISABLED,
          .awd2cr       = 0U,
          .awd3cr       = 0U,
          .smpr         = {
            ADC_SMPR1_SMP_AN1(ADC_SMPR_SMP_6P5),
            0U
          },
          .sqr          = {
            ADC_SQR1_SQ1_N(ADC_CHANNEL_IN1),
            0U,
            0U,
            0U
          }
        };

/*      Thread AdcManager
 * Manages the ADC conversion.
 */
THD_WORKING_AREA( waAdcManager, 512 );
THD_FUNCTION( thdAdcManager, arg ) {
  thread_t * thdpAdcBlinker = (thread_t *) arg;

  chRegSetThreadName( "AdcManager" );

  /* ADC pin configuration and start. */
  palSetPadMode(GPIOA, 0U, PAL_MODE_INPUT_ANALOG);
  adcStart(&ADCD1, NULL);

  while (true) {
    msg_t msg;
    unsigned int avg;

    /* Start conversion and then wait for Resume from callback. */
    adcStartConversion(&ADCD1, &adcgrpcfg, adcmgs_buf, ADCMGR_BUF_DEPTH);

    chSysLock();
    msg = chThdSuspendTimeoutS( &thdrAdcManager, ADCMANAGER_TIMEOUT );
    chSysUnlock();

    /* Calculate the average value from ADC samples. */
    avg = 0L;
    for( int i = 0; i < ADCMGR_BUF_DEPTH; i++ ) {
      avg = avg + (unsigned int) adcmgs_buf[i];
    }
    avg = avg / ADCMGR_BUF_DEPTH;

    /* Evaluate callback message and average value. */
    if( msg == MSG_TIMEOUT ) {
      chMsgSend(thdpAdcBlinker, ADCBLINKER_DEF_PERIOD );
    } else if( msg == ADCMGR_MSG_ADW1 ) {
      if( avg >= ADCMGR_ADW1_HTH ) {
        chMsgSend(thdpAdcBlinker, (msg_t) ADCBLINKER_HIGH_PERIOD );
      } else if( avg <= ADCMGR_ADW1_LTH ) {
        chMsgSend(thdpAdcBlinker, (msg_t) ADCBLINKER_LOW_PERIOD );
      }
    } else if( msg == ADCMGR_MSG_KO ) {
      break;
    }

    /* Hold-off before a new ADC Conversion starts. */
    chThdSleep( ADCMANAGER_TIMEOUT );
  }

  chThdTerminate( thdpAdcBlinker );
  adcStop( &ADCD1 );
}


/*            Thread AdcBlinker
 * Blinks LED based on adc analog whatchdog
 */

THD_WORKING_AREA( waAdcBlinker, 512 );
THD_FUNCTION( thdAdcBlinker, arg ) {
  (void) arg;
  sysinterval_t delay = ADCBLINKER_DEF_PERIOD;

  chRegSetThreadName( "AdcBlinker" );

  palSetLineMode( ADCMGR_LED_LINE, PAL_MODE_OUTPUT_PUSHPULL );
  while( !chThdShouldTerminateX() ) {
    thread_t * tp = chMsgPoll();

    if( tp != NULL ) {
      delay = (sysinterval_t) chMsgGet(tp);
    }

    palToggleLine( ADCMGR_LED_LINE );
    chThdSleep(delay);

    if( tp != NULL ) {
      chMsgRelease( tp, MSG_OK );
    }

  }
}

/*        AdcManager Start Function
 * Use this function to enable AdcManager.
 */
thread_t * startAdcManager( adcsample_t ** samples_ptr, size_t * len  ) {
  static thread_t * thdpAdcManager = (thread_t *) NULL;
  thread_t * thdpAdcBlinker = (thread_t *) NULL;

  if( (thdpAdcManager == (thread_t *) NULL) ) {

    thdpAdcBlinker = chThdCreateStatic( waAdcBlinker, sizeof(waAdcBlinker), NORMALPRIO+5, thdAdcBlinker, NULL );
    thdpAdcManager = chThdCreateStatic( waAdcManager, sizeof(waAdcManager), NORMALPRIO+2, thdAdcManager, (void*) thdpAdcBlinker );

    *samples_ptr = adcmgs_buf;
    *len = ADCMGR_BUF_DEPTH;
  }

  return thdpAdcManager;
}

