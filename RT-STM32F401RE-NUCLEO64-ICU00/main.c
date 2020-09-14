/*
    NeaPolis Innovation Summer Campus 2020 Examples
    Copyright (C) 2020
    - Salvatore Dello Iacono [delloiaconos@gmail.com]
    - Matteo Caiazzo
    - Ciro Mazzocchi

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
 * [ICU00] Using ICU Peripheral and Driver - Example 00
 */

#include "ch.h"
#include "hal.h"
#include "chprintf.h"


BaseSequentialStream * chp = (BaseSequentialStream*) &SD2;

#define ICU_FREQUENCY           1000

int counter = 0;

static void icuperiodcb(ICUDriver *icup) {
  (void)icup;
  counter = counter + 1;
}

static ICUConfig icucfg = {
  ICU_INPUT_ACTIVE_HIGH,
  ICU_FREQUENCY,
  NULL,
  icuperiodcb,
  NULL,
  ICU_CHANNEL_1,
  0U,
  0xFFFFFFFFU
};

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Activates the serial driver 2 using the driver default configuration.
   */
  sdStart(&SD2, NULL);


  palSetPadMode(GPIOA, 0, PAL_MODE_ALTERNATE(2));

  icuStart(&ICUD5, &icucfg);
  icuStartCapture(&ICUD5);
  icuEnableNotifications(&ICUD5);
  chThdSleepMilliseconds(2000);

  while (true) {
    chprintf( chp, "Counter = %d\n\r", counter);
    chThdSleepMilliseconds(1000);
  }

  icuStopCapture(&ICUD5);
  icuStop(&ICUD5);

  return 0;
}

