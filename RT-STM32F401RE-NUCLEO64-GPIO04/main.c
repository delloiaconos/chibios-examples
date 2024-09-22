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
 * [NISC2022-GPIO04] - A simple way to debounce an external button.
 * DESCRIPTION: Debouncing a button connected to a generic GPIO input with
 * external pull-up.
 * NOTE: Add an external pull-up resistor to the button; suggested R >= 4.7 kOhm
 */
#include "ch.h"
#include "hal.h"


#define EBTN_PORT     GPIOC
#define EBTN_PIN      7U
#define EBTN_LINE     PAL_LINE( EBTN_PORT, EBTN_PIN )

int main(void) {

  halInit();
  chSysInit();

  palSetLineMode( EBTN_LINE, PAL_MODE_INPUT );

  while (true) {
    /* flag variable will communicate if the button was pressed! */
    uint32_t flag = 0;

    if( palReadLine( EBTN_LINE ) == PAL_LOW ) {
      /* The following while loop holds until the button is released! */
      while( palReadLine( EBTN_LINE ) == PAL_LOW ) {
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
