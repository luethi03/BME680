#ifndef _AVR_LCD_LIB_H_
#define _AVR_LCD_LIB_H_

// Includes
#include <avr/io.h>

// User defines - Change according to your needs
// Port pin defines
#define LCD_DATA_PORT					PORTE // The LCD data PORT register
#define LCD_DATA_DDR					DDRE 	// The LCD data DDR register
#define LCD_DATA_PIN					PINE	// The LCD data PIN register
#define LCD_RS_PORT      			PORTA	// Port for the LCD Register Select
#define LCD_RW_PORT      			PORTA	// Port for the LCD Read / Write
#define LCD_ENABLE_1_PORT			PORTA	// Port for the Enable controller 1
#define LCD_ENABLE_2_PORT			PORTA	// Port for the Enable controller 2
#define LCD_RS      					0x01	// LCD Register Select
#define LCD_RW      					0x02	// LCD Read / Write
#define LCD_ENABLE_1					0x04	// Enable controller 1
#define LCD_ENABLE_2					0x08	// Enable controller 2

// Defines
#define LCD_LIB_VERSION				"1.0"
#define MCU_FREQ					8000000

// LCD commands
#define LCD_FUNCTION_SET   		0x38
#define LCD_ON_OFF_CONTROL  	0x0C
#define LCD_ENTRY_MODE_SET  	0x06
#define LCD_CLEAR							0x01
#define LCD_HOME							0x02

// LCD register select
#define LCD_COMM							0x00
#define LCD_DATA							0x01

// Masks
#define LCD_WRITE_DIRECTION		0xFF
#define LCD_READ_DIRECTION		0x00
#define LCD_BUSY							0x80

/*** LCD-Konstanten ***/
// Maximum characters to be drawn by the LCDWriteStrXY function.
// 160 is the maximum the LCD can display, before data has to be
// overwritten.
#define LCD_MAX_CHAR					160
#define LCD_MAX_CHAR_LINE			40
#define LCD_CONTROLLER_1			0		// LCD controller 1
#define LCD_CONTROLLER_2			1		// LCD Controller 2

// Macros
//******************************************************************************
#define SetBit( Byte, Bit ) ( Byte |= Bit );
#define ResetBit( Byte, Bit ) ( Byte &= ( ~Bit ) );
// Very inaccurate delay
#define LCD_Delay() for(unsigned int i = 0; i < 235; i++) asm("NOP");

// Functions
//******************************************************************************

/*
 * @brief	in this function it has a repeating cycle to count one ms 
 *			if you use it yourself make sure to update this header file with
 *			your MCU frequency or else it wont work properly
 * 
 * @param	(int) iTimer - it is the amount of ms you need in your code 
 *
 * @return	(void) None
 */
void _LCD_Delay_ms(int iTimer);

/**
 *	@brief			LCDInit Initialise the LCD
 *
 *	@param			(void) -
 *	
 *	@return			(void) -
 */
void LCDInit(void);


/**
 *	@brief			LCDWriteByte Write 1 Byte to the LCD
 *
 *	@param			(unsigned char) ucController - Selection of Controller chip 1 or 2
 *					(unsigned char) ucLCDRegisterSelect - 0:Command, 1:Data
 *					(unsigned char) ucLCDData - Data
 *
 *	@return			(void) -
 */
void LCDWriteByte( unsigned char,  unsigned char, unsigned char );


/**
 *	@brief			LCDReadByte Read 1 Byte from the LCD
 *
 *	@param			(unsigned char) ucController - Selection of Controller chip 1 or 2
 *					(unsigned char) ucLCDRegisterSelect - 0:Busyflag, 1:Daten
 *
 *	@return			(unsigned char) ucLCDData - Data
 */
unsigned char LCDReadByte( unsigned char, unsigned char );


/**
 *	@brief			LCDCursorXY Position cursor on the LCD
 *
 *	@param			(unsigned char) ucXPos - X-Coordinate
 *					(unsigned char) ucYPos - Y-Coordinate
 *
 *	@return			(unsigned char) ucController - Returns on which controller the coordinates are
 *
 *	X:  01-40
 *	Y:  Line 1 : Controller 1
 *		Line 2 : Controller 1
 *		Line 3 : Controller 2
 *		Line 4 : Controller 2
 */
