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
 * [ENCODER] Using GPIO to read Encoder
 * How to use the GPIO peripheral and WaitEvent to read and Encoder.
 */

#include "ch.h"
#include "hal.h"
#include "chprintf.h"


BaseSequentialStream *chp = (BaseSequentialStream*) &SD2;

/*
 * Encoder direction reading.
 * WARNING! You need to enable PAL_USE_WAIT into halconf.h
 */
#define ENCODER_CLK_LINE LINE_ARD_D8
#define ENCODER_DT_LINE  LINE_ARD_D9

static THD_WORKING_AREA(waEncoder, 128);
static THD_FUNCTION(thdEncoder, arg) {
  (void)arg;
  chRegSetThreadName("encoder");

  palSetLineMode(ENCODER_CLK_LINE, PAL_MODE_INPUT_PULLUP);
  palSetLineMode(ENCODER_DT_LINE, PAL_MODE_INPUT_PULLUP);
  palEnableLineEvent( ENCODER_CLK_LINE, PAL_EVENT_MODE_RISING_EDGE );

  while (true) {
      palWaitLineTimeout( ENCODER_CLK_LINE, TIME_INFINITE );
      chThdSleepMicroseconds(100);

      uint32_t dt  = palReadLine( ENCODER_DT_LINE ) == PAL_LOW ? 1 : 0;
      uint32_t clk = palReadLine( ENCODER_CLK_LINE ) == PAL_LOW ? 1 : 0;

      if ( clk ^ dt ) {
        chprintf(chp, "Right\n\r");
      } else {
        chprintf(chp, "Left\n\r");
      }

      chThdSleepMilliseconds(20);
    }
}


/*
 * Application entry point.
 */
int main(void) {

  halInit();
  chSysInit();

  /*
   * Activates the serial driver 2 using the driver default configuration.
   */
  sdStart(&SD2, NULL);

  /*
   * Creates the encoder thread.
   */
  chThdCreateStatic(waEncoder, sizeof(waEncoder), NORMALPRIO + 1, thdEncoder, NULL);

  while (true) {
    chThdSleepMilliseconds(500);
  }
}
