/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "Line.h"

Line::Line(SPI &_spi, PinName _line1, PinName _line2, PinName _line3, PinName _line4) : spi(_spi), Line1(_line1), Line2(_line2), Line3(_line3), Line4(_line4), pid{{KP, KI, KD, 0, -1, 1}, {KP, KI, KD, 0, -1, 1}, {KP, KI, KD, 0, -1, 1}, {KP, KI, KD, 0, -1, 1}} {
    for(int i=0; i<4; i++)
        ss[i] = 1;
}

void Line::getValue(Line_S &sens){
    spi.frequency(100000);
    spi.format(8, 0);
    if(lifted){
        for(int i=0; i<4; i++){
            ss[i] = 0;
            spi.write(6);
            wait_us(10);
            ss[i] = 1;
            value[i] = 0;
        }
    }
    else{
        for(int i=0; i<4; i++){
            for(int j=0; j<2; j++){
                ss[i] = 0;
                spi.write(2);
                wait_us(50);
                value[i] = spi.write(3);
                wait_us(50);
                value[i] |= spi.write(1) << 8;
                wait_us(50);
                ss[i] = 1;
                if((value[i] & 0b000011111111) != 100 && (value[i] & 0b000011111111) != 118 &&
                   !((i%2 == 0 && value[i] == 0b1111111111) || (i%2 == 1 && value[i] == 0b111111111111)))
                    break;
                value[i] = 0;
            }
        }
    }
    
    for (int i = 0; i < 4; i++) {
        l[i][Index] = value[i];
        value[i] = (l[i][0] & l[i][1] | l[i][2]) & (l[i][0] | l[i][1] & l[i][2]);
    }
    Index++;
    Index %= 3;
    
    for (int i = 0; i < 4; i++)
        sens.value[i] = value[i];
    
#if defined(POSITION_OF)
    get_coordinate_OF(sens);
    get_LS_OF(sens);
#elif defined(POSITION_DF)
    get_coordinate_DF(sens);
#endif
}

void Line::set_coordinateX(int dataX){
    coordinateX += dataX;
    countX++;
}

void Line::set_coordinateY(int dataY){
    coordinateY += dataY;
    countY++;
}

void Line::calibrationStart(){
    spi.frequency(100000);
    spi.format(8, 0);
    for(int i=0; i<4; i++){
        ss[i] = 0;
        wait_us(100);
        spi.write(4);
        wait_us(100);
        ss[i] = 1;
    }
    for(int i=0; i<4; i++){
        ss[i] = 0;
        wait_us(100);
        spi.write(4);
        wait_us(100);
        ss[i] = 1;
    }
}

void Line::calibrationStop(){
    spi.frequency(100000);
    spi.format(8, 0);
    for(int i=0; i<4; i++){
        ss[i] = 0;
        wait_us(100);
        spi.write(5);
        wait_us(100);
        ss[i] = 1;
    }
    for(int i=0; i<4; i++){
        ss[i] = 0;
        wait_us(100);
        spi.write(5);
        wait_us(100);
        ss[i] = 1;
    }
}

void Line::begin(){
    READ_REG(SPIX->DR);
    spi.frequency(100000);
    spi.format(8, 0);
    for(int i=0; i<4; i++){
        ss[i] = 0;
        spi.write(7);
        wait_us(10);
        ss[i] = 1;
    }
    for(int i=0; i<4; i++){
        ss[i] = 0;
        spi.write(7);
        wait_us(10);
        ss[i] = 1;
    }
}

