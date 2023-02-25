/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef Core_h
#define Core_h

#include "mbed.h"
#include "UI/UI.h"
#include "Robot.h"
#include "Shared.h"

#if defined(POSITION_OF)
    #include "Algorithm/Offence/Offence.h"
#elif defined(POSITION_DF)
    #include "Algorithm/Defence/Defence.h"
#endif

class Core {
private:
    bool running = false;
    bool lastSwitchState = false;
    Devices devices;
    Settings settings;
    Algorithm* algorithm;
    UI ui;
    
    Timer pushed;
    Timer released;
public:
    Core();
    void init();
    void core();
};

#endif /* Core_h */
