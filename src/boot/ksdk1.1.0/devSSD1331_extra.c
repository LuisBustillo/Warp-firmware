#include <stdint.h>

/*
 *	config.h needs to come first
 */
#include "config.h"

#include "fsl_spi_master_driver.h"
#include "fsl_port_hal.h"

#include "SEGGER_RTT.h"
#include "gpio_pins.h"
#include "warp.h"
#include "devSSD1331_extra.h"
#include "devSSD1331_font.h"


volatile uint8_t	inBuffer[1];
volatile uint8_t	payloadBytes[1];


/*
 *	Override Warp firmware's use of these pins and define new aliases.
 */
enum
{
	kSSD1331PinMOSI		= GPIO_MAKE_PIN(HW_GPIOA, 8),
	kSSD1331PinSCK		= GPIO_MAKE_PIN(HW_GPIOA, 9),
	kSSD1331PinCSn		= GPIO_MAKE_PIN(HW_GPIOB, 11),
	kSSD1331PinDC		= GPIO_MAKE_PIN(HW_GPIOA, 12),
	kSSD1331PinRST		= GPIO_MAKE_PIN(HW_GPIOB, 0),
};

// Useful variables to note, not used in this implementation
// static const uint8_t WIDTH = 0x5F;
// static const uint8_t HIEGHT = 0x3F;
static const uint8_t width = 0x5F;
static const uint8_t height = 0x3F;
uint8_t cursor_x = 0;
uint8_t cursor_y = 0;
uint8_t textsize_x = 1;
uint8_t textsize_y = 1;
bool wrap = true;

// Colours stored in arrays
uint8_t textcolor[3];
uint8_t textbg[3];


void
drawRect(uint8_t start_x, uint8_t start_y, uint8_t width_x, uint8_t width_y, uint8_t * colours) {
	uint8_t red = colours[0];
	uint8_t green = colours[1];
	uint8_t blue = colours[2];

	writeCommand(kSSD1331CommandDRAWRECT);
	writeCommand(start_x);
	writeCommand(start_y);
	writeCommand(start_x + width_x);
	writeCommand(start_y + width_y);
	// Border - assume same as rect.
	writeCommand(blue);
	writeCommand(green);
	writeCommand(red);
	// Rect
	writeCommand(blue);
	writeCommand(green);
	writeCommand(red);

}

void
drawLine(uint8_t start_x, uint8_t start_y, uint8_t end_x, uint8_t end_y, uint8_t * colours){
	uint8_t red = colours[0];
	uint8_t green = colours[1];
	uint8_t blue = colours[2];
	writeCommand(kSSD1331CommandDRAWLINE);
	writeCommand(start_x);
	writeCommand(start_y);
	writeCommand(end_x);
	writeCommand(end_y);
	writeCommand(blue);
	writeCommand(green);
	writeCommand(red);
}

// Implementing writing to screen based on implementation in Adafruit_GFX Library:
void
drawChar(uint8_t x, uint8_t y, uint16_t c, uint8_t * colour, uint8_t * bg, uint8_t size_x, uint8_t size_y){

	if((x >= width)            || // Clip right - Assuming horizontal screen
		 (y >= height)           || // Clip bottom
		 ((x + 6 * size_x - 1) < 0) || // Clip left
		 ((y + 8 * size_y - 1) < 0))   // Clip top
			return;

	for(int8_t i=0; i<5; i++ ) { // Char bitmap = 5 columns
			// ToDo : Store font in program memory & implement the line reader function from the program memory:
			uint8_t line = font[c * 5 + i];
			for(int8_t j=0; j<8; j++, line >>= 1) {
					if(line & 1) {
						if (size_x ==1 && size_y==1){
							drawLine(x+i, y+j,x+i,y+j,colour);
						} else{
							drawRect(x+i*size_x, y+j*size_y, size_x, size_y, colour);
						}
					} //else if(bg != colour) {
						//drawRect(x+i*size_x, y+j*size_y, size_x, size_y, bg);
					//}
			}
			// Commenting out all background colour lines to speed up writing
			// if(bg != colour) { // If opaque, draw vertical line for last column
			// 		drawRect(x+5*size_x, y, size_x, 8*size_y, bg);
			// }
	}
}


void
writeText(char * text) {
	unsigned char * txt = (unsigned char *) text;
	for (uint8_t i = 0; txt[i]; i++){
		uint8_t c = txt[i];
		if (c=='\n'){
			cursor_x = 0;
			cursor_y += textsize_y*8;
		} else if(c!= '\r') {                 // Ignore carriage returns
				if(wrap && ((cursor_x + textsize_x * 6) > width)) { // Off right?
						cursor_x  = 0;                 // Reset x to zero,
						cursor_y += textsize_y * 8;    // advance y one line
				}
				drawChar(cursor_x, cursor_y, c, textcolor, textbg, textsize_x, textsize_y);
				cursor_x += textsize_x * 6;          // Advance x one char
		}
	}

}

// Implementation of float to str from https://www.geeksforgeeks.org/convert-floating-point-number-string/


// Reverses a string 'str' of length 'len'
void reverse(char* str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

// Converts a given integer x to string str[].
// d is the number of digits required in the output.
// If d is more than the number of digits in x,
// then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    // If number of digits required is more, then
    // add spaces at the beginning - removed as not desired behaviour in our case
    // while (i < d)
    //     str[i++] = ' ';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

// Converts a floating-point/double number to a string.
void ftoa(float n, char* res)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 0);

    // check for display option after point

    res[i] = '.'; // add dot

    // Get the value of fraction part upto given no.
    // of points after dot. The third parameter
    // is needed to handle cases like 233.007
    fpart = fpart * 100;

    intToStr((int)fpart, res + i + 1, 2);

}

void
writeNumber(int16_t number) {
	// Setup buffer for text output
	char text[5];
	if (number < 0){
		// print negative sign if required
		writeText("-");
		number = -number;
	}
	// Alternative is to use standard library itoa function:
  // itoa(number,text,5);
	// Write number into the buffer, size 5
	intToStr(number,text,5);
	writeText(text);
}

void
writeFloat(float n) {
	// Setup buffer
	char res[20];
	// SEGGER_RTT_printf(0,"Attempting To Print\n"); - used for debugging
	if (n<0){
		// Print negative sign
		writeText("-");
		SEGGER_RTT_WriteString(0,"-");
		n = -n;
	}
	// Run float conversion to string, we assume only 2 decimal places
	// due to line of best fit accuracy, could be adjusted for more general case
	ftoa(n,res);
	SEGGER_RTT_printf(0,"%s\n",res);
	writeText(res);
}

// Wrapper to clear the screen
void
clearScreen(void) {
	writeCommand(kSSD1331CommandCLEAR);
	writeCommand(0x00);
	writeCommand(0x00);
	writeCommand(0x5F);
	writeCommand(0x3F);
	cursor_x = 0;
	cursor_y = 0;
}