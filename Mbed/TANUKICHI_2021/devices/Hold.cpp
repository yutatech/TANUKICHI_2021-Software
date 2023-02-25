/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "Hold.h"

Hold::Hold(PinName _holdf, PinName _holdb) : holdF(_holdf), holdB(_holdb) {
    timeF.start();
    timeB.start();
}

void Hold::getValue(Hold_S &sens){
    if(holdF.read_u16()>25000){
        if(!holdedF)
            timeF.reset();
        sens.f = holdedF = true;
    }
    else{
        if(holdedF){
            timeF.reset();
            holdedF = false;
            sens.f = true;
        }
        else if(timeF.read_ms() > 50)
            sens.f = holdedF = false;
        else
            sens.f = true;
    }
    if(holdB.read_u16()>25000){
        if(!holdedB)
            timeB.reset();
        sens.b = holdedB = true;
    }
    else{
        if(holdedB){
            timeB.reset();
            holdedB = false;
            sens.b = true;
        }
        else if(timeB.read_ms() > 50)
            sens.b = holdedB = false;
        else
            sens.b = true;
    }
}
