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
 * [THD06] Chibios Multithreading & Libraries - Example 06
 * Example of a Producer/Consumer structure synchronized with FIFOs.
 */

#include "ch.h"
#include "hal.h"

#include "chlib.h"
#include "chprintf.h"

BaseSequentialStream * chp = (BaseSequentialStream *) &SD2;

typedef struct fifo_data_s {
  int type;
  int data[3];
} fifo_data_t;


static THD_WORKING_AREA(waProducer1, 128);
static THD_FUNCTION(thdProducer1, arg) {
  objects_fifo_t * ofp = (objects_fifo_t *) arg;
  int counter = 0;
  chRegSetThreadName("producer1");

  while (true) {
    fifo_data_t * objp;
    objp = (fifo_data_t *) chFifoTakeObjectTimeout(ofp, TIME_MS2I(100) );

    if( objp != (fifo_data_t *) NULL ) {
      objp->type = 1;
      objp->data[0] = counter + 2;
      objp->data[1] = counter + 4;
      objp->data[2] = counter + 6;

      chFifoSendObject(ofp, objp);
    }
    counter += 5;
    chThdSleepMilliseconds( 130 );
  }
}


static THD_WORKING_AREA(waProducer2, 128);
static THD_FUNCTION(thdProducer2, arg) {
  objects_fifo_t * ofp = (objects_fifo_t *) arg;
  int counter = 2;

  chRegSetThreadName("producer2");
  while (true) {
    fifo_data_t * objp;
    objp = (fifo_data_t *) chFifoTakeObjectTimeout(ofp, TIME_MS2I(100) );

    if( objp != (fifo_data_t *) NULL ) {
      objp->type = 2;
      objp->data[0] = counter + 2;
      objp->data[1] = counter + 4;
      objp->data[2] = counter + 6;

      chFifoSendObject(ofp, objp);
    }
    counter += 5;
    chThdSleepMilliseconds( 130 );
  }
}


#define FIFO_LEN 20
static objects_fifo_t fifo;
static fifo_data_t objBuff[FIFO_LEN];
static msg_t msgBuff[FIFO_LEN];

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Activates the serial driver 2 using the driver default configuration.
   */
  sdStart(&SD2, NULL);

  chFifoObjectInit(&fifo, sizeof( fifo_data_t ), FIFO_LEN, (void*) objBuff, msgBuff );

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waProducer1, sizeof(waProducer1), NORMALPRIO + 1, thdProducer1, (void*) &fifo);
  chThdCreateStatic(waProducer2, sizeof(waProducer2), NORMALPRIO + 1, thdProducer2, (void*) &fifo);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (true) {
    msg_t msg;
    fifo_data_t * objp = (fifo_data_t *) NULL;

    msg = chFifoReceiveObjectTimeout(&fifo, (void**) &objp, TIME_MS2I(300) );
    if( msg == MSG_OK && objp != (fifo_data_t *) NULL ) {
      chprintf( chp, "MSG t = %d, d = {%d, %d, %d}\n\r", objp->type,
                objp->data[0], objp->data[1], objp->data[2] );
      chFifoReturnObject(&fifo, (void*) objp);
    } else {
      chprintf( chp, "ERROR\n\r" );
    }

  }
}
