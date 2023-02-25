/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef WirelessLog_h
#define WirelessLog_h

#include "mbed.h"

class WirelessLog {
/*  2 bytes for 1 data
          2byte             1byte
    |7|6|5|4|3|2|1|0|-|7|6|5|4|3|2|1|0|
    
    2byte 7bit    - 1
    2byte 6-4bit  - index num
    2byte 3bit    - sign
    2byte 2-0bit  - high bit of data
    1byte 7bit    - 0
    1byte 6-0bit  - low bit of data
 */
public:
    enum Base{
        dec,
        bin
    };
private:
    Serial &wireless;
    Ticker whenSendLog;
    
    int numOfIndex = 8;
    int numOfIndexInProcess = 8;
    int *buf;
    int *dataInProcess;
    
    bool updated;
    int process = 0;
    
    Base type = dec;
    
    bool enable = false;
    
    void sendLog();
    
    int abs(int x){return x>0?x:-x;}
    int constrain(int amt,int low,int high){return amt<low?low:(amt>high?high:amt);}
public:
    WirelessLog(Serial &_wireless);
    void setNumOfIndex(int num);
    void setData(int *data);
    void setType(Base base);
    void setEnable(bool _enable);
};

#endif /* WirelessLog_h */
