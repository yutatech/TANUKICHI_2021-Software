/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "ILI9341.h"

ILI9341::ILI9341(SPI &_spi, int _spibase, PinName _cs, PinName _dc) : spi(_spi), spibase(_spibase), cs(_cs), dc(_dc){
    cs = 1;
    dc = 1;
}

void ILI9341::begin(){
    spi.frequency(8000000);
    spi.format(8, 0);
    cs = 0;
    dc = 0;
    
    writecommand(0xEF);
    writedata(0x03);
    writedata(0x80);
    writedata(0x02);
    
    writecommand(0xCF);
    writedata(0x00);
    writedata(0XC1);
    writedata(0X30);
    
    writecommand(0xED);
    writedata(0x64);
    writedata(0x03);
    writedata(0X12);
    writedata(0X81);
    
    writecommand(0xE8);
    writedata(0x85);
    writedata(0x00);
    writedata(0x78);
    
    writecommand(0xCB);
    writedata(0x39);
    writedata(0x2C);
    writedata(0x00);
    writedata(0x34);
    writedata(0x02);
    
    writecommand(0xF7);
    writedata(0x20);
    
    writecommand(0xEA);
    writedata(0x00);
    writedata(0x00);
    
    writecommand(ILI9341_PWCTR1);
    writedata(0x23);
    
    writecommand(ILI9341_PWCTR2);
    writedata(0x10);
    
    writecommand(ILI9341_VMCTR1);
    writedata(0x3e);
    writedata(0x28);
    
    writecommand(ILI9341_VMCTR2);
    writedata(0x86);
    
    writecommand(ILI9341_MADCTL);
    writedata(0x48);
    
    writecommand(ILI9341_PIXFMT);
    writedata(0x55);
    
    writecommand(ILI9341_FRMCTR1);
    writedata(0x00);
    writedata(0x18);
    
    writecommand(ILI9341_DFUNCTR);
    writedata(0x08);
    writedata(0x82);
    writedata(0x27);
    
    writecommand(0xF2);
    writedata(0x00);
    
    writecommand(ILI9341_GAMMASET);
    writedata(0x01);
    
    writecommand(ILI9341_GMCTRP1);
    writedata(0x0F);
    writedata(0x31);
    writedata(0x2B);
    writedata(0x0C);
    writedata(0x0E);
    writedata(0x08);
    writedata(0x4E);
    writedata(0xF1);
    writedata(0x37);
    writedata(0x07);
    writedata(0x10);
    writedata(0x03);
    writedata(0x0E);
    writedata(0x09);
    writedata(0x00);
    
    writecommand(ILI9341_GMCTRN1);
    writedata(0x00);
    writedata(0x0E);
    writedata(0x14);
    writedata(0x03);
    writedata(0x11);
    writedata(0x07);
    writedata(0x31);
    writedata(0xC1);
    writedata(0x48);
    writedata(0x08);
    writedata(0x0F);
    writedata(0x0C);
    writedata(0x31);
    writedata(0x36);
    writedata(0x0F);
    
    writecommand(ILI9341_SLPOUT);
    wait_ms(120);
    writecommand(ILI9341_DISPON);
}

void ILI9341::writecommand(uint8_t c){
    cs=0;
    dc=0;
    spiwrite(c);
    wait_us(0);
    cs=1;
}

void ILI9341::writedata(uint8_t c){
    cs=0;
    dc=1;
    spiwrite(c);
    wait_us(0);
    cs=1;
}

uint8_t ILI9341::readdata(){
    cs=0;
    dc=1;
    uint8_t data = spiread();
    cs=1;
    return data;
}

void ILI9341::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    spi.frequency(15000000);
    writecommand(ILI9341_CASET);
    writedata(x0 >> 8);
    writedata(x0 & 0xFF);
    writedata(x1 >> 8);
    writedata(x1 & 0xFF);
    
    writecommand(ILI9341_PASET);
    writedata(y0>>8);
    writedata(y0);
    writedata(y1>>8);
    writedata(y1);
    
    writecommand(ILI9341_RAMWR);
}

