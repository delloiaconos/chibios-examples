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
 * [NISC2022-THD03] - Threading Example 03
 * DESCRIPTION: Execution of a single thread function multiple times with heap
 * allocated working area, using dynamically generated thread names.
 */
#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#define ARRAY_SIZE( arr )    (sizeof(arr)/sizeof(arr[0]))

/* External and Internal Leds and Buttons definitions */
#define LED_LINE    PAL_LINE( GPIOA, 5U ) // Onboard Green LED
#define RLED_LINE   PAL_LINE( GPIOA, 0U ) // ARDUINO A0 (CN8.1)
#define GLED_LINE   PAL_LINE( GPIOA, 1U ) // ARDUINO A1 (CN8.2)
#define BLED_LINE   PAL_LINE( GPIOA, 4U ) // ARDUINO A2 (CN8.3)

/* Structure for thread configuration */
typedef struct {
  ioline_t line;
  sysinterval_t ton;
  sysinterval_t toff;
} ledconfig_t;


#define WA_LED             256
THD_FUNCTION( thdLed, arg ) {

  ledconfig_t *cfg = (ledconfig_t *) arg;
  palSetLineMode( cfg->line, PAL_MODE_OUTPUT_PUSHPULL );
  palClearLine( cfg->line );

  while( 1 ) {
      palSetLine( cfg->line );
      chThdSleep( cfg->ton );
      palClearLine( cfg->line );
      chThdSleep( cfg->toff );
  }
}

/*
 * Static array of configuration structures definition.
 */
static ledconfig_t thdConfs[] = {
    {RLED_LINE, TIME_MS2I( 750 ), TIME_MS2I( 250 )},
    {GLED_LINE, TIME_MS2I( 250 ), TIME_MS2I( 750 )},
    {BLED_LINE, TIME_MS2I( 330 ), TIME_MS2I( 330 )}
};

static char thdNames[ARRAY_SIZE(thdConfs)][6] = {};

int main(void) {

  halInit();
  chSysInit();

  thread_t * thds[ARRAY_SIZE(thdConfs)];
  /*
   * Creating threads from Heap Memory
   */
  for( uint32_t i = 0; i < ARRAY_SIZE(thdConfs); i++ ) {
    chsnprintf( thdNames[i], 6, "led%02d", i );
    thds[i] = chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(WA_LED), thdNames[i], NORMALPRIO-1, thdLed, (void *)&thdConfs[i] );
  }

  while( 1 ) {
      palToggleLine( LED_LINE );
      chThdSleepMilliseconds( 250 );
  }
}
