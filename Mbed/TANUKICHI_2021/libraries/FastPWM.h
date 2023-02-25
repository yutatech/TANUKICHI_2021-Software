/**
 *  This file is a modification of Erik's FastPWM (https://os.mbed.com/users/Sissors/code/FastPWM/)
 */

#include "mbed.h"

#ifndef FASTPWM_H
#define FASTPWM_H

class FastPWM : public PwmOut {
public:
    FastPWM(PinName pin, int prescaler = -1);
    ~FastPWM(); 
    
    void period(double seconds);
    void period_ms(int ms);
    void period_us(double us);
    void period_ticks(uint32_t ticks);
    void pulsewidth(double seconds);
    void pulsewidth_ms(int ms);
    void pulsewidth_us(int us);
    void pulsewidth_us(double us);
    void pulsewidth_ticks(uint32_t ticks);
    void write(double duty);
    double read(void);
    FastPWM& operator= (double value);
    operator double();
    int prescaler(int value);
    
private:
    void initFastPWM(void);
    
    uint32_t setPrescaler( uint32_t reqScale );
    int calcPrescaler(uint64_t clocks);
    uint32_t getPeriod(void);
    
    void updateTicks( uint32_t prescaler );
    uint32_t bits;
    
    double _duty;
    
    double dticks, dticks_us;
    int iticks_ms, iticks_us;
    
    bool dynamicPrescaler;
    
    void *fast_obj;
    
    typedef __IO uint32_t* CHANNEL_P_T;

    typedef struct  {
        CHANNEL_P_T channel;
        uint32_t clk_prescaler;
    } fastpwm_struct;
};
#endif
