/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef LINE_App_h
#define LINE_App_h

#include "ApplicationFramework.h"
#include "../bitmaps/LINE_icon.h"

class LINE_App : public ApplicationFramework {
private:
    Line &line;
    Line_S line_now, line_last;
    bool calibration;
    
public:
    int backColor = tft.color(0,0,0);
    LINE_App(ILI9341 &_tft, Line &_line) : line(_line), ApplicationFramework(_tft, LINE_icon) {
            strcpy(AppName, "Line");
            AppNameLen = 4;
    }
    
    void begin(){
        tft.fillRect(0,0,240,240,backColor);
        tft.fillRect(240,0,80,240,tft.color(60,60,60));
        
        tft.fillRoundRect(245,185,70,50,10,tft.color(0,200,0));
        tft.setFont(Arial_14, tft.Center, 1);
        tft.print("Calibration", 11, 280, 198, tft.color(60,60,60));
        tft.print("Start", 5, 280, 212, tft.color(60,60,60));
        
#if defined(POSITION_OF)
        uint16_t green = tft.color(0,150,0);
        tft.fillCircle(101,36,4,green);
        tft.fillCircle(114,36,4,green);
        tft.fillCircle(127,36,4,green);
        tft.fillCircle(140,36,4,green);
        tft.fillCircle(120,43,4,green);
        tft.fillCircle(120,56,4,green);
        tft.fillCircle(120,69,4,green);
        tft.fillCircle(120,82,4,green);
        tft.fillCircle(107,95,4,green);
        tft.fillCircle(133,95,4,green);
        
        tft.fillCircle(231,101,4,green);
        tft.fillCircle(231,114,4,green);
        tft.fillCircle(231,127,4,green);
        tft.fillCircle(231,140,4,green);
        tft.fillCircle(224,120,4,green);
        tft.fillCircle(211,120,4,green);
        tft.fillCircle(198,120,4,green);
        tft.fillCircle(185,120,4,green);
        tft.fillCircle(172,120,4,green);
        tft.fillCircle(159,120,4,green);
        tft.fillCircle(146,107,4,green);
        tft.fillCircle(146,133,4,green);
        
        tft.fillCircle(101,203,4,green);
        tft.fillCircle(114,203,4,green);
        tft.fillCircle(127,203,4,green);
        tft.fillCircle(140,203,4,green);
        tft.fillCircle(120,196,4,green);
        tft.fillCircle(120,183,4,green);
        tft.fillCircle(120,170,4,green);
        tft.fillCircle(120,157,4,green);
        tft.fillCircle(133,144,4,green);
        tft.fillCircle(107,144,4,green);
        
        tft.fillCircle(10,101,4,green);
        tft.fillCircle(10,114,4,green);
        tft.fillCircle(10,127,4,green);
        tft.fillCircle(10,140,4,green);
        tft.fillCircle(17,120,4,green);
        tft.fillCircle(30,120,4,green);
        tft.fillCircle(43,120,4,green);
        tft.fillCircle(56,120,4,green);
        tft.fillCircle(69,120,4,green);
        tft.fillCircle(82,120,4,green);
        tft.fillCircle(95,133,4,green);
        tft.fillCircle(95,107,4,green);
#elif defined(POSITION_DF)
        uint16_t green = tft.color(0,150,0);
        tft.fillCircle(101,49,4,green);
        tft.fillCircle(114,49,4,green);
        tft.fillCircle(127,49,4,green);
        tft.fillCircle(140,49,4,green);
        tft.fillCircle(120,56,4,green);
        tft.fillCircle(120,69,4,green);
        tft.fillCircle(120,82,4,green);
        tft.fillCircle(107,95,4,green);
        tft.fillCircle(133,95,4,green);

//        tft.fillCircle(231,101,4,green);
        tft.fillCircle(231,114,4,green);
        tft.fillCircle(231,127,4,green);
        tft.fillCircle(231,140,4,green);
        tft.fillCircle(224,120,4,green);
        tft.fillCircle(211,120,4,green);
        tft.fillCircle(198,120,4,green);
        tft.fillCircle(185,120,4,green);
        tft.fillCircle(172,120,4,green);
        tft.fillCircle(159,120,4,green);
        tft.fillCircle(146,107,4,green);
        tft.fillCircle(146,133,4,green);

        tft.fillCircle(101,229,4,green);
        tft.fillCircle(114,229,4,green);
        tft.fillCircle(127,229,4,green);
        tft.fillCircle(140,229,4,green);
        tft.fillCircle(120,222,4,green);
        tft.fillCircle(120,209,4,green);
        tft.fillCircle(120,196,4,green);
        tft.fillCircle(120,183,4,green);
        tft.fillCircle(120,170,4,green);
        tft.fillCircle(120,157,4,green);
        tft.fillCircle(133,144,4,green);
        tft.fillCircle(107,144,4,green);

//        tft.fillCircle(10,101,4,green);
        tft.fillCircle(10,114,4,green);
        tft.fillCircle(10,127,4,green);
        tft.fillCircle(10,140,4,green);
        tft.fillCircle(17,120,4,green);
        tft.fillCircle(30,120,4,green);
        tft.fillCircle(43,120,4,green);
        tft.fillCircle(56,120,4,green);
        tft.fillCircle(69,120,4,green);
        tft.fillCircle(82,120,4,green);
        tft.fillCircle(95,133,4,green);
        tft.fillCircle(95,107,4,green);
#endif
        calibration = false;
    }
    
