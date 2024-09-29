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
 * [THD04] Chibios Multithreading & Libraries - Example 04
 * How to syncronize two threads using a shared variable and a mutex;
 * this is a simple exaple of producer consumer.
 * One thread manage the Button (input), the main thread manages the Led (output)
 */

#include "ch.h"
#include "hal.h"

/* Shared Resource */
typedef int shared_t;
static shared_t shared;

#define WA_BUTTON 256
THD_WORKING_AREA( waButton, WA_BUTTON );

THD_FUNCTION( thdButton, arg ) {
  mutex_t * mp = (mutex_t *) arg;

  chRegSetThreadName( "button" );


  palSetLineMode( LINE_LED_GREEN, PAL_MODE_OUTPUT_PUSHPULL );
  while( 1 ){

      shared_t local;

      /* Lock the shared resource with mutex, read its value and reset it. */
      chMtxLock(mp);
      local = shared;
      shared = 0;
      chMtxUnlock(mp);

      if( local ) {
        palSetLine( LINE_LED_GREEN );
      } else {
        palClearLine( LINE_LED_GREEN );
      }

      chThdSleepMilliseconds( 250 );
    }
}


#define WA_LED 256
THD_WORKING_AREA( waLed, WA_BUTTON );

THD_FUNCTION( thdLed, arg ) {
  mutex_t * mp = (mutex_t *) arg;

  chRegSetThreadName( "led" );


  palSetLineMode( LINE_BUTTON, PAL_MODE_INPUT_PULLUP );
  while( 1 ){

    if( palReadLine( LINE_BUTTON ) == PAL_LOW ) {

      /* Debouncing: wait until voltage (and level) stabilizes */
      chThdSleepMilliseconds(5);

      /* Positive Edge detection: wait while button is pressed */
      while( palReadLine( LINE_BUTTON ) == PAL_LOW ) {
        chThdSleepMilliseconds(10);
      }


      /* Lock the shared resource with mutex */
      chMtxLock(mp);
      shared = TRUE;
      chMtxUnlock(mp);
    }

    chThdSleepMilliseconds( 10 );
  }
}


int main(void) {
  mutex_t mtx;

  halInit();
  chSysInit();

  /* Init the shared resource.
   * No mutex required here, because no thread is using it. */
  shared = 0;

  /* Getting a new Mutex Object */
  chMtxObjectInit( &mtx );

  chThdCreateStatic( waButton, sizeof( waButton ), NORMALPRIO+1, thdButton, (void*) &mtx );
  chThdCreateStatic( waLed, sizeof( waLed ), NORMALPRIO-1, thdLed, (void*) &mtx );

  while( 1 ) {
    chThdSleepMilliseconds( 500 );
  }
}
