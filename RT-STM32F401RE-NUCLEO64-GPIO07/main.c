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
 * [GPIO07] Using GPIO Peripherals - Example 07
 * How to use PAL_LINEs as variables and iterate through an array of output 
 * lines. This example iterate through three different LEDs states (Off, Red, 
 * Green, Blu) using the User Button.
 */

#include "ch.h"
#include "hal.h"

/* External LEDs Red, Green and Blue: Port, Pin, Line */
#define RLED_LINE   PAL_LINE( GPIOA, 0U ) // ARDUINO A0 (CN8.1)
#define GLED_LINE   PAL_LINE( GPIOA, 1U ) // ARDUINO A1 (CN8.2)
#define BLED_LINE   PAL_LINE( GPIOA, 4U ) // ARDUINO A2 (CN8.3)

#define ARRAY_LEN(a)            (sizeof(a)/sizeof(a[0]))

int main(void) {

  halInit();
  chSysInit();

  uint32_t i, state;
  ioline_t leds[] = { RLED_LINE, GLED_LINE, BLED_LINE };

  /* Setup Leds Outputs */
  for( i = 0; i < ARRAY_LEN(leds); i++ ) {
    palSetLineMode( leds[i], PAL_MODE_OUTPUT_PUSHPULL );
  }

  state = i;
  while( 1 ){

    if( palReadLine( LINE_BUTTON ) == PAL_LOW ) {
      chThdSleepMilliseconds(5);
      while( palReadLine( LINE_BUTTON ) == PAL_LOW ) {
        chThdSleepMilliseconds(10);
      }

      /* Action to be performed on Positive Edge Detection */
      state = (state+1) % ( ARRAY_LEN(leds) + 1 );
    }

    for( i = 0; i < ARRAY_LEN(leds); i++ ) {
      palWriteLine( leds[i], i == state ? PAL_HIGH : PAL_LOW );
    }

    chThdSleepMilliseconds( 10 );
  }
}