#include <string.h>
#include "hal.h"
#include "ssd1306.h"
#include "ssd1306_font.c"
#include "ssd1306_font_7_10.c"

#define ABS(x)   ((x) > 0 ? (x) : -(x))

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

static msg_t wrCmd(void *ip, uint8_t cmd) {
  const SSD1306Driver *drvp = (const SSD1306Driver *)ip;
  msg_t ret;
  uint8_t txbuf[] = { 0x00, cmd };

  i2cAcquireBus(drvp->config->i2cp);
  i2cStart(drvp->config->i2cp, drvp->config->i2ccfg);

  ret = i2cMasterTransmitTimeout(drvp->config->i2cp, drvp->config->sad,
                                 txbuf, sizeof(txbuf), NULL, 0, TIME_INFINITE);

  i2cReleaseBus(drvp->config->i2cp);

  return ret;
}

static msg_t wrDat(void *ip, uint8_t *txbuf, uint16_t len) {
  const SSD1306Driver *drvp = (const SSD1306Driver *)ip;
  msg_t ret;

  i2cAcquireBus(drvp->config->i2cp);
  i2cStart(drvp->config->i2cp, drvp->config->i2ccfg);

  ret = i2cMasterTransmitTimeout(drvp->config->i2cp, drvp->config->sad,
                                 txbuf, len, NULL, 0, TIME_INFINITE);

  i2cReleaseBus(drvp->config->i2cp);

  return ret;
}

static void updateScreen(void *ip) {
  SSD1306Driver *drvp = (SSD1306Driver *)ip;
  uint8_t idx;

  for (idx = 0; idx < 8; idx++) {
    wrCmd(drvp, 0xB0 + idx);
    wrCmd(drvp, 0x00);
    wrCmd(drvp, 0x10);

    // Write multi data
    wrDat(drvp, &drvp->fb[SSD1306_WIDTH_FIXED * idx], SSD1306_WIDTH_FIXED);
  }
}

static void toggleInvert(void *ip) {
  SSD1306Driver *drvp = (SSD1306Driver *)ip;
  uint16_t idx;

  // Toggle invert
  drvp->inv = !drvp->inv;

  for (idx = 0; idx < sizeof(drvp->fb); idx++) {
    if (idx % SSD1306_WIDTH_FIXED == 0) continue;
    drvp->fb[idx] = ~drvp->fb[idx];
  }
}

static void fillScreen(void *ip, ssd1306_color_t color) {
  SSD1306Driver *drvp = (SSD1306Driver *)ip;
  uint8_t idx;

  for (idx = 0; idx < 8; idx++) {
    drvp->fb[SSD1306_WIDTH_FIXED * idx] = 0x40;
    memset(&drvp->fb[SSD1306_WIDTH_FIXED * idx + 1],
           color == SSD1306_COLOR_BLACK ? 0x00 : 0xff, SSD1306_WIDTH);
  }
}

static void drawPixel(void *ip, uint8_t x, uint8_t y, ssd1306_color_t color) {
  SSD1306Driver *drvp = (SSD1306Driver *)ip;
  if (x > SSD1306_WIDTH || y > SSD1306_HEIGHT) return;

  // Check if pixels are inverted
  if (drvp->inv) {
    color = (ssd1306_color_t)!color;
  }

  // Set color
  if (color == SSD1306_COLOR_WHITE) {
    drvp->fb[x + (y / 8) * SSD1306_WIDTH_FIXED + 1] |= 1 << (y % 8);
  } else {
    drvp->fb[x + (y / 8) * SSD1306_WIDTH_FIXED + 1] &= ~(1 << (y % 8));
  }
}

static void gotoXy(void *ip, uint8_t x, uint8_t y) {
  SSD1306Driver *drvp = (SSD1306Driver *)ip;

  drvp->x = x;
  drvp->y = y;
}

static char PUTC(void *ip, char ch, const ssd1306_font_t *font, ssd1306_color_t color) {
  SSD1306Driver *drvp = (SSD1306Driver *)ip;
  uint32_t i, b, j;

  // Check available space in OLED
  if (drvp->x + font->fw >= SSD1306_WIDTH ||
      drvp->y + font->fh >= SSD1306_HEIGHT) {
    return 0;
  }

  // Go through font
  for (i = 0; i < font->fh; i++) {
    b = font->dt[(ch - 32) * font->fh + i];
    for (j = 0; j < font->fw; j++) {
      if ((b << j) & 0x8000) {
        drawPixel(drvp, drvp->x + j, drvp->y + i, color);
      } else {
        drawPixel(drvp, drvp->x + j, drvp->y + i,(ssd1306_color_t)! color);
      }
    }
  }

  // Increase pointer
  drvp->x += font->fw;

  // Return character written
  return ch;
}

static char PUTS(void *ip, char *str, const ssd1306_font_t *font, ssd1306_color_t color) {
  // Write characters
  while (*str) {
    // Write character by character
    if (PUTC(ip, *str, font, color) != *str) {
      // Return error
      return *str;
    }

    // Increase string pointer
    str++;
  }

  // Everything OK, zero should be returned
  return *str;
}