unsigned char LCDCursorXY( unsigned char, unsigned char );


/**
 *	@brief			LCDWriteStrXY Write a sring with the start coordinates of XPos and YPos
 *
 *	@param			(unsigned char) ucXPos - X-Coordinate
 *					(unsigned char) ucYPos - Y-Coordinate
 *					(unsigned char*) ucStr - String to write
 *
 *	@return			(void) -
 */
void LCDWriteStrXY(	unsigned char, unsigned char, char*);


/**
 * @brief			it can detect numbers into characters and write it at the same time
 *
 * @author			Daniel Frehner
 *
 * @param			(unsigned char) ucPosX -  horizontal coordinate
 * @param			(unsigned char) ucPosy -  vertical coordinate
 * @param			(unsigned char) ucStellen - this are the amount of numbers behind the decimal point
 * @param			(unsigned char) ucZiffern - this parameter it he amount of letters that are written to the LCD  
 * @param			(double) ulZahl - it is the number that get written at the end 
 *
 * @return			(void) None 
 */
void LCDWriteDoubleXY(unsigned char ucPosX, unsigned char ucPosY, unsigned char ucStellen, unsigned char ucZiffern, double dData);


/**
 *	@brief			LCDWriteLongXY Write datatype long to the LCD
 *
 *  @author			Daniel Frehner
 *
 *	@param			(unsigned char) ucXPos - X-Coordinate
 *					(unsigned char) ucYPos - Y-Coordinate
 *					(unsigned long) ulInt - Number to write
 *
 *	@return			(void) -
 */
void LCDWriteUndefinedLongXY(unsigned char ucPosX, unsigned char ucPosY,  long lData);


/**
 *	@brief			LCDWriteLongXY Write datatype double to the LCD
 *
 *  @author			Daniel Frehner
 *
 *	@param			(unsigned char) ucXPos - X-Coordinate
 *					(unsigned char) ucYPos - Y-Coordinate
 *					(unsigned long) dData - Number to write
 *
 *	@return			(void) -
 */
void LCDWriteUndefinedDoubleXY(unsigned char ucPosX, unsigned char ucPosY,  double dData);



/**
 *	@brief			LCDWriteLongXY Write datatype long to the LCD
 *
 *	@param			(unsigned char) ucXPos - X-Coordinate
 *					(unsigned char) ucYPos - Y-Coordinate
 *					(unsigned char) ucZif - Number of digits to write
 *					(unsigned long) ulInt - Number to write
 *
 *	@return			(void) -
 */
void LCDWriteLongXY(unsigned char ucXPos, unsigned char ucYPos,unsigned char ucDigit, unsigned long ulInt);

/**
 *	@brief			LCDClear Clear the full display.
 *
 *	@param			(void) -
 *
 *	@return			(void) -
 */
void LCDClear(void);

/**
 *	@brief			LCDAddChar Add a new character to the CGRAM to both controllers.
 *
 *	@param			(unsigned char) ucLocation - The CGRAM location. Value between 1 and 8.
 *	@param			(unsigned char) ucBitmap - The Bitmap array with the character info.
 *
 *	@return			(void) -
 */
void LCDAddCGRAMChar(unsigned char, unsigned char*);

/**
 *	@brief			LCDWriteCGRAMChar Write a character from the CGRAM to the display.
 *	
 *	@param			(unsigned char) ucYPos - Y-Coordinate
 *	@param			(unsigned char) ucLocation - The CGRAM location. Value between 1 and 8.
 *
 *	@return			(void) -
 */
void LCDWriteCGRAMChar(unsigned char, unsigned char, unsigned char);

/**
 *	@brief			LCDpower Returns  raised to the power of y
 *
 *	@param			(unsigned long) x
 *	@param			(unsigned long) y
 *
 *	@return			(unsigned long) - the result
 */
unsigned long LCDpower(unsigned long x, unsigned int y);

/**
 *	@brief			LCDround Round a number
 *
 *	@param			(double) dNumber - Number to be rounded
 *	@param			(double) ucDigits - How accurate the rounding should be (in digits)
 *
 *	@return			(double) - Result
 */
double LCDround(double dNumber, unsigned char ucDigits);

#endif
