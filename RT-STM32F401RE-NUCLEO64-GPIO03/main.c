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
 * [NISC2022-GPIO03] - Reading the on-board user button.
 * DESCRIPTION: Read the LINE_BUTTON that refers to the USER button on the
 * NUCLEO-64 Board and if it has been pressed (condition PAL_LOW) turn-on
 * the on-board LED.
 */

#include "ch.h"
#include "hal.h"

int main(void) {

  halInit();
  chSysInit();

  while (true) {
      /*
       * Check if button is pressed.
       */
      if( palReadLine( LINE_BUTTON ) == PAL_LOW ) {
        palSetLine( LINE_LED_GREEN );
      } else {
        palClearLine( LINE_LED_GREEN );
      }

      chThdSleepMilliseconds( 25 );
  }
}
