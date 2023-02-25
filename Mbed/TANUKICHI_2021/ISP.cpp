// ArduinoISP
// Copyright (c) 2018~2023 Yuta Fujiyama
// Copyright (c) 2008-2011 Randall Bohn
// If you require a license, see
// https://opensource.org/licenses/bsd-license.php

#include "ISP.h"

ISP::ISP(SPI &_spi) : spi(_spi) {}

void ISP::setup_ISP() {
    SERIAL.baud(19200);
    READ_REG(SPIX->DR);
    spi.frequency(1000000);
    spi.format(8, 0);
    for(int i = 0; i < 5; i++){
//        if(target != i)
            Resetpin[i] = 1;
    }
    SSLINE1 = 1;
    SSLINE2 = 1;
    SSLINE3 = 1;
    SSLINE4 = 1;
    SSUPPER = 1;
    SSMOUSE = 1;
}

void ISP::heartbeat() {
    if (timer.read() < 40)
        return;
    timer.reset();
    timer.start();
    if (hbval > 192) hbdelta = -hbdelta;
    if (hbval < 32) hbdelta = -hbdelta;
    hbval += hbdelta;
    //    analogWrite(LED_HB, 256 - hbval);
}


void ISP::Reset_target(bool Reset) {
    Resetpin[target] = ((Reset && rst_active_high) || (!Reset && !rst_active_high)) ? 1 : 0;
}


uint8_t ISP::getch() {
    while (!SERIAL.readable());
    return SERIAL.getc();
}

void ISP::fill(int n) {
    for (int x = 0; x < n; x++) {
        buff[x] = getch();
    }
}

void ISP::prog_lamp(int state) {
    if (true) {
        //        digitalWrite(LED_PMODE, !state);
    }
}

uint8_t ISP::spi_transaction(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    spi.write(a);
    spi.write(b);
    spi.write(c);
    return spi.write(d);
}

void ISP::empty_reply() {
    if (CRC_EOP == getch()) {
        SERIAL.putc((char)STK_INSYNC);
        SERIAL.putc((char)STK_OK);
    } else {
        Error++;
        SERIAL.putc((char)STK_NOSYNC);
    }
}

void ISP::breply(uint8_t b) {
    if (CRC_EOP == getch()) {
        SERIAL.putc((char)STK_INSYNC);
        SERIAL.putc((char)b);
        SERIAL.putc((char)STK_OK);
    } else {
        Error++;
        SERIAL.putc((char)STK_NOSYNC);
    }
}

void ISP::get_version(uint8_t c) {
    switch (c) {
        case 0x80:
            breply(HWVER);
            break;
        case 0x81:
            breply(SWMAJ);
            break;
        case 0x82:
            breply(SWMIN);
            break;
        case 0x93:
            breply('S'); // serial programmer
            break;
        default:
            breply(0);
    }
}

void ISP::set_parameters() {
    // call this after reading paramter packet into buff[]
    param.devicecode = buff[0];
    param.revision   = buff[1];
    param.progtype   = buff[2];
    param.parmode    = buff[3];
    param.polling    = buff[4];
    param.selftimed  = buff[5];
    param.lockbytes  = buff[6];
    param.fusebytes  = buff[7];
    param.flashpoll  = buff[8];
    // ignore buff[9] (= buff[8])
    // following are 16 bits (big endian)
    param.eeprompoll = beget16(&buff[10]);
    param.pagesize   = beget16(&buff[12]);
    param.eepromsize = beget16(&buff[14]);
    
    // 32 bits flashsize (big endian)
    param.flashsize = buff[16] * 0x01000000
    + buff[17] * 0x00010000
    + buff[18] * 0x00000100
    + buff[19];
    
    // avr devices have active low Reset, at89sx are active high
    rst_active_high = (param.devicecode >= 0xe0);
}

