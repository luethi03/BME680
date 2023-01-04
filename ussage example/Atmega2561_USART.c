/*** Atmega2561 USART Source-Datei ***/
/******************************************************************************
Kunde:          EPH Electronics AG
Projekt:        Atmega Basics

Dateiname:      Atmega2561_USART.c
Version:        1.00 Exp.
Prozessor:      ATmega2561
Autor:			Marlon Tanner

Copyright:      EPH Electronics AG
                Ratihard 5
                8253 Diessenhofen

Datum:          02.11.20
Beschreibung:   Sourcefile des Programms.
*******************************************************************************/

/*** AVR-Includes ***/
#include <avr/io.h>
#include <avr/interrupt.h>

//#include "Atmega2561.h"			// definitions
#include "Atmega2561_USART.h"	// USART receive & send functions
//#include "samsilib.h"			// CRC calculations, conversions

// Variables
unsigned char ucUSART1RecvHeader[HEADER_SIZE_MAX];	// Empfangsvariable für USART1
unsigned char ucUSART1RecvData[DATA_SIZE_MAX];		// Empfangsvariable für USART1
unsigned int  uiRecvDataLength = 0;						// Received datalength

void USART0Init(unsigned int uiUbrr)
{
	PRR0 &= PRUSART0;			// Power Reduction für USART0 deaktivieren.
	UBRR0H = (uiUbrr >> 8);		// Baudrate High Register setzen
	UBRR0L = uiUbrr;			// Baudrate Low Register  setzen
	UCSR0A = INIT_UCSR0A;		// UCSRA0 setzen
	UCSR0C = INIT_UCSR0C;		// UCSRC0 setzen
	UCSR0B = INIT_UCSR0B;		// UCSRB0 setzen
}

void USART1Init(unsigned int uiUbrr)
{
	PRR1 &= PRUSART1;			// Power Reduction für USART0 deaktivieren.
	UBRR1H = (uiUbrr >> 8);		// Baudrate High Register setzen
	UBRR1L = uiUbrr;			// Baudrate Low Register  setzen
	UCSR1A = INIT_UCSR1A;		// UCSRA0 setzen
	UCSR1C = INIT_UCSR1C;		// UCSRC0 setzen
	UCSR1B = INIT_UCSR1B;		// UCSRB0 setzen
}

void USART0SendByte(unsigned char ucData)
{
	while(!( UCSR0A & 0x20))
	{
		// UDRE0 = 1: Buffer Leer, UDRE0 = 0: Buffer voll
		// Warten bis TX buffer leer ist
	}
	UDR0 = ucData;	// Daten senden
}

void USART1SendByte(unsigned char ucData)
{
	while(!( UCSR1A & 0x20))
	{
		// UDRE0 = 1: Buffer Leer, UDRE0 = 0: Buffer voll
		// Warten bis TX buffer leer ist
	}
	UDR1 = ucData;	// Daten senden
}

char USART1RecvByte(void)
{
	if(f_USART.MsgTypeNone)
	{
		f_USART.MsgTypeNone = 0;		// Resetting flag
		return ucUSART1RecvHeader[0];	// Returning the raw data
	}
	
	return 0;
}

