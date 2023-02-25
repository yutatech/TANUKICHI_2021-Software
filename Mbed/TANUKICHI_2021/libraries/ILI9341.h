/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef ILI9341_h
#define ILI9341_h

#include "mbed.h"
#include "fonts/Arial14.h"
#include "fonts/Arial_bold_14.h"

#define ILI9341_TFTWIDTH  240
#define ILI9341_TFTHEIGHT 320

#define ILI9341_NOP     0x00
#define ILI9341_SWRESET 0x01
#define ILI9341_RDDID   0x04
#define ILI9341_RDDST   0x09

#define ILI9341_SLPIN   0x10
#define ILI9341_SLPOUT  0x11
#define ILI9341_PTLON   0x12
#define ILI9341_NORON   0x13

#define ILI9341_RDMODE  0x0A
#define ILI9341_RDMADCTL  0x0B
#define ILI9341_RDPIXFMT  0x0C
#define ILI9341_RDIMGFMT  0x0D
#define ILI9341_RDSELFDIAG  0x0F

#define ILI9341_INVOFF  0x20
#define ILI9341_INVON   0x21
#define ILI9341_GAMMASET 0x26
#define ILI9341_DISPOFF 0x28
#define ILI9341_DISPON  0x29

#define ILI9341_CASET   0x2A
#define ILI9341_PASET   0x2B
#define ILI9341_RAMWR   0x2C
#define ILI9341_RAMRD   0x2E

#define ILI9341_PTLAR   0x30
#define ILI9341_MADCTL  0x36
#define ILI9341_PIXFMT  0x3A

#define ILI9341_FRMCTR1 0xB1
#define ILI9341_FRMCTR2 0xB2
#define ILI9341_FRMCTR3 0xB3
#define ILI9341_INVCTR  0xB4
#define ILI9341_DFUNCTR 0xB6

#define ILI9341_PWCTR1  0xC0
#define ILI9341_PWCTR2  0xC1
#define ILI9341_PWCTR3  0xC2
#define ILI9341_PWCTR4  0xC3
#define ILI9341_PWCTR5  0xC4
#define ILI9341_VMCTR1  0xC5
#define ILI9341_VMCTR2  0xC7

#define ILI9341_RDID1   0xDA
#define ILI9341_RDID2   0xDB
#define ILI9341_RDID3   0xDC
#define ILI9341_RDID4   0xDD

#define ILI9341_GMCTRP1 0xE0
#define ILI9341_GMCTRN1 0xE1

#define swap(a, b) { int16_t t = a; a = b; b = t; }
#define swapxy(a, b) { int16_t t = a; a = 240 - b; b = t; }
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

class ILI9341 {
public:
    const uint8_t Left = 0, Center = 1, Right = 2;
    
private:
public:
    SPI &spi;
    int spibase;
    DigitalOut cs;
    DigitalOut dc;
    
    const uint8_t *font = Arial_14;
    uint8_t reference = Left;
    uint8_t space = 1;
    
    void writecommand(uint8_t);
    void writedata(uint8_t);
    uint8_t readdata();
    void writedataf(uint8_t);
    void setAddrWindow(uint16_t, uint16_t, uint16_t, uint16_t);
    void spiwrite(uint8_t);
    uint8_t spiread();
    void drawCircleHelper( int16_t x0, int16_t y0,int16_t r, uint8_t cornername, uint16_t color);
    void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
public:
    ILI9341(SPI &_spi, int _spibase, PinName _cs, PinName _dc);
    void begin();
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    uint16_t color(uint8_t r, uint8_t g, uint8_t b);
    void drawimg(const uint16_t *image, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void cutImage(const uint16_t *image, int putx, int puty, int cutx, int cuty, int w, int h);
    void drawc(uint8_t c, uint16_t x, uint16_t y, uint16_t color);
    void print(const char c[], uint8_t structnum, uint16_t x, uint16_t y, uint16_t color);
    void print(int c, uint16_t x, uint16_t y, uint16_t color);
    void setFont(const uint8_t *_font, uint8_t _reference, uint8_t _space);
    void drawCircle(int16_t x0, int16_t y0, int16_t r,uint16_t color);
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
    void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
    void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t colo);
    void fillArcOffsetted(uint16_t cx, uint16_t cy, uint16_t radius, uint16_t thickness, float start, float end, uint16_t color);
    void fillArc(uint16_t x, uint16_t y, uint16_t radius, uint16_t thickness, float start, float end, uint16_t color);
    void drawPopup(int16_t sx, int16_t sy, int16_t width, int16_t hight, int16_t refX, int16_t refY);
    void drawCIAO(void);
};

#endif /* ILI9341_h */
