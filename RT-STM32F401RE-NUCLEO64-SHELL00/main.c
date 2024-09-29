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
 * [SHELL00] Using the SHELL - Example 00
 * An example project with the shell imported for the STM32 NUCLEO64-F401
 * development board.
 */

#include "ch.h"
#include "hal.h"

#include "shell.h"
#include "chprintf.h"

/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)
static uint32_t counter = 0;


/* Can be measured using dd if=/dev/xxxx of=/dev/null bs=512 count=10000.*/
static void cmd_write(BaseSequentialStream *chp, int argc, char *argv[]) {
  static uint8_t buf[] =
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: write\r\n");
    return;
  }

  while (chnGetTimeout((BaseChannel *)chp, TIME_IMMEDIATE) == Q_TIMEOUT) {
#if 1
    /* Writing in channel mode.*/
    chnWrite(&SD2, buf, sizeof buf - 1);
#else
    /* Writing in buffer mode.*/
    (void) obqGetEmptyBufferTimeout(&SD2.obqueue, TIME_INFINITE);
    memcpy(SD2.obqueue.ptr, buf, SERIAL_USB_BUFFERS_SIZE);
    obqPostFullBuffer(&SD2.obqueue, SERIAL_USB_BUFFERS_SIZE);
#endif
  }
  chprintf(chp, "\r\n\nstopped\r\n");
}

static void cmd_count(BaseSequentialStream *chp, int argc, char *argv[]) {

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: count\r\n");
  }
  else {
    counter++;
    chprintf(chp, "%d\r\n", counter);
  }
}

static const ShellCommand commands[] = {
  {"write", cmd_write},
  {"count", cmd_count},
  {NULL, NULL}
};

static const ShellConfig shell_cfg1 = {
  (BaseSequentialStream *)&SD2,
  commands
};

static THD_WORKING_AREA(waThd1, 256);
static THD_FUNCTION(Thd1, arg){
  (void) arg;
  chRegSetThreadName("Thread 1");
  while(TRUE){
    palClearPad(GPIOA, GPIOA_LED_GREEN);
        chThdSleepMilliseconds(500);
        palSetPad(GPIOA, GPIOA_LED_GREEN);
        chThdSleepMilliseconds(500);
  }
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

  /*
   * Shell manager initialization.
   */
  shellInit();

  chThdCreateStatic(waThd1, sizeof(waThd1), NORMALPRIO + 1,
                    Thd1, NULL);
  sdStart(&SD2, NULL);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (true) {
    thread_t *shelltp = chThdCreateFromHeap(NULL, SHELL_WA_SIZE, "shell", NORMALPRIO + 1,
                                            shellThread, (void *)&shell_cfg1);

    chThdWait(shelltp);               /* Waiting termination.             */
  }
}