void USART1RecvMsgType0(char *cMessageID)
{
	if(f_USART.MsgType0 && f_USART.HeaderRecvDone)
	{
		uint16_t uiCRC16Msg;
		uint16_t uiCRC16;
		unsigned char ucHex[4];
		
		// Resetting variables and flags
		f_USART.MsgType0 = 0;
		f_USART.MsgType1 = 0;
		f_USART.HeaderRecvDone = 0;
		f_USART.DataRecvDone = 0;
		uiUSART1RecvCnt = 0;
		
		// Calculate CRC16
		uiCRC16 = CalculateCRC16(&ucUSART1RecvHeader[0], 4, POLYNOM);
		
		// Convert Hex CRC value from ASCII string to int
		/*
		ucHex[0] = ucUSART1RecvHeader[4];
		ucHex[1] = ucUSART1RecvHeader[5];
		ucHex[2] = ucUSART1RecvHeader[6];
		ucHex[3] = ucUSART1RecvHeader[7];
		// long int strtol(const char *str, char **endptr, int base)
		// converts the initial part of the string in str to a
		// long int value according to the given base, which must be
		// between 2 and 36 inclusive, or be the special value 0.
		uiCRC16Msg = (uint16_t)strtol(ucHex, NULL, 16);
		*/
		ucHex[0] = HexcharToInt(ucUSART1RecvHeader[4]);
		ucHex[1] = HexcharToInt(ucUSART1RecvHeader[5]);
		ucHex[2] = HexcharToInt(ucUSART1RecvHeader[6]);
		ucHex[3] = HexcharToInt(ucUSART1RecvHeader[7]);
		uiCRC16Msg = ucHex[0] << 12;
		uiCRC16Msg |= ucHex[1] << 8;
		uiCRC16Msg |= ucHex[2] << 4;
		uiCRC16Msg |= ucHex[3];
		
		// Compare CRC value
		if(uiCRC16 == uiCRC16Msg)
		{
			// Write Message ID
			cMessageID[2] = '\0';
			cMessageID[0] = ucUSART1RecvHeader[1];
			cMessageID[1] = ucUSART1RecvHeader[2];
			USART1SendByte(ACK);		// Answer with ACK
		}else{
			f_USART.InvalidData = 1;
			USART1SendByte(NAK);		// Answer with NAK
		}
	}
}

void USART1SendMsgType0(unsigned char ucMessageID)
{
	unsigned char ucHex[4];				// CRC16 array
	unsigned char ucDataArray[4];		// Data array
	
	ucDataArray[0] = SOH;									// Start of heading
	ucDataArray[1] = ((ucMessageID >> 4) & 0x0F) + 0x30;	// Message ID 1
	ucDataArray[2] = (ucMessageID        & 0x0F) + 0x30;	// Message ID 2
	ucDataArray[3] = 0x30;									// Message Type 0
		
	unsigned int uiCRC16 = CalculateCRC16(&ucDataArray[0], 4, POLYNOM);
	ucHex[0] = (uiCRC16 >> 12) & 0x000F;
	ucHex[1] = (uiCRC16 >> 8)  & 0x000F;
	ucHex[2] = (uiCRC16 >> 4)  & 0x000F;
	ucHex[3] =  uiCRC16        & 0x000F;
	
	USART1SendByte(ucDataArray[0]);		// Start of heading
	USART1SendByte(ucDataArray[1]);		// Message ID 1
	USART1SendByte(ucDataArray[2]);		// Message ID 2
	USART1SendByte(ucDataArray[3]);		// Message Type 0
	USART1SendByte(ucHex[0]);			// CRC16 1
	USART1SendByte(ucHex[1]);			// CRC16 2
	USART1SendByte(ucHex[2]);			// CRC16 3
	USART1SendByte(ucHex[3]);			// CRC16 4
}

