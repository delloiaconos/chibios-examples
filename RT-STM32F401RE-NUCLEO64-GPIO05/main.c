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
 * [GPIO05] Using GPIO Peripheral - Example 05
 * How to debounce an user button: a simple way!
 */

#include "ch.h"
#include "hal.h"


#define EXTBTN_PORT     GPIOC
#define EXTBTN_PIN      7U
#define EXTBTN_LINE     PAL_LINE( EXTBTN_PORT, EXTBTN_PIN )

int main(void) {

  halInit();
  chSysInit();

  palSetLineMode( EXTBTN_LINE, PAL_MODE_INPUT );

  while (true) {
    /* flag variable will communicate if the button was pressed! */
    uint32_t flag = 0;

    if( palReadLine( EXTBTN_LINE ) == PAL_LOW ) {
      /* The following while loop holds until the button is released! */
      while( palReadLine( EXTBTN_LINE ) == PAL_LOW ) {
        chThdSleepMilliseconds(20);
      }
      flag = 1;
    }
    /* If the button has been pressed the Onboard Green LED is toggled. */
    if( flag == 1 ) {
      palToggleLine( LINE_LED_GREEN );
    }

    chThdSleepMilliseconds(20);
  }
}
