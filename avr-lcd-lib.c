#include "avr-lcd-lib.h"

void _LCD_Delay_ms(int iTimer)
{
	for (int i = 0; i < iTimer; i++)  // mach 1 ms zyklus bis die angegebene menge ms vorbei sibd
	{
		for (int x = 0; x < (MCU_FREQ/10000); x++) //Mach das bis eine ms Vorbei ist
		{
			asm("NOP");	//im Asambly ein no operation welcher genau 1 zyklus dauert und bei mir
		}
	}
}

// LCD init function
void LCDInit(void)
{
	// Wait for stable power supply
	for(unsigned long i = 0; i < 9400; i++){
		asm("NOP");
	}

	ResetBit(LCD_RS_PORT, LCD_RS);		// Select instruction register
	ResetBit(LCD_RW_PORT, LCD_RW);		// Select write

	asm("NOP");

	// Function Set command
	SetBit(LCD_ENABLE_1_PORT, LCD_ENABLE_1);
	SetBit(LCD_ENABLE_2_PORT, LCD_ENABLE_2);

	asm("NOP");

	LCD_DATA_PORT = LCD_FUNCTION_SET;

	asm("NOP");

	ResetBit(LCD_ENABLE_1_PORT, LCD_ENABLE_1);
	ResetBit(LCD_ENABLE_2_PORT, LCD_ENABLE_2);

	LCD_Delay();

	// Function Set command
	SetBit(LCD_ENABLE_1_PORT, LCD_ENABLE_1);
	SetBit(LCD_ENABLE_2_PORT, LCD_ENABLE_2);

	asm("NOP");

	LCD_DATA_PORT = LCD_FUNCTION_SET;

	asm("NOP");

	ResetBit(LCD_ENABLE_1_PORT, LCD_ENABLE_1);
	ResetBit(LCD_ENABLE_2_PORT, LCD_ENABLE_2);

	LCD_Delay();

	// LCD ON/OFF Control command
	SetBit(LCD_ENABLE_1_PORT, LCD_ENABLE_1);
	SetBit(LCD_ENABLE_2_PORT, LCD_ENABLE_2);

	asm("NOP");

	LCD_DATA_PORT = LCD_ON_OFF_CONTROL;

	asm("NOP");

	ResetBit(LCD_ENABLE_1_PORT, LCD_ENABLE_1);
	ResetBit(LCD_ENABLE_2_PORT, LCD_ENABLE_2);

	LCD_Delay();

	// LCD Clear command
	SetBit(LCD_ENABLE_1_PORT, LCD_ENABLE_1);
	SetBit(LCD_ENABLE_2_PORT, LCD_ENABLE_2);

	asm("NOP");

	LCD_DATA_PORT = LCD_CLEAR;

	asm("NOP");

	ResetBit(LCD_ENABLE_1_PORT, LCD_ENABLE_1);
	ResetBit(LCD_ENABLE_2_PORT, LCD_ENABLE_2);

	LCD_Delay();

	// LCD Entry Mode Set command
	SetBit(LCD_ENABLE_1_PORT, LCD_ENABLE_1);
	SetBit(LCD_ENABLE_2_PORT, LCD_ENABLE_2);

	asm("NOP");

	LCD_DATA_PORT = LCD_ENTRY_MODE_SET;

	asm("NOP");

	ResetBit(LCD_ENABLE_1_PORT, LCD_ENABLE_1);
	ResetBit(LCD_ENABLE_2_PORT, LCD_ENABLE_2);

	LCD_Delay();

	return;
}