void USART1RecvMsgType1(char *cMessageID, char *cMessageData)
{
	if(f_USART.MsgType1 && f_USART.DataRecvDone)
	{
		unsigned char ucHex[4];
		uint16_t uiCRC16;
		uint16_t uiCRC16Msg;
		
		// Header ------------------------------------------
		// Resetting variables and flags
		f_USART.MsgType0 = 0;
		f_USART.MsgType1 = 0;
		f_USART.HeaderRecvDone = 0;
		f_USART.DataRecvDone = 0;
		uiUSART1RecvCnt = 0;
		
		// Calculate CRC16		
		uiCRC16 = CalculateCRC16(&ucUSART1RecvHeader[0], 8, POLYNOM);

		// Convert Hex CRC value from ASCII string to int
		ucHex[0] = HexcharToInt(ucUSART1RecvHeader[8]);
		ucHex[1] = HexcharToInt(ucUSART1RecvHeader[9]);
		ucHex[2] = HexcharToInt(ucUSART1RecvHeader[10]);
		ucHex[3] = HexcharToInt(ucUSART1RecvHeader[11]);
		uiCRC16Msg = ucHex[0] << 12;
		uiCRC16Msg |= ucHex[1] << 8;
		uiCRC16Msg |= ucHex[2] << 4;
		uiCRC16Msg |= ucHex[3];

		// Compare CRC value
		if(uiCRC16 == uiCRC16Msg)
		{
			// Write Message ID
			cMessageID[2] = '\0';
			cMessageID[0] = ucUSART1RecvHeader[1];
			cMessageID[1] = ucUSART1RecvHeader[2];
		}else{
			f_USART.InvalidData = 1;
		}
		
		// Data -----------------------------------------------
		int iDataBufferSize = uiRecvDataLength + 2;
		// Calculate CRC16
		uiCRC16 = CalculateCRC16(&ucUSART1RecvData[0], iDataBufferSize, POLYNOM);

		// Convert Hex CRC value from ASCII string to int
		ucHex[0] = HexcharToInt(ucUSART1RecvData[iDataBufferSize]);
		ucHex[1] = HexcharToInt(ucUSART1RecvData[iDataBufferSize+1]);
		ucHex[2] = HexcharToInt(ucUSART1RecvData[iDataBufferSize+2]);
		ucHex[3] = HexcharToInt(ucUSART1RecvData[iDataBufferSize+3]);
		uiCRC16Msg = ucHex[0] << 12;
		uiCRC16Msg |= ucHex[1] << 8;
		uiCRC16Msg |= ucHex[2] << 4;
		uiCRC16Msg |= ucHex[3];

		// Compare CRC value
		if(uiCRC16 == uiCRC16Msg)
		{
			// Write Message ID
			cMessageID[2] = '\0';
			cMessageID[0] = ucUSART1RecvHeader[1];
			cMessageID[1] = ucUSART1RecvHeader[2];
			USART1SendByte(ACK);		// Answer with ACK
		}else{
			f_USART.InvalidData = 1;
			USART1SendByte(NAK);		// Answer with NAK
		}
		
		// Writing the data to the provided pointer
		for(int i = 1; i < (iDataBufferSize-1); i++)
		{
			cMessageData[i-1] = ucUSART1RecvData[i];
		}
		cMessageData[iDataBufferSize-2] = '\0';
	}
}

void USART1SendMsgType1(unsigned char ucMessageID, char *cMessageData)
{
	// Variables
	unsigned char ucHex[4];						// CRC16 array
	unsigned char ucDatalengthArray[4];			// Datalength array
	unsigned char ucHeaderArray[8];				// Header array
	unsigned char ucDataArray[DATA_SIZE_MAX];	// Data array
	unsigned int uiDatalength;
	int i = 0;
	
	// Calculate datalength
	while(*(cMessageData + i) != '\0')
	{
		i++;
	}
	uiDatalength = i;		// Datalength minus the '\0' operator
	
	// Split datalength into 4 bytes and turn them into ASCII characters
	ucDatalengthArray[0] = ((uiDatalength / 1000) % 10) + 0x30;
	ucDatalengthArray[1] = ((uiDatalength / 100)  % 10) + 0x30;
	ucDatalengthArray[2] = ((uiDatalength / 10)   % 10) + 0x30;
	ucDatalengthArray[3] =  (uiDatalength         % 10) + 0x30;
	
	// Write message array
	ucHeaderArray[0] = SOH;									// Start of heading
	ucHeaderArray[1] = ((ucMessageID >> 4) & 0x0F) + 0x30;	// Message ID 1
	ucHeaderArray[2] = (ucMessageID        & 0x0F) + 0x30;	// Message ID 2
	ucHeaderArray[3] = 0x31;								// Message Type 1
	ucHeaderArray[4] = ucDatalengthArray[0];				// Datalength 1
	ucHeaderArray[5] = ucDatalengthArray[1];				// Datalength 2
	ucHeaderArray[6] = ucDatalengthArray[2];				// Datalength 3
	ucHeaderArray[7] = ucDatalengthArray[3];				// Datalength 4
	
	// Calculate Header CRC16
	unsigned int uiCRC16 = CalculateCRC16(&ucHeaderArray[0], 8, POLYNOM);
	ucHex[0] = (uiCRC16 >> 12) & 0x000F;
	ucHex[1] = (uiCRC16 >> 8)  & 0x000F;
	ucHex[2] = (uiCRC16 >> 4)  & 0x000F;
	ucHex[3] =  uiCRC16        & 0x000F;
	
	// Write Data to array
	ucDataArray[0] = STX;
	for(i = 0; i < uiDatalength; i++)
	{
		ucDataArray[i + 1] = cMessageData[i];
	}
	ucDataArray[i + 1] = ETX;
	
	// Calculate Data CRC16
	uiCRC16 = CalculateCRC16(&ucDataArray[0], (uiDatalength + 2), POLYNOM);
	ucHex[0] = (uiCRC16 >> 12) & 0x000F;
	ucHex[1] = (uiCRC16 >> 8)  & 0x000F;
	ucHex[2] = (uiCRC16 >> 4)  & 0x000F;
	ucHex[3] =  uiCRC16        & 0x000F;
	
	// Send message
	// Header
	USART1SendByte(ucHeaderArray[0]);	// Start of heading
	USART1SendByte(ucHeaderArray[1]);	// Message ID 1
	USART1SendByte(ucHeaderArray[2]);	// Message ID 2
	USART1SendByte(ucHeaderArray[3]);	// Message Type 0
	USART1SendByte(ucHeaderArray[4]);	// Datalength
	USART1SendByte(ucHex[0]);			// CRC16 1
	USART1SendByte(ucHex[1]);			// CRC16 2
	USART1SendByte(ucHex[2]);			// CRC16 3
	USART1SendByte(ucHex[3]);			// CRC16 4
	// Data
	USART1SendByte(0x02);				// Start of text
	for(i = 0; i < DATA_SIZE_MAX; i++)
	{
		USART1SendByte(*(cMessageData + i));	// Data
	}
	USART1SendByte(0x03);				// End of text	
}