void ISP::start_pmode() {
    
    // Reset target before driving SCK or MOSI
    
    // SPI.begin() will configure SS as output,
    // so SPI master mode is selected.
    // We have defined Reset as pin 10,
    // which for many arduino's is not the SS pin.
    // So we have to configure Reset as output here,
    // (Reset_target() first sets the correct level)
    Reset_target(true);
    
    // See avr datasheets, chapter "Serial_PRG Programming Algorithm":
    
    // Pulse Reset after SCK is low:
    wait_ms(20); // discharge SCK, value arbitrally chosen
    Reset_target(false);
    // Pulse must be minimum 2 target CPU clock cycles
    // so 100 usec is ok for CPU speeds above 20KHz
    wait_us(100);
    Reset_target(true);
    
    spi.frequency(1000000/6);
    spi.format(8, 0);
    // Send the enable programming command:
    wait_ms(50); // datasheet: must be > 20 msec
    spi_transaction(0xAC, 0x53, 0x00, 0x00);
    pmode = 1;
}

void ISP::end_pmode() {
    //    SPI.end();
    // We're about to take the target out of Reset
    // so configure SPI pins as input
    Reset_target(false);
    pmode = 0;
}

void ISP::universal() {
    uint8_t ch;
    
    fill(4);
    ch = spi_transaction(buff[0], buff[1], buff[2], buff[3]);
    breply(ch);
}

void ISP::flash(uint8_t hilo, unsigned int addr, uint8_t data) {
    spi_transaction(0x40 + 8 * hilo,
                    addr >> 8 & 0xFF,
                    addr & 0xFF,
                    data);
}

void ISP::commit(unsigned int addr) {
    if (true) {
        prog_lamp(LOW);
    }
    spi_transaction(0x4C, (addr >> 8) & 0xFF, addr & 0xFF, 0);
    if (true) {
        wait_ms(PTIME);
        prog_lamp(HIGH);
    }
}

unsigned int ISP::current_page() {
    if (param.pagesize == 32) {
        return here & 0xFFFFFFF0;
    }
    if (param.pagesize == 64) {
        return here & 0xFFFFFFE0;
    }
    if (param.pagesize == 128) {
        return here & 0xFFFFFFC0;
    }
    if (param.pagesize == 256) {
        return here & 0xFFFFFF80;
    }
    return here;
}

uint8_t ISP::write_flash_pages(int length) {
    int x = 0;
    unsigned int page = current_page();
    while (x < length) {
        if (page != current_page()) {
            commit(page);
            page = current_page();
        }
        flash(LOW, here, buff[x++]);
        flash(HIGH, here, buff[x++]);
        here++;
    }
    
    commit(page);
    
    return STK_OK;
}

void ISP::write_flash(int length) {
    fill(length);
    if (CRC_EOP == getch()) {
        SERIAL.putc((char) STK_INSYNC);
        SERIAL.putc((char) write_flash_pages(length));
    } else {
        Error++;
        SERIAL.putc((char) STK_NOSYNC);
    }
}

// write (length) bytes, (start) is a byte address
uint8_t ISP::write_eeprom_chunk(unsigned int start, unsigned int length) {
    // this writes byte-by-byte,
    // page writing may be faster (4 bytes at a time)
    fill(length);
    prog_lamp(LOW);
    for (unsigned int x = 0; x < length; x++) {
        unsigned int addr = start + x;
        spi_transaction(0xC0, (addr >> 8) & 0xFF, addr & 0xFF, buff[x]);
        wait_ms(45);
    }
    prog_lamp(HIGH);
    return STK_OK;
}

uint8_t ISP::write_eeprom(unsigned int length) {
    // here is a word address, get the byte address
    unsigned int start = here * 2;
    unsigned int remaining = length;
    if (length > param.eepromsize) {
        Error++;
        return STK_FAILED;
    }
    while (remaining > EECHUNK) {
        write_eeprom_chunk(start, EECHUNK);
        start += EECHUNK;
        remaining -= EECHUNK;
    }
    write_eeprom_chunk(start, remaining);
    return STK_OK;
}

void ISP::program_page() {
    char result = (char) STK_FAILED;
    unsigned int length = 256 * getch();
    length += getch();
    char memtype = getch();
    // flash memory @here, (length) bytes
    if (memtype == 'F') {
        write_flash(length);
        return;
    }
    if (memtype == 'E') {
        result = (char)write_eeprom(length);
        if (CRC_EOP == getch()) {
            SERIAL.putc((char) STK_INSYNC);
            SERIAL.putc(result);
        } else {
            Error++;
            SERIAL.putc((char) STK_NOSYNC);
        }
        return;
    }
    SERIAL.putc((char)STK_FAILED);
    return;
}

