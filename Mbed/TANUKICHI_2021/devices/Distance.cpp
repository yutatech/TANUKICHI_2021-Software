/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "Distance.h"

Distance::Distance(SPI &_spi, I2C &_i2c, PinName _ss) : spi(_spi), i2c(_i2c), ss(_ss), gpioExp(i2c, 0x40), vl{i2c, i2c, i2c, i2c, i2c, i2c}{
    ss = 1;
}

void Distance::init(){
#if defined(POSITION_OF)
    gpioExp.pinMode(0,1);
    gpioExp.pinMode(1,1);
    gpioExp.pinMode(2,1);
    gpioExp.pinMode(3,1);
    gpioExp.pinMode(4,1);
    gpioExp.pinMode(5,1);

    gpioExp.digitalWrite(0,0);
    gpioExp.digitalWrite(1,0);
    gpioExp.digitalWrite(2,0);
    gpioExp.digitalWrite(3,0);
    gpioExp.digitalWrite(4,0);
    for(int i = 0; i < 6; i++){
        vl[i].setTimeout(50);
        vl[i].init();
        vl[i].setAddress(9 + i);
        if(i==0){
            vl[i].setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
            vl[i].setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
        }
        else{
            vl[i].setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 10);
            vl[i].setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 6);

        }
        gpioExp.digitalWrite(i,1);
    }
#elif defined(POSITION_DF)
    vl[0].setTimeout(50);
    vl[0].init();
    vl[0].setAddress(9);
    vl[0].setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
    vl[0].setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
#endif
}

void Distance::getValue(Distance_S &sens){
    READ_REG(SPIX->DR);
    spi.frequency(1000000);
    spi.format(8, 0);
    ss = 0;
    spi.write(0);
    wait_us(10);
    for(int i = 0; i < 2; i++){
        value[i * 2 + 1] = spi.write(i + 1);
        wait_us(10);
        if(value[i * 2 + 1] == i || value[i * 2 + 1] == 100){
            spi.write(i);
            wait_us(10);
            value[i * 2 + 1] = spi.write(i + 1);
            wait_us(10);
            if(value[i * 2 + 1] == i || value[i * 2 + 1] == 100)
                value[i * 2 + 1] = data.value[i * 2 + 1];
        }
    }
    ss = 1;
    data.value[1] = value[1];
    data.value[3] = value[3];
    
#if defined(POSITION_OF)
    for(int i=0; i<6; i++)
        vl[i].getData(vldata[i]);
    
    const float ratio = 0.6;
    if(vldata[0] != lastVldata[0]){
        if(data.value[2] < 8188 && vldata[0] < 8188)
            data.value[2] = vldata[0] * ratio + data.value[2] * (1-ratio);
        else
            data.value[2] = vldata[0];
    }
    for(int i=0; i<5; i++){
        if(vldata[i+1] != lastVldata[i+1]){
            if(data.valueF[i] < 8188 && vldata[i+1] < 8188)
                data.valueF[i] = vldata[i+1] * ratio + data.valueF[i] * (1-ratio);
            else
                data.valueF[i] = vldata[i+1];
        }
    }
    
    for(int i=0; i<6; i++)
        lastVldata[i] = vldata[i];
    
    data.value[0] = -1;
    for(int i=0; i<5; i++){
        if(data.valueF[i] < 8188)
            data.value[0] = max(data.value[0], data.valueF[i] * cos((i-2) * 20));
    }
    if(data.value[0] == -1)
        data.value[0] = 8190;
    
    for(int i=0; i<5; i++)
        data.lidar.value[i] = data.valueF[i];
    
    if(abs(data.lidar.value[0] * sin(40) * 0.1 - data.value[3]*1.2) < 30)
        data.lidar.value[0] = data.value[0];
    if(abs(data.lidar.value[1] * sin(20) * 0.1 - data.value[3]*1.2) < 30)
        data.lidar.value[1] = data.value[0];
    if(abs(data.lidar.value[3] * sin(20) * 0.1 - data.value[1]*1.2) < 30)
        data.lidar.value[3] = data.value[0];
    if(abs(data.lidar.value[4] * sin(40) * 0.1 - data.value[1]*1.2) < 30)
        data.lidar.value[4] = data.value[0];

    int ave = 0;
    int num = 0;
    for(int i=0; i<5; i++){
        if(data.lidar.value[i] <= 8188){
            ave += data.lidar.value[i];
            num++;
        }
    }
    if(num == 0)
        ave = 8190;
    else
        ave /= num;

    data.lidar.threshould = ave * (constrain((float)ave/2000+0.5, 0.65, 0.95));
    int noDetected[7] = {-1,0,0,0,0,0,0};
    int index = 1;
    for(int i=0; i<5; i++){
        if(data.lidar.value[i] > data.lidar.threshould)
            noDetected[index++] = i;
    }
    noDetected[index++]=5;
    if(index == 7||index == 2||index == 3){
        data.lidar.angleL = None;
        data.lidar.angleC = None;
        data.lidar.angleR = None;
        data.lidar.distance = None;
    }
    else{
        int gap[6] = {0,0,0,0,0,0};
        for(int i=0; i<index-1; i++)
            gap[i] = noDetected[i+1] - noDetected[i];
        float maxGap = 0;
        int maxPos = 0;
        for(int i=0; i<index-1; i++){
            if(maxGap < gap[i]){
                maxGap = gap[i]-1;
                maxPos = i;
            }
        }

        data.lidar.angleL = (noDetected[maxPos] - 1) * 20;
        data.lidar.angleC = (noDetected[maxPos] + (maxGap-1) * 0.5 - 1) * 20;
        data.lidar.angleR = (noDetected[maxPos] + maxGap - 2) * 20;
        data.lidar.distance = None;
        for(int i=maxPos; i<maxPos+maxGap; i++)
            data.lidar.distance = min(data.lidar.distance, data.lidar.value[i]);
    }
    
#elif defined(POSITION_DF)
    vl[0].getData(data.value[2]);
#endif
    
    sens = data;
}

void Distance::getValue(Lidar_S &sens){
    Distance_S data;
    getValue(data);
    sens = data.lidar;
}
