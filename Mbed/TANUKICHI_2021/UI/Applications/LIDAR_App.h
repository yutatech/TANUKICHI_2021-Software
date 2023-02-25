/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef LIDAR_App_h
#define LIDAR_App_h

#include "ApplicationFramework.h"
#include "../bitmaps/LIDAR_icon.h"

class LIDAR_App : public ApplicationFramework {
private:
    Distance &distance;
    Lidar_S data, lastData;
    void drawr(int x, int y, uint16_t color){
        tft.drawPixel(x,y+2,color);
        tft.drawPixel(x+1,y+3,color);
        tft.drawLine(x+2,y+3,x+2,y+10,color);
        tft.drawPixel(x+3,y+2,color);
        tft.drawLine(x+4,y+1,x+5,y+1,color);
    }
public:
    int backColor = tft.color(0,0,0);
    LIDAR_App(ILI9341 &_tft, Distance &_distance) : ApplicationFramework(_tft, LIDAR_icon), distance(_distance) {
        strcpy(AppName, "LiDAR");
        AppNameLen = 5;
    }
    
    void begin(){
        distance.getValue(data);
        tft.fillRect(0,0,320,240,backColor);
        tft.drawLine(160,215,160,225, tft.color(255,255,255));
        tft.drawLine(155,220,165,220, tft.color(255,255,255));
        
        tft.setFont(Arial_14, tft.Center, 2);
        tft.print(data.value[0], 60, 10, tft.color(255,255,255));
        tft.print(data.value[1], 110, 10, tft.color(255,255,255));
        tft.print(data.value[2], 160, 10, tft.color(255,255,255));
        tft.print(data.value[3], 210, 10, tft.color(255,255,255));
        tft.print(data.value[4], 260, 10, tft.color(255,255,255));
        
        tft.fillCircle(160 - data.value[0]*0.64*0.25, 220 - data.value[0]*0.77*0.25, 5, tft.color(255,255,255));
        tft.fillCircle(160 - data.value[1]*0.34*0.25, 220 - data.value[1]*0.94*0.25, 5, tft.color(255,255,255));
        tft.fillCircle(160 , 220 - data.value[2]*0.25, 5, tft.color(255,255,255));
        tft.fillCircle(160 + data.value[3]*0.34*0.25, 220 - data.value[3]*0.94*0.25, 5, tft.color(255,255,255));
        tft.fillCircle(160 + data.value[4]*0.64*0.25, 220 - data.value[4]*0.77*0.25, 5, tft.color(255,255,255));
        
        tft.fillCircle(160 - data.threshould*0.64*0.25, 220 - data.threshould*0.77*0.25, 3, tft.color(150,150,150));
        tft.fillCircle(160 - data.threshould*0.34*0.25, 220 - data.threshould*0.94*0.25, 3, tft.color(150,150,150));
        tft.fillCircle(160 , 220 - data.threshould*0.25, 3, tft.color(150,150,150));
        tft.fillCircle(160 + data.threshould*0.34*0.25, 220 - data.threshould*0.94*0.25, 3, tft.color(150,150,150));
        tft.fillCircle(160 + data.threshould*0.64*0.25, 220 - data.threshould*0.77*0.25, 3, tft.color(150,150,150));
        
        tft.print(data.value[3], 210, 10, tft.color(255,255,255));
        tft.print(data.value[4], 260, 10, tft.color(255,255,255));
        
        tft.setFont(Arial_14, tft.Left, 2);
        tft.print("L =", 3, 264, 173, tft.color(255,255,255));
        tft.print("C =", 3, 263, 187, tft.color(255,255,255));
        tft.print("R =", 3, 263, 201, tft.color(255,255,255));
        drawr(265,215,tft.color(255,255,255));
        tft.print("=", 1, 275, 215, tft.color(255,255,255));
        if(data.angleC == None){
            tft.print("None", 4, 286, 173, tft.color(255,255,255));
            tft.print("None", 4, 286, 187, tft.color(255,255,255));
            tft.print("None", 4, 286, 201, tft.color(255,255,255));
            tft.print("None", 4, 286, 215, tft.color(255,255,255));
        }
        else{
            tft.print(data.angleL, 286, 173, tft.color(255,255,255));
            tft.print(data.angleC, 286, 187, tft.color(255,255,255));
            tft.print(data.angleR, 286, 201, tft.color(255,255,255));
            tft.print(data.distance, 286, 215, tft.color(255,255,255));
        }
        lastData = data;
    }
    
