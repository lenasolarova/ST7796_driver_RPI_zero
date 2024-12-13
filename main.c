#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "main.h"
#include "draw.h"
#include "prep.h"

int main(void){
    unsigned char buffer[DISPLAY_SIZE] = {0x00};
    set_up(buffer);

    for (int i = 0; i < DISPLAY_SIZE; i++){
        buffer[i] = 0x00;
    }

    CS_ACTIVE;
    CD_COMMAND;
    unsigned char command16[] = {0x00, WRITE_DATA};
    wiringPiSPIDataRW(CHANNEL, command16, 2);

    sleep(1);

    fill_display_from_buffer(buffer);

    printf("after black, before white\n");
    sleep(2);

    CS_ACTIVE;

    unsigned char buffer1[DISPLAY_SIZE] = {0xFF};
    for (int i = 0; i < DISPLAY_SIZE; i++) {
        buffer1[i] = 0xFF;
    }

    fill_display_from_buffer(buffer1);

    CS_ACTIVE;

    unsigned char buffer2[DISPLAY_SIZE] = {0xFF};
    int color = 0x556aeb;
    fill_buffer(buffer2, color);

    fill_display_from_buffer(buffer2);

    return EXIT_SUCCESS;
}