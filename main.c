#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <WiringPi/wiringPi/wiringPi.h>
#include <WiringPi/wiringPi/wiringPiSPI.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define CS_ACTIVE digitalWrite(8, LOW)
#define CS_IDLE digitalWrite(8, HIGH)
#define CD_COMMAND digitalWrite(25, LOW)
#define CD_DATA digitalWrite(25, HIGH)
#define RESET digitalWrite(27, LOW)

#define WRITE_DATA 0x2C

#define DISPLAY_SIZE 480 * 320 * 2
//4.0' SPI Module ST7796

//display pin 5 (DC/RS) from rpi pin 25 (HIGH = data, LOW = command)
//0x2C = data write command = CC
//0x2A = column address set
//0x2B = page address set
//display pin 4 (RESET) from rpi pin 27 (LOW = reset)
//"CS_ACTIVE" = CS LOW, "CS_IDLE" = CS HIGH, gpio 8


const int CHANNEL = 1;

//resets display
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

//write data
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

//init sequence
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
    printf("%x %x %x\n", red8, green8, blue8);

    int red5 = red8 / 255.0 * 31;
    int green6 = green8 / 255.0 * 63;
    int blue5 = blue8 / 255.0 * 31;
    printf("%x %x %x\n", red5, green6, blue5);

    int red5_shifted = red5 << 11;
    int green6_shifted = green6 << 5;
    printf("%x %x \n", red5_shifted, green6_shifted);

    return red5_shifted | green6_shifted | blue5;
}

//fill buffer of DISPLAY_SIZE size by color(rgb 24bit)
void fill_buffer(unsigned char* buffer, int color){
    uint16_t rgb565_color = rgb565_color_from_hexrgb(color);
    for (int i = 0; i < DISPLAY_SIZE; i += 2) {
        buffer[i] = rgb565_color >> 8;
        buffer[i + 1] = rgb565_color;
    }
    printf("%d\n", buffer[0]);
    printf("%d\n", buffer[1]);
}


void fill_display_from_buffer(unsigned char* buffer){
    CD_DATA;
    for(int i = 0; i < (DISPLAY_SIZE); i += 2){
        wiringPiSPIDataRW(CHANNEL, buffer + i, 2);
    }
    CS_IDLE;
}

int main(void){
    int fd, result;
    unsigned char buffer[DISPLAY_SIZE] = {0x00};
    fd = wiringPiSPISetup(CHANNEL, 4000000);
    wiringPiSetupGpio();

    //set pins used as outputs
    pinMode(8, OUTPUT);
    pinMode(27, OUTPUT);
    pinMode(25, OUTPUT);

    reset(buffer);

    printf("after reset, before init\n");
    sleep(2);

    init();

    printf("after init, before black\n");
    sleep(1);

    set_addr_window();
    sleep(1);

    for (int i = 0; i < DISPLAY_SIZE; i++){
        buffer[i] = 0x00;
    }

    CS_ACTIVE;
    CD_COMMAND;
    unsigned char command16[] = {0x00, WRITE_DATA};
    wiringPiSPIDataRW(CHANNEL, command16, 2);
    sleep(1);

    fill_display_from_buffer(buffer);

    //----------------------------------------------
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
    /*for (int i = 0; i < DISPLAY_SIZE; i += 2) {
        buffer2[i] = 0x42;
        buffer2[i + 1] = 0xdf;
    }*/

    fill_display_from_buffer(buffer2);

    return EXIT_SUCCESS;
}