// LCD Write byte function
void LCDWriteByte(unsigned char ucController, unsigned char ucLCDRegisterSelect,
unsigned char ucLCDData)
{
	unsigned char ucLCDWork = LCD_BUSY;

	// Read busy flag
	while(ucLCDWork & LCD_BUSY){
		ucLCDWork = LCDReadByte(ucController, LCD_COMM );
	}

	// Select data or instruction
	if(ucLCDRegisterSelect == LCD_DATA){
		// Write data
		SetBit(LCD_RS_PORT, LCD_RS);
		}else{
		// Write instructions
		ResetBit(LCD_RS_PORT, LCD_RS);
	}

	LCD_DATA_DDR = LCD_WRITE_DIRECTION;				// Set Port E to output

	ResetBit(LCD_RW_PORT, LCD_RW);				// Set write mode

	asm("NOP");

	// 1 = Upper controller, 2 = Bottom controller
	if(ucController == 1){
		SetBit(LCD_ENABLE_1_PORT, LCD_ENABLE_1);
		asm("NOP");
		LCD_DATA_PORT = ucLCDData;							// Write data
		asm("NOP");
		ResetBit(LCD_ENABLE_1_PORT, LCD_ENABLE_1);
		asm("NOP");
		}else{
		SetBit(LCD_ENABLE_2_PORT, LCD_ENABLE_2);
		asm("NOP");
		LCD_DATA_PORT = ucLCDData;							// Write data
		asm("NOP");
		ResetBit(LCD_ENABLE_2_PORT, LCD_ENABLE_2);
		asm("NOP");
	}

	return;
}


// LCD Read Byte function
unsigned char LCDReadByte(unsigned char ucController,
unsigned char ucLCDRegisterSelect)
{
	unsigned char ucLCDData;

	// Check if data or busy flag is to be read
	if(ucLCDRegisterSelect == LCD_DATA){
		// DDRAM data is read
		SetBit(LCD_RS_PORT, LCD_RS);
		}else{
		// Busy flag and address counter is read
		ResetBit(LCD_RS_PORT, LCD_RS);
	}

	LCD_DATA_DDR = LCD_READ_DIRECTION;				// Set Port E to read

	SetBit(LCD_RW_PORT, LCD_RW);					// Set LCD to read mode

	asm("NOP");

	// 1 = Upper controller, 2 = Bottom controller
	if(ucController == 1){
		SetBit(LCD_ENABLE_1_PORT, LCD_ENABLE_1);
		asm("NOP");
		ucLCDData = LCD_DATA_PIN;								// Read data
		asm("NOP");
		ResetBit(LCD_ENABLE_1_PORT, LCD_ENABLE_1);
		}else{
		SetBit(LCD_ENABLE_2_PORT, LCD_ENABLE_2);
		asm("NOP");
		ucLCDData = LCD_DATA_PIN;								// Read data
		asm("NOP");
		ResetBit(LCD_ENABLE_2_PORT, LCD_ENABLE_2);
	}

	return ucLCDData;
}


// Function to position the cursor
unsigned char LCDCursorXY(unsigned char ucXPos, unsigned char ucYPos)
{
	unsigned char ucController = 1;
	unsigned char ucDDRamAddress = 0x80;

	// Test if ucXPos is valid
	if((ucXPos > 0) && (ucXPos < 41)){
		ucDDRamAddress += (ucXPos - 1);		// Set cursor to X position
		}else{
		// Invalid X position
		ucDDRamAddress += 0x00;						// Set cursor to first position
	}

	// Check which controller needs to be used
	if((ucYPos > 0) && (ucYPos < 3)){
		ucController = 1;

		if(ucYPos == 2){
			ucDDRamAddress += 0x40;		// Set Y Position 2
			}else{
			ucDDRamAddress += 0x00;		// Set Y Position 1
		}
		}else if((ucYPos > 2) && (ucYPos < 5)){
		ucController = 2;

		if(ucYPos == 4){
			ucDDRamAddress += 0x40;		// Set Y Position 4
			}else{
			ucDDRamAddress += 0x00;		// Set Y Position 3
		}
	}

	LCDWriteByte(ucController, LCD_COMM, ucDDRamAddress);

	return ucController;
}


// Write a string to the LCD
void LCDWriteStrXY(unsigned char ucXPos, unsigned char ucYPos, char *cStr)
{
	unsigned char ucController = 1;
	unsigned char i;

	ucController = LCDCursorXY(ucXPos, ucYPos);

	// Write until the max character amount is reached
	for(i = 0; i <= (LCD_MAX_CHAR - ucXPos); i++){
		// Switch to controller 2 when reaching end of line 2
		if(ucController == 1 && (i + ucXPos - 1) == (LCD_MAX_CHAR_LINE * 2)){
			ucController = LCDCursorXY(1, 3);
		}
		// Stop when end of string is reached
		if(cStr[i] == '\0'){
			break;
			}else{
			LCDWriteByte( ucController, LCD_DATA, cStr[i] );	// Write string
		}
	}
	return;
}


