/*
 *	See https://github.com/adafruit/Adafruit-SSD1331-OLED-Driver-Library-for-Arduino for the Arduino driver.
 */

typedef enum
{
	kSSD1331ColororderRGB		= 1,
	kSSD1331DelaysHWFILL		= 3,
	kSSD1331DelaysHWLINE		= 1,
} SSD1331Constants;

typedef enum
{
	kSSD1331CommandDRAWLINE		= 0x21,
	kSSD1331CommandDRAWRECT		= 0x22,
	kSSD1331CommandCLEAR		= 0x25,
	kSSD1331CommandFILL		= 0x26,
	kSSD1331CommandSETCOLUMN	= 0x15,
	kSSD1331CommandSETROW		= 0x75,
	kSSD1331CommandCONTRASTA	= 0x81,
	kSSD1331CommandCONTRASTB	= 0x82,
	kSSD1331CommandCONTRASTC	= 0x83,
	kSSD1331CommandMASTERCURRENT	= 0x87,
	kSSD1331CommandSETREMAP		= 0xA0,
	kSSD1331CommandSTARTLINE	= 0xA1,
	kSSD1331CommandDISPLAYOFFSET	= 0xA2,
	kSSD1331CommandNORMALDISPLAY	= 0xA4,
	kSSD1331CommandDISPLAYALLON	= 0xA5,
	kSSD1331CommandDISPLAYALLOFF	= 0xA6,
	kSSD1331CommandINVERTDISPLAY	= 0xA7,
	kSSD1331CommandSETMULTIPLEX	= 0xA8,
	kSSD1331CommandSETMASTER	= 0xAD,
	kSSD1331CommandDISPLAYOFF	= 0xAE,
	kSSD1331CommandDISPLAYON	= 0xAF,
	kSSD1331CommandPOWERMODE	= 0xB0,
	kSSD1331CommandPRECHARGE	= 0xB1,
	kSSD1331CommandCLOCKDIV		= 0xB3,
	kSSD1331CommandPRECHARGEA	= 0x8A,
	kSSD1331CommandPRECHARGEB	= 0x8B,
	kSSD1331CommandPRECHARGEC	= 0x8C,
	kSSD1331CommandPRECHARGELEVEL	= 0xBB,
	kSSD1331CommandVCOMH		= 0xBE,
	kSSD1331CommandCOPY			= 0x23,
} SSD1331Commands;

int	devSSD1331init(void);

void writeText(char * text);

void writeNumber(int16_t number);

void writeFloat(float n);

void clearScreen(void);


// void writeText(uint8_t c);

//
// class SSD1331 {
// private:
// 	uint8_t width;
// 	uint8_t height;
// 	uint8_t cursor_x;
// 	uint8_t cursor_y;
//
// 	int writeCommand(uint8_t commandByte);
// 	void drawRect(uint8_t start_x, uint8_t start_y, uint8_t width_x, uint8_t width_y, uint8_t * color_pointer);
// 	void drawChar(uint8_t x, uint8_t y, uint16_t c, uint8_t * colour, uint8_t * bg, uint8_t size_x, uint8_t size_y);
//
//
// public:
// 	void writeText(uint8_t c);
// 	int init(void);
// 	uint8_t textsize_x;
// 	uint8_t textsize_y;
// 	uint8_t textcolor[3];
// 	uint8_t textbg[3];
// 	bool wrap;
//
// 	Screen(uint8_t w,	uint8_t h){
// 		width = w;
// 		height = h;
// 		cursor_x = 0;
// 		cursor_y = 0;
// 		textsize_x = 1;
// 		textsize_y = 1;
// 		wrap = true;
// 		textcolor[0] = 0x00 ;
// 		// Green for hacker vibes
// 		textcolor[1] = 0x3E ;
// 		textcolor[2] = 0x00 ;
// 		textbg[0] = 0x00;
// 		textbg[1] = 0x00;
// 		textbg[2] = 0x00;
//
// 	}
// }
