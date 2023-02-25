/**
 *  This file is a modification of Erik's FastPWM (https://os.mbed.com/users/Sissors/code/FastPWM/)
 */

#include "FastPWM.h"

FastPWM::FastPWM(PinName pin, int prescaler) : PwmOut(pin) {
    fast_obj = NULL;
    initFastPWM();
    this->prescaler(prescaler);
    
    period(0.02);
    write(0);
}

FastPWM::~FastPWM( void ) {
    if (fast_obj != NULL)
        free(fast_obj);
} 

void FastPWM::period(double seconds) {
    if (dynamicPrescaler)
        calcPrescaler((uint64_t)(seconds * (double) SystemCoreClock));

     period_ticks(seconds * dticks + 0.5);
}

void FastPWM::period_ms(int ms) {
    if (dynamicPrescaler)
        calcPrescaler(ms * (SystemCoreClock / 1000));
        
    period_ticks(ms * iticks_ms);
}

void FastPWM::period_us(double us) {
    if (dynamicPrescaler)
        calcPrescaler((uint64_t)(us * (double)(SystemCoreClock / 1000000)));
        
    period_ticks(us * dticks_us + 0.5);
}

void FastPWM::pulsewidth(double seconds) {
    pulsewidth_ticks(seconds * dticks + 0.5);
}

void FastPWM::pulsewidth_ms(int ms) {
    pulsewidth_ticks(ms * iticks_ms);
}

void FastPWM::pulsewidth_us(int us) {
    pulsewidth_ticks(us * iticks_us);
}

void FastPWM::pulsewidth_us(double us) {
    pulsewidth_ticks(us * dticks_us + 0.5);
}

void FastPWM::write(double duty) {
    _duty=duty;
    pulsewidth_ticks(duty*getPeriod());
}

double FastPWM::read( void ) {
    return _duty;
    }
    
FastPWM & FastPWM::operator= (double value) {
    write(value);
    return(*this);
    }
    
FastPWM::operator double() {
    return _duty;
}

int FastPWM::prescaler(int value) {
    int retval;
    if (value == -1) {
        dynamicPrescaler = true;
        value = 0;
    }
    else
        dynamicPrescaler = false;
    
    retval = setPrescaler(value);
    updateTicks(retval);
    return retval;
}

void FastPWM::updateTicks( uint32_t prescaler ) {
    dticks = SystemCoreClock / (double)prescaler;
    dticks_us = dticks / 1000000.0f;
    iticks_us = (int)(dticks_us + 0.5);
    iticks_ms = (int)(dticks_us * 1000.0 + 0.5);
}

int FastPWM::calcPrescaler(uint64_t clocks) {
    uint32_t scale = (clocks >> bits) + 1;
    uint32_t retval = setPrescaler(scale);
    updateTicks(retval);
    return retval;
}     

void FastPWM::initFastPWM( void ) {
    fast_obj = malloc(sizeof(fastpwm_struct));
    ((fastpwm_struct*)fast_obj)->channel = (&((TIM_TypeDef*)_pwm.pwm)->CCR1 + _pwm.channel - 1);
    
    uint32_t current_hz = SystemCoreClock / (((TIM_TypeDef*)_pwm.pwm)->PSC + 1) / (((TIM_TypeDef*)_pwm.pwm)->ARR+1);
    ((fastpwm_struct*)fast_obj)->clk_prescaler = (current_hz + 1) / 50;
    if ( (((fastpwm_struct*)fast_obj)->clk_prescaler == 0 ) || (((fastpwm_struct*)fast_obj)->clk_prescaler > 16)) {
        ((fastpwm_struct*)fast_obj)->clk_prescaler = 1;
    }
    
    ((TIM_TypeDef*)_pwm.pwm)->CR1 |= TIM_CR1_ARPE;
    
    bits = 16;
}

void FastPWM::pulsewidth_ticks( uint32_t ticks ) {
    *(((fastpwm_struct*)fast_obj)->channel) = ticks;
}

void FastPWM::period_ticks( uint32_t ticks ) {
    ((TIM_TypeDef*)_pwm.pwm)->ARR = ticks - 1;
}

uint32_t FastPWM::getPeriod( void ) {
    return ((TIM_TypeDef*)_pwm.pwm)->ARR + 1;
}

uint32_t FastPWM::setPrescaler(uint32_t reqScale) {
    if (reqScale == 0) {
        return (((TIM_TypeDef*)_pwm.pwm)->PSC + 1) * ((fastpwm_struct*)fast_obj)->clk_prescaler;
    }
    if (reqScale > (uint32_t)(((fastpwm_struct*)fast_obj)->clk_prescaler<<16)) {
        reqScale = ((fastpwm_struct*)fast_obj)->clk_prescaler<<16;
    }
    
    ((TIM_TypeDef*)_pwm.pwm)->PSC = (reqScale + ((fastpwm_struct*)fast_obj)->clk_prescaler - 1)/((fastpwm_struct*)fast_obj)->clk_prescaler - 1;

    return setPrescaler(0);
}
