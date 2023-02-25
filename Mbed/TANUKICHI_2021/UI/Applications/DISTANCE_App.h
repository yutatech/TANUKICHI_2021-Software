/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef DISTANCE_App_h
#define DISTANCE_App_h

#include "ApplicationFramework.h"
#include "../bitmaps/DISTANCE_icon.h"

class DISTANCE_App : public ApplicationFramework {
private:
    Distance &distance;
    Distance_S data, lastData;
    
public:
    int backColor = tft.color(0,0,0);
    DISTANCE_App(ILI9341 &_tft, Distance &_distance) : distance(_distance), ApplicationFramework(_tft, DISTANCE_icon) {
            strcpy(AppName, "Distance");
            AppNameLen = 8;
    }
    
    void begin(){
        tft.fillRect(0,0,320,240,backColor);
        distance.getValue(lastData);
        
#if defined(POSITION_OF)
        tft.setFont(Arial_bold_14, tft.Center, 3);
        tft.print(lastData.valueF[0], 30, 60, tft.color(255,255,255));
        tft.print(lastData.valueF[1], 90, 28, tft.color(255,255,255));
        tft.print(lastData.valueF[2], 160, 20, tft.color(255,255,255));
        tft.print(lastData.valueF[3], 230, 28, tft.color(255,255,255));
        tft.print(lastData.valueF[4], 290, 60, tft.color(255,255,255));
        
        int yF = constrain(120 - lastData.value[FRONT] * 0.6, 45, 120);
        tft.drawLine(160, 120, 160, yF, tft.color(255,255,255));
        tft.drawLine(155, yF, 165, yF, tft.color(255,255,255));
        tft.setFont(Arial_bold_14, tft.Right, 3);
        tft.print(lastData.value[FRONT], 151, constrain(yF + 5, 50, 100), tft.color(255,255,255));
#endif
        
        int xR = constrain(165 + lastData.value[RIGHT] * 0.7, 170, 275);
        tft.drawLine(160, 120, xR, 120, tft.color(255,255,255));
        tft.drawLine(xR, 115, xR, 125, tft.color(255,255,255));
        tft.setFont(Arial_bold_14, tft.Right, 3);
        tft.print(lastData.value[RIGHT], constrain(xR - 5, 180, 320), 100, tft.color(255,255,255));
        
        int yB = constrain(125 + lastData.value[BACK] * 0.2, 130, 235);
        tft.drawLine(160, 120, 160, yB, tft.color(255,255,255));
        tft.drawLine(155, yB, 165, yB, tft.color(255,255,255));
        tft.setFont(Arial_bold_14, tft.Left, 3);
        tft.print(lastData.value[BACK], 169, constrain(yB - 19, 130, 240), tft.color(255,255,255));
        
        int xL = constrain(155 - lastData.value[LEFT] * 0.7, 45, 155);
        tft.drawLine(160, 120, xL, 120, tft.color(255,255,255));
        tft.drawLine(xL, 115, xL, 125, tft.color(255,255,255));
        tft.setFont(Arial_bold_14, tft.Left, 3);
        tft.print(lastData.value[LEFT], constrain(xL + 5, 0, 140), 129, tft.color(255,255,255));
    }
    
