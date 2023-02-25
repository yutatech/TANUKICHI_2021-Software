/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef Statistics_h
#define Statistics_h

#include "Math.h"

class Statistics {
private:
public:
    double variance = 0;
    double standardDeviation = 0;
    double average = 0;
    double summation = 0;
//    double mean = 0;
    
    template<typename T>
    Statistics& calculate(T *val, int num, T None){
        double sum = 0;
        double sum2 = 0;
        int validNum = 0;
        for(int i = 0; i < num; i++){
            if(val[i] != None){
                sum += val[i];
                sum2 += val[i]*val[i];
                validNum++;
            }
        }
        
        if(validNum == 0){
            average = 0;
            variance = 0;
            standardDeviation = 0;
            return *this;
        }
        
        average = sum / validNum;
        
        variance = sum2/validNum - average*average;
        
        standardDeviation = sqrt((long)variance);
        
        summation = sum;
        
        return *this;
    }
    
    template <typename T, typename U>
    Statistics& calculate(T (U::*func)(), int num, T None){
        double sum = 0;
        double sum2 = 0;
        int validNum = 0;
        for(int i = 0; i < num; i++){
            T val = func(i);
            if(val != None){
                sum += val;
                sum2 += val*val;
                validNum++;
            }
        }
        
        if(validNum == 0){
            average = 0;
            variance = 0;
            standardDeviation = 0;
            return *this;
        }
        
        average = sum / validNum;
        
        variance = sum2/validNum - average*average;
        
        standardDeviation = sqrt((long)variance);
        
        return *this;
    }
};

#endif /* Statistics_h */