void drawLine(void *ip, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, ssd1306_color_t color) {

    SSD1306Driver *drvp = (SSD1306Driver *)ip;
    int16_t dx, dy, sx, sy, err, e2, i, tmp;

    /* Check for overflow */
    if (x0 >= SSD1306_WIDTH) {
        x0 = SSD1306_WIDTH - 1;
    }
    if (x1 >= SSD1306_WIDTH) {
        x1 = SSD1306_WIDTH - 1;
    }
    if (y0 >= SSD1306_HEIGHT) {
        y0 = SSD1306_HEIGHT - 1;
    }
    if (y1 >= SSD1306_HEIGHT) {
        y1 = SSD1306_HEIGHT - 1;
    }

    dx = (x0 < x1) ? (x1 - x0) : (x0 - x1);
    dy = (y0 < y1) ? (y1 - y0) : (y0 - y1);
    sx = (x0 < x1) ? 1 : -1;
    sy = (y0 < y1) ? 1 : -1;
    err = ((dx > dy) ? dx : -dy) / 2;

    if (dx == 0) {
        if (y1 < y0) {
            tmp = y1;
            y1 = y0;
            y0 = tmp;
        }

        if (x1 < x0) {
            tmp = x1;
            x1 = x0;
            x0 = tmp;
        }

        /* Vertical line */
        for (i = y0; i <= y1; i++) {
            drawPixel(drvp, x0, i, color);
        }

        /* Return from function */
        return;
    }

    if (dy == 0) {
        if (y1 < y0) {
            tmp = y1;
            y1 = y0;
            y0 = tmp;
        }

        if (x1 < x0) {
            tmp = x1;
            x1 = x0;
            x0 = tmp;
        }

        /* Horizontal line */
        for (i = x0; i <= x1; i++) {
          drawPixel(drvp, i, y0, color);
        }

        /* Return from function */
        return;
    }

    while (1) {
        drawPixel(drvp, x0, y0, color);
        if (x0 == x1 && y0 == y1) {
            break;
        }
        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y0 += sy;
        }
    }
}

void drawRect(void *ip, uint16_t x, uint16_t y, uint16_t w, uint16_t h, ssd1306_color_t color) {

    SSD1306Driver *drvp = (SSD1306Driver *)ip;

    /* Check input parameters */
    if (
        x >= SSD1306_WIDTH ||
        y >= SSD1306_HEIGHT
    ) {
        /* Return error */
        return;
    }

    /* Check width and height */
    if ((x + w) >= SSD1306_WIDTH) {
        w = SSD1306_WIDTH - x;
    }
    if ((y + h) >= SSD1306_HEIGHT) {
        h = SSD1306_HEIGHT - y;
    }

    /* Draw 4 lines */
    drawLine(drvp, x, y, x + w, y, color);         /* Top line */
    drawLine(drvp, x, y + h, x + w, y + h, color); /* Bottom line */
    drawLine(drvp, x, y, x, y + h, color);         /* Left line */
    drawLine(drvp, x + w, y, x + w, y + h, color); /* Right line */
}

void drawRectFill(void *ip, uint16_t x, uint16_t y, uint16_t w, uint16_t h, ssd1306_color_t color) {

    SSD1306Driver *drvp = (SSD1306Driver *)ip;
    uint8_t i;

    /* Check input parameters */
    if (
        x >= SSD1306_WIDTH ||
        y >= SSD1306_HEIGHT
    ) {
        /* Return error */
        return;
    }

    /* Check width and height */
    if ((x + w) >= SSD1306_WIDTH) {
        w = SSD1306_WIDTH - x;
    }
    if ((y + h) >= SSD1306_HEIGHT) {
        h = SSD1306_HEIGHT - y;
    }

    /* Draw lines */
    for (i = 0; i <= h; i++) {
        /* Draw lines */
        drawLine(drvp, x, y + i, x + w, y + i, color);
    }
}

void drawTri(void *ip, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, ssd1306_color_t color) {

    SSD1306Driver *drvp = (SSD1306Driver *)ip;

    /* Draw lines */
    drawLine(drvp, x1, y1, x2, y2, color);
    drawLine(drvp, x2, y2, x3, y3, color);
    drawLine(drvp, x3, y3, x1, y1, color);
}

void drawTriFill(void *ip, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, ssd1306_color_t color) {

    SSD1306Driver *drvp = (SSD1306Driver *)ip;

    int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
    yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
    curpixel = 0;

    deltax = ABS(x2 - x1);
    deltay = ABS(y2 - y1);
    x = x1;
    y = y1;

    if (x2 >= x1) {
        xinc1 = 1;
        xinc2 = 1;
    } else {
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1) {
        yinc1 = 1;
        yinc2 = 1;
    } else {
        yinc1 = -1;
        yinc2 = -1;
    }

    if (deltax >= deltay){
        xinc1 = 0;
        yinc2 = 0;
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax;
    } else {
        xinc2 = 0;
        yinc1 = 0;
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay;
    }

    for (curpixel = 0; curpixel <= numpixels; curpixel++) {
        drawLine(drvp, x, y, x3, y3, color);

        num += numadd;
        if (num >= den) {
            num -= den;
            x += xinc1;
            y += yinc1;
        }
        x += xinc2;
        y += yinc2;
    }
}

