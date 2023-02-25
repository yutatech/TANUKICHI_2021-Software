/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef Hold_h
#define Hold_h

#include "mbed.h"

typedef struct {
    bool f = false, b = false;
    void print(){
        printf("Hold:f=%d,b=%d\t",f, b);
    }
} Hold_S;

class Hold {
private:
    AnalogIn holdF, holdB;
public:
    Timer timeF, timeB;
    bool holdedF, holdedB;
    
    Hold(PinName _holdf, PinName _holdb);
    void getValue(Hold_S &sens);
};

#endif /* Hold_h */