uint8_t ISP::flash_read(uint8_t hilo, unsigned int addr) {
    return spi_transaction(0x20 + hilo * 8,
                           (addr >> 8) & 0xFF,
                           addr & 0xFF,
                           0);
}

char ISP::flash_read_page(int length) {
    for (int x = 0; x < length; x += 2) {
        uint8_t low = flash_read(LOW, here);
        SERIAL.putc((char) low);
        uint8_t high = flash_read(HIGH, here);
        SERIAL.putc((char) high);
        here++;
    }
    return STK_OK;
}

char ISP::eeprom_read_page(int length) {
    // here again we have a word address
    int start = here * 2;
    for (int x = 0; x < length; x++) {
        int addr = start + x;
        uint8_t ee = spi_transaction(0xA0, (addr >> 8) & 0xFF, addr & 0xFF, 0xFF);
        SERIAL.putc((char) ee);
    }
    return STK_OK;
}

void ISP::read_page() {
    char result = (char)STK_FAILED;
    int length = 256 * getch();
    length += getch();
    char memtype = getch();
    if (CRC_EOP != getch()) {
        Error++;
        SERIAL.putc((char) STK_NOSYNC);
        return;
    }
    SERIAL.putc((char) STK_INSYNC);
    if (memtype == 'F') result = flash_read_page(length);
    if (memtype == 'E') result = eeprom_read_page(length);
    SERIAL.putc(result);
}

void ISP::read_signature() {
    if (CRC_EOP != getch()) {
        Error++;
        SERIAL.putc((char) STK_NOSYNC);
        return;
    }
    SERIAL.putc((char) STK_INSYNC);
    uint8_t high = spi_transaction(0x30, 0x00, 0x00, 0x00);
    SERIAL.putc((char) high);
    uint8_t middle = spi_transaction(0x30, 0x00, 0x01, 0x00);
    SERIAL.putc((char) middle);
    uint8_t low = spi_transaction(0x30, 0x00, 0x02, 0x00);
    SERIAL.putc((char) low);
    SERIAL.putc((char) STK_OK);
}
//////////////////////////////////////////
//////////////////////////////////////////


////////////////////////////////////
////////////////////////////////////
void ISP::avrisp() {
    uint8_t ch = getch();
    switch (ch) {
        case '0': // signon
            Error = 0;
            empty_reply();
            break;
        case '1':
            if (getch() == CRC_EOP) {
                SERIAL.putc((char) STK_INSYNC);
                SERIAL.printf("AVR ISP");
                SERIAL.putc((char) STK_OK);
            }
            else {
                Error++;
                SERIAL.putc((char) STK_NOSYNC);
            }
            break;
        case 'A':
            get_version(getch());
            break;
        case 'B':
            fill(20);
            set_parameters();
            empty_reply();
            break;
        case 'E': // extended parameters - ignore for now
            fill(5);
            empty_reply();
            break;
        case 'P':
            if (!pmode)
                start_pmode();
            empty_reply();
            break;
        case 'U': // set address (word)
            here = getch();
            here += 256 * getch();
            empty_reply();
            break;
            
        case 0x60: //STK_PROG_FLASH
            getch(); // low addr
            getch(); // high addr
            empty_reply();
            break;
        case 0x61: //STK_PROG_DATA
            getch(); // data
            empty_reply();
            break;
            
        case 0x64: //STK_PROG_PAGE
            program_page();
            break;
            
        case 0x74: //STK_READ_PAGE 't'
            read_page();
            break;
            
        case 'V': //0x56
            universal();
            break;
        case 'Q': //0x51
            Error = 0;
            end_pmode();
            empty_reply();
            state = false;
            break;
            
        case 0x75: //STK_READ_SIGN 'u'
            read_signature();
            break;
            
            // expecting a command, not CRC_EOP
            // this is how we can get back in sync
        case CRC_EOP:
            Error++;
            SERIAL.putc((char) STK_NOSYNC);
            break;
            
            // anything else we will return STK_UNKNOWN
        default:
            Error++;
            if (CRC_EOP == getch())
                SERIAL.putc((char)STK_UNKNOWN);
            else
                SERIAL.putc((char)STK_NOSYNC);
    }
}

void ISP::loop_ISP(void) {
    heartbeat();
    if (SERIAL.readable()) {
        state = true;
        avrisp();
    }
}