void ILI9341::spiwrite(uint8_t data){
    while(!(((SPI_TypeDef *) spibase)->SR & SPI_SR_TXE));
    ((SPI_TypeDef *) spibase)->DR = data;
}

uint8_t ILI9341::spiread(){
    return spi.write(0);
}

void ILI9341::drawPixel(int16_t x, int16_t y, uint16_t color){
    swapxy(x, y);
    if(x>=240||y>=320||x<0||y<0)
        return;
    setAddrWindow(x,y,x+1,y+1);
    cs=0;
    dc=1;
    spiwrite(color >> 8);
    spiwrite(color);
    wait_us(0);
    cs=1;
}

void ILI9341::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    swapxy(x, y);
    x -= h - 1;
    if(x>=240||y>=320||y<0)
        return;
    if(x+h-1>=240)
        h=240-x;
    if(x<0){
        h+=x;
        x=0;
    }
    setAddrWindow(x, y, constrain(x+h, 0, 239), y);
    uint8_t hi = color >> 8, lo = color;
    cs=0;
    dc=1;
    while (h--) {
        spiwrite(hi);
        spiwrite(lo);
    }
    wait_us(0);
    cs=1;
}

void ILI9341::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    swapxy(x, y);
    if(x>=240||y>=320||x<0)
        return;
    if(y+w-1>=320)
        w=320-y;
    if(y<0){
        w+=y;
        y=0;
    }
    setAddrWindow(x, y, x, constrain(y+w-1, 0, 319));
    uint8_t hi = color >> 8, lo = color;
    cs=0;
    dc=1;
    while (w--) {
        spiwrite(hi);
        spiwrite(lo);
    }
    wait_us(0);
    cs=1;
}

uint16_t ILI9341::color(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


void ILI9341::drawimg(const uint16_t *image, uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    swapxy(x, y);
    x -= h;
    setAddrWindow(x, y, x+h-1, y+w-1);
    uint32_t totalpixel = w*h;
    uint32_t nowpixel = totalpixel;
    cs=0;
    dc=1;
    while (nowpixel--) {
        spiwrite(image[totalpixel-nowpixel-1] >> 8);
        spiwrite(image[totalpixel-nowpixel-1]);
    }
    wait_us(0);
    cs=1;
}

void ILI9341::cutImage(const uint16_t *image, int putx, int puty, int cutx, int cuty, int w, int h){
    swapxy(putx, puty);
    putx -= h;
    setAddrWindow(putx, puty, putx+h-1, puty+w-1);
    uint32_t totalpixel = w*h;
    uint32_t nowpixel = totalpixel;
    cs=0;
    dc=1;
    while (nowpixel--) {
        spiwrite(image[((cutx + (totalpixel-nowpixel-1) / h) * 240 + 240 - h - cuty + (totalpixel-nowpixel-1)  % h)] >> 8);
        spiwrite(image[((cutx + (totalpixel-nowpixel-1) / h) * 240 + 240 - h - cuty + (totalpixel-nowpixel-1)  % h)]);
    }
    wait_us(0);
    cs=1;
}

void ILI9341::drawc(uint8_t c, uint16_t x, uint16_t y, uint16_t color){
    uint16_t offset=0;
    for(uint8_t i=6; i<c-26;i++) offset += font[i]*2;
    
    for(uint8_t i=0; i < font[c - 26] + 1; i++) {
        uint16_t line;
        if(i < font[c - 26])
            line = (font[102 + offset + i + font[c - 26]] << 6) + font[102 + offset + i];
        else
            line = 0x0;
        for(uint8_t j = 0; j < font[3]; j++, line >>= 1) {
            if(line & 0x1)
                drawPixel(x+i, y+j, color);
        }
    }
}

void ILI9341::setFont(const uint8_t *_font, uint8_t _reference, uint8_t _space){
    font = _font;
    reference = _reference;
    space = _space;
}

void ILI9341::print(const char c[], uint8_t structnum, uint16_t x, uint16_t y, uint16_t color){
    int offset = 0;
    for(int i = 0; i < structnum; i++)
        offset += font[c[i] - 26];
    offset += space * (structnum - 1);
    switch(reference){
        case 1:
            x -= offset / 2;
            break;
        case 2:
            x -= offset;
            break;
    }
    for(int i = 0; i < structnum; i++){
        drawc(c[i], x, y, color);
        x += font[c[i] - 26] + space;
    }
}

void ILI9341::print(int c, uint16_t x, uint16_t y, uint16_t color){
    
    bool sign = c < 0;
    if(c < 0) c = -c;
    int digit = 1, pow = 10;
    for(; c >= pow && digit <10; pow *= 10) digit++;
    char c2[digit + sign];
    if(sign) c2[0] = '-';
    for(int num = sign; num < digit + sign; num++){
        pow /= 10;
        c2[num] = 0x30 + c / pow;
        c -= c / pow * pow;
    }
    print(c2, digit + sign, x, y, color);
}



/* The part below here is copied from Adafruit-GFX-Library(https://github.com/adafruit/Adafruit-GFX-Library) */

/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
 */


void ILI9341::drawCircle(int16_t x0, int16_t y0, int16_t r,uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    
    drawPixel(x0  , y0+r, color);
    drawPixel(x0  , y0-r, color);
    drawPixel(x0+r, y0  , color);
    drawPixel(x0-r, y0  , color);
    
    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        
        drawPixel(x0 + x, y0 + y, color);
        drawPixel(x0 - x, y0 + y, color);
        drawPixel(x0 + x, y0 - y, color);
        drawPixel(x0 - x, y0 - y, color);
        drawPixel(x0 + y, y0 + x, color);
        drawPixel(x0 - y, y0 + x, color);
        drawPixel(x0 + y, y0 - x, color);
        drawPixel(x0 - y, y0 - x, color);
    }
}

