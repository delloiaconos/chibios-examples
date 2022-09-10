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
 * [NISC2022-GPIO02] - PAL Line usage.
 * DESCRIPTION: How to use the HAL GPIO Line to write on generic PIN.
 */

#include "ch.h"
#include "hal.h"

/*
 * Definition of two constants for the PORT and the PIN where the LED should be connected.
 */
#define LED_PORT    GPIOA
#define LED_PIN     6U
#define LED_LINE    PAL_LINE( LED_PORT, LED_PIN )

int main(void) {

  halInit();
  chSysInit();

  /*
   * Setup the port as an OUTPUT with PUSH/PULL capabilities
   */
  palSetLineMode( LED_LINE, PAL_MODE_OUTPUT_PUSHPULL );

  while (true) {
      palWriteLine(LED_LINE, PAL_LOW);
      chThdSleepMilliseconds(1000);

      palWriteLine(LED_LINE, PAL_HIGH);
      chThdSleepMilliseconds(250);
  }
}