void Line::get_coordinate_OF(Line_S &sens){
    if((value[0] | value[1] | value[2] | value[3]) == 0){
        sens.coordinateX = None;
        sens.coordinateY = None;
        sens.subCoordinateX = None;
        sens.subCoordinateY = None;
    }
    else{
        countX = countY = 0;
        coordinateX = coordinateY = 0;
        //X
        if(value[1] & 0b111100000000)
            set_coordinateX(34);
        if(value[1] & 0b000010000000)
            set_coordinateX(32);
        if(value[1] & 0b000001000000)
            set_coordinateX(28);
        if(value[1] & 0b000000100000)
            set_coordinateX(24);
        if(value[1] & 0b000000010000)
            set_coordinateX(20);
        if(value[1] & 0b000000001000)
            set_coordinateX(16);
        if(value[1] & 0b000000000100)
            set_coordinateX(12);
        if(value[1] & 0b000000000011)
            set_coordinateX(8);
        if((value[0] & 0b000000000001) | (value[2] & 0b000000000010))
            set_coordinateX(6);
        if((value[0] & 0b000000000000) | (value[2] & 0b000000000000))
            set_coordinateX(2);
        if((value[0] & 0b000000000100) | (value[2] & 0b000000000100))
            set_coordinateX(0);
        if((value[0] & 0b000000000000) | (value[2] & 0b000000000000))
            set_coordinateX(-2);
        if((value[0] & 0b000000000010) | (value[2] & 0b000000000001))
            set_coordinateX(-6);
        if(value[3] & 0b000000000011)
            set_coordinateX(-8);
        if(value[3] & 0b000000000100)
            set_coordinateX(-12);
        if(value[3] & 0b000000001000)
            set_coordinateX(-16);
        if(value[3] & 0b000000010000)
            set_coordinateX(-20);
        if(value[3] & 0b000000100000)
            set_coordinateX(-24);
        if(value[3] & 0b000001000000)
            set_coordinateX(-28);
        if(value[3] & 0b000010000000)
            set_coordinateX(-32);
        if(value[3] & 0b111100000000)
            set_coordinateX(-34);
        if(countX)
            sens.coordinateX = ((float)coordinateX) / countX;
        else
            sens.coordinateX = None;
        
        //Y
        if(value[0] & 0b001111000000)
            set_coordinateY(26);
        if(value[0] & 0b000000100000)
            set_coordinateY(24);
        if(value[0] & 0b000000010000)
            set_coordinateY(20);
        if(value[0] & 0b000000001000)
            set_coordinateY(16);
        if(value[0] & 0b000000000100)
            set_coordinateY(12);
        if(value[0] & 0b000000000011)
            set_coordinateY(8);
        if((value[1] & 0b000000000010) | (value[3] & 0b000000000001))
            set_coordinateY(6);
        if((value[1] & 0b000000000000) | (value[3] & 0b000000000000))
            set_coordinateY(2);
        if((value[1] & 0b000000000100) | (value[3] & 0b000000000100))
            set_coordinateY(0);
        if((value[1] & 0b000000000000) | (value[3] & 0b000000000000))
            set_coordinateY(-2);
        if((value[1] & 0b000000000001) | (value[3] & 0b000000000010))
            set_coordinateY(-6);
        if(value[2] & 0b000000000011)
            set_coordinateY(-8);
        if(value[2] & 0b000000000100)
            set_coordinateY(-12);
        if(value[2] & 0b000000001000)
            set_coordinateY(-16);
        if(value[2] & 0b000000010000)
            set_coordinateY(-20);
        if(value[2] & 0b000000100000)
            set_coordinateY(-24);
        if(value[2] & 0b001111000000)
            set_coordinateY(-26);
        if(countY)
            sens.coordinateY = ((float)coordinateY) / countY;
        else
            sens.coordinateY = None;
        
        
        
        countX = countY = 0;
        coordinateX = coordinateY = 0;
        //subX
        if((value[0] & 0b000001000000) | (value[2] & 0b001000000000))
            set_coordinateX(6);
        if((value[0] & 0b000010000000) | (value[2] & 0b000100000000))
            set_coordinateX(2);
        if((value[0] & 0b000100000000) | (value[2] & 0b000010000000))
            set_coordinateX(-2);
        if((value[0] & 0b001000000000) | (value[2] & 0b000001000000))
            set_coordinateX(-6);
        if(countX)
            sens.subCoordinateX = ((float)coordinateX) / countX;
        else
            sens.subCoordinateX = None;
        
        //subY
        if((value[1] & 0b100000000000) | (value[3] & 0b000100000000))
            set_coordinateY(6);
        if((value[1] & 0b010000000000) | (value[3] & 0b001000000000))
            set_coordinateY(2);
        if((value[1] & 0b001000000000) | (value[3] & 0b010000000000))
            set_coordinateY(-2);
        if((value[1] & 0b000100000000) | (value[3] & 0b100000000000))
            set_coordinateY(-6);
        if(countY)
            sens.subCoordinateY = ((float)coordinateY) / countY;
        else
            sens.subCoordinateY = None;
    }
}

