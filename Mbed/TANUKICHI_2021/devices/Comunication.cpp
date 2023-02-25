/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "Comunication.h"

Comunication::Comunication(Serial &_wireless) : wireless(_wireless) {}

void Comunication::interrupt(){
    char data = wireless.getc();
    if(bufCount == 3){
        if(data == Address){
            bufCount = 0;
            active = true;
        }
    }
    else{
        buf[bufCount++] = data;
        if(bufCount == 3){
            processData();
            loseTime.attach(this, &Comunication::lostComu, timeout);
        }
    }
}

void Comunication::lostComu(){
    if(timeoutCount == 1)
        active = false;
    else
        timeoutCount++;
    loseTime.attach(this, &Comunication::lostComu, timeout);
}

void Comunication::processData(){
    switch(buf[0]){
        case 1:
            break;
        default:
            break;
    }
}

void Comunication::init(){
    if(!enable)
        return;
    
    wireless.baud(115200);
    wireless.attach(this, &Comunication::interrupt, Serial::RxIrq);
    
    loseTime.attach(this, &Comunication::lostComu, timeout);
}

void Comunication::setTimeout(float _timeout){
    timeout = _timeout;
    loseTime.attach(this, &Comunication::lostComu, timeout);
}


//1 アドレス || ヘッダー
//2 コマンド
//3 x
//4 y