/*
ISR(USART1_RX_vect)
{
	// Data
	if(f_USART.BufferSelect){
		// read UART Data Register
		ucUSART1RecvData[uiUSART1RecvCnt] = UDR1;

		// Reset error flag and Counter when receiving a SOH
		if(f_USART.InvalidData && ucUSART1RecvData[uiUSART1RecvCnt] == SOH)
		{
			ucUSART1RecvHeader[0] = SOH;
			uiUSART1RecvCnt = 0;
			f_USART.BufferSelect = 0;
		}
		
		// Skip if last byte was invalid
		if(!f_USART.InvalidData)
		{
			// First Receive (STX, Start of Text)
			if(uiUSART1RecvCnt == 0)
			{
				if(ucUSART1RecvData[uiUSART1RecvCnt] != STX)
				{
					f_USART.InvalidData = 1;
				}
			}
			// Data receive
			if(uiUSART1RecvCnt > 0 && uiUSART1RecvCnt <= uiRecvDataLength)
			{
				if(ucUSART1RecvData[uiUSART1RecvCnt] < 0x20 &&
					ucUSART1RecvData[uiUSART1RecvCnt] > 0x7E)
				{
					f_USART.InvalidData = 1;
				}
			}
			// End of text (ETX)
			if(uiUSART1RecvCnt == (uiRecvDataLength + 1))
			{
				if(ucUSART1RecvData[uiUSART1RecvCnt] != ETX)
				{
					f_USART.InvalidData = 1;
				}
			}
			// CRC16 4th and last bit
			if(uiUSART1RecvCnt >= (uiRecvDataLength + 5))
			{
				f_USART.DataRecvDone = 1;
				f_USART.ReceivingData = 0;
			}
			// Raise counter
			if(uiUSART1RecvCnt >= (uiRecvDataLength + 5))
			{
				uiUSART1RecvCnt = 0;
				f_USART.BufferSelect = 0;
			}else{
				uiUSART1RecvCnt++;
			}
		}
	// Header
	}else{
		// read UART Data Register
		ucUSART1RecvHeader[uiUSART1RecvCnt] = UDR1;
		
		// Reset error flag and Counter when receiving a SOH
		if(f_USART.InvalidData && ucUSART1RecvHeader[uiUSART1RecvCnt] == SOH)
		{
			ucUSART1RecvHeader[0] = SOH;
			uiUSART1RecvCnt = 0;
			f_USART.InvalidData = 0;
		}
		
		// Skip if last byte was invalid
		if(!f_USART.InvalidData)
		{
			switch(uiUSART1RecvCnt)
			{
				// First Receive (SOH)
				case 0:
					if(ucUSART1RecvHeader[uiUSART1RecvCnt] != SOH)
					{
						f_USART.MsgTypeNone = 1;
						//f_USART.InvalidData = 1;
						f_USART.ReceivingData = 1;
					}
					break;
			
				// Second Receive (Message ID 1)
				case 1:
					if(ucUSART1RecvHeader[uiUSART1RecvCnt] < 0x30 ||
							ucUSART1RecvHeader[uiUSART1RecvCnt] > 0x39)
					{
						f_USART.InvalidData = 1;
					}
					break;
			
				// Third Receive (Message ID 2)
				case 2:
					if(ucUSART1RecvHeader[uiUSART1RecvCnt] < 0x30 ||
							ucUSART1RecvHeader[uiUSART1RecvCnt] > 0x39)
					{
						f_USART.InvalidData = 1;
					}
					break;
			
				// Fourth Receive (Message type)
				case 3:
					// Message Type 0
					if(ucUSART1RecvHeader[uiUSART1RecvCnt] == 0x30)
					{
						f_USART.MsgType0 = 1;
					}
					// Message Type 1
					else if(ucUSART1RecvHeader[uiUSART1RecvCnt] == 0x31)
					{
						f_USART.MsgType1 = 1;
					// Error
					}else{
						f_USART.InvalidData = 1;
					}
					break;
			
				// 5th Receive (CRC16 or Datalength)
				case 4:
				// 6th Receive (CRC16 or Datalength)
				case 5:
				// 7th Receive (CRC16 or Datalength)
				case 6:
				// 8th Receive (End of CRC16 or Datalength)
				case 7:
					// Check if out of bounds
					if((ucUSART1RecvHeader[uiUSART1RecvCnt] < 0x30) ||
						(ucUSART1RecvHeader[uiUSART1RecvCnt] > 0x39 &&
						ucUSART1RecvHeader[uiUSART1RecvCnt] < 0x41) ||
						(ucUSART1RecvHeader[uiUSART1RecvCnt] > 0x5A &&
						ucUSART1RecvHeader[uiUSART1RecvCnt] < 0x61) ||
						(ucUSART1RecvHeader[uiUSART1RecvCnt] > 0x7A)){
						f_USART.InvalidData = 1;
					}
					// break if not the 8th byte
					if(uiUSART1RecvCnt != 7){
						break;
					}
				
					// Message Type 0 receive done
					if(f_USART.MsgType0)
					{
						f_USART.HeaderRecvDone = 1;
						f_USART.ReceivingData = 0;
					}
					break;
				// End of Message Type 0
			
				// 9th Receive (CRC16) Message Type 1
				case 8:
				// 10th Receive (CRC16)
				case 9:
				// 11th Receive (CRC16)
				case 10:
				// 12th Receive (End of CRC16)
				case 11:
					if(f_USART.MsgType0)
					{
						f_USART.InvalidData = 1;
					}
					// Check if out of bounds
					if(ucUSART1RecvHeader[uiUSART1RecvCnt] < 0x30){
						f_USART.InvalidData = 1;
					}else if(ucUSART1RecvHeader[uiUSART1RecvCnt] > 0x39 &&
					ucUSART1RecvHeader[uiUSART1RecvCnt] < 0x41){
						f_USART.InvalidData = 1;
					}else if(ucUSART1RecvHeader[uiUSART1RecvCnt] > 0x5A &&
					ucUSART1RecvHeader[uiUSART1RecvCnt] < 0x61){
						f_USART.InvalidData = 1;
					}else if(ucUSART1RecvHeader[uiUSART1RecvCnt] > 0x7A){
						f_USART.InvalidData = 1;
					}
				
					// break if not the 11th byte
					if(uiUSART1RecvCnt != 11){
						break;
					}
					f_USART.HeaderRecvDone = 1;
					
					// calculate datalength
					unsigned char ucDec[4];
					ucDec[0] = DeccharToInt(ucUSART1RecvHeader[4]);
					ucDec[1] = DeccharToInt(ucUSART1RecvHeader[5]);
					ucDec[2] = DeccharToInt(ucUSART1RecvHeader[6]);
					ucDec[3] = DeccharToInt(ucUSART1RecvHeader[7]);
					uiRecvDataLength  =  ucDec[0] * 1000;
					uiRecvDataLength += (ucDec[1] * 100);
					uiRecvDataLength += (ucDec[2] * 10);
					uiRecvDataLength += (ucDec[3]);
					
					f_USART.BufferSelect = 1;
					break;
				// End of Header for Message Type 1
			}
		
			if(f_USART.MsgTypeNone || f_USART.BufferSelect == 1){
				uiUSART1RecvCnt = 0;
			}else{
				uiUSART1RecvCnt++;
			}
		}
	}
	
	return ;
}
*/