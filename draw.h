#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

/**sets address window as the whole display
 * return value: void**/
void set_addr_window();

/**transforms hexadecimal rgb color into a rgb565 (from 24b to 16b)
 * parameters: int hexrgb - color in hex rgb format
 * return value: uint_16t - color in rgb565 - 16b format
 **/
uint16_t rgb565_color_from_hexrgb(int hexrgb);

/**fills buffer of DISPLAY_SIZE size with color
 * parameters: unsigned char* buffer of DISPLAY_SIZE and int hexadecimal rgb color (24b)
 * return value: void
 **/
void fill_buffer(unsigned char* buffer, int color);

/**draws data from buffer onto display
 * parameters: unsigned char* buffer of DISPLAY_SIZE
 * return value: void
 **/
void fill_display_from_buffer(unsigned char* buffer);