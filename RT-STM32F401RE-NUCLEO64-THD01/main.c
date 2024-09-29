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
 * [THD01] Chibios Multithreading & Libraries - Example 01
 * A simple parametric thread.
 */

#include "ch.h"
#include "hal.h"

/* External and Internal Leds and Buttons definitions */
#define LED_LINE    PAL_LINE( GPIOA, 5U )
#define ELED_LINE   PAL_LINE( GPIOA, 0U )

#define WA_LED_SIZE      128
THD_WORKING_AREA( waLed, WA_LED_SIZE );
THD_FUNCTION( thdLed, arg ) {

  ioline_t line = *((ioline_t *) arg );
  palSetLineMode( line, PAL_MODE_OUTPUT_PUSHPULL );

  while( 1 ) {
      palToggleLine( line );
      chThdSleepMilliseconds( 500 );
  }
}


int main(void) {

  halInit();
  chSysInit();

  ioline_t line = ELED_LINE;

  chThdCreateStatic(waLed, sizeof(waLed), NORMALPRIO-1, thdLed, (void*) &line );

  while( 1 ) {
    palToggleLine( LED_LINE );
    chThdSleepMilliseconds( 250 );
  }
}

