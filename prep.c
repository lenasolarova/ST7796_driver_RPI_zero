#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "main.h"
#include "prep.h"
#include "draw.h"

void set_up(unsigned char* buffer){
    int fd = wiringPiSPISetup(CHANNEL, 4000000);
    wiringPiSetupGpio();

    //set pins used as outputs
    pinMode(8, OUTPUT);
    pinMode(27, OUTPUT);
    pinMode(25, OUTPUT);

    reset(buffer);

    init();

    printf("after init, before black\n");
    sleep(1);

    set_addr_window();
    sleep(1);
}

void reset(unsigned char* buffer){
    CS_IDLE;
    RESET;
    sleep(2);
    digitalWrite(27, HIGH);

    CS_ACTIVE;
    CD_COMMAND;
    *buffer = 0x00;
    wiringPiSPIDataRW(CHANNEL, buffer, 4);

    CS_IDLE;
}

void write16(unsigned char command, unsigned char* data, int count){
    CS_ACTIVE;
    CD_COMMAND;
    unsigned char command16[] = {0x00, command};
    wiringPiSPIDataRW(CHANNEL, command16, 2);
    if (count > 0){
        CD_DATA;
        int i = 0;
        while(count > 0){
            wiringPiSPIDataRW(CHANNEL, data + i++, 1);
            count--;
        }
    }
    CS_IDLE;
}

void init(){
    //format: command, amount of data, data
    const unsigned char ST7796S[] = {
        0xF0, 1, 0xC3,
        0xF0, 1, 0x96, 
        0x36, 1, 0x68,
        0x3A, 1, 0x05,
        0xB0, 1, 0x80, 
        0xB6, 2, 0x00, 0x02,
        0xB5, 4, 0x02, 0x03, 0x00, 0x04,
        0xB1, 2, 0x80, 0x10, 
        0xB4, 1, 0x00,
        0xB7, 1, 0xC6,
        0xC5, 1, 0x24,
        0xE4, 1, 0x31,
        0xE8, 8, 0x40, 0x8A, 0x00, 0x00, 0x29, 0x19, 0xA5, 0x33,
        0xC2, 0,
        0xA7, 0,
        0xE0, 14, 0xF0, 0x09, 0x13, 0x12, 0x12, 0x2B, 0x3C, 0x44, 0x4B, 0x1B, 0x18, 0x17, 0x1D, 0x21,
        0xE1, 14, 0xF0, 0x09, 0x13, 0x0C, 0x0D, 0x27, 0x3B, 0x44, 0x4D, 0x0B, 0x17 ,0x17, 0x1D, 0x21,				
        0x36, 1, 0x48,
        0xF0, 1, 0xC3,
        0xF0, 1, 0x69, 
        0x13, 0,
        0x11, 0,
        0x29, 0,
    };
    int size = sizeof(ST7796S);
    int curr = 0;
    unsigned char data[20] = {0};
    while(size > 0){
        unsigned char cmd = ST7796S[curr++];
        unsigned char count = ST7796S[curr++];
        for(int i = 0; i < count; i++){
            data[i] = ST7796S[curr + i];
        }
        write16(cmd, data, count);
        curr += count;
        size -= (count + 2);
    }
}