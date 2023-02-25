/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef LED_h
#define LED_h

#include "mbed.h"
#include "../shared.h"

class LED {
private:
    SPI &spi;
    bool illuminationEnabled = true;
public:
    LED(SPI &_spi) : spi(_spi){}
    
    void setEnable(bool enable){
        if(!enable)
            set(0);
        illuminationEnabled = enable;
    }
    
    void set(int color){
        if(!illuminationEnabled)
            return;
        READ_REG(SPIX->DR);
        spi.frequency(1000000);
        spi.format(8, 0);
        
        SSUPPER = 0;
        spi.write(9);
        wait_us(10);
        spi.write(color);
        wait_us(10);
        SSUPPER = 1;
    }
    
    void set(int num, int color){
        if(!illuminationEnabled)
            return;
        READ_REG(SPIX->DR);
        spi.frequency(1000000);
        spi.format(8, 0);
        
        SSUPPER = 0;
        spi.write(10);
        wait_us(10);
        spi.write(num);
        wait_us(10);
        spi.write(color);
        wait_us(10);
        SSUPPER = 1;
    }
    
    void setDefault(){
        READ_REG(SPIX->DR);
        spi.frequency(1000000);
        spi.format(8, 0);
        
        SSUPPER = 0;
        spi.write(9);
        wait_us(10);
        spi.write(2);
        wait_us(10);
        SSUPPER = 1;
    }
};


#endif /* LED_h */
