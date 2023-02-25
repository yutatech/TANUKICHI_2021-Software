/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef Shared_h
#define Shared_h

#include "mbed.h"

#include "Position.h"
#include "WirelessLog.h"

#if (defined(POSITION_OF) && defined(POSITION_DF)) || (!defined(POSITION_OF) && !defined(POSITION_DF))
    #error invalid definition of position
#endif

#define MOSI        PA_7
#define MISO        PA_6
#define SCK         PA_5
#define SDA         PC_9
#define SCL         PA_8
#define TX_PC       PA_2
#define RX_PC       PA_3
#define TX_MV       PC_6
#define RX_MV       PC_7
#define TX_WIRELESS PC_10
#define RX_WIRELESS PC_11

#define SPIX_BASE   SPI1_BASE
#define SPIX        ((SPI_TypeDef *) SPIX_BASE)

#define BATT        PC_0

#define L_STATE     PA_10
#define L_ERROR     PA_11

#define START       PB_13
#define SPEAKER     PD_2
#define HOLD_F      PC_4
#define HOLD_B      PC_2

#define RST_LINE1   PA_0
#define RST_LINE2   PC_5
#define RST_LINE3   PC_1
#define RST_LINE4   PB_1
#define RST_UPPER   PB_10
#define SS_LINE1    PA_1
#define SS_LINE2    PC_3
#define SS_LINE3    PC_13
#define SS_LINE4    PB_0
#define SS_UPPER    PB_2
#define SS_MOUSE    PA_4

#define MA          PB_4
#define MB          PB_9_ALT1
#define MC          PB_7
#define MD          PB_3
#define MRST        PB_8
#define KICKER_R    PB_6
#define KICKER_L    PB_5
#define DRIBBLER_F  PB_15
#define DRIBBLER_B  PB_14
#define CS          PA_9
#define DC          PC_8

#define CYCLE       0.01

#define DONTMOVE    UINT16_MAX
const int None = UINT16_MAX;

#define DBL_MAX     1.79769e+308

#define FRONT       0
#define RIGHT       1
#define BACK        2
#define LEFT        3

extern DigitalIn    Start;
extern DigitalOut   L_State;
extern DigitalOut   L_Error;
extern AnalogIn     Batt;

extern DigitalOut   RSTLINE1;
extern DigitalOut   RSTLINE2;
extern DigitalOut   RSTLINE3;
extern DigitalOut   RSTLINE4;
extern DigitalOut   RSTUPPER;
extern DigitalOut   SSLINE1;
extern DigitalOut   SSLINE2;
extern DigitalOut   SSLINE3;
extern DigitalOut   SSLINE4;
extern DigitalOut   SSUPPER;
extern DigitalOut   SSMOUSE;

extern Serial pc;
extern Serial wireless;
extern WirelessLog Log;

void _sp_tick(void);

void sound(int frequency);

void printBIN(int data, int dig);

#endif /* Shared_h */

/* I2C ADDRESS 7bit
 EbcoderF       0x07
 EbcoderB       0x08
 ToF            0x09 ~ 0x14
 FT6206         0x38
 BNO055         0x28 / (0x29)
 EEPROM         0x50
 ToF-Default    0x29
 I/O Expander   0x20
*/
