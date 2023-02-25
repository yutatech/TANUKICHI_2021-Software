/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef CAM_App_h
#define CAM_App_h

#include "ApplicationFramework.h"
#include "../bitmaps/CAM_icon.h"

class CAM_App : public ApplicationFramework {
private:
    Cam &cam;
    EEPROM &eeprom;
    Settings &settings;
    Cam_S data, lastData;
    char &opponentGoal;
    bool detectGoal;
    void drawTheta(int x, int y, uint16_t color){
        tft.drawLine(x+1,y+1,x+4,y+1,color);
        tft.drawLine(x+1,y+10,x+4,y+10,color);
        tft.drawLine(x,y+2,x,y+9,color);
        tft.drawLine(x+5,y+2,x+5,y+9,color);
        tft.drawLine(x+1,y+6,x+2,y+6,color);
        tft.drawLine(x+3,y+5,x+4,y+5,color);
    }
    void drawr(int x, int y, uint16_t color){
        tft.drawPixel(x,y+2,color);
        tft.drawPixel(x+1,y+3,color);
        tft.drawLine(x+2,y+3,x+2,y+10,color);
        tft.drawPixel(x+3,y+2,color);
        tft.drawLine(x+4,y+1,x+5,y+1,color);
    }
    
public:
    int backColor = tft.color(0,0,0);
    CAM_App(ILI9341 &_tft, Cam &_cam, EEPROM &_eeprom, Settings &_settings) : cam(_cam), eeprom(_eeprom), settings( _settings), opponentGoal(settings.data[settings.addOGoal]), ApplicationFramework(_tft, CAM_icon) {
        strcpy(AppName, "Cam");
        AppNameLen = 3;
    }
    
    void drawIcon(int x, int y){
        uint16_t icon[2500];
        iconXY.x = popupRect.x = x;
        iconXY.y = popupRect.y = y;
        for(int i = 0; i < 2500; i++){
            icon[i] = CAM_icon[i];
            if(icon[i] == 0){
                icon[i] = BackGround[(x + i / 50) * 240 + 190 - y
                                     + i % 50];
            }
#if defined(POSITION_OF)
            else if(icon[i] == 0xFFE0 && opponentGoal){
                icon[i] = 0x001F;
            }
            else if(icon[i] == 0x001F && opponentGoal){
                icon[i] = 0xFFE0;
            }
#elif defined(POSITION_DF)
            else if(icon[i] == 0xFFE0 && !opponentGoal){
                icon[i] = 0x001F;
            }
            else if(icon[i] == 0x001F && !opponentGoal){
                icon[i] = 0xFFE0;
            }
#endif
            
        }
        tft.drawimg(icon, x, y, 50, 50);
        tft.setFont(Arial_14, tft.Center, 3);
        tft.print(AppName, AppNameLen, x+25, y+55, tft.color(255,255,255));
        cam.getValue(lastData);
        if(cam.impossible)
            tft.drawRoundRect(x,y+1,50,50,10,tft.color(255,0,0));
    }
    
