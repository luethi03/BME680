/*** Header file for USART operation on the Atmega2561 ***/
/******************************************************************************
Kunde:          EPH Electronics AG
Projekt:        Atmega Basics

Dateiname:      Atmega2561_USART.h
Version:        1.00 Exp.
Prozessor:      ATmega2561
Autor:			Marlon Tanner

Copyright:      EPH Electronics AG
                Ratihard 5
                8253 Diessenhofen

Datum:          02.11.20
Beschreibung:   Headerfile des Programms.
*******************************************************************************/

#ifndef _ATMEGA2561_USART_H_
#define _ATMEGA2561_USART_H_

// Defines
#define INIT_UCSR0A			0x02	// U2X0 setzen -> Double Transmission Speed
#define INIT_UCSR0B			0x98	// RXCIE0 = 1 / TXCIE0 = 0 / UDRIE0 = 0 / RXEN0 = 1 / TXEN0 = 1 / UCSZ02 = 0/ RXB80 / TXB80
#define INIT_UCSR0C			0x0E	// UMSEL01 & UMSEL00 = 00 / UPM01 & UPM00 = 00 / USBS0 = 1 / UCSZ01 & UCSZ00 = 11 / UCPOL0 = 0

#define INIT_UCSR1A			0x02	// U2X0 setzen -> Double Transmission Speed
#define INIT_UCSR1B			0x98	// RXCIE0 = 1 / TXCIE0 = 0 / UDRIE0 = 0 / RXEN0 = 1 / TXEN0 = 1 / UCSZ02 = 0/ RXB80 / TXB80
#define INIT_UCSR1C			0x0E	// UMSEL01 & UMSEL00 = 00 / UPM01 & UPM00 = 00 / USBS0 = 1 / UCSZ01 & UCSZ00 = 11 / UCPOL0 = 0

#define HEADER_SIZE_MAX		12		// Header max. size in Bytes
#define DATA_SIZE_MAX		128		// Data max. size in Bytes. Real usable size is DATA_SIZE_MAX - 2

#define POLYNOM				0x1021	// Polynom to calculate CRC16

#define SOH					0x01	// Start of heading
#define STX					0x02	// Start of text
#define ETX					0x03	// End of text
#define ACK					0x06	// Acknowledge
#define NAK					0x15	// Negative Acknowledge

// Variables
unsigned int uiUSART1RecvCnt;					// Empfangsvariable für USART1

// Structs
struct USART_Flags
{
	unsigned MsgType0 : 1;			// Message Type
	unsigned MsgType1 : 1;			// Message Type
	unsigned MsgTypeNone : 1;		// Message Type
	unsigned MsgLength : 4;			// Message length
	unsigned BufferSelect : 1;		// Specifies in which buffer the received bytes should be wrote to
	unsigned InvalidData : 1;		// Invalid Data Error
	unsigned HeaderRecvDone : 1;	// Receiving Header done
	unsigned DataRecvDone : 1;		// Receiving Data done
	unsigned ReceivingData : 1;		// uC is receiving data over USART if this flag is set
}
f_USART;

/*** USARTInit ***/

/*******************************************************************************

Name:           USART0Init
Version:        1.00
Autor:          S.Janesch

Übergabe:       (unsigned char) ucUbrr -> Baudrate
Rückgabe:       (void) keine

Beschreibung:   Diese Funktion Initialisiert baudrate und Schreib/Lesefunktionen 
				der USART features

Änderungen:     05.08.08    S.Janesch	0.00

- Erstellung der Funktion.

*******************************************************************************/
void USART0Init(unsigned int uiUbrr);


/*******************************************************************************

Name:           USART1Init
Version:        1.00
Autor:          Marlon Tanner

Übergabe:       (unsigned char) ucUbrr -> Baudrate
Rückgabe:       (void) keine

Beschreibung:   Diese Funktion Initialisiert baudrate und Schreib/Lesefunktionen 
				der USART features

Änderungen:     02.11.20    M.Tanner	0.00

- Erstellung der Funktion.

*******************************************************************************/
void USART1Init(unsigned int uiUbrr);


/*** TxD ***/

/*******************************************************************************

Name:           SendByteUSART0
Version:        1.00
Autor:          Yannick Jud

Übergabe:       (unsigned char) ucData -> Daten die gesendet werden sollen
Rückgabe:       (void) keine

Beschreibung:   Diese Funktion schreib ein Byte in den Buffer

Änderungen:     

*******************************************************************************/
void USART0SendByte(unsigned char ucData);


/**
 *	@brief		USART1SendByte
 *					Write a byte to the USART Data Register
 *
 *	@author		Marlon Tanner
 *	
 *	@param		(unsigned char) ucData -> Daten die gesendet werden sollen
 *	
 *	@return		(void) -
 */
void USART1SendByte(unsigned char ucData);


/**
 *	@brief		USART1RecvByte
 *					Read a byte from the USART Data Register
 *
 *	@author		Marlon Tanner
 *	
 *	@param		(void) -
 *	
 *	@return		(unsigned int8_t) Received byte
 */
char USART1RecvByte(void);


/**
 *	@brief		USART1RecvMsgType0
 *					Read the Message ID of a Type 0 message
 *
 *	@author		Marlon Tanner
 *	
 *	@param		(char *) char pointer where the Message ID will be copied into
 *	
 *	@return		(void) -
 */
void USART1RecvMsgType0(char *cMessageID);


/**
 *	@brief		USART1SendMsgType0
 *					Send a Message Type 0
 *
 *	@author		Marlon Tanner
 *
 *	@param		(char *) char pointer to the Message ID that will be sent
 *
 *	@return		(void) -
 */
void USART1SendMsgType0(unsigned char ucMessageID);


/**
 *	@brief		USART1RecvMsgType1
 *					Read the Message ID and Message Data of a Type 1 message
 *
 *	@author		Marlon Tanner
 *	
 *	@param		(char *) char pointer where the Message ID will be copied into
 *	@param		(char *) char pointer where the Message Data will be copied into
 *	
 *	@return		(void) -
 */
void USART1RecvMsgType1(char *cMessageID, char *cMessageData);


/**
 *	@brief		USART1SendMsgType1
 *					Send a Message Type 1
 *
 *	@author		Marlon Tanner
 *
 *	@param		(char *) char pointer to the Message ID that will be sent
 *	@param		(char *) char pointer to the Message data that will be sent
 *
 *	@return		(void) -
 */
void USART1SendMsgType1(unsigned char ucMessageID, char *cMessageData);


/*******************************************************************************

Name:           USART1 RX vector - Interruptroutine
Version:        1.00
Autor:          Marlon Tanner

Übergabe:       (void) keine
Rückgabe:       (void) keine

Beschreibung:   Ein Interrupt handler für den Empfang von Daten an USART1

Änderungen:     02.11.20    M.Tanner    1.00

                - Funktion erfolgreich getestet.

				02.11.20    M.Tanner    0.01

                - Implementation der Funktion.

				02.11.20    M.Tanner    0.00

                - Erstellung der Funktion.

*******************************************************************************/
ISR(USART1_RX_vect);

#endif /* ATMEGA2561_USART_H_ */