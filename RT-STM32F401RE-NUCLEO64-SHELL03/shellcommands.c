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
 * shellcommands.c - Shell Commands Functions
*/
#include "ch.h"
#include "hal.h"

#include <stdlib.h>
#include <string.h>

#include "shell.h"
#include "chprintf.h"
#include "chscanf.h"


void cmd_menu(BaseSequentialStream *chp, int argc, char *argv[]) {
  static const int menu_min = 0;
  static const int menu_max = 2;
  if( argc == 1 ) {
    int menu_num;
    menu_num = atoi( argv[0] );
    if( menu_num >= menu_min && menu_num <= menu_max ) {
      shellExit(MSG_OK+menu_num);
    }
  }
  chprintf( chp, "Usage: menu [%d-%d]\n\r", menu_min, menu_max );
}

void cmd_menu0(BaseSequentialStream *chp, int argc, char *argv[]) {
  (void) argc;
  (void) argv;

  chprintf(chp, "Selected Menu 0!\r\n");
  shellExit(MSG_OK+0);
}

void cmd_menu1(BaseSequentialStream *chp, int argc, char *argv[]) {
  (void) argc;
  (void) argv;

  chprintf(chp, "Selected Menu 1!\r\n");
  shellExit(MSG_OK+1);
}

void cmd_menu2(BaseSequentialStream *chp, int argc, char *argv[]) {
  (void) argc;
  (void) argv;

  chprintf(chp, "Selected Menu 2!\r\n");
  shellExit(MSG_OK+2);
}

void cmd_hello(BaseSequentialStream *chp, int argc, char *argv[]) {
  (void) argc;
  (void) argv;

  chprintf(chp, "Hello World!\r\n");
}

void cmd_say(BaseSequentialStream *chp, int argc, char *argv[]) {
  (void) argc;
  (void) argv;

  chprintf(chp, "Say Something!\r\n");
}

void cmd_tell(BaseSequentialStream *chp, int argc, char *argv[]) {
  (void) argc;
  (void) argv;

  chprintf(chp, "Tell a Story!\r\n");
}

