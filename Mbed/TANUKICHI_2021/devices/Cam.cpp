/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "Cam.h"

Cam::Cam(PinName _tx, PinName _rx) : openMV(_tx, _rx) {
    intervalTimer.start();
}

void Cam::interrupt() {
    int Data = READ_REG(USART6->DR);
//    int Data = openMV.getc();
    if (Data == 254){
        bufcont = 0;
        if(color)
            openMV.putc('b');
        else
            openMV.putc('y');
        ready = true;
        intervalus = intervalTimer.read_us();
        intervalTimer.reset();
    }
    else if(bufcont < 7){
        buf[bufcont] = Data;
        bufcont++;
    }
    impossible = false;
}

void Cam::init(){
    openMV.baud(115200);
    openMV.attach(callback(this, &Cam::interrupt), Serial::RxIrq);
}

int Cam::decompress(int data){
    data -= 126;
    if (data > 73)
        data = data*2 - 73;
    else if (data < -75)
        data = data*2 + 73;
    return data;
}

void Cam::getValue(Cam_S &sens){
    if(ready){
        data.updated = true;
        if(buf[0]==255){
            data.bAngle = None;
            data.bDistance = None;
        }
        else{
            data.bAngle = decompress(buf[0]);
            int x = buf[1];
            data.bDistance = (2*x) / (1.8 - 0.0115 * sqrt(8306 + x*x));
            //            data.bDistance = x;
        }
        
        if(buf[3]==255){
            data.gAngleC = None;
            data.gAngleL = None;
            data.gAngleR = None;
            data.gDistance = None;
        }
        else{
            data.gAngleL = decompress(buf[2]);
            data.gAngleC = decompress(buf[3]);
            data.gAngleR = decompress(buf[4]);
            int x = buf[5];
            data.gDistance = (2*x) / (1.8 - 0.0115 * sqrt(8306 + x*x));
            //        data.gDistance = x;
            
            //        if(data.gAngleL > data.gAngleR){
            //            int cswap = data.gAngleL;
            //            data.gAngleL = data.gAngleR;
            //            data.gAngleR = cswap;
            //        }
        }
        
        data.fps = buf[6];
        
        data.intervalus = intervalus;
        
        lastData = data;
        sens = data;
        
        ready = false;
    }
    else{
        sens = lastData;
        data.updated = false;
        sens.updated = false;
    }
}

void Cam::setGoal(bool setColor){
    color = setColor;
}
