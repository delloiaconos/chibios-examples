/*
    NeaPolis Innovation Summer Campus 2022 Examples
    Copyright (C) 2020-2022 Salvatore Dello Iacono [delloiaconos@gmail.com]

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
 * [NISC2022-THD00] - Threading Example 00
 * DESCRIPTION: Simple way to exchange a variable between two threads.
 */
#include "ch.h"
#include "hal.h"

/* External and Internal Leds and Buttons definitions */
#define LED_LINE    PAL_LINE( GPIOA, 5U )
#define ELED_LINE   PAL_LINE( GPIOA, 0U )
#define EBTN_LINE   PAL_LINE( GPIOC, 3U )


/* FLAG: statically define variable */
static int flag = 0;

#define WA_BUTTON_SIZE 128
THD_WORKING_AREA( waButton, WA_BUTTON_SIZE );
THD_FUNCTION( thdButton, arg ) {
  (void) arg;
  chRegSetThreadName( "button" );

  palSetLineMode( EBTN_LINE, PAL_MODE_INPUT_PULLUP );
  while( 1 ){

    if( palReadLine( EBTN_LINE ) == PAL_LOW ) {
      chThdSleepMilliseconds(5);
      while( palReadLine( EBTN_LINE ) == PAL_LOW ) {
        chThdSleepMilliseconds(10);
      }

      /* Set the Flag! */
      flag = 1;
    }
    chThdSleepMilliseconds( 10 );
  }
}


#define WA_LED_SIZE 256
THD_WORKING_AREA( waLed, WA_LED_SIZE );
THD_FUNCTION( thdLed, arg ) {
  (void) arg;
  chRegSetThreadName( "led" );

  palSetLineMode( ELED_LINE, PAL_MODE_OUTPUT_PUSHPULL );
  palClearLine( ELED_LINE );

  while( 1 ){

    /* If the flag has been set to 1 then the External LED is toggled. */
    if( flag == 1 ) {

      palSetLine( ELED_LINE );
      chThdSleepMilliseconds( 500 );
      palClearLine( ELED_LINE );
      /* Reset the flag. */
      flag = 0;
    }

    chThdSleepMilliseconds( 10 );
  }
}

int main(void) {

  halInit();
  chSysInit();

  chThdCreateStatic( waButton, sizeof(waButton), NORMALPRIO+1, thdButton, NULL);
  chThdCreateStatic( waLed, sizeof(waLed), NORMALPRIO+1, thdLed, NULL);

  palSetLineMode( LED_LINE, PAL_MODE_OUTPUT_PUSHPULL );
  while (true) {
    palToggleLine( LED_LINE );
    chThdSleepMilliseconds(500);
  }
}
