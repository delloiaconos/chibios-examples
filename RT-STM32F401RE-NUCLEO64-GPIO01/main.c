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
 * [NISC2022-GPIO01] - Generic output PIN configuration and use.
 * DESCRIPTION: How to use the HAL GPIO driver to blink a generic PIN.
 */

#include "ch.h"
#include "hal.h"

/* Green LED: Port, Pin definitions */
#define LED_PORT   GPIOA
#define LED_PIN    5U

int main(void) {

  halInit();
  chSysInit();
  
  /*
   * Configure the LED_PIN at LED_PORT in Output (pushpull) mode
   */
  palSetPadMode( LED_PORT, LED_PIN, PAL_MODE_OUTPUT_PUSHPULL );

  while (true) {
      /*
       * Set GPIOA LED_PIN pin to logic LOW and wait
       */
      palClearPad(GPIOA, LED_PIN);
      chThdSleepMilliseconds(1000);

      /*
       * Set GPIOA LED_PIN pin to logic HIGH and wait
       */
      palSetPad(GPIOA, LED_PIN);
      chThdSleepMilliseconds(250);
  }
}