void Line::get_coordinate_DF(Line_S &sens){
    if((value[0] | value[1] | value[2] | value[3]) == 0){
        sens.coordinateX = None;
        sens.coordinateY = None;
    }
    else{
        countX = countY = 0;
        coordinateX = coordinateY = 0;
        //X
        if(value[1] & 0b111100000000)
            set_coordinateX(34);
        if(value[1] & 0b000010000000)
            set_coordinateX(32);
        if(value[1] & 0b000001000000)
            set_coordinateX(28);
        if(value[1] & 0b000000100000)
            set_coordinateX(24);
        if(value[1] & 0b000000010000)
            set_coordinateX(20);
        if(value[1] & 0b000000001000)
            set_coordinateX(16);
        if(value[1] & 0b000000000100)
            set_coordinateX(12);
        if(value[1] & 0b000000000011)
            set_coordinateX(8);
        if((value[0] & 0b000001000001) | (value[2] & 0b100000000010))
            set_coordinateX(6);
        if((value[0] & 0b000001000000) | (value[2] & 0b010000000000))
            set_coordinateX(2);
        if((value[0] & 0b000000011100) | (value[2] & 0b000011111100))
            set_coordinateX(0);
        if((value[0] & 0b000010000000) | (value[2] & 0b001000000000))
            set_coordinateX(-2);
        if((value[0] & 0b000100000010) | (value[2] & 0b000100000001))
            set_coordinateX(-6);
        if(value[3] & 0b000000000011)
            set_coordinateX(-8);
        if(value[3] & 0b000000000100)
            set_coordinateX(-12);
        if(value[3] & 0b000000001000)
            set_coordinateX(-16);
        if(value[3] & 0b000000010000)
            set_coordinateX(-20);
        if(value[3] & 0b000000100000)
            set_coordinateX(-24);
        if(value[3] & 0b000001000000)
            set_coordinateX(-28);
        if(value[3] & 0b000010000000)
            set_coordinateX(-32);
        if(value[3] & 0b111100000000)
            set_coordinateX(-34);
        if(countX)
            sens.coordinateX = ((float)coordinateX + 0.5) / countX;
        else
            sens.coordinateX = 0;
        
        //Y
        if(value[0] & 0b000111100000)
            set_coordinateY(22);
        if(value[0] & 0b000000010000)
            set_coordinateY(20);
        if(value[0] & 0b000000001000)
            set_coordinateY(16);
        if(value[0] & 0b000000000100)
            set_coordinateY(12);
        if(value[0] & 0b000000000011)
            set_coordinateY(8);
        if((value[1] & 0b100000000010) | (value[3] & 0b000100000001))
            set_coordinateY(6);
        if((value[1] & 0b010000000000) | (value[3] & 0b001000000000))
            set_coordinateY(2);
        if((value[1] & 0b000011111100) | (value[3] & 0b000011111100))
            set_coordinateY(0);
        if((value[1] & 0b001000000000) | (value[3] & 0b010000000000))
            set_coordinateY(-2);
        if((value[1] & 0b000100000001) | (value[3] & 0b100000000010))
            set_coordinateY(-6);
        if(value[2] & 0b000000000011)
            set_coordinateY(-8);
        if(value[2] & 0b000000000100)
            set_coordinateY(-12);
        if(value[2] & 0b000000001000)
            set_coordinateY(-16);
        if(value[2] & 0b000000010000)
            set_coordinateY(-20);
        if(value[2] & 0b000000100000)
            set_coordinateY(-24);
        if(value[2] & 0b000001000000)
            set_coordinateY(-28);
        if(value[2] & 0b000010000000)
            set_coordinateY(-32);
        if(value[2] & 0b111100000000)
            set_coordinateY(-34);
        if(countY)
            sens.coordinateY = ((float)coordinateY + 0.5) / countY;
        else
            sens.coordinateY = 0;
    }
}

