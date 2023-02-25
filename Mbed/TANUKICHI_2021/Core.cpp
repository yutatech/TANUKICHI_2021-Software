/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "Core.h"
Core::Core() :
    ui(CS, DC, SS_UPPER, devices, settings)
{}

void Core::init(){
    char startUpData = 0;
    devices.i2c.frequency(100000);
    //    devices.eeprom.Read(100, &startUpData, 1);
    
    RSTLINE1 = 1;
    RSTLINE2 = 1;
    RSTLINE3 = 1;
    RSTLINE4 = 1;
    RSTUPPER = 1;
    
    SSLINE1 = 1;
    SSLINE2 = 1;
    SSLINE3 = 1;
    SSLINE4 = 1;
    SSUPPER = 1;
    SSMOUSE = 1;
    
    L_State = 1;
    
    if(startUpData == 0){
        pc.baud(115200);
        wireless.baud(115200);
        pc.printf("\nStarting...");
    }
    
    devices.mouse.init();
    
    devices.dribblerF.init();
    devices.dribblerB.init();
    
    if(startUpData == 0){
        ui.begin();
        //devices.line.begin();
    }
    
    devices.eeprom.Read(settings.addRom, settings.data, settings.dataLen);
    devices.kicker.maxTime = (float)(150 - settings.data[settings.addKickPow]) / 10000;
    devices.led.setEnable(getBit(settings.data[settings.addState], SilluminationEnabled));
    
    devices.cam.init();
    devices.cam.setGoal(settings.data[settings.addOGoal]);
    
    devices.gyro.init();
    devices.gyro.compassIs = settings.data[settings.addCompIs];
    devices.gyro.calibX = settings.data[settings.addCompCalibX];
    devices.gyro.calibY = settings.data[settings.addCompCalibY];
    devices.gyro.cycle_us = 10000;
    
    if(startUpData == 0){
        devices.distance.init();
        
        Timer timer;
        timer.start();
        int i=0;
        const int loadingLength = 3200;
        const int interval = (loadingLength - 800) / 200;
        while(timer.read_ms() < loadingLength){
            if(interval * (i+1) < timer.read_ms() && i < 200){
                ui.loading(i++);
                if(i%10 == 0)
                    pc.putc('.');
            }
            if(Start.read()){
                startUpData = 1;
                break;
            }
        }
    }
    
    L_State = 0;
    
    if(startUpData == 0){
        ui.showHome();
        ui.drawBattery(20);
        READ_REG(SPIX->DR);
        
        pushed.start();
        released.start();
        devices.gyro.reset();
        
        printf("ready\n");
        wireless.printf("ready\n");
        Log.setEnable(getBit(settings.data[settings.addState], SwirelessLog));
    }
    else{
        ui.backLightCommand(7);
        running = true;
        pushed.start();
        released.start();
        READ_REG(SPIX->DR);
        algorithm = new Algorithm(devices, settings);
    }
}

void Core::core(){
    if(running) {
        algorithm->move();
    }
    else {
        ui.loop();
        READ_REG(SPIX->DR);
    }
    
    bool switchState = Start.read();
    if(switchState) {
        if(!lastSwitchState){
            if(!running)
                pushed.reset();
        }
        if(pushed.read_ms() > 300 && !running)
            sound(5000);
    }
    else {
        if(lastSwitchState && released.read_ms() > 500){
            unsigned long closedTime = pushed.read_ms();
            if(running && 70 < closedTime){
                delete algorithm;
                ui.backLightCommand(8);
                sound(0);
                devices.motor.open();
                devices.dribblerF.set(0);
                devices.dribblerB.set(0);
                devices.mouse.positionReset();
                running = false;
                
                char startUpData = false;
                devices.eeprom.Write(100, &startUpData, 1);
            }
            else if(!running){
                unsigned long pushedTime = pushed.read_ms();
                if(pushedTime < 300){
                    devices.mouse.positionReset();
                    devices.motor.EnablePID(true);
                    ui.backLightCommand(7);
                    running = true;
                    
                    printf("\n");
                    wireless.printf("\n");
                    
                    char startUpData = true;
                    devices.eeprom.Write(100, &startUpData, 1);
                    algorithm = new Algorithm(devices, settings);
                }
                else{
                    devices.gyro.reset();
                    sound(0);
                    wait_ms(100);
                    sound(4500);
                    wait_ms(100);
                    sound(0);
                    wait_ms(50);
                    sound(4500);
                    wait_ms(100);
                    sound(0);
                }
            }
            released.reset();
        }
        pushed.reset();
    }
    lastSwitchState = switchState;
}
