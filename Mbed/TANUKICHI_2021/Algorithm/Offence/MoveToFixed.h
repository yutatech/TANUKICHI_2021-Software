/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef MoveToFixed_h
#define MoveToFixed_h

#include "../../shared.h"
#include "../../Robot.h"
#include "../../libraries/PID.h"
#include "../../libraries/Math.h"

class MoveToFixed {
private:
public:
    Move neutral(SensorsArray &sensAry);
    Move home(SensorsArray &sensAry);
};
#endif /* MoveToFixed_h */
