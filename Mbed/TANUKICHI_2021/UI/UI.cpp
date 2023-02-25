/*!
 * TANUKICHI 2021
 *
 * Copyright (c) 2018~2023 Yuta Fujiyama
 *
 * This software is released under the MIT license.
 * see http://opensource.org/licenses/mit-license.php
 */

#include "UI.h"

UI::UI(PinName _cs, PinName _dc, PinName _ssupper, Devices &_devices, Settings &_settings) :
tft(_devices.spi, SPIX_BASE, _cs, _dc),
touch(_devices.i2c),
SSUPPER(_ssupper),
spi(_devices.spi),
devices(_devices),
settings(_settings),
cui(_devices, _settings)
{
    SSUPPER = 1;
}

void UI::begin(){
    tft.begin();
    tft.fillRect(0,0,320,240,0);
    tft.drawCIAO();
    tft.fillRect(60, 200, 200, 3, tft.color(60,60,60));
}

void UI::loading(int state){
    tft.fillRect(60 + loadingState, 200, state - loadingState, 3, 0x03E0);
    loadingState = state;
}

void UI::showHome(){
    backLightCommand(6);
    wait_ms(100);
    tft.drawimg(BackGround, 0, 0, 320, 240);
    drawIcons();
    drawBattery(battVol);
    started = true;
}

void UI::loop(){
    Sensors sensors;
    devices.gyro.getValue(sensors.gyro);
    devices.line.lifted = devices.gyro.lifted;
    if(page != LINE_PAGE)
        devices.line.getValue(sensors.line);
    if(page != CAM_PAGE)
        devices.cam.getValue(sensors.cam);
    if(page != DISTANCE_PAGE)
        devices.distance.getValue(sensors.distance);
    if(page != COMPASS_PAGE)
        devices.gyro.getValue(sensors.gyro);
    if(page != HOLD_PAGE)
        devices.hold.getValue(sensors.hold);
    
    devices.mouse.getValue(sensors.mouse, sensors.gyro.value);
    //getForce();
    
    if(!started)
        showHome();
    
    touch.readData();
    tp.now.x = touch.x[0];
    tp.now.y = touch.y[0];
    tp.now.touches = (touch.touches != 0);
    if(tp.now.touches && !tp.last.touches)
        tp.first = tp.now;
    
    if(!tp.now.touches && tp.last.touches){
        if(tp.first.y - tp.last.y > 30 && tp.first.y > 180 && range(45, tp.last.x, 275) && page != HOME_PAGE){
            page = HOME_PAGE;
            devices.led.set(2);
            showHome();
            drawBattery(battVol);
        }
    }
    
    if(page == HOME_PAGE)
        Home();
    else
        Apps[page]->loop(tp);
    
    tp.last = tp.now;
    
    if(pc.readable())
        cui.loop(pc.getc());
}

void UI::Home(){
    int lastBatt = battVol;
    getBattery();
    if(battVol != lastBatt)
        drawBattery(battVol);
    
    if(!touch.touches && tp.last.touches && !pushing){
        if((page = static_cast<PageName>(identify())) != HOME_PAGE){
            devices.led.set(page+3);
            backLightCommand(6);
            Apps[page]->begin();
        }
    }
    else if(touch.touches && force < 23 && !pushing){
        if((popup_page = static_cast<PageName>(identify())) != HOME_PAGE){
            pushing = true;
            Apps[popup_page]->drawPopup();
        }
    }
    else if(pushed){
        Home_Popup();
    }
    
    if(!pushed && pushing && !tp.now.touches && tp.last.touches)
        pushed = true;
}

void UI::Home_Popup(){
    int responce = OTHER_App;

    responce = Apps[popup_page]->loop_popup(tp);
    if(responce == OPEN_App){
        drawNextPage(popup_page);
        popup_page = HOME_PAGE;
        pushed = false;
        pushing = false;
    }
    else if(responce == CANCEL_App){
        Apps[popup_page]->erase_popup();
        drawIcons();
        pushed = false;
        pushing = false;
        popup_page = HOME_PAGE;
    }
}

void UI::drawIcons(){
    Apps[0]->drawIcon(15,19);
    Apps[1]->drawIcon(95,19);
    Apps[2]->drawIcon(175,19);
    Apps[3]->drawIcon(255,19);
    Apps[4]->drawIcon(15,93);
    Apps[5]->drawIcon(95,93);
    Apps[6]->drawIcon(175,93);
    Apps[7]->drawIcon(255,93);
    Apps[8]->drawIcon(15,167);
    Apps[9]->drawIcon(95,167);
    Apps[10]->drawIcon(175,167);
    Apps[11]->drawIcon(255,167);
}

