/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef DodgeOpponent_h
#define DodgeOpponent_h

#include "../../shared.h"
#include "../../Robot.h"
#include "../../libraries/PID.h"
#include "../../libraries/Math.h"

class DodgeOpponent {
private:
public:
    Move motion1(SensorsArray &sensAry, Dribbler &dribblerF);
    Move motion2(SensorsArray &sensAry);
};

#endif /* DodgeOpponent_h */