    void loop(TP tp){
        distance.getValue(data);
        
#if defined(POSITION_OF)
        tft.setFont(Arial_bold_14, tft.Center, 3);
        if(data.valueF[0] != lastData.valueF[0]){
            tft.print(lastData.valueF[0], 30, 60, tft.color(0,0,0));
            tft.print(data.valueF[0], 30, 60, tft.color(255,255,255));
        }
        if(data.valueF[1] != lastData.valueF[1]){
            tft.print(lastData.valueF[1], 90, 28, tft.color(0,0,0));
            tft.print(data.valueF[1], 90, 28, tft.color(255,255,255));
        }
        if(data.valueF[2] != lastData.valueF[2]){
            tft.print(lastData.valueF[2], 160, 20, tft.color(0,0,0));
            tft.print(data.valueF[2], 160, 20, tft.color(255,255,255));
        }
        if(data.valueF[3] != lastData.valueF[3]){
            tft.print(lastData.valueF[3], 230, 28, tft.color(0,0,0));
            tft.print(data.valueF[3], 230, 28, tft.color(255,255,255));
        }
        if(data.valueF[4] != lastData.valueF[4]){
            tft.print(lastData.valueF[4], 290, 60, tft.color(0,0,0));
            tft.print(data.valueF[4], 290, 60, tft.color(255,255,255));
        }
        
        if(data.value[FRONT] != lastData.value[FRONT]){
            int lyF = constrain(120 - lastData.value[FRONT] * 0.6, 45, 120);
            int yF = constrain(120 - data.value[FRONT] * 0.6, 45, 120);
            tft.drawLine(160, yF - 1, 160, lyF, tft.color(0,0,0));
            tft.drawLine(155, lyF, 165, lyF, tft.color(0,0,0));
            tft.setFont(Arial_bold_14, tft.Right, 3);
            tft.print(lastData.value[FRONT], 151, constrain(lyF + 5, 50, 100), tft.color(0,0,0));
            
            tft.drawLine(160, 120, 160, yF, tft.color(255,255,255));
            tft.drawLine(155, yF, 165, yF, tft.color(255,255,255));
            tft.setFont(Arial_bold_14, tft.Right, 3);
            tft.print(data.value[FRONT], 151, constrain(yF + 5, 50, 100), tft.color(255,255,255));
        }
#endif
        
        if(data.value[RIGHT] != lastData.value[RIGHT]){
            int lxR = constrain(165 + lastData.value[RIGHT] * 0.7, 170, 275);
            int xR = constrain(165 + data.value[RIGHT] * 0.7, 170, 275);
            tft.drawLine(xR + 1, 120, lxR, 120, tft.color(0,0,0));
            tft.drawLine(lxR, 115, lxR, 125, tft.color(0,0,0));
            tft.setFont(Arial_bold_14, tft.Right, 3);
            tft.print(lastData.value[RIGHT], constrain(lxR - 5, 180, 320), 100, tft.color(0,0,0));
            
            tft.drawLine(160, 120, xR, 120, tft.color(255,255,255));
            tft.drawLine(xR, 115, xR, 125, tft.color(255,255,255));
            tft.setFont(Arial_bold_14, tft.Right, 3);
            tft.print(data.value[RIGHT], constrain(xR - 5, 180, 320), 100, tft.color(255,255,255));
        }
        
        if(data.value[BACK] != lastData.value[BACK]){
            int lyB = constrain(125 + lastData.value[BACK] * 0.2, 130, 235);
            int yB = constrain(125 + data.value[BACK] * 0.2, 130, 235);
            tft.drawLine(160, yB + 1, 160, lyB, tft.color(0,0,0));
            tft.drawLine(155, lyB, 165, lyB, tft.color(0,0,0));
            tft.setFont(Arial_bold_14, tft.Left, 3);
            tft.print(lastData.value[BACK], 169, constrain(lyB - 19, 130, 240), tft.color(0,0,0));
            
            tft.drawLine(160, 120, 160, yB, tft.color(255,255,255));
            tft.drawLine(155, yB, 165, yB, tft.color(255,255,255));
            tft.setFont(Arial_bold_14, tft.Left, 3);
            tft.print(data.value[BACK], 169, constrain(yB - 19, 130, 240), tft.color(255,255,255));
        }
        
        if(data.value[LEFT] != lastData.value[LEFT]){
            int lxL = constrain(155 - lastData.value[LEFT] * 0.7, 45, 155);
            int xL = constrain(155 - data.value[LEFT] * 0.7, 45, 155);
            tft.drawLine(xL - 1, 120, lxL, 120, tft.color(0,0,0));
            tft.drawLine(lxL, 115, lxL, 125, tft.color(0,0,0));
            tft.setFont(Arial_bold_14, tft.Left, 3);
            tft.print(lastData.value[LEFT], constrain(lxL + 5, 0, 140), 129, tft.color(0,0,0));
            
            tft.drawLine(160, 120, xL, 120, tft.color(255,255,255));
            tft.drawLine(xL, 115, xL, 125, tft.color(255,255,255));
            tft.setFont(Arial_bold_14, tft.Left, 3);
            tft.print(data.value[LEFT], constrain(xL + 5, 0, 140), 129, tft.color(255,255,255));
        }
        lastData = data;
        
        if(!tp.now.touches && tp.last.touches){
        }
    }
};

#endif /* DISTANCE_App_h */
