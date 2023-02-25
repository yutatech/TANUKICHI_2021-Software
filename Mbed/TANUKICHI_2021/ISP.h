// ArduinoISP
// Copyright (c) 2018~2023 Yuta Fujiyama
// Copyright (c) 2008-2011 Randall Bohn
// If you require a license, see
// https://opensource.org/licenses/bsd-license.php

#ifndef ISP_h
#define ISP_h

#include "mbed.h"
#include "shared.h"
#include "Robot.h"

#define PROG_FLICKER true

#define SPI_CLOCK     (1000000/6)

#define LED_HB      7
#define LED_ERR     6
#define LED_PMODE   5

#define BAUDRATE  19200

#define HWVER 2
#define SWMAJ 1
#define SWMIN 18

#define STK_OK      0x10
#define STK_FAILED  0x11
#define STK_UNKNOWN 0x12
#define STK_INSYNC  0x14
#define STK_NOSYNC  0x15
#define CRC_EOP     0x20 //ok it is a space...
#define beget16(addr) (*addr * 256 + *(addr+1) )
#define PTIME 30
#define EECHUNK (32)

#define LOW     0
#define HIGH    1

#define SERIAL pc

class ISP{
private:
    DigitalOut Resetpin[5] = {RSTLINE1, RSTLINE2, RSTLINE3, RSTLINE4, RSTUPPER};
    SPI &spi;
    
    int Error = 0;
    int pmode = 0;
    // address for reading and writing, set by 'U' command
    unsigned int here;
    uint8_t buff[256]; // global block storage
    
    typedef struct param {
        uint8_t devicecode;
        uint8_t revision;
        uint8_t progtype;
        uint8_t parmode;
        uint8_t polling;
        uint8_t selftimed;
        uint8_t lockbytes;
        uint8_t fusebytes;
        uint8_t flashpoll;
        uint16_t eeprompoll;
        uint16_t pagesize;
        uint16_t eepromsize;
        uint32_t flashsize;
    }
    parameter;
    
    parameter param;
    
    // this provides a heartbeat on pin 9, so you can tell the software is running.
    uint8_t hbval = 128;
    int8_t hbdelta = 8;
    Timer timer;
    
    bool rst_active_high;
    
    
    void heartbeat();
    void Reset_target(bool Reset);
    uint8_t getch();
    void fill(int n);
    void prog_lamp(int state);
    uint8_t spi_transaction(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
    void empty_reply();
    void breply(uint8_t b);
    void get_version(uint8_t c);
    void set_parameters();
    void start_pmode();
    void end_pmode();
    void universal();
    void flash(uint8_t hilo, unsigned int addr, uint8_t data);
    void commit(unsigned int addr);
    unsigned int current_page();
    uint8_t write_flash_pages(int length);
    void write_flash(int length);
    uint8_t write_eeprom_chunk(unsigned int start, unsigned int length);
    uint8_t write_eeprom(unsigned int length);
    void program_page();
    uint8_t flash_read(uint8_t hilo, unsigned int addr);
    char flash_read_page(int length);
    char eeprom_read_page(int length);
    void read_page();
    void read_signature();
    void avrisp();
public:
    bool state;
    int target = 1;
    ISP(SPI &_spi);
    void setup_ISP();
    void loop_ISP();
};

#endif /* ISP_h */