    void begin(){
        detectGoal = opponentGoal;
        cam.getValue(lastData);
        tft.setFont(Arial_14, tft.Left, 2);
        tft.fillRect(0,0,240,240,backColor);
        tft.fillRect(240,0,80,240,tft.color(60,60,60));
        
        drawTheta(265,50,tft.color(255,100,0));
        tft.print("=", 1, 275, 50, tft.color(255,100,0));
        drawr(265,64,tft.color(255,100,0));
        tft.print("=", 1, 275, 64, tft.color(255,100,0));
        if(lastData.bAngle == None){
            tft.print("None", 4, 286, 50, tft.color(255,100,0));
            tft.print("None", 4, 286, 64, tft.color(255,100,0));
        }
        else{
            tft.print(lastData.bAngle, 286, 50, tft.color(255,100,0));
            tft.print(lastData.bDistance, 286, 64, tft.color(255,100,0));
            tft.fillCircle(120 + sin(lastData.bAngle) * (lastData.bDistance/10) * 1, 120 - cos(lastData.bAngle) * (lastData.bDistance/10) * 1, 10, tft.color(0,0,0));
        }
        
        uint16_t goalColor = detectGoal ? tft.color(0,0,255) : tft.color(255,255,0);
        tft.print("L =", 3, 264, 86, goalColor);
        tft.print("C =", 3, 263, 100, goalColor);
        tft.print("R =", 3, 263, 114, goalColor);
        drawr(265,128,goalColor);
        tft.print("=", 1, 275, 128, goalColor);
        if(lastData.gAngleC == None){
            tft.print("None", 4, 286, 86, goalColor);
            tft.print("None", 4, 286, 100, goalColor);
            tft.print("None", 4, 286, 114, goalColor);
            tft.print("None", 4, 286, 128, goalColor);
        }
        else{
            tft.print(lastData.gAngleL, 286, 86, goalColor);
            tft.print(lastData.gAngleC, 286, 100, goalColor);
            tft.print(lastData.gAngleR, 286, 114, goalColor);
            tft.print(lastData.gDistance, 286, 128, goalColor);
        }
        
        tft.print("fps :", 5, 258, 150, tft.color(255,255,255));
        tft.print(lastData.fps, 286, 150, tft.color(255,255,255));
        
        goalColor = opponentGoal ? tft.color(0,0,255) : tft.color(255,255,0);
        
        tft.fillRoundRect(245,185,70,50,10,goalColor);
        tft.setFont(Arial_bold_14, tft.Center, 1);
#if defined(POSITION_OF)
        tft.print("Opponent", 8, 280, 198, tft.color(60,60,60));
#elif defined(POSITION_DF)
        tft.print("My", 2, 280, 198, tft.color(60,60,60));
#endif
        tft.print("Goal", 4, 280, 212, tft.color(60,60,60));
        
        tft.drawLine(120,115,120,125, tft.color(255,255,255));
        tft.drawLine(115,120,125,120, tft.color(255,255,255));
    }
    
