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
 * [NISC2022-SHELL03] - Using the shell as a menu.
 * DESCRIPTION: Single shell thread with a runtime defined configuration.
 */

#include "ch.h"
#include "hal.h"

#include "shell.h"
#include "shellcommands.h"


static ShellConfig shell_cfg = {
  (BaseSequentialStream *)&SD2,
  (ShellCommand *) NULL
};

static const ShellCommand cmds0[] = {
  {"menu",  cmd_menu},
  {"menu1", cmd_menu1},
  {"menu2", cmd_menu2},
  {"hello", cmd_hello},
  {NULL, NULL}
};

static const ShellCommand cmds1[] = {
  {"menu", cmd_menu},
  {"back", cmd_menu0},
  {"say",  cmd_say},
  {NULL, NULL}
};

static const ShellCommand cmds2[] = {
  {"menu", cmd_menu},
  {"back", cmd_menu0},
  {"tell", cmd_tell},
  {NULL, NULL}
};


#define WA_SHELL   2048
THD_WORKING_AREA( waShell, WA_SHELL );

/*
 * Application entry point.
 */
int main(void) {

  halInit();
  chSysInit();

  /*
   * Initializes a SERIAL driver.
   */
  palSetPadMode( GPIOA, 2, PAL_MODE_ALTERNATE(7) );
  palSetPadMode( GPIOA, 3, PAL_MODE_ALTERNATE(7) );

  sdStart(&SD2, NULL);

  /*
   * Shell manager initialization.
   */
  shellInit();

  shell_cfg.sc_commands = cmds0;

  while (true) {
    thread_t * tp;
    msg_t msg;

    tp = chThdCreateStatic(waShell, sizeof(waShell), NORMALPRIO, shellThread, (void *)&shell_cfg);
    msg = chThdWait( tp );

    switch( msg ) {
    case MSG_OK+1:
      shell_cfg.sc_commands = cmds1;
      break;
    case MSG_OK+2:
      shell_cfg.sc_commands = cmds2;
      break;
    default:
    case MSG_OK:
      shell_cfg.sc_commands = cmds0;
    }

    chThdSleepMilliseconds(100);
  }
}
