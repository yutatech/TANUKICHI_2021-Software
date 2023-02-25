/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef ApplicationFramework_h
#define ApplicationFramework_h

#include "../../Robot.h"
#include "../../Shared.h"
#include "../../libraries/Math.h"
#include "../../libraries/ILI9341.h"
#include "../../libraries/FT6206.h"
#include "../../ISP.h"

#include "../bitmaps/BackGround.h"

#define OPEN_App 0
#define CANCEL_App 1
#define OTHER_App 2

typedef struct {
    uint16_t x,y;
    bool touches;
} XY;

typedef struct {
    XY now;
    XY last;
    XY first;
} TP;

typedef struct {
    int x;
    int y;
} Icon;

typedef struct {
    int x;
    int y;
    int width;
    int hight;
} Rect;

class ApplicationFramework {
protected:
    ILI9341 &tft;
    int backColor = tft.color(0,0,0);
    uint16_t *icondata;
    char AppName[9] = "App";
    int AppNameLen = 3;
    
    Rect popupRect;
public:
    Icon iconXY;
    bool isApp = true;
    
    ApplicationFramework(ILI9341 &_tft, uint16_t *_icondata) : tft(_tft), icondata(_icondata) {}
    
    virtual void drawIcon(int x, int y){
        uint16_t icon[2500];
        iconXY.x = popupRect.x = x;
        iconXY.y = popupRect.y = y;
        for(int i = 0; i < 2500; i++){
            icon[i] = icondata[i];
            if(icon[i] == 0){
                icon[i] = BackGround[(x + i / 50) * 240 + 190 - y
                                     + i % 50];
            }
        }
        tft.drawimg(icon, x, y, 50, 50);
        if(AppNameLen < 7)
            tft.setFont(Arial_14, tft.Center, 3);
        else
            tft.setFont(Arial_14, tft.Center, 1);
        tft.print(AppName, AppNameLen, x+25, y+55, tft.color(255,255,255));
    }
    
    virtual void begin(){
        tft.fillRect(0,0,320,240,backColor);
    }
    
    virtual void drawPopup(){
        popupRect.width = 200;
        popupRect.hight = 100;
        if(popupRect.x < 15)
            popupRect.x = 15;
        else if(popupRect.x + popupRect.width - 1 > 304){
            popupRect.x = 305 - popupRect.width;
            if(popupRect.x < 15)
                popupRect.x = 15;
        }
        if(popupRect.y < 19)
            popupRect.y = 19;
        else if(popupRect.y + popupRect.hight - 1 > 217){
            popupRect.y = 218 - popupRect.hight;
            if(popupRect.y < 19)
                popupRect.y = 19;
        }
        tft.drawPopup(popupRect.x, popupRect.y, popupRect.width, popupRect.hight, iconXY.x + 25, iconXY.y + 25);
        tft.drawLine(popupRect.x + 2, popupRect.y + 50, popupRect.x + 197, popupRect.y + 50 , tft.color(100,100,100));
        
        tft.setFont(Arial_bold_14, tft.Center, 3);
        tft.print("Open", 4, popupRect.x+100, popupRect.y+18, tft.color(0,0,0));
        tft.print("Cancel", 6, popupRect.x+100, popupRect.y+68, tft.color(0,0,0));
    }
    
    virtual void loop(TP tp){
        if(!tp.now.touches && tp.last.touches){
        }
    }
    
    virtual int loop_popup(TP tp){
        if(!tp.now.touches && tp.last.touches){
            if(range(popupRect.x, tp.last.x, popupRect.x + 200) && range(popupRect.y, tp.last.y, popupRect.y + 50)){    //Open
                return OPEN_App;
            }
            else{   //Cancel
                return CANCEL_App;
            }
        }
        return OTHER_App;
    }
    
    virtual void erase_popup(){
        tft.cutImage(BackGround,popupRect.x,popupRect.y,popupRect.x,popupRect.y,popupRect.width,popupRect.hight);
    }
};

#endif /* ApplicationFramework_h */
