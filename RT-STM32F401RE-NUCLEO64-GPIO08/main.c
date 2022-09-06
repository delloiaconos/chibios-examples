/*
    NeaPolis Innovation Summer Campus 2021 Examples
    Copyright (C) 2021 Salvatore Dello Iacono [delloiaconos@gmail.com]

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
 * [GPIO08] Using GPIO Peripherals - Example 08
 * How to use listeners and callbacks to measure time
 */

#include "ch.h"
#include "hal.h"

#define BTN_PORT     GPIOC
#define BTN_PIN      GPIOC_BUTTON
#define BTN_LINE     PAL_LINE( BTN_PORT, BTN_PIN )


static uint32_t time;

static void button_cb(void *arg) {

  (void)arg;

  static systime_t start, stop;
  chSysLockFromISR();
  if( palReadLine( BTN_LINE ) == PAL_LOW ) {
    start = chVTGetSystemTimeX();
  } else {
    sysinterval_t delta;
    stop = chVTGetSystemTimeX();
    delta = chTimeDiffX( start, stop );
    time = TIME_I2MS( delta );
  }
  chSysUnlockFromISR();
}

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

  /* Enabling events on both edges of the button line.*/
  palEnableLineEvent( BTN_LINE, PAL_EVENT_MODE_BOTH_EDGES);
  palSetLineCallback( BTN_LINE, button_cb, NULL);

  /*
   * Normal main() thread activity.
   */
  while (true) {
    chThdSleepMilliseconds( 1000 );
  }
}
