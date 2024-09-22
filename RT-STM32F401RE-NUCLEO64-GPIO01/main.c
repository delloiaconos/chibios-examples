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
 * [GPIO01] Using GPIO Peripheral - Example 01
 * How to use the GPIO peripheral to blink the on board Green LED
 */

#include "ch.h"
#include "hal.h"

int main(void) {

  halInit();
  chSysInit();

  while (true) {
      /*
       * Set GPIOA LED_GREEN pin to logic LOW and wait
       */
      palClearPad(GPIOA, GPIOA_LED_GREEN);
      chThdSleepMilliseconds(1000);

      /*
       * Set GPIOA LED_GREEN pin to logic HIGH and wait
       */
      palSetPad(GPIOA, GPIOA_LED_GREEN);
      chThdSleepMilliseconds(250);
  }
}