void Line::get_LS_OF(Line_S &sens){
    LS_ave[0] = 0;
    LS_ave[1] = 0;
    LS_num_detected = 0;
    
    if(value[FRONT] & 0b1000000000)
        set_data_LS_OF(-18,78);
    if(value[FRONT] & 0b0100000000)
        set_data_LS_OF(-6,78);
    if(value[FRONT] & 0b0010000000)
        set_data_LS_OF(6,78);
    if(value[FRONT] & 0b0001000000)
        set_data_LS_OF(18,78);
    if(value[FRONT] & 0b0000100000)
        set_data_LS_OF(0,66);
    if(value[FRONT] & 0b0000010000)
        set_data_LS_OF(0,54);
    if(value[FRONT] & 0b0000001000)
        set_data_LS_OF(0,42);
    if(value[FRONT] & 0b0000000100)
        set_data_LS_OF(0,30);
    if(value[FRONT] & 0b0000000010)
        set_data_LS_OF(-12,24);
    if(value[FRONT] & 0b0000000001)
        set_data_LS_OF(12,24);
    
    if(value[RIGHT] & 0b100000000000)
        set_data_LS_OF(102,18);
    if(value[RIGHT] & 0b010000000000)
        set_data_LS_OF(102,6);
    if(value[RIGHT] & 0b001000000000)
        set_data_LS_OF(102,-6);
    if(value[RIGHT] & 0b000100000000)
        set_data_LS_OF(102,-18);
    if(value[RIGHT] & 0b000010000000)
        set_data_LS_OF(90,0);
    if(value[RIGHT] & 0b000001000000)
        set_data_LS_OF(78,0);
    if(value[RIGHT] & 0b000000100000)
        set_data_LS_OF(66,0);
    if(value[RIGHT] & 0b000000010000)
        set_data_LS_OF(54,0);
    if(value[RIGHT] & 0b000000001000)
        set_data_LS_OF(42,0);
    if(value[RIGHT] & 0b000000000100)
        set_data_LS_OF(30,0);
    if(value[RIGHT] & 0b000000000010)
        set_data_LS_OF(24,12);
    if(value[RIGHT] & 0b000000000001)
        set_data_LS_OF(24,-12);
    
    if(value[BACK] & 0b1000000000)
        set_data_LS_OF(18,-78);
    if(value[BACK] & 0b0100000000)
        set_data_LS_OF(6,-78);
    if(value[BACK] & 0b0010000000)
        set_data_LS_OF(-6,-78);
    if(value[BACK] & 0b0001000000)
        set_data_LS_OF(-18,-78);
    if(value[BACK] & 0b0000100000)
        set_data_LS_OF(0,-66);
    if(value[BACK] & 0b0000010000)
        set_data_LS_OF(0,-54);
    if(value[BACK] & 0b0000001000)
        set_data_LS_OF(0,-42);
    if(value[BACK] & 0b0000000100)
        set_data_LS_OF(0,-30);
    if(value[BACK] & 0b0000000010)
        set_data_LS_OF(12,-24);
    if(value[BACK] & 0b0000000001)
        set_data_LS_OF(-12,-24);
    
    if(value[LEFT] & 0b100000000000)
        set_data_LS_OF(-102,-18);
    if(value[LEFT] & 0b010000000000)
        set_data_LS_OF(-102,6);
    if(value[LEFT] & 0b001000000000)
        set_data_LS_OF(-102,-6);
    if(value[LEFT] & 0b000100000000)
        set_data_LS_OF(-102,18);
    if(value[LEFT] & 0b000010000000)
        set_data_LS_OF(-90,0);
    if(value[LEFT] & 0b000001000000)
        set_data_LS_OF(-78,0);
    if(value[LEFT] & 0b000000100000)
        set_data_LS_OF(-66,0);
    if(value[LEFT] & 0b000000010000)
        set_data_LS_OF(-54,0);
    if(value[LEFT] & 0b000000001000)
        set_data_LS_OF(-42,0);
    if(value[LEFT] & 0b000000000100)
        set_data_LS_OF(-30,0);
    if(value[LEFT] & 0b000000000010)
        set_data_LS_OF(-24,-12);
    if(value[LEFT] & 0b000000000001)
        set_data_LS_OF(-24,12);
    
    if(LS_num_detected == 0){
        sens.slope = None;
        sens.angle = None;
        sens.distance = None;
    }
    else if(LS_num_detected == 1){
        sens.angle = atan2(LS_ave[0], LS_ave[1]);
        sens.distance = sqrt(LS_ave[0]*LS_ave[0] + LS_ave[1]*LS_ave[1]);
        sens.slope = sens.angle + (sens.angle > 0 ? -90 : 90);
    }
    else if(LS_num_detected == 2 && abs(LS_detected[0][0] - LS_detected[1][0]) <= 12 && abs(LS_detected[0][1] - LS_detected[1][1]) <= 12 && abs(LS_detected[0][0] - LS_detected[1][0]) * abs(LS_detected[0][1] - LS_detected[1][1]) == 0){
        sens.angle = atan2(LS_ave[0]/2, LS_ave[1]/2);
        sens.distance = sqrt(LS_ave[0]*LS_ave[0]/4 + LS_ave[1]*LS_ave[1]/4);
        sens.slope = sens.angle + (sens.angle > 0 ? -90 : 90);
    }
    else{
        float avex = (float)LS_ave[0] / (float)LS_num_detected;
        float avey = (float)LS_ave[1] / (float)LS_num_detected;
        
        float cov = 0;
        float st_dev_by2_x = 0;
        float st_dev_by2_y = 0;
        for(int i=0; i<LS_num_detected; i++){
            cov += ((float)LS_detected[i][0] - avex) * ((float)LS_detected[i][1] - avey);
            st_dev_by2_x += ((float)LS_detected[i][0] - avex) * ((float)LS_detected[i][0] - avex);
            st_dev_by2_y += ((float)LS_detected[i][1] - avey) * ((float)LS_detected[i][1] - avey);
        }
        cov /= (float)LS_num_detected;
        st_dev_by2_x /= (float)LS_num_detected;
        st_dev_by2_y /= (float)LS_num_detected;
        
        if(st_dev_by2_x > st_dev_by2_y){
            if(st_dev_by2_x == 0){
                sens.slope = 0;
                sens.angle = 0;
                sens.distance = 0;
            }
            else{
                float a = cov / st_dev_by2_x;
                float b = (float)LS_ave[0] - a*(float)LS_ave[1];
                sens.slope = atan2((int)st_dev_by2_x, (int)cov);
                sens.angle = _atan2((double)-a*b, (double)b);
                sens.distance = sqrt((b*b)/(a*a+1));
            }
        }
        else{
            if(st_dev_by2_y == 0){
                sens.slope = 90;
                sens.angle = 0;
                sens.distance = 0;
            }
            else{
                float a = cov / st_dev_by2_y;
                float b = (float)LS_ave[0] - a*(float)LS_ave[1];
                sens.slope = atan2((int)cov, (int)st_dev_by2_y);
                sens.angle = _atan2((double)b, (double)-a*b);
                sens.distance = sqrt((b*b)/(a*a+1));
            }
        }
        if(sens.slope > 90)
            sens.slope -= 180;
        else if(sens.slope <= -90)
            sens.slope += 180;
    }
}

void Line::set_data_LS_OF(int dataX, int dataY){
    LS_detected[LS_num_detected][0] = dataX;
    LS_detected[LS_num_detected++][1] = dataY;
    LS_ave[0] += dataX;
    LS_ave[1] += dataY;
}