void ILI9341::drawCircleHelper( int16_t x0, int16_t y0,int16_t r, uint8_t cornername, uint16_t color) {
    //    swapxy(x0, y0);
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;
    
    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        if (cornername & 0x4) {
            drawPixel(x0 + x, y0 + y, color);
            drawPixel(x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) {
            drawPixel(x0 + x, y0 - y, color);
            drawPixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
            drawPixel(x0 - y, y0 + x, color);
            drawPixel(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
            drawPixel(x0 - y, y0 - x, color);
            drawPixel(x0 - x, y0 - y, color);
        }
    }
}

void ILI9341::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    drawFastVLine(x0, y0-r, 2*r+1, color);
    fillCircleHelper(x0, y0, r, 3, 0, color);
}

// Used to do circles and roundrects
void ILI9341::fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color) {
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;
    
    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        
        if (cornername & 0x1) {
            drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
            drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
        }
        if (cornername & 0x2) {
            drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
            drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
        }
    }
}

void ILI9341::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        swap(x0, y0);
        swap(x1, y1);
    }
    
    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }
    
    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);
    
    int16_t err = dx / 2;
    int16_t ystep;
    
    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }
    
    for (; x0<=x1; x0++) {
        if (steep) {
            drawPixel(y0, x0, color);
        } else {
            drawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void ILI9341::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    drawFastHLine(x, y, w, color);
    drawFastHLine(x, y+h-1, w, color);
    drawFastVLine(x, y, h, color);
    drawFastVLine(x+w-1, y, h, color);
}

void ILI9341::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    swapxy(x, y);
    swap(w, h);
    x -= w;
    setAddrWindow(x, y, x+w-1, y+h-1);
    uint8_t hi = color >> 8, lo = color;
    cs=0;
    dc=1;
    uint32_t num = w * h;
    while (num--) {
        spiwrite(hi);
        spiwrite(lo);
    }
    wait_us(0);
    cs=1;
}

void ILI9341::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    // smarter version
    drawFastHLine(x+r  , y    , w-2*r, color); // Top
    drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
    drawFastVLine(x    , y+r  , h-2*r, color); // Left
    drawFastVLine(x+w-1, y+r  , h-2*r, color); // Right
    // draw four corners
    drawCircleHelper(x+r    , y+r    , r, 1, color);
    drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
    drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
    drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}