void UI::drawNextPage(PageName nextPage){
    backLightCommand(6);
    page = nextPage;
    Apps[nextPage]->begin();
}

int UI::identify(){
    for (int i = 0; i < 12; i++){
        if(range(Apps[i]->iconXY.x, tp.last.x, Apps[i]->iconXY.x + 50) && range(Apps[i]->iconXY.y, tp.last.y, Apps[i]->iconXY.y + 50) && range(Apps[i]->iconXY.x, tp.first.x, Apps[i]->iconXY.x + 50) && range(Apps[i]->iconXY.y, tp.first.y, Apps[i]->iconXY.y + 50) && Apps[i]->isApp)
            return i;
    }
    return HOME_PAGE;
}

int UI::getBattery(){
    int vol = Batt.read_u16();
    if(abs(batt - vol) > 500)
        batt = vol;
    else
        batt = batt * 0.99 + Batt.read_u16() * 0.01;
    
    battVol = (float)batt * 0.02776 + 42.39;
    //    printf("%d\n", battVol);
    return battVol;
}

void UI::backLightCommand(char comm){
    READ_REG(SPIX->DR);
    spi.frequency(1000000);
    spi.format(8, 0);
    
    SSUPPER = 0;
    spi.write(comm);
    wait_us(10);
    spi.write(comm);
    SSUPPER = 1;
    wait_ms(5);
}

int UI::getForce(){
    READ_REG(SPIX->DR);
    spi.frequency(1000000);
    spi.format(8, 0);
    
    for(int i=0; i<2; i++){
        SSUPPER = 0;
        spi.write(11);
        wait_us(10);
        force = spi.write(4);
        SSUPPER = 1;
        
        if(force != 11)
            break;
        force = 255;
    }
    
    if(force < 5)
        force = 255;
    
//    printf("%d\n", force);
    
    return force;
}

void UI::drawBattery(int val){
    tft.cutImage(BackGround, 262, 3, 262, 3, 57, 12);

    val = constrain(val, 0, 1260);
    
    if(val > 500){
        int width = constrain((val-1110) / 7.1, 0, 21);
        tft.drawRoundRect(292,4,25,12,3,tft.color(150,150,150));
        tft.drawLine(318,8,318,11,tft.color(150,150,150));
        tft.fillRect(294,5,width,8,val>1150?tft.color(200,200,200):tft.color(200,0,0));
        
        tft.setFont(Arial_14, tft.Right, 1);
        tft.print(val/100, 272, 5, tft.color(200,200,200));
        tft.print(".", 1, 274, 5, tft.color(200,200,200));
        tft.setFont(Arial_14, tft.Left, 1);
        tft.print(val/10 - val/100*10, 275, 5, tft.color(200,200,200));
        tft.print("V", 1, 282, 5, tft.color(200,200,200));
    }
}

uint16_t UI::readPixel(uint16_t x, uint16_t y){
    //    tft.setAddrWindow(x, y, x, y);
    //    tft.writecommand(ILI9341_RAMRD);
    //    READ_REG(SPIX->DR);
    //    tft.readdata();
    //    uint8_t r = tft.readdata();
    //    uint8_t g = tft.readdata();
    //    uint8_t b = tft.readdata();
    //    printf("%d\t",r);
    //    printf("%d\t",g);
    //    printf("%d\r\n",b);
    //    return (r << 10) | (g  << 5) | (b>>1);
    
    
    tft.spi.frequency(8000000);
    tft.writecommand(ILI9341_CASET); // Column addr set
    tft.dc=1;
    tft.spiwrite(x >> 8);
    tft.spiwrite(x & 0xFF);     // XSTART
    tft.spiwrite(x >> 8);
    tft.spiwrite(x & 0xFF);     // XEND
    
    tft.writecommand(ILI9341_PASET); // Row addr set
    tft.dc=1;
    tft.spiwrite(y >> 8);
    tft.spiwrite(y & 0xFF);     // YSTART
    tft.spiwrite(y >> 8);
    tft.spiwrite(y & 0xFF);     // YEND
    
    tft.writecommand(ILI9341_RAMRD);
    tft.dc=1;
    tft.spiread();
    tft.spiread();
    uint8_t r = tft.spiread();
    uint8_t g = tft.spiread();
    uint8_t b = tft.spiread();
    tft.cs=1;
    printf("%d\t",r);
    printf("%d\t",g);
    printf("%d\t",b);
    printf("%d\r\n",((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    
}