void drawCircle(void *ip, int16_t x0, int16_t y0, int16_t r, ssd1306_color_t color) {

    SSD1306Driver *drvp = (SSD1306Driver *)ip;

    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    drawPixel(drvp, x0, y0 + r, color);
    drawPixel(drvp, x0, y0 - r, color);
    drawPixel(drvp, x0 + r, y0, color);
    drawPixel(drvp, x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drawPixel(drvp, x0 + x, y0 + y, color);
        drawPixel(drvp, x0 - x, y0 + y, color);
        drawPixel(drvp, x0 + x, y0 - y, color);
        drawPixel(drvp, x0 - x, y0 - y, color);

        drawPixel(drvp, x0 + y, y0 + x, color);
        drawPixel(drvp, x0 - y, y0 + x, color);
        drawPixel(drvp, x0 + y, y0 - x, color);
        drawPixel(drvp, x0 - y, y0 - x, color);
    }
}

void drawCircleFill(void *ip, int16_t x0, int16_t y0, int16_t r, ssd1306_color_t color) {

    SSD1306Driver *drvp = (SSD1306Driver *)ip;

    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    drawPixel(drvp, x0, y0 + r, color);
    drawPixel(drvp, x0, y0 - r, color);
    drawPixel(drvp, x0 + r, y0, color);
    drawPixel(drvp, x0 - r, y0, color);
    drawLine(drvp, x0 - r, y0, x0 + r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drawLine(drvp, x0 - x, y0 + y, x0 + x, y0 + y, color);
        drawLine(drvp, x0 + x, y0 - y, x0 - x, y0 - y, color);

        drawLine(drvp, x0 + y, y0 + x, x0 - y, y0 + x, color);
        drawLine(drvp, x0 + y, y0 - x, x0 - y, y0 - x, color);
    }
}

static void setDisplay(void *ip, uint8_t on) {
  wrCmd(ip, 0x8D);
  wrCmd(ip, on ? 0x14 : 0x10);
  wrCmd(ip, 0xAE);
}

static const struct SSD1306VMT vmt_ssd1306 = {
  updateScreen, toggleInvert, fillScreen, drawPixel,
  gotoXy, PUTC, PUTS, drawLine, drawRect, drawRectFill,
  drawTri, drawTriFill, drawCircle, drawCircleFill, setDisplay
};

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

void ssd1306ObjectInit(SSD1306Driver *devp) {
  devp->vmt = &vmt_ssd1306;
  devp->config = NULL;

  devp->state = SSD1306_STOP;
}

void ssd1306Start(SSD1306Driver *devp, const SSD1306Config *config) {
  const uint8_t cmds[] = {
    0xAE,   // display off
    0x20,   // Set memory address
    0x10,   // 0x00: horizontal addressing mode, 0x01: vertical addressing mode
    0xB0,   // Set page start address for page addressing mode: 0 ~ 7
    0xC8,   // Set COM output scan direction
    0x00,   // Set low column address
    0x10,   // Set height column address
    0x40,   // Set start line address
    0x81,   // Set contrast control register
    0xFF,
    0xA1,   // Set segment re-map 0 to 127
    0xA6,   // Set normal display
    0xA8,   // Set multiplex ratio(1 to 64)
    0x3F,
    0xA4,   // 0xa4: ouput follows RAM content, 0xa5: ouput ignores RAM content
    0xD3,   // Set display offset
    0x00,   // Not offset
    0xD5,   // Set display clock divide ratio/oscillator frequency
    0xF0,   // Set divide ration
    0xD9,   // Set pre-charge period
    0x22,
    0xDA,   // Set COM pins hardware configuration
    0x12,
    0xDB,   // Set VCOMH
    0x20,   // 0x20: 0.77*Vcc
    0x8D,   // Set DC-DC enable
    0x14,
    0xAF,   // turn on SSD1306panel
  };
  uint8_t idx;

  chDbgCheck((devp != NULL) && (config != NULL));

  chDbgAssert((devp->state == SSD1306_STOP) || (devp->state == SSD1306_READY),
              "ssd1306Start(), invalid state");

  devp->config = config;

  chThdSleepMilliseconds(100);

  // OLED initialize
  for (idx = 0; idx < sizeof(cmds); idx++) {
    wrCmd(devp, cmds[idx]);
  }

  // Clear screen
  fillScreen(devp, SSD1306_COLOR_WHITE);

  // Update screen
  updateScreen(devp);

  // Set default value
  devp->x = 0;
  devp->y = 0;

  devp->state = SSD1306_READY;
}

void ssd1306Stop(SSD1306Driver *devp) {
  chDbgAssert((devp->state == SSD1306_STOP) || (devp->state == SSD1306_READY),
              "ssd1306Stop(), invalid state");

  if (devp->state == SSD1306_READY) {
    // Turn off display
    setDisplay(devp, 0);
  }

  devp->state = SSD1306_STOP;
}