// Fill a rounded rectangle
void ILI9341::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    // smarter version
    fillRect(x+r, y, w-2*r, h, color);
    
    // draw four corners
    fillCircleHelper(x+w-r-1, y+r+1, r, 1, h-2*r-1, color);
    fillCircleHelper(x+r    , y+r+1, r, 2, h-2*r-1, color);
}

void ILI9341::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}

void ILI9341::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    int16_t a, b, y, last;
    
    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        swap(y0, y1); swap(x0, x1);
    }
    if (y1 > y2) {
        swap(y2, y1); swap(x2, x1);
    }
    if (y0 > y1) {
        swap(y0, y1); swap(x0, x1);
    }
    
    if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if(x1 < a)      a = x1;
        else if(x1 > b) b = x1;
        if(x2 < a)      a = x2;
        else if(x2 > b) b = x2;
        drawFastHLine(a, y0, b-a+1, color);
        return;
    }
    
    int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;
    int32_t
    sa   = 0,
    sb   = 0;
    
    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if(y1 == y2) last = y1;   // Include y1 scanline
    else         last = y1-1; // Skip it
    
    for(y=y0; y<=last; y++) {
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
         a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
         b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
         */
        if(a > b) swap(a,b);
        drawFastHLine(a, y, b-a+1, color);
    }
    
    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for(; y<=y2; y++) {
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
         a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
         b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
         */
        if(a > b) swap(a,b);
        drawFastHLine(a, y, b-a+1, color);
    }
}