// Write a long to the LCD
void LCDWriteLongXY(unsigned char ucXPos, unsigned char ucYPos,unsigned char ucDigit, unsigned long ulInt)
{
	unsigned char ucController = 1;

	long lx = LCDpower(10, ucDigit - 1);
	// Position cursor
	ucController = LCDCursorXY(ucXPos, ucYPos);

	// Write number
	for(int i = 1; i <= ucDigit; i++){
		//LCDWriteByte(ucController, LCD_DATA,((ulInt - (ulInt % (LCDpower(10, ucDigit - i))))/ LCDpower(10, ucDigit - i)) + 0x30);
		LCDWriteByte(ucController, LCD_DATA, (((ulInt-(ulInt % lx)) / lx) + 0x30));
		ulInt = ulInt % lx;
		lx = lx / 10;

	}

	return;
}


void LCDWriteUndefinedLongXY(unsigned char ucPosX, unsigned char ucPosY, long lData){
	long lInt = lData;//Eingabe daten coppieren
	unsigned char ucStellen = 11;// max stellen definieren
	long lx = 1000000000;//divisor devieniren
	while(lData == lInt&&ucStellen!=0){//ausführen bis die erdste zahl weggerechnet wird
		lInt = lInt % lx;//zahl wegrechen
		lx = lx / 10;//divisor verkleinern
		ucStellen--;//für stellen devinition
	}
	LCDWriteLongXY(ucPosX, ucPosY, ucStellen, lData);//daten schreiben
}


void LCDWriteUndefinedDoubleXY(unsigned char ucPosX, unsigned char ucPosY,  double dData){
	char cNegativ = 0;
	if (dData<0)
	{
		dData*=-1;
		cNegativ = 1;
	}
	unsigned char ucStellen = 0;// max stellen definieren
	unsigned char ucZiffern = 11;// max nachkommastellen
	long lInt = dData;//Vorkomma zehlaen löchn
	long lx = 1000000000;//divisor devieniren
	long lKomma=0;// Eingabe daten coppieren
	long lData = lInt;


	while(lInt == lData){//ausführen bis die erdste zahl weggerechnet wird
		lInt = lInt % lx;//zahl wegrechen
		lx = lx / 10;//divisor verkleinern
		ucZiffern--;//Stellen ermitteln
	}
	
	lx = 1000000000;
	lKomma = (dData-lData) *lx;
	while(lKomma != 0){//ausführen bis die letzt zahl weggerechnet wird
		lKomma %= lx;//zahl wegrechen
		lx = lx / 10;//divisor verkleinern
		ucStellen++;//Stellen ERmitteln ERmitteln
	}
	ucZiffern+=ucStellen;
	ucStellen --;
	if (cNegativ==1)
	{
		dData*=-1;
		ucZiffern++;
	}
	
	LCDWriteDoubleXY(ucPosX, ucPosY, ucStellen, ucZiffern, dData);//daten schreiben
}


