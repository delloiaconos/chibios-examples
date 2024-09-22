/*
    ChibiOS Examples
    Copyright (C) 2020-2021 Alessandro Salvatore Buccato [a.buccato@gmail.com]

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
 * Demo Test for SSD1366 and PWM Driver.
 * Connect Display on 3V3, GND, PB8 (SCL) and PB9 (SDA).
 * PWM Signal, generated on PB4 pin.
 */

#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#include "ssd1306.h"
#include "stdio.h"

#define PWM_TIMER_FREQUENCY     10000
#define PWM_PERIOD              50000
#define BUFF_SIZE               20

char buff[BUFF_SIZE];

/*
 * Configures PWM Drivers.
 */
void pwmWidtchCb(PWMDriver *pwmp){
  (void)pwmp;
}

static PWMConfig pwmcfg = {
  PWM_TIMER_FREQUENCY,
  PWM_PERIOD,
  pwmWidtchCb,
  {
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL}
  },
  0,
  0
};

/*
 * Configures I2C and SSD1306 Drivers.
 */
static const I2CConfig i2ccfg = {
  OPMODE_I2C,
  400000,
  FAST_DUTY_CYCLE_2,
};

static const SSD1306Config ssd1306cfg = {
  &I2CD1,
  &i2ccfg,
  SSD1306_SAD_0X78,
};

static SSD1306Driver SSD1306D1;

/*
 * Oled Display Thread.
 */
static THD_WORKING_AREA(waOledDisplay, 512);
static THD_FUNCTION(OledDisplay, arg) {
  (void)arg;

  chRegSetThreadName("OledDisplay");
  ssd1306ObjectInit(&SSD1306D1);

  ssd1306Start(&SSD1306D1, &ssd1306cfg);

  ssd1306FillScreen(&SSD1306D1, 0x00);

  while (true) {
    ssd1306GotoXy(&SSD1306D1, 0, 1);
    chsnprintf(buff, BUFF_SIZE, "NeaPolis");
    ssd1306Puts(&SSD1306D1, buff, &ssd1306_font_11x18, SSD1306_COLOR_WHITE);

    ssd1306GotoXy(&SSD1306D1, 0, 20);
    chsnprintf(buff, BUFF_SIZE, "Innovation");
    ssd1306Puts(&SSD1306D1, buff, &ssd1306_font_7x10, SSD1306_COLOR_BLACK);

    ssd1306UpdateScreen(&SSD1306D1);
    chThdSleepMilliseconds(1000);

    ssd1306GotoXy(&SSD1306D1, 0, 36);
    chsnprintf(buff, BUFF_SIZE, "BYE BYE");
    ssd1306Puts(&SSD1306D1, buff, &ssd1306_font_7x10, SSD1306_COLOR_WHITE);
    ssd1306UpdateScreen(&SSD1306D1);
    chThdSleepMilliseconds(2000);

	uint8_t txbuf[] = { 0x00, 0xAE };
	i2cAcquireBus(&I2CD1);
	i2cStart(&I2CD1, &i2ccfg);
	i2cMasterTransmitTimeout(&I2CD1, SSD1306_SAD_0X78, txbuf, sizeof(txbuf), NULL, 0, TIME_INFINITE);
	i2cReleaseBus(&I2CD1);
  }
}

/*
 * Green LED Blinker Thread.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {
  (void)arg;
  chRegSetThreadName("blinker");

  while (true) {
    palClearPad(GPIOA, GPIOA_LED_GREEN);
    chThdSleepMilliseconds(500);
    palSetPad(GPIOA, GPIOA_LED_GREEN);
    chThdSleepMilliseconds(500);
  }
}

int main(void) {
  halInit();
  chSysInit();

  /*
   * Configuring I2C related PINs.
   */
  palSetLineMode(LINE_ARD_D15, PAL_MODE_ALTERNATE(4) |
                 PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_OSPEED_HIGHEST |
                 PAL_STM32_PUPDR_PULLUP);
  palSetLineMode(LINE_ARD_D14, PAL_MODE_ALTERNATE(4) |
                 PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_OSPEED_HIGHEST |
                 PAL_STM32_PUPDR_PULLUP);


  /*
   * Configuring PWM related PINs.
   */
  palSetPadMode(GPIOB, 4, PAL_MODE_ALTERNATE(2));

  pwmStart(&PWMD3, &pwmcfg);
  pwmEnablePeriodicNotification(&PWMD3);
  pwmEnableChannel(&PWMD3, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 2500));


  /*
   * Creates threads.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO + 1, Thread1, NULL);
  chThdCreateStatic(waOledDisplay, sizeof(waOledDisplay), NORMALPRIO, OledDisplay, NULL);

  /*
   * Infinite Loop.
   */
  while (true) {
	// do nothing
	chThdSleepMilliseconds(10);
  }
}
