/**sets up basics such as speed, output pins and address window
 * parameters: unsigned char* buffer of DISPLAY_SIZE (of zeroes)
 * return value: void
 **/
void set_up(unsigned char* buffer);

/**resets display to get it ready for data
 * parameters: unsigned char* buffer of DISPLAY_SIZE
 * return value: void
 **/
void reset(unsigned char* buffer);

/**sends command as 16b and data by 8b
 * parameters: unsigned char command to be sent, unsigned char* data to be sent and 
 * int count specifying how much data is being sent
 * return value: void
 **/
void write16(unsigned char command, unsigned char* data, int count);

/**init sequence for the ST7796S display
 **/
void init();