void ILI9341::fillArcOffsetted(uint16_t cx, uint16_t cy, uint16_t radius, uint16_t thickness, float start, float end, uint16_t color) {
    int16_t xmin = 65535, xmax = -32767, ymin = 32767, ymax = -32767;
    float cosStart, sinStart, cosEnd, sinEnd;
    float r, t;
    float startAngle, endAngle;
    
    //Serial << "start: " << start << " end: " << end << endl;
    startAngle = start;    // 252
    endAngle = end;        // 807
    //Serial << "startAngle: " << startAngle << " endAngle: " << endAngle << endl;
    
    while (startAngle < 0) startAngle += 360;
    while (endAngle < 0) endAngle += 360;
    while (startAngle > 360) startAngle -= 360;
    while (endAngle > 360) endAngle -= 360;
    //Serial << "startAngleAdj: " << startAngle << " endAngleAdj: " << endAngle << endl;
    //if (endAngle == 0) endAngle = 360;
    
    if (startAngle > endAngle) {
        fillArcOffsetted(cx, cy, radius, thickness, startAngle, 360, color);
        fillArcOffsetted(cx, cy, radius, thickness, 0, endAngle, color);
    }
    else {
        // Calculate bounding box for the arc to be drawn
        cosStart = cos(startAngle*3.14/180);
        sinStart = sin(startAngle*3.14/180);
        cosEnd = cos(endAngle*3.14/180);
        sinEnd = sin(endAngle*3.14/180);
        
        //Serial << cosStart << " " << sinStart << " " << cosEnd << " " << sinEnd << endl;
        
        r = radius;
        // Point 1: radius & startAngle
        t = r * cosStart;
        if (t < xmin) xmin = t;
        if (t > xmax) xmax = t;
        t = r * sinStart;
        if (t < ymin) ymin = t;
        if (t > ymax) ymax = t;
        
        // Point 2: radius & endAngle
        t = r * cosEnd;
        if (t < xmin) xmin = t;
        if (t > xmax) xmax = t;
        t = r * sinEnd;
        if (t < ymin) ymin = t;
        if (t > ymax) ymax = t;
        
        r = radius - thickness;
        // Point 3: radius-thickness & startAngle
        t = r * cosStart;
        if (t < xmin) xmin = t;
        if (t > xmax) xmax = t;
        t = r * sinStart;
        if (t < ymin) ymin = t;
        if (t > ymax) ymax = t;
        
        // Point 4: radius-thickness & endAngle
        t = r * cosEnd;
        if (t < xmin) xmin = t;
        if (t > xmax) xmax = t;
        t = r * sinEnd;
        if (t < ymin) ymin = t;
        if (t > ymax) ymax = t;
        
        
        //Serial << xmin << " " << xmax << " " << ymin << " " << ymax << endl;
        // Corrections if arc crosses X or Y axis
        if ((startAngle < 90) && (endAngle > 90)) {
            ymax = radius;
        }
        
        if ((startAngle < 180) && (endAngle > 180)) {
            xmin = -radius;
        }
        
        if ((startAngle < 270) && (endAngle > 270)) {
            ymin = -radius;
        }
        
        // Slopes for the two sides of the arc
        float sslope = (float)cosStart / (float)sinStart;
        float eslope = (float)cosEnd / (float)sinEnd;
        
        //Serial << "sslope2: " << sslope << " eslope2:" << eslope << endl;
        
        if (endAngle == 360) eslope = -1000000;
        
        int ir2 = (radius - thickness) * (radius - thickness);
        int or2 = radius * radius;
        //Serial << "ymin: " << ymin << " ymax: " << ymax << endl;
        
//        fillScanline16(color);
        
        for (int x = xmin; x <= xmax; x++) {
            bool y1StartFound = false, y2StartFound = false;
            bool y1EndFound = false, y2EndSearching = false;
            int y1s = 0, y1e = 0, y2s = 0;
            for (int y = ymin; y <= ymax; y++)
            {
                int x2 = x * x;
                int y2 = y * y;
                
                if (
                    (x2 + y2 < or2 && x2 + y2 >= ir2) && (
                                                          (y > 0 && startAngle < 180 && x <= y * sslope) ||
                                                          (y < 0 && startAngle > 180 && x >= y * sslope) ||
                                                          (y < 0 && startAngle <= 180) ||
                                                          (y == 0 && startAngle <= 180 && x < 0) ||
                                                          (y == 0 && startAngle == 0 && x > 0)
                                                          ) && (
                                                                (y > 0 && endAngle < 180 && x >= y * eslope) ||
                                                                (y < 0 && endAngle > 180 && x <= y * eslope) ||
                                                                (y > 0 && endAngle >= 180) ||
                                                                (y == 0 && endAngle >= 180 && x < 0) ||
                                                                (y == 0 && startAngle == 0 && x > 0)))
                {
                    if (!y1StartFound)    //start of the higher line found
                    {
                        y1StartFound = true;
                        y1s = y;
                    }
                    else if (y1EndFound && !y2StartFound) //start of the lower line found
                    {
                        //Serial << "Found y2 start x: " << x << " y:" << y << endl;
                        y2StartFound = true;
                        //drawPixel_cont(cx+x, cy+y, ILI9341_BLUE);
                        y2s = y;
                        y += y1e - y1s - 1;    // calculate the most probable end of the lower line (in most cases the length of lower line is equal to length of upper line), in the next loop we will validate if the end of line is really there
                        if (y > ymax - 1) // the most probable end of line 2 is beyond ymax so line 2 must be shorter, thus continue with pixel by pixel search
                        {
                            y = y2s;    // reset y and continue with pixel by pixel search
                            y2EndSearching = true;
                        }
                        
                        //Serial << "Upper line length: " << (y1e - y1s) << " Setting y to " << y << endl;
                    }
                    else if (y2StartFound && !y2EndSearching)
                    {
                        // we validated that the probable end of the lower line has a pixel, continue with pixel by pixel search, in most cases next loop with confirm the end of lower line as it will not find a valid pixel
                        y2EndSearching = true;
                    }
                    //Serial << "x:" << x << " y:" << y << endl;
                    //drawPixel_cont(cx+x, cy+y, ILI9341_BLUE);
                }
                else
                {
                    if (y1StartFound && !y1EndFound) //higher line end found
                    {
                        y1EndFound = true;
                        y1e = y - 1;
                        //Serial << "line: " << y1s << " - " << y1e << endl;
                        drawFastVLine(cx + x, cy + y1s, y - y1s, color);
                        if (y < 0)
                        {
                            //Serial << x << " " << y << endl;
                            y = abs(y); // skip the empty middle
                        }
                        else
                            break;
                    }
                    else if (y2StartFound)
                    {
                        if (y2EndSearching)
                        {
                            //Serial << "Found final end at y: " << y << endl;
                            // we found the end of the lower line after pixel by pixel search
                            drawFastVLine(cx + x, cy + y2s, y - y2s, color);
                            y2EndSearching = false;
                            break;
                        }
                        else
                        {
                            //Serial << "Expected end not found" << endl;
                            // the expected end of the lower line is not there so the lower line must be shorter
                            y = y2s;    // put the y back to the lower line start and go pixel by pixel to find the end
                            y2EndSearching = true;
                        }
                    }
                    //else
                    //drawPixel_cont(cx+x, cy+y, ILI9341_RED);
                }
                //
                
                //delay(75);
            }
            if (y1StartFound && !y1EndFound)
            {
                y1e = ymax;
                //Serial << "line: " << y1s << " - " << y1e << endl;
                drawFastVLine(cx + x, cy + y1s, y1e - y1s + 1, color);
            }
            else if (y2StartFound && y2EndSearching)    // we found start of lower line but we are still searching for the end
            {                                        // which we haven't found in the loop so the last pixel in a column must be the end
                drawFastVLine(cx + x, cy + y2s, ymax - y2s + 1, color);
            }
        }
    }
}


