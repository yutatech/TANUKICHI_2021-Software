/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "Dribbler.h"

Dribbler::Dribbler(PinName _dribbler) : dribbler(_dribbler) {
}

void Dribbler::init(){
    dribbler.pulsewidth(0.0010);
}

void Dribbler::set(int val){
    val = constrain(val ,0 ,100);
    dribbler.pulsewidth(0.0010 + (float)val / 100000);
}
