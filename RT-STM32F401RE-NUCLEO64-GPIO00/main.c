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
 * [NISC2022-GPIO00] - Onboard Green LED Toggling.
 * DESCRIPTION: Simplest project using ChibiOS-RT and HAL PAL driver on
 * STM32 Nucleo-64 Onboard Green LED.
 */

#include "ch.h"
#include "hal.h"

int main(void) {

  halInit();
  chSysInit();

  while( 1 ) {
      /*
       * Invert GPIOA LED_GREEN pin Output and wait
       */
      palTogglePad(GPIOA, GPIOA_LED_GREEN);
      chThdSleepMilliseconds(1000);
  }
}
