/*
    NeaPolis Innovation Summer Campus 2022 Examples
    Copyright (C) 2020-2023 Salvatore Dello Iacono [delloiaconos@gmail.com]

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

#ifndef __SHELLCONF_H__
#define __SHELLCONF_H__

#include "ch.h"
#include "hal.h"

/* Shell Prompt string */
#define SHELL_PROMPT_STR            "nisc23> "


#define SHELL_CMD_EXIT_ENABLED              FALSE
#define SHELL_CMD_INFO_ENABLED              TRUE
#define SHELL_CMD_ECHO_ENABLED              FALSE
#define SHELL_CMD_SYSTIME_ENABLED           TRUE
#define SHELL_CMD_MEM_ENABLED               FALSE
#define SHELL_CMD_THREADS_ENABLED           TRUE
#define SHELL_CMD_TEST_ENABLED              FALSE

#endif // __SHELLCONF_H__
