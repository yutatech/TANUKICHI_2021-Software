/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#ifndef CUI_h
#define CUI_h

#include "mbed.h"
#include "../Robot.h"
#include "../Shared.h"

class CUI{
private:
    Devices &devices;
    Settings &settings;
    
    int startPlusCount = 0;
    
    void drawStartPage(){
        printf("\n\n--- TANUKICHI 2021 Settings ---\n");
        printf("k\t: Kicker\n");
        printf("E\t: End\n");
        printf(">> ");
    }
    
    enum PageName{
        HOME_PAGE       = 12,
        KICKER_PAGE     = 0,
        LINE_PAGE       = 1,
        CAM_PAGE        = 2,
        COMPASS_PAGE    = 3,
        SETTINGS_PAGE   = 4,
        MOTOR_PAGE      = 5,
        DISTANCE_PAGE   = 6,
        ISP_PAGE        = 7,
        HOLD_PAGE       = 8
    };
    
    PageName page = HOME_PAGE;
public:
    CUI(Devices &_devices, Settings &_settings) : devices(_devices), settings(_settings) {
    }
    
    void interrupt(){
        loop(pc.getc());
    }
    bool running = false;
    
    void loop(char str){
        if(running){
            switch(page){
                case HOME_PAGE:
                    switch(str){
                        case 'k':
                            page = KICKER_PAGE;
                            printf("k\n");
                            printf("put kick angle\n");
                            printf("0\t: -45\n");
                            printf("1\t: -30\n");
                            printf("2\t: -15\n");
                            printf("3\t: 0\n");
                            printf("4\t: 15\n");
                            printf("5\t: 30\n");
                            printf("6\t: 45\n");
                            printf("B\t: Back\n");
                            printf(">> ");
                            break;
                        case 'E':
                            printf("E\n");
                            running = false;
                            printf("Good bye\n");
                            printf("-------------------------------\n");
                            break;
                    }
                    break;
                case KICKER_PAGE:
                    switch(str){
                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                            printf("%d\n", ((int)str - 51) * 15);
                            devices.kicker.kick(((int)str - 51) * 15, 1);
                            printf(">> ");
                            break;
                        case 'B':
                            page = HOME_PAGE;
                            drawStartPage();
                            break;
                    }
                    break;
            }
        }
        else{
            if(str == '+'){
                printf("+");
                startPlusCount++;
                if(startPlusCount >= 3){
                    running = true;
                    startPlusCount = 0;
                    drawStartPage();
                }
            }
        }
    }
};

#endif /* CUI_h */
