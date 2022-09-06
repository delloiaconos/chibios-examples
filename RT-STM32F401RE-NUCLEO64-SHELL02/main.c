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
 * [SHELL02] Using the SHELL - Example 02
 * A much more clean project with the ChibiOS shell:
 * - Tests commands are disabled
 * - Exit command is disabled
 * - Info command is disabled
 * The shell files are imported into the project!
 */

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "shell.h"

static THD_WORKING_AREA(waShell, 512);

static void cmd_hello(BaseSequentialStream *chp, int argc, char *argv[]) {
  (void) argc;
  (void) argv;
  chprintf( chp, "Hello NISC2020\n\r" );
}

static const ShellCommand commands[] = {
  {"hello", cmd_hello},
  {NULL, NULL}
};

static const ShellConfig shell_cfg = {
  (BaseSequentialStream *) &SD2,
  commands
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

  /*
   * Shell manager initialization.
   */
  shellInit();

  chThdCreateStatic(waShell, sizeof( waShell ), NORMALPRIO + 1, shellThread, (void *) &shell_cfg);
  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (true) {
    palTogglePad(GPIOA, GPIOA_LED_GREEN);
    chThdSleepMilliseconds(500);
  }
}
