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
 * [GPIO04] Using GPIO Peripherals - Example 04
 * How to use a thread allocated in the heap memory and with parameters
 * to control 2 different LED with LINES and different timings for ON and
 * OFF time.
 */

#include "ch.h"
#include "hal.h"


#define LED_RED_LINE            PAL_LINE( GPIOA, 5U )
#define LED_GREEN_LINE          PAL_LINE( GPIOA, 6U )
#define LED_BLU_LINE            PAL_LINE( GPIOA, 7U )

/*
 * Structure containing the configuration of the thdLed thread function.
 */
typedef struct {
  ioline_t line;
  uint32_t tOn;
  uint32_t tOff;
} ledconfig_t;


#define THDLED_WA_SIZE      THD_WORKING_AREA_SIZE(128)

static THD_FUNCTION( thdLed, arg ) {
  ledconfig_t * cfg = (ledconfig_t *) arg;

  palSetLineMode( cfg->line, PAL_MODE_OUTPUT_PUSHPULL );
  while(true) {
    palSetLine( cfg->line );
    chThdSleepMilliseconds( cfg->tOn );
    palClearLine( cfg->line );
    chThdSleepMilliseconds( cfg->tOff );
  }
}

/*
 * Creating configuration structures.
 */
static ledconfig_t configs[] = {
    {LED_BLU_LINE, 500, 250},
    {LED_RED_LINE, 1000, 1500}
};

static thread_t * thds[sizeof(configs)/sizeof(configs[0])];

int main(void) {

  halInit();
  chSysInit();

  /*
   * Creating threads from Heap Memory
   */
  for( uint32_t i = 0; i < sizeof(configs)/sizeof(configs[0]); i++ ) {
    thds[i] = chThdCreateFromHeap(NULL, THDLED_WA_SIZE, "Led", NORMALPRIO + 1, thdLed, (void *)&configs[i] );
  }

  palSetLineMode( LED_GREEN_LINE, PAL_MODE_OUTPUT_PUSHPULL );
  while (true) {
      palToggleLine( LED_GREEN_LINE );
      chThdSleepMilliseconds(500);
  }
}

