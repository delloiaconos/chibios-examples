/*
    NeaPolis Innovation Summer Campus Examples
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

#include "ch.h"
#include "hal.h"

#include "adcmanager.h"

#include "shell.h"
#include "chprintf.h"
#include <string.h>
#include <stdio.h>

/* Module Variables */
adcsample_t * samples;
size_t samples_len;

/* DAC Configuration Structure */
static const DACConfig daccfg = {
  .init         = 1000U,
  .datamode     = DAC_DHRM_12BIT_RIGHT,
  .cr           = 0
};

/* DAC Single Shot Function */
void dacSingleShot(DACDriver *dacp, dacsample_t sample ) {
  osalSysLock();

  osalDbgCheck( (dacp != NULL) );
  osalDbgAssert((dacp->state == DAC_READY) ||
                (dacp->state == DAC_ERROR),
                "not ready");

  dacp->params->dac->DHR12R1 = sample;
  dacp->params->dac->CR = DAC_CR_EN1;
  dacp->params->dac->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;

  osalSysUnlock();
}

/* SHELL command functions */

static void cmd_adc(BaseSequentialStream *chp, int argc, char *argv[]) {
  (void) argc;
  (void) argv;
  float avg = 0.0;
  for( int i = 0; i < (int) samples_len; i++ ) {
    avg += samples[i];
  }
  avg = avg/( (int) samples_len);
  chprintf(chp, "ADC average %.2f\r\n", avg );
}


static void cmd_dac(BaseSequentialStream *chp, int argc, char *argv[]) {
  if( argc == 1 ) {
    int val = 0;
    sscanf( argv[0], "%d", &val );
    if( val >= 0 ) {
      chprintf(chp, "DAC output %d\r\n", val );
      dacSingleShot( &DACD1, val );
    } else {
      chprintf(chp, "<val> should be >= 0\r\n" );
    }
  } else {
    chprintf(chp, "Usage: dac <val>\r\n");
  }
}

/* SHELL custom Working Area and configurations
 * structures. */
THD_WORKING_AREA( waShell, 3*1024 );

static const ShellCommand commands[] = {
  {"adc", cmd_adc},
  {"dac", cmd_dac},
  {NULL, NULL}
};

static const ShellConfig shell_cfg1 = {
  (BaseSequentialStream *) &SD2,
  commands
};


/*
 * Supposing you have: STM32 NUCLEO-G474RE
 * ARDUINO A0 -> PA0 -> ADC12_IN1
 * ARDUINO A2 -> PA4 -> DAC1_OUT1
 */
int main(void) {

  halInit();
  chSysInit();

  /* DAC setup and start.*/
  palSetPadMode(GPIOA, 4U, PAL_MODE_INPUT_ANALOG);
  dacStart(&DACD1, &daccfg);

  /* SERIAL port setup and start.*/
  palSetPadMode( GPIOA, 2, PAL_MODE_ALTERNATE(7) );
  palSetPadMode( GPIOA, 3, PAL_MODE_ALTERNATE(7) );

  sdStart(&SD2, NULL);

  /* AdcManager start.*/
  samples = (adcsample_t *) NULL;
  samples_len = 0;

  startAdcManager( &samples, &samples_len );

  /* Shell manager initialization and start.*/
  shellInit();
  chThdCreateStatic(waShell, sizeof(waShell), NORMALPRIO, shellThread, (void *)&shell_cfg1);

  /* Main Thread Loop */
  while (true) {
    chThdSleepMilliseconds(100);
  }

  dacStop( &DACD1 );
}