    void loop(TP tp){
        distance.getValue(data);
        tft.setFont(Arial_14, tft.Center, 2);
        if(lastData.threshould != data.threshould){
            tft.fillCircle(160 - lastData.threshould*0.64*0.25, 220 - lastData.threshould*0.77*0.25, 3, tft.color(0,0,0));
            tft.fillCircle(160 - lastData.threshould*0.34*0.25, 220 - lastData.threshould*0.94*0.25, 3, tft.color(0,0,0));
            tft.fillCircle(160 , 220 - lastData.threshould*0.25, 3, tft.color(0,0,0));
            tft.fillCircle(160 + lastData.threshould*0.34*0.25, 220 - lastData.threshould*0.94*0.25, 3, tft.color(0,0,0));
            tft.fillCircle(160 + lastData.threshould*0.64*0.25, 220 - lastData.threshould*0.77*0.25, 5, tft.color(0,0,0));
            
            tft.fillCircle(160 - data.threshould*0.64*0.25, 220 - data.threshould*0.77*0.25, 3, tft.color(150,150,150));
            tft.fillCircle(160 - data.threshould*0.34*0.25, 220 - data.threshould*0.94*0.25, 3, tft.color(150,150,150));
            tft.fillCircle(160 , 220 - data.threshould*0.25, 3, tft.color(150,150,150));
            tft.fillCircle(160 + data.threshould*0.34*0.25, 220 - data.threshould*0.94*0.25, 3, tft.color(150,150,150));
            tft.fillCircle(160 + data.threshould*0.64*0.25, 220 - data.threshould*0.77*0.25, 3, tft.color(150,150,150));
        }
        if(lastData.value[0] != data.value[0]){
            tft.print(lastData.value[0], 60, 10, tft.color(0,0,0));
            tft.print(data.value[0], 60, 10, tft.color(255,255,255));
            tft.fillCircle(160 - lastData.value[0]*0.64*0.25, 220 - lastData.value[0]*0.77*0.25, 5, tft.color(0,0,0));
        }
        if(lastData.value[1] != data.value[1]){
            tft.print(lastData.value[1], 110, 10, tft.color(0,0,0));
            tft.print(data.value[1], 110, 10, tft.color(255,255,255));
            tft.fillCircle(160 - lastData.value[1]*0.34*0.25, 220 - lastData.value[1]*0.94*0.25, 5, tft.color(0,0,0));
        }
        if(lastData.value[2] != data.value[2]){
            tft.print(lastData.value[2], 160, 10, tft.color(0,0,0));
            tft.print(data.value[2], 160, 10, tft.color(255,255,255));
            tft.fillCircle(160, 220 - lastData.value[2]*0.25, 5, tft.color(0,0,0));
        }
        if(lastData.value[3] != data.value[3]){
            tft.print(lastData.value[3], 210, 10, tft.color(0,0,0));
            tft.print(data.value[3], 210, 10, tft.color(255,255,255));
            tft.fillCircle(160 + lastData.value[3]*0.34*0.25, 220 - lastData.value[3]*0.94*0.25, 5, tft.color(0,0,0));
        }
        if(lastData.value[4] != data.value[4]){
            tft.print(lastData.value[4], 260, 10, tft.color(0,0,0));
            tft.print(data.value[4], 260, 10, tft.color(255,255,255));
            tft.fillCircle(160 + lastData.value[4]*0.64*0.25, 220 - lastData.value[4]*0.77*0.25, 5, tft.color(0,0,0));
        }
        
        tft.fillCircle(160 - data.value[0]*0.64*0.25, 220 - data.value[0]*0.77*0.25, 5, data.value[0]<data.threshould ? tft.color(255,0,0) : tft.color(255,255,255));
        tft.fillCircle(160 - data.value[1]*0.34*0.25, 220 - data.value[1]*0.94*0.25, 5, data.value[1]<data.threshould ? tft.color(255,0,0) : tft.color(255,255,255));
        tft.fillCircle(160 , 220 - data.value[2]*0.25, 5, data.value[2]<data.threshould ? tft.color(255,0,0) : tft.color(255,255,255));
        tft.fillCircle(160 + data.value[3]*0.34*0.25, 220 - data.value[3]*0.94*0.25, 5, data.value[3]<data.threshould ? tft.color(255,0,0) : tft.color(255,255,255));
        tft.fillCircle(160 + data.value[4]*0.64*0.25, 220 - data.value[4]*0.77*0.25, 5, data.value[4]<data.threshould ? tft.color(255,0,0) : tft.color(255,255,255));
        
        tft.drawLine(160,215,160,225, tft.color(255,255,255));
        tft.drawLine(155,220,165,220, tft.color(255,255,255));
        
        
        tft.setFont(Arial_14, tft.Left, 2);
        if(lastData.angleL != data.angleL){
            if(lastData.angleL == None)
                tft.print("None", 4, 286, 173, tft.color(0,0,0));
            else
                tft.print(lastData.angleL, 286, 173, tft.color(0,0,0));
            
            if(data.angleL == None)
                tft.print("None", 4, 286, 173, tft.color(255,255,255));
            else
                tft.print(data.angleL, 286, 173, tft.color(255,255,255));
        }
        if(lastData.angleC != data.angleC){
            if(lastData.angleC == None)
                tft.print("None", 4, 286, 187, tft.color(0,0,0));
            else
                tft.print(lastData.angleC, 286, 187, tft.color(0,0,0));
            
            if(data.angleC == None)
                tft.print("None", 4, 286, 187, tft.color(255,255,255));
            else
                tft.print(data.angleC, 286, 187, tft.color(255,255,255));
        }
        if(lastData.angleR != data.angleR){
            if(lastData.angleR == None)
                tft.print("None", 4, 286, 201, tft.color(0,0,0));
            else
                tft.print(lastData.angleR, 286, 201, tft.color(0,0,0));
            
            if(data.angleR == None)
                tft.print("None", 4, 286, 201, tft.color(255,255,255));
            else
                tft.print(data.angleR, 286, 201, tft.color(255,255,255));
        }
        if(lastData.distance != data.distance){
            if(lastData.distance == None)
                tft.print("None", 4, 286, 215, tft.color(0,0,0));
            else
                tft.print(lastData.distance, 286, 215, tft.color(0,0,0));
            
            if(data.distance == None)
                tft.print("None", 4, 286, 215, tft.color(255,255,255));
            else
                tft.print(data.distance, 286, 215, tft.color(255,255,255));
        }
        lastData = data;
    }
};


#endif /* LIDAR_App_h */
