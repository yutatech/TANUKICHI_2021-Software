/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "Math.h"

const float sinval[91] = {0   , //0
                        0.0175, //1
                        0.0349, //2
                        0.0523, //3
                        0.0698, //4
                        0.0872, //5
                        0.1045, //6
                        0.1219, //7
                        0.1392, //8
                        0.1564, //9
                        0.1736, //10
                        0.1908, //11
                        0.2079, //12
                        0.2250, //13
                        0.2419, //14
                        0.2588, //15
                        0.2756, //16
                        0.2924, //17
                        0.3090, //18
                        0.3256, //19
                        0.3420, //20
                        0.3584, //21
                        0.3746, //22
                        0.3907, //23
                        0.4067, //24
                        0.4226, //25
                        0.4384, //26
                        0.4540, //27
                        0.4695, //28
                        0.4848, //29
                        0.5000, //30
                        0.5150, //31
                        0.5299, //32
                        0.5446, //33
                        0.5592, //34
                        0.5736, //35
                        0.5878, //36
                        0.6018, //37
                        0.6157, //38
                        0.6293, //39
                        0.6428, //40
                        0.6561, //41
                        0.6691, //42
                        0.6820, //43
                        0.6947, //44
                        0.7071, //45
                        0.7193, //46
                        0.7314, //47
                        0.7431, //48
                        0.7547, //49
                        0.7660, //50
                        0.7771, //51
                        0.7880, //52
                        0.7986, //53
                        0.8090, //54
                        0.8192, //55
                        0.8290, //56
                        0.8387, //57
                        0.8480, //58
                        0.8572, //59
                        0.8660, //60
                        0.8746, //61
                        0.8829, //62
                        0.8910, //63
                        0.8988, //64
                        0.9063, //65
                        0.9135, //66
                        0.9205, //67
                        0.9272, //68
                        0.9336, //69
                        0.9397, //70
                        0.9455, //71
                        0.9511, //72
                        0.9563, //73
                        0.9613, //74
                        0.9659, //75
                        0.9703, //76
                        0.9744, //77
                        0.9781, //78
                        0.9816, //79
                        0.9848, //80
                        0.9877, //81
                        0.9903, //82
                        0.9925, //83
                        0.9945, //84
                        0.9962, //85
                        0.9976, //86
                        0.9986, //87
                        0.9994, //88
                        0.9998, //89
                        1.0000  //90
};

float sin(int sindirect) {
    sindirect %= 360;
    if (sindirect < 0)
        sindirect += 360;
    
    if (sindirect <= 90)
        return sinval[sindirect];
    else if (sindirect <= 180)
        return sinval[180 - sindirect];
    else if (sindirect <= 270)
        return -sinval[sindirect - 180];
    else
        return -sinval[360 - sindirect];
}

float cos(int cosdirect) {
    cosdirect %= 360;
    if (cosdirect < 0)
        cosdirect += 360;
    
    if (cosdirect <= 90)
        return sinval[90 - cosdirect];
    else if (cosdirect <= 180)
        return -sinval[cosdirect - 90];
    else if (cosdirect <= 270)
        return -sinval[270 - cosdirect];
    else
        return sinval[cosdirect - 270];
}

float tan(int tandirect) {
    tandirect %= 360;
    if (tandirect < 0)
        tandirect += 360;
    
    if(cos(tandirect) == 0)
        return 0;
    else
        return sin(tandirect) / cos(tandirect);
}

int _atan2(double _y, double _x) {
    if(_y == 0 && _x >= 0)
        return 0;
    else if(_y == 0 && _x < 0)
        return 180;
    else if(_x == 0 && _y > 0)
        return 90;
    else if(_x == 0 && _y < 0)
        return -90;
    double x = abs(_x);
    double y = abs(_y);
    double   z;
    bool    c;
    
    c = y < x;
    if (c)z = (float)y / x;
    else  z = (float)x / y;
    
    double a;
    //a = z * (-1556 * z + 6072);                     //2次曲線近似
    //a = z * (z * (-448 * z - 954) + 5894);          //3次曲線近似
    a = z * (z * (z * (829 * z - 2011) - 58) + 5741); //4次曲線近似
    
    if (c) {
        if (_x > 0) {
            if (_y < 0)a *= -1;
        }
        if (_x < 0) {
            if (_y > 0)a = 18000 - a;
            if (_y < 0)a = a - 18000;
        }
    }
    
    if (!c) {
        if (_x > 0) {
            if (_y > 0) a = 9000 - a;
            if (_y < 0) a = a - 9000;
        }
        if (_x < 0) {
            if (_y > 0) a = a + 9000;
            if (_y < 0) a = -a - 9000;
        }
    }
    
    return (a+50)/100;
}

int atan2(int _y, int _x) {
    return _atan2((int)_y, (int)_x);
}

int asin2(double _y, double _r){
    //-90 ~ 90
    if(_r == 0)
        return 0;
    float z = 10 * _y / _r;
    if(abs(z) > 10)
        return z >= 0 ? 90 : -90;
    for(int i = 0; i < 91; i++){
        if(sinval[i] * 10 <= abs(z))
            return z >= 0 ? i : -i;
    }
    return 0;
}

long pow(int value, int count){
    if(!count)
        return 1;
    
    long data = value;
    for(; count > 1; count--)
        data *= value;
    return data;
}

float sqrt(long f){
    double s = f,t;
    if(f == 0) return 0;
    do {
        t = s;
        s = (t + f / t) / 2;
    } while(s < t);
    return t;
}
