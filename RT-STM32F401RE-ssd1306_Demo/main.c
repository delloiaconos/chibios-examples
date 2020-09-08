#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "ssd1306.h"
#include "stdio.h"

#define SIZE   20
char buffer[SIZE];

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

//%--------------------------------------------------------

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

//%--------------------------------------------------------

static THD_WORKING_AREA(waOledDisplay, 512);
static THD_FUNCTION(OledDisplay, arg) {
  (void)arg;

  chRegSetThreadName("OledDisplay");

  ssd1306ObjectInit(&SSD1306D1);

  ssd1306Start(&SSD1306D1, &ssd1306cfg);

  ssd1306FillScreen(&SSD1306D1, 0x00);

  while (true) {

    ssd1306GotoXy(&SSD1306D1, 0, 1);
    chsnprintf(buffer, SIZE, "Pippo");
    ssd1306Puts(&SSD1306D1, buffer, &ssd1306_font_11x18, SSD1306_COLOR_WHITE);

    ssd1306GotoXy(&SSD1306D1, 0, 20);
    chsnprintf(buffer, SIZE, "Pluto");
    ssd1306Puts(&SSD1306D1, buffer, &ssd1306_font_7x10, SSD1306_COLOR_BLACK);

    ssd1306UpdateScreen(&SSD1306D1);
    chThdSleepMilliseconds(500);
  }

}


//%--------------------------------------------------------


int main(void) {

  halInit();
  chSysInit();

  /* Configuring I2C related PINs */
   palSetLineMode(LINE_ARD_D15, PAL_MODE_ALTERNATE(4) |
                  PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_OSPEED_HIGHEST |
                  PAL_STM32_PUPDR_PULLUP);
   palSetLineMode(LINE_ARD_D14, PAL_MODE_ALTERNATE(4) |
                  PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_OSPEED_HIGHEST |
                  PAL_STM32_PUPDR_PULLUP);

  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO+1, Thread1, NULL);
  chThdCreateStatic(waOledDisplay, sizeof(waOledDisplay), NORMALPRIO, OledDisplay, NULL);

  while (true) {
    /*Infinite loop*/
    uint32_t flag = 0;

    if(palReadLine(LINE_BUTTON) == PAL_LOW) {
      while(palReadLine(LINE_BUTTON) == PAL_LOW){
        chThdSleepMilliseconds(20);
      }
      flag = 1;
    }

    if (flag == 1) {
    ssd1306GotoXy(&SSD1306D1, 0, 36);
    chsnprintf(buffer, SIZE, "Topolino");
    ssd1306Puts(&SSD1306D1, buffer, &ssd1306_font_7x10, SSD1306_COLOR_WHITE);
    ssd1306UpdateScreen(&SSD1306D1);
    }

    chThdSleepMilliseconds(50);
  }
}

/*
 * 	Simple code to turn off Oled Display
 *
    uint8_t txbuf[] = { 0x00, 0xAE };
    i2cAcquireBus(&I2CD1);
    i2cStart(&I2CD1, &i2ccfg);
    i2cMasterTransmitTimeout(&I2CD1, SSD1306_SAD_0X78, txbuf, sizeof(txbuf), NULL, 0, TIME_INFINITE);
    i2cReleaseBus(&I2CD1);
 *
 *
 */

