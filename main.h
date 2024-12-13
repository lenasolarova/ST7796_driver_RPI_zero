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
#define CHANNEL 1