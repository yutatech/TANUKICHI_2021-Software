/*
MIT License

Copyright (c) 2019 Yuta Fujiyama
Copyright (c) 2019 Matthew DeHaven

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "MCP23017.h"

uint8_t MCP23017::bitForPin(uint8_t pin){
    return pin%8;
}

uint8_t MCP23017::regForPin(uint8_t pin, uint8_t portAaddr, uint8_t portBaddr){
    return(pin<8) ?portAaddr:portBaddr;
}

uint8_t MCP23017::readRegister(uint8_t addr){
    char buffer[] = {addr};
    i2c.write(i2caddr, buffer, 1);
    
    char data;
    i2c.read(i2caddr + 1, &data, 1);
    
    return data;
}

void MCP23017::writeRegister(uint8_t regAddr, uint8_t regValue){
    char  buffer[2];
    
    buffer[0] = regAddr;
    buffer[1] = regValue;
    i2c.write(i2caddr, buffer, 2);
}

void MCP23017::updateRegisterBit(uint8_t pin, uint8_t pValue, uint8_t portAaddr, uint8_t portBaddr) {
    uint8_t regValue;
    uint8_t regAddr=regForPin(pin,portAaddr,portBaddr);
    uint8_t bit=bitForPin(pin);
    regValue = readRegister(regAddr);
    
    bitWrite(regValue,bit,pValue);
    
    writeRegister(regAddr,regValue);
}

void MCP23017::begin() {
    writeRegister(MCP23017_IODIRA,0xff);
    writeRegister(MCP23017_IODIRB,0xff);
}

void MCP23017::pinMode(uint8_t p, uint8_t d) {
    updateRegisterBit(p,(d==0),MCP23017_IODIRA,MCP23017_IODIRB);
}

uint8_t MCP23017::readGPIO(uint8_t b) {
    char buffer[] = {(b == 0 ? (char)MCP23017_GPIOA : (char)MCP23017_GPIOB)};
    i2c.write(i2caddr, buffer, 1);
    
    char data;
    i2c.read(i2caddr + 1, &data, 1);
    
    return data;
}

void MCP23017::digitalWrite(uint8_t pin, uint8_t d) {
    uint8_t gpio;
    uint8_t bit=bitForPin(pin);
    
    uint8_t regAddr=regForPin(pin,MCP23017_OLATA,MCP23017_OLATB);
    gpio = readRegister(regAddr);
    
    bitWrite(gpio,bit,d);
    
    regAddr=regForPin(pin,MCP23017_GPIOA,MCP23017_GPIOB);
    writeRegister(regAddr,gpio);
}

void MCP23017::pullUp(uint8_t p, uint8_t d) {
    updateRegisterBit(p,d,MCP23017_GPPUA,MCP23017_GPPUB);
}

uint8_t MCP23017::digitalRead(uint8_t pin) {
    uint8_t bit=bitForPin(pin);
    uint8_t regAddr=regForPin(pin,MCP23017_GPIOA,MCP23017_GPIOB);
    return (readRegister(regAddr) >> bit) & 0x1;
}

void MCP23017::setupInterrupts(uint8_t mirroring, uint8_t openDrain, uint8_t polarity){
    uint8_t ioconfValue=readRegister(MCP23017_IOCONA);
    bitWrite(ioconfValue,6,mirroring);
    bitWrite(ioconfValue,2,openDrain);
    bitWrite(ioconfValue,1,polarity);
    writeRegister(MCP23017_IOCONA,ioconfValue);
    
    ioconfValue=readRegister(MCP23017_IOCONB);
    bitWrite(ioconfValue,6,mirroring);
    bitWrite(ioconfValue,2,openDrain);
    bitWrite(ioconfValue,1,polarity);
    writeRegister(MCP23017_IOCONB,ioconfValue);
}

void MCP23017::setupInterruptPin(uint8_t pin, uint8_t mode) {
    
    updateRegisterBit(pin,(mode!=CHANGE),MCP23017_INTCONA,MCP23017_INTCONB);
    updateRegisterBit(pin,(mode==FALLING),MCP23017_DEFVALA,MCP23017_DEFVALB);
    updateRegisterBit(pin,1,MCP23017_GPINTENA,MCP23017_GPINTENB);
    
}

uint8_t MCP23017::getLastInterruptPin(){
    uint8_t intf;
    
    intf=readRegister(MCP23017_INTFA);
    for(int i=0;i<8;i++) if (bitRead(intf,i)) return i;
    
    intf=readRegister(MCP23017_INTFB);
    for(int i=0;i<8;i++) if (bitRead(intf,i)) return i+8;
    
    return MCP23017_INT_ERR;
    
}
uint8_t MCP23017::getLastInterruptPinValue(){
    uint8_t intPin=getLastInterruptPin();
    if(intPin!=MCP23017_INT_ERR){
        uint8_t intcapreg=regForPin(intPin,MCP23017_INTCAPA,MCP23017_INTCAPB);
        uint8_t bit=bitForPin(intPin);
        return (readRegister(intcapreg)>>bit) & (0x01);
    }
    
    return MCP23017_INT_ERR;
}


