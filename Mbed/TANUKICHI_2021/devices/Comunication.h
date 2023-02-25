/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef Comunication_h
#define Comunication_h

#include "mbed.h"

class Comunication {
private:
    Serial &wireless;
    Timeout loseTime;
    
    void interrupt();
    void lostComu();
    void processData();
    
    char buf[3];
    int bufCount = 3;
    float timeout = 0.100;
    int timeoutCount = 0;
public:
    Comunication(Serial &_wireless);
    void init();
    void setTimeout(float _timeout);
    
    bool active = false;
    bool enable = true;
    char Address = 36;
};

#endif /* Comunication_h */