// Write a double to the LCD
void LCDWriteDoubleXY(unsigned char ucPosX, unsigned char ucPosY, unsigned char ucStellen,unsigned char ucZiffern, double dData){
	unsigned char ucController;
	long lResult = 0;

	dData = LCDround(dData, ucStellen);

	ucController = LCDCursorXY(ucPosX, ucPosY); //Register ermitteln
	lResult = dData; //Mit unterer zeile Daten auf nachkommastellen reduzieren
	dData = dData - lResult;

	if (dData >= 0)
	{
		ucZiffern -= 1;
		if((ucZiffern - ucStellen) >= 1){
			LCDWriteLongXY(ucPosX, ucPosY, (ucZiffern -ucStellen), lResult); //vorkomma stellen Darstellen
			lResult = dData * LCDpower(10, ucStellen); //nachkomma stellen hochrechen für darstellung
			LCDWriteByte(ucController, LCD_DATA, 0x2E);
			LCDWriteLongXY((ucPosX + (ucZiffern - ucStellen) + 1), ucPosY, ucStellen, lResult); //nachkommastellen DArstellen
		}
		else
		{
			LCDWriteStrXY(ucPosX, ucPosY, "ERROR1");
		}
	}
	else
	{
		ucZiffern -= 2;
		if ((ucZiffern - ucStellen) >= 1){
			LCDWriteByte(ucController, LCD_DATA, 0x2D);
			lResult = lResult*-1; //Minus für andere funktionen grauchbar machen
			LCDWriteLongXY(ucPosX +1, ucPosY, (ucZiffern -ucStellen) , lResult); //vorkomma stellen Darstellen
			lResult = (dData*-1) * LCDpower(10, ucStellen); //nachkomma stellen hochrechen für darstellung
			LCDWriteByte(ucController, LCD_DATA, 0x2E);
			LCDWriteLongXY((ucPosX + (ucZiffern - ucStellen) + 2), ucPosY, ucStellen, lResult); //nachkommastellen DArstellen
		}
		else
		{
			LCDWriteStrXY(ucPosX, ucPosY, "ERROR1");
		}
	}

	return;
}


// Clear the LCD screen
void LCDClear(void)
{
	LCDWriteByte(1, LCD_COMM, LCD_CLEAR);
	LCDWriteByte(2, LCD_COMM, LCD_CLEAR);
}

// Add Character to CGRAM
void LCDAddCGRAMChar(unsigned char ucLocation, unsigned char *ucBitmap)
{
	// Check of ucLocation is between 1 and 8
	if(ucLocation > 0 && ucLocation < 9)
	{
		// Value ok
		}else{
		// Default to 1
		ucLocation = 1;
	}

	ucLocation = ((ucLocation - 1) << 3) + 0x40;

	LCDWriteByte(1, LCD_COMM, ucLocation);		// Set CGRAM Address
	LCDWriteByte(2, LCD_COMM, ucLocation);		// Set CGRAM Address

	for(int i = 0; i < 8; i++){
		LCDWriteByte(1, LCD_DATA, ucBitmap[i]);		// Set character bitmap
		LCDWriteByte(2, LCD_DATA, ucBitmap[i]);		// Set character bitmap
	}
}

void LCDWriteCGRAMChar(unsigned char ucXPos, unsigned char ucYPos, unsigned char ucLocation)
{
	unsigned char ucController = 1;

	// Check of ucLocation is between 1 and 8
	if(ucLocation > 0 && ucLocation < 9)
	{
		// Value ok
		}else{
		// Default to 1
		ucLocation = 1;
	}

	ucLocation -= 1;		// Adjust location

	// Position cursor
	ucController = LCDCursorXY(ucXPos, ucYPos );

	// Write CGRAM character to LCD
	LCDWriteByte(ucController, LCD_DATA, ucLocation);

	return;
}

// Returns x raised to the power of y
unsigned long LCDpower(unsigned long x, unsigned int y){
	long lResult = 1;
	if(y == 0){
		return 1;
	}
	for(unsigned int i = 0; i < y; i++){
		lResult = x * lResult;
	}
	return lResult;
}

// Round a number
double LCDround(double dNumber, unsigned char ucDigits){
	long lResult = 0;
	int iNegativ = 0;
	float fZwischenResult = 0;
	if (dNumber < 0){
		dNumber = dNumber * -1;
		iNegativ = 2;
	}
	if (ucDigits > 0)
	{
		dNumber *= LCDpower(10, ucDigits);
	}

	lResult = dNumber; //mit unterer zeile fRunden auf nachkommastellen reduzieren
	fZwischenResult = dNumber - lResult;
	if (fZwischenResult >= 0.5){ // auf oder abrunden herausfinden
		dNumber = dNumber + 1;
	}
	dNumber /= LCDpower(10, ucDigits);

	if (iNegativ == 2){
		dNumber = dNumber * -1;
	}

	return dNumber;
}
