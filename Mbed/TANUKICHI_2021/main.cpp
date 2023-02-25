/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "mbed.h"
#include "Core.h"

Core core;
int main() {
    core.init();
    while(1) {
        core.core();
    }
    return 0;
}