    void loop(TP tp){
        uint16_t green = tft.color(0,150,0);
        uint16_t white = tft.color(255,255,255);
        if(!calibration)
            line.getValue(line_now);
        
#if defined(POSITION_OF)
        if((line_now.value[FRONT] & 0b1000000000) != (line_last.value[FRONT] & 0b1000000000))
            tft.fillCircle(101,36,4,(line_now.value[FRONT] & 0b1000000000) ? white : green);
        if((line_now.value[FRONT] & 0b0100000000) != (line_last.value[FRONT] & 0b0100000000))
            tft.fillCircle(114,36,4,(line_now.value[FRONT] & 0b0100000000) ? white : green);
        if((line_now.value[FRONT] & 0b0010000000) != (line_last.value[FRONT] & 0b0010000000))
            tft.fillCircle(127,36,4,(line_now.value[FRONT] & 0b0010000000) ? white : green);
        if((line_now.value[FRONT] & 0b0001000000) != (line_last.value[FRONT] & 0b0001000000))
            tft.fillCircle(140,36,4,(line_now.value[FRONT] & 0b0001000000) ? white : green);
        if((line_now.value[FRONT] & 0b0000100000) != (line_last.value[FRONT] & 0b0000100000))
            tft.fillCircle(120,43,4,(line_now.value[FRONT] & 0b0000100000) ? white : green);
        if((line_now.value[FRONT] & 0b0000010000) != (line_last.value[FRONT] & 0b0000010000))
            tft.fillCircle(120,56,4,(line_now.value[FRONT] & 0b0000010000) ? white : green);
        if((line_now.value[FRONT] & 0b0000001000) != (line_last.value[FRONT] & 0b0000001000))
            tft.fillCircle(120,69,4,(line_now.value[FRONT] & 0b0000001000) ? white : green);
        if((line_now.value[FRONT] & 0b0000000100) != (line_last.value[FRONT] & 0b0000000100))
            tft.fillCircle(120,82,4,(line_now.value[FRONT] & 0b0000000100) ? white : green);
        if((line_now.value[FRONT] & 0b0000000010) != (line_last.value[FRONT] & 0b0000000010))
            tft.fillCircle(107,95,4,(line_now.value[FRONT] & 0b0000000010) ? white : green);
        if((line_now.value[FRONT] & 0b0000000001) != (line_last.value[FRONT] & 0b0000000001))
            tft.fillCircle(133,95,4,(line_now.value[FRONT] & 0b0000000001) ? white : green);
        
        
        if((line_now.value[RIGHT] & 0b100000000000) != (line_last.value[RIGHT] & 0b100000000000))
            tft.fillCircle(231,101,4,(line_now.value[RIGHT] & 0b100000000000) ? white : green);
        if((line_now.value[RIGHT] & 0b010000000000) != (line_last.value[RIGHT] & 0b010000000000))
            tft.fillCircle(231,114,4,(line_now.value[RIGHT] & 0b010000000000) ? white : green);
        if((line_now.value[RIGHT] & 0b001000000000) != (line_last.value[RIGHT] & 0b001000000000))
            tft.fillCircle(231,127,4,(line_now.value[RIGHT] & 0b001000000000) ? white : green);
        if((line_now.value[RIGHT] & 0b000100000000) != (line_last.value[RIGHT] & 0b000100000000))
            tft.fillCircle(231,140,4,(line_now.value[RIGHT] & 0b000100000000) ? white : green);
        if((line_now.value[RIGHT] & 0b000010000000) != (line_last.value[RIGHT] & 0b000010000000))
            tft.fillCircle(224,120,4,(line_now.value[RIGHT] & 0b000010000000) ? white : green);
        if((line_now.value[RIGHT] & 0b000001000000) != (line_last.value[RIGHT] & 0b000001000000))
            tft.fillCircle(211,120,4,(line_now.value[RIGHT] & 0b000001000000) ? white : green);
        if((line_now.value[RIGHT] & 0b000000100000) != (line_last.value[RIGHT] & 0b000000100000))
            tft.fillCircle(198,120,4,(line_now.value[RIGHT] & 0b000000100000) ? white : green);
        if((line_now.value[RIGHT] & 0b000000010000) != (line_last.value[RIGHT] & 0b000000010000))
            tft.fillCircle(185,120,4,(line_now.value[RIGHT] & 0b000000010000) ? white : green);
        if((line_now.value[RIGHT] & 0b000000001000) != (line_last.value[RIGHT] & 0b000000001000))
            tft.fillCircle(172,120,4,(line_now.value[RIGHT] & 0b000000001000) ? white : green);
        if((line_now.value[RIGHT] & 0b000000000100) != (line_last.value[RIGHT] & 0b000000000100))
            tft.fillCircle(159,120,4,(line_now.value[RIGHT] & 0b000000000100) ? white : green);
        if((line_now.value[RIGHT] & 0b000000000010) != (line_last.value[RIGHT] & 0b000000000010))
            tft.fillCircle(146,107,4,(line_now.value[RIGHT] & 0b000000000010) ? white : green);
        if((line_now.value[RIGHT] & 0b000000000001) != (line_last.value[RIGHT] & 0b000000000001))
            tft.fillCircle(146,133,4,(line_now.value[RIGHT] & 0b000000000001) ? white : green);
        
        
        if((line_now.value[BACK] & 0b1000000000) != (line_last.value[BACK] & 0b1000000000))
            tft.fillCircle(140,203,4,(line_now.value[BACK] & 0b1000000000) ? white : green);
        if((line_now.value[BACK] & 0b0100000000) != (line_last.value[BACK] & 0b0100000000))
            tft.fillCircle(127,203,4,(line_now.value[BACK] & 0b0100000000) ? white : green);
        if((line_now.value[BACK] & 0b0010000000) != (line_last.value[BACK] & 0b0010000000))
            tft.fillCircle(114,203,4,(line_now.value[BACK] & 0b0010000000) ? white : green);
        if((line_now.value[BACK] & 0b0001000000) != (line_last.value[BACK] & 0b0001000000))
            tft.fillCircle(101,203,4,(line_now.value[BACK] & 0b0001000000) ? white : green);
        if((line_now.value[BACK] & 0b0000100000) != (line_last.value[BACK] & 0b0000100000))
            tft.fillCircle(120,196,4,(line_now.value[BACK] & 0b0000100000) ? white : green);
        if((line_now.value[BACK] & 0b0000010000) != (line_last.value[BACK] & 0b0000010000))
            tft.fillCircle(120,183,4,(line_now.value[BACK] & 0b0000010000) ? white : green);
        if((line_now.value[BACK] & 0b0000001000) != (line_last.value[BACK] & 0b0000001000))
            tft.fillCircle(120,170,4,(line_now.value[BACK] & 0b0000001000) ? white : green);
        if((line_now.value[BACK] & 0b0000000100) != (line_last.value[BACK] & 0b0000000100))
            tft.fillCircle(120,157,4,(line_now.value[BACK] & 0b0000000100) ? white : green);
        if((line_now.value[BACK] & 0b0000000010) != (line_last.value[BACK] & 0b0000000010))
            tft.fillCircle(133,144,4,(line_now.value[BACK] & 0b0000000010) ? white : green);
        if((line_now.value[BACK] & 0b0000000001) != (line_last.value[BACK] & 0b00000000001))
            tft.fillCircle(107,144,4,(line_now.value[BACK] & 0b0000000001) ? white : green);
        
        
        if((line_now.value[LEFT] & 0b100000000000) != (line_last.value[LEFT] & 0b100000000000))
            tft.fillCircle(10,140,4,(line_now.value[LEFT] & 0b100000000000) ? white : green);
        if((line_now.value[LEFT] & 0b010000000000) != (line_last.value[LEFT] & 0b010000000000))
            tft.fillCircle(10,127,4,(line_now.value[LEFT] & 0b010000000000) ? white : green);
        if((line_now.value[LEFT] & 0b001000000000) != (line_last.value[LEFT] & 0b001000000000))
            tft.fillCircle(10,114,4,(line_now.value[LEFT] & 0b001000000000) ? white : green);
        if((line_now.value[LEFT] & 0b000100000000) != (line_last.value[LEFT] & 0b000100000000))
            tft.fillCircle(10,101,4,(line_now.value[LEFT] & 0b000100000000) ? white : green);
        if((line_now.value[LEFT] & 0b000010000000) != (line_last.value[LEFT] & 0b000010000000))
            tft.fillCircle(17,120,4,(line_now.value[LEFT] & 0b000010000000) ? white : green);
        if((line_now.value[LEFT] & 0b000001000000) != (line_last.value[LEFT] & 0b000001000000))
            tft.fillCircle(30,120,4,(line_now.value[LEFT] & 0b000001000000) ? white : green);
        if((line_now.value[LEFT] & 0b000000100000) != (line_last.value[LEFT] & 0b000000100000))
            tft.fillCircle(43,120,4,(line_now.value[LEFT] & 0b000000100000) ? white : green);
        if((line_now.value[LEFT] & 0b000000010000) != (line_last.value[LEFT] & 0b000000010000))
            tft.fillCircle(56,120,4,(line_now.value[LEFT] & 0b000000010000) ? white : green);
        if((line_now.value[LEFT] & 0b000000001000) != (line_last.value[LEFT] & 0b000000001000))
            tft.fillCircle(69,120,4,(line_now.value[LEFT] & 0b000000001000) ? white : green);
        if((line_now.value[LEFT] & 0b000000000100) != (line_last.value[LEFT] & 0b000000000100))
            tft.fillCircle(82,120,4,(line_now.value[LEFT] & 0b000000000100) ? white : green);
        if((line_now.value[LEFT] & 0b000000000010) != (line_last.value[LEFT] & 0b000000000010))
            tft.fillCircle(95,133,4,(line_now.value[LEFT] & 0b000000000010) ? white : green);
        if((line_now.value[LEFT] & 0b000000000001) != (line_last.value[LEFT] & 0b000000000001))
            tft.fillCircle(95,107,4,(line_now.value[LEFT] & 0b000000000001) ? white : green);
#elif defined(POSITION_DF)
        if((line_now.value[FRONT] & 0b100000000) != (line_last.value[FRONT] & 0b100000000))
            tft.fillCircle(101,49,4,(line_now.value[FRONT] & 0b100000000) ? white : green);
        if((line_now.value[FRONT] & 0b010000000) != (line_last.value[FRONT] & 0b010000000))
            tft.fillCircle(114,49,4,(line_now.value[FRONT] & 0b010000000) ? white : green);
        if((line_now.value[FRONT] & 0b001000000) != (line_last.value[FRONT] & 0b001000000))
            tft.fillCircle(127,49,4,(line_now.value[FRONT] & 0b001000000) ? white : green);
        if((line_now.value[FRONT] & 0b000100000) != (line_last.value[FRONT] & 0b000100000))
            tft.fillCircle(140,49,4,(line_now.value[FRONT] & 0b000100000) ? white : green);
        if((line_now.value[FRONT] & 0b000010000) != (line_last.value[FRONT] & 0b000010000))
            tft.fillCircle(120,56,4,(line_now.value[FRONT] & 0b000010000) ? white : green);
        if((line_now.value[FRONT] & 0b000001000) != (line_last.value[FRONT] & 0b000001000))
            tft.fillCircle(120,69,4,(line_now.value[FRONT] & 0b000001000) ? white : green);
        if((line_now.value[FRONT] & 0b000000100) != (line_last.value[FRONT] & 0b000000100))
            tft.fillCircle(120,82,4,(line_now.value[FRONT] & 0b000000100) ? white : green);
        if((line_now.value[FRONT] & 0b000000010) != (line_last.value[FRONT] & 0b000000010))
            tft.fillCircle(107,95,4,(line_now.value[FRONT] & 0b000000010) ? white : green);
        if((line_now.value[FRONT] & 0b000000001) != (line_last.value[FRONT] & 0b000000001))
            tft.fillCircle(133,95,4,(line_now.value[FRONT] & 0b000000001) ? white : green);


//        if((line_now.value[RIGHT] & 0b100000000000) != (line_last.value[RIGHT] & 0b100000000000))
//            tft.fillCircle(231,101,4,(line_now.value[RIGHT] & 0b100000000000) ? white : green);
        if((line_now.value[RIGHT] & 0b010000000000) != (line_last.value[RIGHT] & 0b010000000000))
            tft.fillCircle(231,114,4,(line_now.value[RIGHT] & 0b010000000000) ? white : green);
        if((line_now.value[RIGHT] & 0b001000000000) != (line_last.value[RIGHT] & 0b001000000000))
            tft.fillCircle(231,127,4,(line_now.value[RIGHT] & 0b001000000000) ? white : green);
        if((line_now.value[RIGHT] & 0b000100000000) != (line_last.value[RIGHT] & 0b000100000000))
            tft.fillCircle(231,140,4,(line_now.value[RIGHT] & 0b000100000000) ? white : green);
        if((line_now.value[RIGHT] & 0b000010000000) != (line_last.value[RIGHT] & 0b000010000000))
            tft.fillCircle(224,120,4,(line_now.value[RIGHT] & 0b000010000000) ? white : green);
        if((line_now.value[RIGHT] & 0b000001000000) != (line_last.value[RIGHT] & 0b000001000000))
            tft.fillCircle(211,120,4,(line_now.value[RIGHT] & 0b000001000000) ? white : green);
        if((line_now.value[RIGHT] & 0b000000100000) != (line_last.value[RIGHT] & 0b000000100000))
            tft.fillCircle(198,120,4,(line_now.value[RIGHT] & 0b000000100000) ? white : green);
        if((line_now.value[RIGHT] & 0b000000010000) != (line_last.value[RIGHT] & 0b000000010000))
            tft.fillCircle(185,120,4,(line_now.value[RIGHT] & 0b000000010000) ? white : green);
        if((line_now.value[RIGHT] & 0b000000001000) != (line_last.value[RIGHT] & 0b000000001000))
            tft.fillCircle(172,120,4,(line_now.value[RIGHT] & 0b000000001000) ? white : green);
        if((line_now.value[RIGHT] & 0b000000000100) != (line_last.value[RIGHT] & 0b000000000100))
            tft.fillCircle(159,120,4,(line_now.value[RIGHT] & 0b000000000100) ? white : green);
        if((line_now.value[RIGHT] & 0b000000000010) != (line_last.value[RIGHT] & 0b000000000010))
            tft.fillCircle(146,107,4,(line_now.value[RIGHT] & 0b000000000010) ? white : green);
        if((line_now.value[RIGHT] & 0b000000000001) != (line_last.value[RIGHT] & 0b000000000001))
            tft.fillCircle(146,133,4,(line_now.value[RIGHT] & 0b000000000001) ? white : green);


        if((line_now.value[BACK] & 0b100000000000) != (line_last.value[BACK] & 0b100000000000))
            tft.fillCircle(140,229,4,(line_now.value[BACK] & 0b100000000000) ? white : green);
        if((line_now.value[BACK] & 0b010000000000) != (line_last.value[BACK] & 0b010000000000))
            tft.fillCircle(127,229,4,(line_now.value[BACK] & 0b010000000000) ? white : green);
        if((line_now.value[BACK] & 0b001000000000) != (line_last.value[BACK] & 0b001000000000))
            tft.fillCircle(114,229,4,(line_now.value[BACK] & 0b001000000000) ? white : green);
        if((line_now.value[BACK] & 0b000100000000) != (line_last.value[BACK] & 0b000100000000))
            tft.fillCircle(101,229,4,(line_now.value[BACK] & 0b000100000000) ? white : green);
        if((line_now.value[BACK] & 0b000010000000) != (line_last.value[BACK] & 0b000010000000))
            tft.fillCircle(120,222,4,(line_now.value[BACK] & 0b000010000000) ? white : green);
        if((line_now.value[BACK] & 0b000001000000) != (line_last.value[BACK] & 0b000001000000))
            tft.fillCircle(120,209,4,(line_now.value[BACK] & 0b000001000000) ? white : green);
        if((line_now.value[BACK] & 0b000000100000) != (line_last.value[BACK] & 0b000000100000))
            tft.fillCircle(120,196,4,(line_now.value[BACK] & 0b000000100000) ? white : green);
        if((line_now.value[BACK] & 0b000000010000) != (line_last.value[BACK] & 0b000000010000))
            tft.fillCircle(120,183,4,(line_now.value[BACK] & 0b000000010000) ? white : green);
        if((line_now.value[BACK] & 0b000000001000) != (line_last.value[BACK] & 0b000000001000))
            tft.fillCircle(120,170,4,(line_now.value[BACK] & 0b000000001000) ? white : green);
        if((line_now.value[BACK] & 0b000000000100) != (line_last.value[BACK] & 0b000000000100))
            tft.fillCircle(120,157,4,(line_now.value[BACK] & 0b000000000100) ? white : green);
        if((line_now.value[BACK] & 0b000000000010) != (line_last.value[BACK] & 0b000000000010))
            tft.fillCircle(133,144,4,(line_now.value[BACK] & 0b000000000010) ? white : green);
        if((line_now.value[BACK] & 0b000000000001) != (line_last.value[BACK] & 0b000000000001))
            tft.fillCircle(107,144,4,(line_now.value[BACK] & 0b000000000001) ? white : green);


        if((line_now.value[LEFT] & 0b100000000000) != (line_last.value[LEFT] & 0b100000000000))
            tft.fillCircle(10,140,4,(line_now.value[LEFT] & 0b100000000000) ? white : green);
        if((line_now.value[LEFT] & 0b010000000000) != (line_last.value[LEFT] & 0b010000000000))
            tft.fillCircle(10,127,4,(line_now.value[LEFT] & 0b010000000000) ? white : green);
        if((line_now.value[LEFT] & 0b001000000000) != (line_last.value[LEFT] & 0b001000000000))
            tft.fillCircle(10,114,4,(line_now.value[LEFT] & 0b001000000000) ? white : green);
//        if((line_now.value[LEFT] & 0b000100000000) != (line_last.value[LEFT] & 0b000100000000))
//            tft.fillCircle(10,101,4,(line_now.value[LEFT] & 0b000100000000) ? white : green);
        if((line_now.value[LEFT] & 0b000010000000) != (line_last.value[LEFT] & 0b000010000000))
            tft.fillCircle(17,120,4,(line_now.value[LEFT] & 0b000010000000) ? white : green);
        if((line_now.value[LEFT] & 0b000001000000) != (line_last.value[LEFT] & 0b000001000000))
            tft.fillCircle(30,120,4,(line_now.value[LEFT] & 0b000001000000) ? white : green);
        if((line_now.value[LEFT] & 0b000000100000) != (line_last.value[LEFT] & 0b000000100000))
            tft.fillCircle(43,120,4,(line_now.value[LEFT] & 0b000000100000) ? white : green);
        if((line_now.value[LEFT] & 0b000000010000) != (line_last.value[LEFT] & 0b000000010000))
            tft.fillCircle(56,120,4,(line_now.value[LEFT] & 0b000000010000) ? white : green);
        if((line_now.value[LEFT] & 0b000000001000) != (line_last.value[LEFT] & 0b000000001000))
            tft.fillCircle(69,120,4,(line_now.value[LEFT] & 0b000000001000) ? white : green);
        if((line_now.value[LEFT] & 0b000000000100) != (line_last.value[LEFT] & 0b000000000100))
            tft.fillCircle(82,120,4,(line_now.value[LEFT] & 0b000000000100) ? white : green);
        if((line_now.value[LEFT] & 0b000000000010) != (line_last.value[LEFT] & 0b000000000010))
            tft.fillCircle(95,133,4,(line_now.value[LEFT] & 0b000000000010) ? white : green);
        if((line_now.value[LEFT] & 0b000000000001) != (line_last.value[LEFT] & 0b000000000001))
            tft.fillCircle(95,107,4,(line_now.value[LEFT] & 0b000000000001) ? white : green);
#endif
        
        line_last = line_now;
        
        if(!tp.now.touches && tp.last.touches){
            if(range(245, tp.last.x, 315) && range(185, tp.last.y, 235)){   //calibration
                if(!calibration){
                    tft.fillRoundRect(245,185,70,50,10,tft.color(60,60,60));
                    tft.drawRoundRect(245,185,70,50,10,tft.color(0,255,0));
                    tft.setFont(Arial_14, tft.Center, 1);
                    tft.print("Calibration", 11, 280, 198, tft.color(0,255,0));
                    tft.print("Stop", 4, 280, 212, tft.color(0,255,0));
                    line.calibrationStart();
                    calibration = !calibration;
                }
                else{
                    tft.fillRoundRect(245,185,70,50,10,tft.color(0,200,0));
                    tft.setFont(Arial_14, tft.Center, 1);
                    tft.print("Calibration", 11, 280, 198, tft.color(60,60,60));
                    tft.print("Start", 5, 280, 212, tft.color(60,60,60));
                    line.calibrationStop();
                    calibration = !calibration;
                }
            }
        }
    }
};

#endif /* LINE_App_h */
