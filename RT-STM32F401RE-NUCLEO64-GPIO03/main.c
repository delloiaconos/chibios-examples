/*
    NeaPolis Innovation Summer Campus Examples
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
 * [NISC2022-GPIO03] - Reading the on-board user button.
 * DESCRIPTION: How to use a GPIO PIN as an User Input.
 */

#include "ch.h"
#include "hal.h"

#include "ch.h"
#include "hal.h"

/* Green LED: Port, Pin and Line */
#define LED_PORT   GPIOA
#define LED_PIN    5U
#define LED_LINE   PAL_LINE( LED_PORT, LED_PIN )

/* User Button: Port, Pin and Line */
#define BTN_PORT   GPIOC
#define BTN_PIN    13U
#define BTN_LINE   PAL_LINE( BTN_PORT, BTN_PIN )

int main(void) {

  halInit();
  chSysInit();

  palSetLineMode( LED_LINE, PAL_MODE_OUTPUT_PUSHPULL );
  palSetLineMode( BTN_LINE, PAL_MODE_INPUT );

  while( 1 ) {
    int btnState;

    btnState = palReadLine( BTN_LINE );
    palWriteLine( LED_LINE, btnState );
    chThdSleepMilliseconds( 200 );
  }
}
