/*
    NeaPolis Innovation Summer Campus 2020 Examples
    Copyright (C) 2020 Salvatore Dello Iacono [delloiaconos@gmail.com]

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
 * [GPIO01] Using GPIO Peripherals - Example 02
 * How to use the GPIO peripheral to control an external LED connected to GPIOA pin #5
 */

#include "ch.h"
#include "hal.h"

/*
 * Definition of two constants for the PORT and the PIN where the LED should be connected.
 */
#define LED_PORT    GPIOA
#define LED_PIN     6U

int main(void) {

  halInit();
  chSysInit();

  /*
   * Setup the port as an OUTPUT with PUSH/PULL capabilities
   */
  palSetPadMode( LED_PORT, LED_PIN, PAL_MODE_OUTPUT_PUSHPULL );

  while (true) {
      palClearPad(LED_PORT, LED_PIN);
      chThdSleepMilliseconds(1000);

      palSetPad(LED_PORT, LED_PIN);
      chThdSleepMilliseconds(250);
  }
}
