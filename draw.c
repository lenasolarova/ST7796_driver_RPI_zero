#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "main.h"
#include "draw.h"
#include "prep.h"

void set_addr_window(){
    CS_ACTIVE;
    int x1 = 0x00;
    int x2 = 0x140;
    int y1 = 0x00;
    int y2 = 0x1E0;

    uint8_t x_buf[] = {x1 >> 8, x1 & 0xFF, x2 >> 8, x2 & 0xFF};
    uint8_t y_buf[] = {y1 >> 8, y1 & 0xFF, y2 >> 8, y2 & 0xFF};

    write16(0x2A, x_buf, 4);
    write16(0x2B, y_buf, 4);
    CS_IDLE;
}

uint16_t rgb565_color_from_hexrgb(int hexrgb){
    int red8 = (hexrgb & 0xff0000) >> 16;
    int green8 = (hexrgb & 0x00ff00) >> 8;
    int blue8 = (hexrgb & 0x0000ff);

    int red5 = red8 / 255.0 * 31;
    int green6 = green8 / 255.0 * 63;
    int blue5 = blue8 / 255.0 * 31;

    int red5_shifted = red5 << 11;
    int green6_shifted = green6 << 5;

    return red5_shifted | green6_shifted | blue5;
}

void fill_buffer(unsigned char* buffer, int color){
    uint16_t rgb565_color = rgb565_color_from_hexrgb(color);
    for (int i = 0; i < DISPLAY_SIZE; i += 2) {
        buffer[i] = rgb565_color >> 8;
        buffer[i + 1] = rgb565_color;
    }
}

void fill_display_from_buffer(unsigned char* buffer){
    CD_DATA;
    for(int i = 0; i < (DISPLAY_SIZE); i += 2){
        wiringPiSPIDataRW(CHANNEL, buffer + i, 2);
    }
    CS_IDLE;
}