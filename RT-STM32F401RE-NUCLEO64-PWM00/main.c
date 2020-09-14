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
 * [PWM00] Using PWM Peripheral and Driver - Example 00
 */
#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#define PWM_TIMER_FREQUENCY     10000
#define PWM_PERIOD              50000

void pwmWidtchCb(PWMDriver *pwmp){
  (void)pwmp;
}

/*
 * Configures PWM Driver.
 */
static PWMConfig pwmcfg = {
  PWM_TIMER_FREQUENCY,
  PWM_PERIOD,
  pwmWidtchCb,
  {
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_DISABLED, NULL}
  },
  0,
  0
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

  palSetPadMode(GPIOB, 4, PAL_MODE_ALTERNATE(2));
  palSetPadMode(GPIOC, 7, PAL_MODE_ALTERNATE(2));
  palSetPadMode(GPIOC, 8, PAL_MODE_ALTERNATE(2));

  pwmStart(&PWMD3, &pwmcfg);
  pwmEnablePeriodicNotification(&PWMD3);
  pwmEnableChannel(&PWMD3, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 2500));
  pwmEnableChannel(&PWMD3, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 5000));
  pwmEnableChannel(&PWMD3, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 7500));

  while (true) {
    chThdSleepMilliseconds(500);
  }

  pwmDisableChannel(&PWMD3, 0);
  pwmDisableChannel(&PWMD3, 1);
  pwmDisableChannel(&PWMD3, 2);

  pwmStop(&PWMD3);
}
