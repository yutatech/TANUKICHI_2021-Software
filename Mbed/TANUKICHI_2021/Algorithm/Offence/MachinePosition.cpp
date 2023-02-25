/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "MachinePosition.h"

MachinePosition::MachinePosition() :
    prvPos(qtyOfPrevious),
    prvVel(qtyOfPrevious) {}

void MachinePosition::calculate(SensorsArray &sensAry){
    positionX = sensAry.get(0).mouse.positionX * mouseUnit;
    positionY = sensAry.get(0).mouse.positionY * mouseUnit;
    
    velocityX = sensAry.get(0).mouse.deltaX * mouseUnit * 1000000 / sensAry.get(0).intervalus;
    velocityY = sensAry.get(0).mouse.deltaY * mouseUnit * 1000000 / sensAry.get(0).intervalus;
    velocity = sqrt(velocityX*velocityX + velocityY*velocityY);
    
    acceleX   = (sensAry.get(0).mouse.deltaX - sensAry.get(1).mouse.deltaX) * mouseUnit * 1000000 / sensAry.get(0).intervalus;
    acceleY   = (sensAry.get(0).mouse.deltaY - sensAry.get(1).mouse.deltaY) * mouseUnit * 1000000 / sensAry.get(0).intervalus;
    
    prvPos.x.update(positionX);
    prvPos.y.update(positionY);
    prvVel.x.update(velocityX);
    prvVel.y.update(velocityY);
}
