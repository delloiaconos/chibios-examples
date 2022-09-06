/*
    NeaPolis Innovation Summer Campus 2021 Examples 
    Copyright (C) 2020-2021 Salvatore Dello Iacono [delloiaconos@gmail.com]

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
 * How use a thread to read the user input and a thread to manage the LED.
 */

#include "ch.h"
#include "hal.h"


#define EXTBTN_PORT     GPIOC
#define EXTBTN_PIN      7U
#define EXTBTN_LINE     PAL_LINE( EXTBTN_PORT, EXTBTN_PIN )


#define LED_BLU_PORT    GPIOA
#define LED_BLU_PIN     7U
#define LED_BLU_LINE    PAL_LINE( LED_BLU_PORT, LED_BLU_PIN )

/*
 * This global flag variable will communicate if the button was pressed!
 */
static uint32_t flag = 0;

/*
 * Working Area and Thread declarations.
 */
static THD_WORKING_AREA( waLed, 128 );
static THD_FUNCTION( thdLed, arg ) {
  (void) arg;

  palSetLineMode( LED_BLU_LINE, PAL_MODE_OUTPUT_PUSHPULL );
  while( 1 ) {
    /* If the flag equals to 1 than:
     * 1- the flag becomes 0 (again)
     * 2- the led toggled
     */
    if( flag == 1 ) {
      flag = 0;
      palToggleLine( LED_BLU_LINE );
    }
    chThdSleepMilliseconds( 200 );
  }
}


int main(void) {

  halInit();
  chSysInit();

  chThdCreateStatic(waLed, sizeof(waLed), NORMALPRIO + 1, thdLed, NULL );

  palSetLineMode( EXTBTN_LINE, PAL_MODE_INPUT );
  while (true) {
    /* This loop does not set the flag variable to 0 */

    if( palReadLine( EXTBTN_LINE ) == PAL_LOW ) {
      while( palReadLine( EXTBTN_LINE ) == PAL_LOW ) {
        chThdSleepMilliseconds(20);
      }
      flag = 1;
    }

    chThdSleepMilliseconds(20);
  }
}
