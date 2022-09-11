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
 * shellcommands.h - Shell Commands Header File
*/
#ifndef __SHELLCOMMANDS_H__
#define __SHELLCOMMANDS_H__

#include "hal.h"

void cmd_menu(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_menu0(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_menu1(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_menu2(BaseSequentialStream *chp, int argc, char *argv[]);

void cmd_hello(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_say(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_tell(BaseSequentialStream *chp, int argc, char *argv[]);

#endif // __SHELLCOMMANDS_H__
