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
 * [THD02] Chibios Multithreading & Libraries - Example 02
 * Execution of a single thread function multiple times on
 * multiple static difined working area with configuration.
 */

#include "ch.h"
#include "hal.h"

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

#define WA_LED_SIZE      256
THD_WORKING_AREA( waLed1, WA_LED_SIZE );
THD_WORKING_AREA( waLed2, WA_LED_SIZE );
THD_WORKING_AREA( waLed3, WA_LED_SIZE );
THD_FUNCTION( thdLed, arg ) {

  ledconfig_t *cfg = (ledconfig_t *) arg;
  palSetLineMode( cfg->line, PAL_MODE_OUTPUT_PUSHPULL );

  while( 1 ) {
      palSetLine( cfg->line );
      chThdSleep( cfg->ton );
      palClearLine( cfg->line );
      chThdSleep( cfg->toff );
  }
}

static ledconfig_t cfgLed1 = {
   .line = RLED_LINE,
   .ton = TIME_MS2I( 250 ),
   .toff = TIME_MS2I( 750 )
};

static ledconfig_t cfgLed2 = {
   .line = GLED_LINE,
   .ton = TIME_MS2I( 750 ),
   .toff = TIME_MS2I( 250 )
};

static ledconfig_t cfgLed3 = {
   .line = BLED_LINE,
   .ton = TIME_MS2I( 1000 ),
   .toff = TIME_MS2I( 1000 )
};


int main(void) {

  halInit();
  chSysInit();

  chThdCreateStatic(waLed1, sizeof(waLed1), NORMALPRIO-1, thdLed, (void*) &cfgLed1 );
  chThdCreateStatic(waLed2, sizeof(waLed2), NORMALPRIO-1, thdLed, (void*) &cfgLed2 );
  chThdCreateStatic(waLed3, sizeof(waLed3), NORMALPRIO-1, thdLed, (void*) &cfgLed3 );

  while( 1 ) {
    palToggleLine( LED_LINE );
    chThdSleepMilliseconds( 250 );
  }
}
