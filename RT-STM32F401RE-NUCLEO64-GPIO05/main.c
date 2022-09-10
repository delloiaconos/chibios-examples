/*
    NeaPolis Innovation Summer Campus Examples
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
 * [NISC2022-GPIO05] - Multithreading use of GPIO with button and LED.
 * DESCRIPTION: How use a thread to read the user input and a thread to manage the LED.
 * NOTE: Add an external pull-up resistor to the button; suggested R >= 4.7 kOhm
 */

#include "ch.h"
#include "hal.h"


#define EBTN_PORT     GPIOC
#define EBTN_PIN      7U
#define EBTN_LINE     PAL_LINE( EBTN_PORT, EBTN_PIN )

#define BLED_PORT    GPIOA
#define BLED_PIN     7U
#define BLED_LINE    PAL_LINE( BLED_PORT, BLED_PIN )

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

  palSetLineMode( BLED_LINE, PAL_MODE_OUTPUT_PUSHPULL );
  while( 1 ) {
    /* If the flag equals to 1 than:
     * 1- the flag becomes 0 (again)
     * 2- the led toggled
     */
    if( flag == 1 ) {
      flag = 0;
      palToggleLine( BLED_LINE );
    }
    chThdSleepMilliseconds( 200 );
  }
}


int main(void) {

  halInit();
  chSysInit();

  chThdCreateStatic(waLed, sizeof(waLed), NORMALPRIO + 1, thdLed, NULL );

  palSetLineMode( EBTN_LINE, PAL_MODE_INPUT );
  while (true) {

    /* This loop does not set the flag variable to 0 */
    if( palReadLine( EBTN_LINE ) == PAL_LOW ) {
      while( palReadLine( EBTN_LINE ) == PAL_LOW ) {
        chThdSleepMilliseconds(20);
      }
      flag = 1;
    }

    chThdSleepMilliseconds(20);
  }
}