    void loop(TP tp){
        cam.getValue(data);
        tft.setFont(Arial_14, tft.Left, 2);
        if(lastData.bAngle != data.bAngle){
            if(lastData.bAngle == None)
                tft.print("None", 4, 286, 50, tft.color(60,60,60));
            else
                tft.print(lastData.bAngle, 286, 50, tft.color(60,60,60));
            
            if(data.bAngle == None)
                tft.print("None", 4, 286, 50, tft.color(255,100,0));
            else
                tft.print(data.bAngle, 286, 50, tft.color(255,100,0));
        }
        if(lastData.bDistance != data.bDistance){
            if(lastData.bDistance == None)
                tft.print("None", 4, 286, 64, tft.color(60,60,60));
            else
                tft.print(lastData.bDistance, 286, 64, tft.color(60,60,60));
            
            if(data.bDistance == None)
                tft.print("None", 4, 286, 64, tft.color(255,100,0));
            else
                tft.print(data.bDistance, 286, 64, tft.color(255,100,0));
        }
        
        uint16_t goalColor = detectGoal ? tft.color(0,0,255) : tft.color(255,255,0);
        if(lastData.gAngleL != data.gAngleL){
            if(lastData.gAngleL == None)
                tft.print("None", 4, 286, 86, tft.color(60,60,60));
            else
                tft.print(lastData.gAngleL, 286, 86, tft.color(60,60,60));
            
            if(data.gAngleL == None)
                tft.print("None", 4, 286, 86, goalColor);
            else
                tft.print(data.gAngleL, 286, 86, goalColor);
        }
        if(lastData.gAngleC != data.gAngleC){
            if(lastData.gAngleC == None)
                tft.print("None", 4, 286, 100, tft.color(60,60,60));
            else
                tft.print(lastData.gAngleC, 286, 100, tft.color(60,60,60));
            
            if(data.gAngleC == None)
                tft.print("None", 4, 286, 100, goalColor);
            else
                tft.print(data.gAngleC, 286, 100, goalColor);
        }
        if(lastData.gAngleR != data.gAngleR){
            if(lastData.gAngleR == None)
                tft.print("None", 4, 286, 114, tft.color(60,60,60));
            else
                tft.print(lastData.gAngleR, 286, 114, tft.color(60,60,60));
            
            if(data.gAngleR == None)
                tft.print("None", 4, 286, 114, goalColor);
            else
                tft.print(data.gAngleR, 286, 114, goalColor);
        }
        if(lastData.gDistance != data.gDistance){
            if(lastData.gDistance == None)
                tft.print("None", 4, 286, 128, tft.color(60,60,60));
            else
                tft.print(lastData.gDistance, 286, 128, tft.color(60,60,60));
            
            if(data.gDistance == None)
                tft.print("None", 4, 286, 128, goalColor);
            else
                tft.print(data.gDistance, 286, 128, goalColor);
        }

        
        if(lastData.bAngle != data.bAngle || lastData.bDistance != data.bDistance){
            if(lastData.bAngle != None)
                tft.fillCircle(120 + sin(lastData.bAngle) * (lastData.bDistance/10) * 1, 120 - cos(lastData.bAngle) * (lastData.bDistance/10) * 1, 10, tft.color(0,0,0));
            if(data.bAngle != None)
                tft.fillCircle(120 + sin(data.bAngle) * (data.bDistance/10) * 1, 120 - cos(data.bAngle) * (data.bDistance/10) * 1, 10, tft.color(255,100,0));
        }
        if(lastData.gAngleR != data.gAngleR || lastData.gAngleL != data.gAngleL || lastData.gDistance != data.gDistance){
            if(lastData.gAngleC != None){
                if(abs(lastData.gAngleL - lastData.gAngleR) > 180)
                    tft.fillArc(120, 120, lastData.gDistance/10 - (115-lastData.gDistance/10)/6, (115-lastData.gDistance/10)/3, lastData.gAngleR, lastData.gAngleL, 0);
                else
                    tft.fillArc(120, 120, lastData.gDistance/10 - (115-lastData.gDistance/10)/6, (115-lastData.gDistance/10)/3, lastData.gAngleL, lastData.gAngleR, 0);
            }
        }
        
        if(data.gAngleC != None){
            if(abs(data.gAngleL - data.gAngleR) > 180)
                tft.fillArc(120, 120, data.gDistance/10 - (115-data.gDistance/10)/6, (115-data.gDistance/1.0)/3, data.gAngleR, data.gAngleL, goalColor);
            else
                tft.fillArc(120, 120, data.gDistance/10 - (115-data.gDistance/10)/6, (115-data.gDistance/10)/3, data.gAngleL, data.gAngleR, goalColor);
        }
        
        if(lastData.fps != data.fps){
            tft.print(lastData.fps, 286, 150, tft.color(0,0,0));
            tft.print(data.fps, 286, 150, tft.color(255,255,255));
        }
        
        tft.drawLine(120,115,120,125, tft.color(255,255,255));
        tft.drawLine(115,120,125,120, tft.color(255,255,255));
        
        lastData = data;
        
        if(!tp.now.touches && tp.last.touches){
            if(range(245, tp.last.x, 315) && range(185, tp.last.y, 235)){
                detectGoal = opponentGoal = !opponentGoal;
                uint16_t goalColor = opponentGoal ? tft.color(0,0,255) : tft.color(255,255,0);
                
                tft.fillRoundRect(245,185,70,50,10,goalColor);
                tft.setFont(Arial_bold_14, tft.Center, 1);
#if defined(POSITION_OF)
                tft.print("Opponent", 8, 280, 198, tft.color(60,60,60));
#elif defined(POSITION_DF)
                tft.print("My", 2, 280, 198, tft.color(60,60,60));
#endif
                tft.print("Goal", 4, 280, 212, tft.color(60,60,60));
                tft.setFont(Arial_14, tft.Left, 2);
                tft.print("L =", 3, 264, 86, goalColor);
                tft.print("C =", 3, 263, 100, goalColor);
                tft.print("R =", 3, 263, 114, goalColor);
                drawr(265,128,goalColor);
                tft.print("=", 1, 275, 128, goalColor);
                cam.setGoal(settings.data[settings.addOGoal]);
                eeprom.Write(settings.addRom, settings.data, settings.dataLen);
            }
            else if(range(245, tp.last.x, 315) && range(95, tp.last.y, 133)){
                detectGoal = !detectGoal;
                cam.setGoal(detectGoal);
                uint16_t goalColor = detectGoal ? tft.color(0,0,255) : tft.color(255,255,0);
                tft.setFont(Arial_14, tft.Left, 2);
                tft.print("L =", 3, 264, 86, goalColor);
                tft.print("C =", 3, 263, 100, goalColor);
                tft.print("R =", 3, 263, 114, goalColor);
                drawr(265,128,goalColor);
                tft.print("=", 1, 275, 128, goalColor);
                tft.print("=", 1, 275, 114, goalColor);
            }
        }
    }
};

#endif /* CAM_App_h */
