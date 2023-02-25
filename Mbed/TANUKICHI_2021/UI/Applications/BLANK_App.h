/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef BLANK_h
#define BLANK_h

#include "ApplicationFramework.h"
#include "../bitmaps/Black.h"

class BLANK_App : public ApplicationFramework {
private:
public:
    BLANK_App(ILI9341 &_tft) : ApplicationFramework(_tft, Black_icon) {
        strcpy(AppName, "Blank");
        AppNameLen = 5;
        isApp = false;
    }
    
    void drawIcon(int x, int y){
    }
};

#endif /* BLANK_h */
