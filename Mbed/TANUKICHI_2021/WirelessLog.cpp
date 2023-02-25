/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "WirelessLog.h"

WirelessLog::WirelessLog(Serial &_wireless) : updated(false), wireless(_wireless), buf(new int[numOfIndex]), dataInProcess(new int[numOfIndexInProcess]) {}

void WirelessLog::sendLog(){
    if(!updated && process == 0)
        return;
    if(process == 0){
        updated = false;
        if(numOfIndexInProcess != numOfIndex){
            delete dataInProcess;
            dataInProcess = new int[numOfIndexInProcess = numOfIndex];
        }
            
        dataInProcess = buf;
        
        if(type == dec){
            for(int i=0; i<numOfIndexInProcess; i++)
                dataInProcess[i] = constrain(dataInProcess[i], -2047, 2047);
        }
    }
    
    wireless.putc(abs(dataInProcess[process]) & 0b01111111);
    char bit6to4 = process << 4;
    char bit3 = dataInProcess[process] < 0 ? 0b00001000 : 0;
    char bit2to0 = (abs(dataInProcess[process]) >> 7) & 0b00000111;
    wireless.putc(0b10000000 | bit6to4 | bit3 | bit2to0);
    process++;
    process %= numOfIndexInProcess;
}

void WirelessLog::setData(int *data){
    for(int i=0; i<numOfIndex; i++)
        buf[i] = data[i];
    updated = true;
}

void WirelessLog::setNumOfIndex(int num){
    num = constrain(num, 0, 8);
    if(numOfIndex == num)
        return;
    
    delete buf;
    buf = new int[numOfIndex = num];
}

void WirelessLog::setType(Base _type){
    type = _type;
}

void WirelessLog::setEnable(bool _enable){
    if(enable && !_enable)
        whenSendLog.detach();
    else if(!enable && _enable)
        whenSendLog.attach_us(callback(this, &WirelessLog::sendLog), 1000);
    
    enable = _enable;
}
