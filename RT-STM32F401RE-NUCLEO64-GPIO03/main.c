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
 * [GPIO03] Using GPIO Peripherals - Example 03
 * How to use threads to control 2 LED with LINES and different timings
 */

#include "ch.h"
#include "hal.h"

#define LED_GREEN_PORT        GPIOA
#define LED_GREEN_PIN         5U

/*
 * Definition of LED_GREEN and LED_RED Lines.
 */
#define LED_GREEN_LINE          PAL_LINE( LED_GREEN_PORT, LED_GREEN_PIN )
#define LED_RED_LINE            PAL_LINE( GPIOA, 6U )

/*
 * Definition of a thread working area (waLed) and of a thread function thdLed
 */
static THD_WORKING_AREA( waLed, 128 );
static THD_FUNCTION( thdLed, arg ) {
  (void) arg;

  palSetLineMode( LED_RED_LINE, PAL_MODE_OUTPUT_PUSHPULL );
  while( 1 ) {
    palToggleLine( LED_RED_LINE );
    chThdSleepMilliseconds( 330 );
  }
}

int main(void) {

  halInit();
  chSysInit();

  /*
   * Creation of a new thread into a static memory area (waLed) with priority
   * "NORMALPRIO + 1" and thread function "thdLed"
   */
  chThdCreateStatic(waLed, sizeof(waLed), NORMALPRIO + 1, thdLed, NULL );

  palSetLineMode( LED_GREEN_LINE, PAL_MODE_OUTPUT_PUSHPULL );
  while (true) {
      palClearLine( LED_GREEN_LINE );
      chThdSleepMilliseconds(750);
      palSetLine( LED_GREEN_LINE );
      chThdSleepMilliseconds(250);
  }
}
