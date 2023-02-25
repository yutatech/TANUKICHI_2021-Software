/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "Shared.h"

DigitalIn    Start(START);
DigitalOut   L_State(L_STATE);
DigitalOut   L_Error(L_ERROR);
AnalogIn     Batt(BATT);

DigitalOut   RSTLINE1(RST_LINE1);
DigitalOut   RSTLINE2(RST_LINE2);
DigitalOut   RSTLINE3(RST_LINE3);
DigitalOut   RSTLINE4(RST_LINE4);
DigitalOut   RSTUPPER(RST_UPPER);
DigitalOut   SSLINE1(SS_LINE1);
DigitalOut   SSLINE2(SS_LINE2);
DigitalOut   SSLINE3(SS_LINE3);
DigitalOut   SSLINE4(SS_LINE4);
DigitalOut   SSUPPER(SS_UPPER);
DigitalOut   SSMOUSE(SS_MOUSE);

Serial pc(TX_PC, RX_PC);
Serial wireless(TX_WIRELESS, RX_WIRELESS);
WirelessLog Log(wireless);

Ticker _sp_timer;
DigitalOut _sp_pin(SPEAKER);
int _sp_last;

void _sp_tick(void) {
    _sp_pin.write(!_sp_pin);
}

void sound(int frequency) {
//    if(_sp_last != frequency){
//        if(!frequency)
//            _sp_timer.detach();
//        else
//            _sp_timer.attach(&_sp_tick, 1 / (float)frequency);
//        _sp_last = frequency;
//    }
}

void printBIN(int data, int dig){
    printf("0b");
    for(int i=dig-1; i>=0; i--){
        if(data & 0b1<<i)
            printf("1");
        else
            printf("0");
    }
}