void ILI9341::fillArc(uint16_t x, uint16_t y, uint16_t radius, uint16_t thickness, float start, float end, uint16_t color){
    cs=0;
    dc=1;
    if (start == 0 && end == 360)
        fillArcOffsetted(x, y, radius, thickness, 0, 360, color);
    else
        fillArcOffsetted(x, y, radius, thickness, start - 90, end - 90, color);
    wait_us(0);
    cs=1;
}

/* The part above here is copied from Adafruit-GFX-Library(https://github.com/adafruit/Adafruit-GFX-Library) */



void ILI9341::drawPopup(int16_t sx, int16_t sy, int16_t width, int16_t hight, int16_t refX, int16_t refY){
    int16_t ex = sx + width - 1, ey = sy + hight - 1;
    
//    if(sx < 15){
//        ex += 15 - sx;
//        sx = 15;
//    }
//    else if(ex > 304){
//        sx -= ex - 304;
//        ex = 304;
//        if(sx < 15){
//            ex += 15 - sx;
//            sx = 15;
//        }
//    }
//    if(sy < 22){
//        ey += 22 - sx;
//        sy = 22;
//    }
//    else if(ey > 217){
//        sy -= ey - 217;
//        ey = 217;
//        if(sy < 22){
//            ey += 22 - sx;
//            sy = 22;
//        }
//    }
    refX = constrain(refX, sx + 3, ex - 3);
    refY = constrain(refY, sy + 3, ey - 3);

    int16_t numXp = ex - refX;
    int16_t numXm = refX - sx;
    int16_t numYp = ey - refY;
    int16_t numYm = refY - sy;
    numXp = numXp > 0 ? numXp : 10000;
    numXm = numXm > 0 ? numXm : 10000;
    numYp = numYp > 0 ? numYp : 10000;
    numYm = numYm > 0 ? numYm : 10000;
    
    int16_t iXp = 1, iXm = 1;
    int16_t iYp = 1, iYm = 1;
    
    int16_t roundEdge[] = {7,5,4,3,2,1,1};
    
    drawPixel(refX, refY + 1, color(200,200,200));
    for(int16_t i = 1; i <= 400; i++){
        if(round(400 * iXp / numXp) == i){
            int16_t st,ed;
            if(numXp - iXp < 7 && sy + 1 + (numXp - iXp < 7 ? roundEdge[numXp - iXp] : 0) > refY - iYm + 2)
                st = sy + 1 + roundEdge[numXp - iXp];
            else
                st = refY - iYm + 2;
            
            if(numXp - iXp < 7 && ey + 1 - (numXp - iXp < 7 ? roundEdge[numXp - iXp] : 0) < refY + iYp + 1)
                ed = ey + 1 - roundEdge[numXp - iXp];
            else
                ed = refY + iYp + 1;
                
            drawFastVLine(refX + iXp, st, ed - st + 1, color(200,200,200));
            iXp++;
        }
        if(round(400 * iXm / numXm) == i){
            int16_t st,ed;
            if(numXm - iXm < 7 && sy + 1 + (numXm - iXm < 7 ? roundEdge[numXm - iXm] : 0) > refY - iYm + 2)
                st = sy + 1 + roundEdge[numXm - iXm];
            else
                st = refY - iYm + 2;
            
            if(numXm - iXm < 7 && ey + 1 - (numXm - iXm < 7 ? roundEdge[numXm - iXm] : 0) < refY + iYp + 1)
                ed = ey + 1 - roundEdge[numXm - iXm];
            else
                ed = refY + iYp + 1;
                
            drawFastVLine(refX - iXm, st, ed - st + 1, color(200,200,200));
            iXm++;
        }
        
        if(round(400 * iYp / numYp) == i){
            int16_t st,ed;
            if(numYp - iYp < 7 && sx  + (numYp - iYp < 7 ? roundEdge[numYp - iYp] : 0) > refX - iXm + 1)
                st = sx + roundEdge[numYp - iYp];
            else
                st = refX - iXm + 1;
            
            if(numYp - iYp < 7 && ex - (numYp - iYp < 7 ? roundEdge[numYp - iYp] : 0) < refX + iXp)
                ed = ex - roundEdge[numYp - iYp];
            else
                ed = refX + iXp;
                
            drawFastHLine(st, refY + iYp + 1, ed - st + 1, color(200,200,200));
            iYp++;
        }
        if(round(400 * iYm / numYm) == i){
            int16_t st,ed;
            if(numYm - iYm < 7 && sx  + (numYm - iYm < 7 ? roundEdge[numYm - iYm] : 0) > refX - iXm + 1)
                st = sx + roundEdge[numYm - iYm];
            else
                st = refX - iXm + 1;
            
            if(numYm - iYm < 7 && ex - (numYm - iYm < 7 ? roundEdge[numYm - iYm] : 0) < refX + iXp)
                ed = ex - roundEdge[numYm - iYm];
            else
                ed = refX + iXp;
                
            drawFastHLine(st, refY - iYm + 1, ed - st + 1, color(200,200,200));
            iYm++;
        }
        
        wait_us((400 - i*i*i/160000) * 0.6);
    }
}

