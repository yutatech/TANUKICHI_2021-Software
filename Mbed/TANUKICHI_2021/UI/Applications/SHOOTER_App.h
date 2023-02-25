/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef SHOOTER_App_h
#define SHOOTER_App_h

#include "ApplicationFramework.h"
#include "../bitmaps/SHOOTER_icon.h"

#include "../../Algorithm/PrecisionShooter/PrecisionShooter.h"

class SHOOTER_App : public ApplicationFramework {
private:
    PrecisionShooter precisionShooter;
    Devices &devices;
public:
    SHOOTER_App(ILI9341 &_tft, Devices &_devices, Settings &_settings) : precisionShooter(_devices, _settings), devices(_devices), ApplicationFramework(_tft, SHOOTER_icon) {
        strcpy(AppName, "Shooter");
        AppNameLen = 7;
    }
    
    void begin(){
        tft.fillRect(0,0,320,240,tft.color(255,255,255));
        tft.fillCircle(160,140, 56, tft.color(0,255,0));
        tft.fillTriangle(140, 110, 140, 170, 190, 140, tft.color(255,255,255));
        tft.setFont(Arial_bold_14, tft.Center, 2);
        tft.print("Precision Shooter", 17, 160, 35, tft.color(0,0,0));
    }
    
    void loop(TP tp){
        if(!tp.now.touches && tp.last.touches){
            tft.fillCircle(160,140, 56, tft.color(255,255,255));
            tft.fillTriangle(290, 30, 290, 50, 307, 40, tft.color(150,150,150));
            tft.fillTriangle(270, 30, 270, 50, 287, 40, tft.color(150,150,150));
            
            while(!Start.read());
            
            devices.mouse.positionReset();
            devices.motor.EnablePID(true);
            
            while(Start.read());
            
            READ_REG(SPIX->DR);
            devices.spi.frequency(1000000);
            devices.spi.format(8, 0);
            SSUPPER = 0;
            devices.spi.write(7);
            wait_us(10);
            devices.spi.write(7);
            SSUPPER = 1;
            wait_ms(100);
            
            Timer time;
            time.start();
            while(time.read_ms() < 4900 && !Start.read());
            while(!Start.read()){
                precisionShooter.move();
            }
            
            tft.fillTriangle(290, 30, 290, 50, 307, 40, tft.color(255,255,255));
            tft.fillTriangle(270, 30, 270, 50, 287, 40, tft.color(255,255,255));
            tft.fillCircle(160,140, 56, tft.color(0,255,0));
            tft.fillTriangle(140, 110, 140, 170, 190, 140, tft.color(255,255,255));
            READ_REG(SPIX->DR);
            devices.spi.frequency(1000000);
            devices.spi.format(8, 0);
            SSUPPER = 0;
            devices.spi.write(8);
            wait_us(10);
            devices.spi.write(8);
            SSUPPER = 1;
            wait_ms(5);
            sound(0);
            devices.motor.open();
            devices.dribblerF.set(0);
            devices.dribblerB.set(0);
            devices.mouse.positionReset();
            
            while(Start.read());
            wait_ms(100);
        }
    }
};

#endif /* SHOOTER_App_h */
