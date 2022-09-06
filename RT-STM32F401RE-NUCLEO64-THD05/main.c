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
 * [NISC2022-THD05] - Threads Example 05: Threads Synchronization
 * DESCRIPTION: Threads are suspended and then resumed by the main.
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


#define WA_LED                  256
THD_FUNCTION( thdLed, arg ) {
  ledconfig_t *cfg = (ledconfig_t *) arg;
  thread_t * thp;

  palSetLineMode( cfg->line, PAL_MODE_OUTPUT_PUSHPULL );
  palClearLine( cfg->line );

  while( 1 ) {
      /* Suspend this thread */
      chThdSuspendS( &thp );

      /* Do your job */
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
    {BLED_LINE, TIME_MS2I( 500 ), TIME_MS2I( 1000 )}
};


static char thdNames[ARRAY_SIZE(thdConfs)][6] = {};

int main(void) {

  halInit();
  chSysInit();

  uint32_t i;
  thread_t * thds[ARRAY_SIZE(thdConfs)+1];
  /*
   * Creating threads from Heap Memory
   */
  for( i = 0; i < ARRAY_SIZE(thdConfs); i++ ) {
    chsnprintf( thdNames[i], 6, "led%02d", i );
    thds[i] = chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(WA_LED), thdNames[i], NORMALPRIO-1, thdLed, (void *)&thdConfs[i] );
  }
  thds[i] = (thread_t *) NULL;

  while( 1 ) {
      palToggleLine( LED_LINE );

      if( palReadLine( LINE_BUTTON ) == PAL_LOW ) {
        /* Resume all the threads */
        for( i = 0;  thds[i] != (thread_t *) NULL; i++ ) {
          /* Make a copy of the thread pointer and resume it! */
          thread_t * thp = thds[i];
          chThdResume( &thp, MSG_OK );
        }
      }

      chThdSleepMilliseconds(1000);
  }
}