void ILI9341::drawCIAO(void) {
    fillTriangle(55, 90, 55, 100, 65, 110,  0x03E0);//C
    fillTriangle(55, 90, 65, 80, 65, 110 , 0x03E0);
    fillTriangle(80, 65, 100, 65, 70, 75 , 0x03E0);
    fillTriangle(110, 75, 100, 65, 70, 75 , 0x03E0);
    fillTriangle(70, 115, 80, 125, 110, 115 , 0x03E0);
    fillTriangle(100, 125, 80, 125, 110, 115 , 0x03E0);
    
    
    
    fillRect(120, 65, 30, 10, 0x03E0);//I
    fillRect(130, 80, 10, 30, 0x03E0);
    fillRect(120, 115, 30, 10, 0x03E0);
    
    
    
    fillTriangle(167, 105, 160, 125, 177, 105 , 0x03E0);//A
    fillTriangle(170, 125, 160, 125, 177, 105 , 0x03E0);
    fillTriangle(170, 95, 180, 95, 180, 65 , 0x03E0);
    fillTriangle(190, 65, 180, 95, 180, 65 , 0x03E0);
    fillTriangle(180, 65, 190, 65, 188, 90 , 0x03E0);
    fillTriangle(198, 90, 190, 65, 188, 90 , 0x03E0);
    fillTriangle(186, 95, 182, 105, 206, 95 , 0x03E0);
    fillTriangle(202, 105, 182, 105, 206, 95 , 0x03E0);
    fillTriangle(195, 110, 205, 110, 200, 125 , 0x03E0);
    fillTriangle(210, 125, 205, 110, 200, 125 , 0x03E0);
    
    
    
    fillRect(230, 115, 20, 10, 0x03E0);//O
    fillRect(230, 65, 20, 10, 0x03E0);
    fillTriangle(215, 80, 225, 70, 215, 110 , 0x03E0);
    fillTriangle(225, 120, 225, 70, 215, 110 , 0x03E0);
    fillTriangle(255, 70, 265, 80, 255, 120 , 0x03E0);
    fillTriangle(265, 110, 265, 80, 255, 120 , 0x03E0);
    
    
    
    ///////////////////////////////////////////////////////////////////////////
    fillRect(57, 137, 16, 4, 0x03E0);//T
    fillRect(63, 137, 4, 28, 0x03E0);
    
    
    fillRect(77, 145, 16, 4, 0x03E0);//e
    fillRect(77, 145, 4, 20, 0x03E0);
    fillRect(89, 145, 4, 12, 0x03E0);
    fillRect(77, 161, 16, 4, 0x03E0);
    fillRect(77, 153, 16, 4, 0x03E0);
    
    
    fillRect(97, 145, 16, 4, 0x03E0);//z
    fillRect(97, 161, 16, 4, 0x03E0);
    fillTriangle(109, 146, 112, 149, 97, 161, 0x03E0);
    fillTriangle(100, 164, 112, 149, 97, 161, 0x03E0);
    
    
    fillRect(117, 145, 4, 20, 0x03E0);//u
    fillRect(129, 145, 4, 20, 0x03E0);
    fillRect(117, 161, 16, 4, 0x03E0);
    
    
    fillRect(137, 137, 4, 28, 0x03E0);//k
    fillTriangle(147, 146, 137, 155, 143, 155, 0x03E0);
    fillTriangle(147, 146, 152, 146, 143, 155, 0x03E0);
    fillTriangle(147, 165, 137, 155, 143, 155, 0x03E0);
    fillTriangle(147, 165, 152, 165, 143, 155, 0x03E0);
    
    
    fillRect(157, 145, 16, 4, 0x03E0);//a
    fillRect(169, 145, 4, 20, 0x03E0);
    fillRect(157, 153, 16, 4, 0x03E0);
    fillRect(157, 153, 4, 12, 0x03E0);
    fillRect(157, 161, 16, 4, 0x03E0);
    
    
    fillTriangle(177, 145, 181, 145, 185, 156, 0x03E0);//y
    fillTriangle(177, 145, 184, 161, 185, 156, 0x03E0);
    fillTriangle(189, 145, 179, 167, 183, 167, 0x03E0);
    fillTriangle(189, 145, 193, 145, 183, 167, 0x03E0);
    fillRect(177, 165, 6, 4, 0x03E0);
    
    
    fillRect(197, 145, 16, 4, 0x03E0);//a
    fillRect(209, 145, 4, 20, 0x03E0);
    fillRect(197, 153, 16, 4, 0x03E0);
    fillRect(197, 153, 4, 12, 0x03E0);
    fillRect(197, 161, 16, 4, 0x03E0);
    
    fillRect(217, 145, 20, 4, 0x03E0); //m
    fillRect(217, 145, 4, 20, 0x03E0);
    fillRect(225, 145, 4, 20, 0x03E0);
    fillRect(233, 145, 4, 20, 0x03E0);
    
    
    fillRect(241, 145, 16, 4, 0x03E0);//a
    fillRect(253, 145, 4, 20, 0x03E0);
    fillRect(241, 153, 16, 4, 0x03E0);
    fillRect(241, 153, 4, 12, 0x03E0);
    fillRect(241, 161, 16, 4, 0x03E0);
    return;
}
