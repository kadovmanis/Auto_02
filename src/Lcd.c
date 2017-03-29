#include	"Lcd.h"

#if	((LCD != LCD_NO) && (LCD != LCD_RGB))

#include	"Lcd_Fonts.h"
#include	"Ports.h"
#include	"Timers.h"
#include	"Spi_i2c.h"
#include	<string.h>
#include	<stdio.h>

//void LcdString_old(U8 line, U8 position, char* str);
const FONT_INFO* Fonts[] = 
{
	&Arial_7x5_Info,
	&SansSerif_16_Info,
	&Segoe_24_Info
};

const char LcdLineStartAddr[] =
{
//	X_LAST,
//	0xBC,
//	0xBC,
//	0xBC,
	X_START,
	Y_START_H,
	Y_START_L
};

			U8	LcdBuff[DISP_WITH_PIX * DISP_HIGH_BYTES];
volatile	U16	LcdFlags;
//volatile	P_U8	linePtr = &LcdInitSequence[0];
volatile	P_U8	linePtr;
static		U8	LcdState = LCD_STATE_INIT, SpiDataCounter = 0;
/*
void Spi1_Init(void)
{
	SPI1STAT = 0b0000000000010100;	// SPI1STAT Register
			//	 |||||||||||||||+	-- SPIRBF: SPIx Receive Buffer Full Status
			//	 ||||||||||||||+-	-- SPITBF: SPIx Transmit Buffer Full Status bit
			//	 |||||||||||+++--	-- SISEL<2:0>: SPIx Buffer Interrupt Mode bits (valid in Enhanced Buffer mode) 101=Interrupt when the last bit is shifted out of SPIxSR; now the transmit is complete
			//	 ||||||||||+-----	-- SRXMPT: Receive FIFO Empty bit (valid in Enhanced Buffer mode)
			//	 |||||||||+------	-- SPIROV: Receive Overflow Flag
			//	 ||||||||+-------	-- SRMPT: Shift Register (SPIxSR) Empty bit (valid in Enhanced Buffer mode)
			//	 |||||+++--------	-- SPIBEC<2:0>: SPIx Buffer Element Count bits (valid in Enhanced Buffer mode)
			//	 |||++-----------	-- Unimplemented
			//	 ||+-------------	-- SPISIDL: Stop in Idle Mode
			//	 |+--------------	-- Unimplemented
			//	 +---------------	-- SPIEN: SPIx Enable
	SPI1CON1 = 0b0000001001111011;	// SPI1CON1 Register
			//	 ||||||||||||||++	-- PPRE<1:0>: Primary Prescale bits (Master mode)	(1:1)
			//	 |||||||||||+++--	-- SPRE<2:0>: Secondary Prescale bits (Master mode)	(1:2)
			//	 ||||||||||+-----	-- MSTEN: Master Mode Enable
			//	 |||||||||+------	-- CKP: Clock Polarity Select (1 = SCK Idle state - high level)
			//	 ||||||||+-------	-- SSEN: Slave Select Enable
			//	 |||||||+--------	-- CKE: SPIx Clock Edge Select (0-Idle to active clock state)
			//	 ||||||+---------	-- SMP: SPIx Data Input Sample Phase
			//	 |||||+----------	-- MODE16: Word/Byte Communication Select
			//	 ||||+-----------	-- DISSDO: Disable SDOx Pin bit
			//	 |||+------------	-- DISSCK: Disable SCKx Pin bit (SPI Master modes only)
			//	 +++-------------	-- Unimplemented
	SPI1CON2 = 0b0000000000000001;	// SPI1CON2 Register
			//	 |||||||||||||||+	-- SPIBEN: Enhanced Buffer Enable
			//	 ||||||||||||||+-	-- SPIFE: Frame Sync Pulse Edge Select
			//	 |||+++++++++++--	-- Unimplemented
			//	 ||+-------------	-- SPIFPOL: Frame Sync Pulse Polarity bit (Frame mode only)
			//	 |+--------------	-- SPIFSD: Frame Sync Pulse Direction Control on SSx Pin
			//	 +---------------	-- FRMEN: Framed SPIx Support
	_SPI1IF	= 1;				// Set the SPI1 Interrupt Flag
	_SPI1IE	= 0;				// Clear SPI1 Interrupt
	SPI1STATbits.SPIEN	= 1;	// Enable SPI1
}
*/
//	*********************************************************************
//	*							SPI1 INTERRUPT							*
//	*********************************************************************
void __attribute__((interrupt, no_auto_psv)) _SPI1Interrupt(void)
{
	U8 currentState = LcdState;
	do
	{
		_SPI1IF	= 0;				// Clear SPI1 Interrupt Flag
		SPI1STATbits.SPIROV = 0;	// Clear Receive Overflow Flag
	//	data = SPI1BUF;

		if (LcdState == LCD_STATE_LINE)
		{
			if (!SpiDataCounter)
			{
				LCD_CS_OFF();
				LCD_DAT();
				LCD_CS_ON();
				#ifdef LCD_REVERSE
					SpiDataCounter = DISP_WITH_PIX;
				#endif
			}
			#ifdef LCD_REVERSE
				SPI1BUF = linePtr[--SpiDataCounter];
				if (!SpiDataCounter)
			#else
				SPI1BUF = linePtr[SpiDataCounter++];
				if (SpiDataCounter >= DISP_WITH_PIX)
			#endif
			{
				SpiDataCounter = 0;
				LcdState = LCD_STATE_ADDRESS;
	//			_SPI1IE	= 0;				// Disable SPI1 Interrupt
			}
		}
		else if (LcdState == LCD_STATE_ADDRESS)
		{
			if (!SpiDataCounter)
			{
				static U8	lineNumber = 0;
				U8 currentLine = lineNumber;
				while (true)
				{
					if ((LcdFlags >> lineNumber) & 0x0001)
						break;
					if (++lineNumber >= DISP_HIGH_BYTES)
						lineNumber = 0;
					if (lineNumber == currentLine)
					{
						LCD_CS_OFF();
						lineNumber = 0;
						_SPI1IE	= 0;				// Disable SPI1 Interrupt
						_SPI1IF	= 1;				// Set SPI1 Interrupt Flag
						return;
					}
				}
				LCD_CS_OFF();
				LCD_CMD();
				LCD_CS_ON();
//				SPI1BUF = LcdLineStartAddr[0] - lineNumber;		// ðeit jânorâda pareizâ rinda
				SPI1BUF = LcdLineStartAddr[0] + lineNumber;		// ðeit jânorâda pareizâ rinda
				linePtr = &LcdBuff[lineNumber * DISP_WITH_PIX];	// uz tekoðo rindu
				LcdFlags &= ~(0x0001 << lineNumber);
				if (++lineNumber >= DISP_HIGH_BYTES)
					lineNumber = 0;
			}
			else
				SPI1BUF = LcdLineStartAddr[SpiDataCounter];
			if (++SpiDataCounter >= 3)
			{
				SpiDataCounter = 0;
				LcdState = LCD_STATE_LINE;
			}
		}
	//	else if (LcdState == INIT)
		else
		{
			if (!SpiDataCounter)
			{
				LCD_CS_OFF();
				LCD_CMD();
				LCD_CS_ON();
			}
			SPI1BUF = LcdInitSequence[SpiDataCounter++];
			if (LcdInitSequence[SpiDataCounter] == 0x11)
			{
				SpiDataCounter = 0;
				LcdState = LCD_STATE_ADDRESS;
			}
		}
	} while (!SPI1STATbits.SPITBF && (LcdState == currentState));
}


void LcdInit(void)
{
//	LCD_PORT_INIT();
	LCD_POW_ON();
	LCD_BL_OFF();
	LCD_RESET();
	DelayMs(1);
	LCD_reset();
	Spi1_Init();

	LcdClear();
	LCD_BL_ON();
}
/*


	
	SendByte(LCD_RESET_CMD);
	SendByte(0x2F);				// POWER_CONTROL_CMD
	SendByte(0x25);				// INTERNAL_VOLTAGE
	SendByte(0xA3);				// LCD_BIAS_SET
	SendByte(0x81);				// EL_VOLUME_MODE
	SendByte(0x1F);	//lcd light adj
	SendByte(0xC0);				// COM_OUT_DIRECTION
	SendByte(0xA0);				// RAM_ADDR_SET
	SendByte(0x60);
	SendByte(0xA6);				// NORMAL_INDICATION
	SendByte(0xAF);				// LCD_ON_CMD

*	
//	#ifdef	DISPLAY_BLUE
	SendByte(RAM_ADDR_SET);
	SendByte(0x60);
//	#else
//		START_ADDR,			// Display start line set
//	#endif
	SendByte(ADC_1);
	SendByte(COM_OUT_DIRECTION);
	SendByte(NORMAL_INDICATION);
	SendByte(LCD_BIAS_SET_0);
	SendByte(POWER_CONTROL_CMD);
	SendByte(BOOSTER_RATIO);
	SendByte(0x00);
	SendByte(INTERNAL_VOLTAGE_7);
	SendByte(EL_VOLUME_MODE);
	SendByte(0x1F);
	SendByte(INDICATOR);
	SendByte(0x00);
	SendByte(LCD_ON_CMD);
	LCD_CS_OFF();
*
*/

void LcdClear(void)
{
	memset((void*)LcdBuff, 0x00, sizeof(LcdBuff));
	LcdFlags = 0b0000000011111111;
	_SPI1IE	= 1;				// Enable SPI1 Interrupt
}

void AsciiComand(U8* line, U8* offset, U8 ascii, const FONT_INFO* font)
{
	switch (ascii)
	{
	case '\r':
		*offset = 0;
		break;
	case '\n':
		*line += font->heightPages;
		if (*line >= DISP_HIGH_BYTES)
			*line -= DISP_HIGH_BYTES;
		break;
	case '\t':
		break;
	case '\b':
		break;
	}
//	if (ch == '\b')
//		BackSpace();
//	else if (ch == '\r')
//		LcdCoordinates(Row, 0);
//	else if (ch == '\n')
//	{
//		if (++Row >= DISP_HIGH_BYTES)
//			Row = 0;
//		LcdCoordinates(Row, Column);
//	}
}

// line		0-7
// offset	0-127
// ascii	ASCII code
// font		defined enum fonts
//
// return:	line & offset points to position for next char (space included)
//			MUST BE ENABLED SPI INTERRUPT (flag) after
void Lcd_Char(U8* line, U8* offset, U8 ascii, FONT f)
{
	const FONT_INFO* font = Fonts[f];

	if (ascii < 0x20)
		return AsciiComand(line, offset, ascii, font);
	else if (ascii > 0x80)
		ascii = SwitchCode(ascii);

	register U8*	ptr;
	register U8		i, j = 0;

	if ((*offset + font->charInfo[ascii].widthBits) > DISP_WITH_PIX)
	{
		LcdString(0, 0, "WITH !", font_7x5);
		for (i = 0; i < font->heightPages; i++)
		{
			ptr	= &LcdBuff[(DISP_WITH_PIX * *line)];
			for (j = *offset; j < DISP_WITH_PIX; j++)
				ptr[j] = 0x00;
			LcdFlags |= (0x0001 << *line);
			if (++*line >= DISP_HIGH_BYTES)
				*line = 0;
		}
		*offset = 0;
	}

	const U8* code = &font->data[font->charInfo[ascii].offset];
	for (i = 0; i < font->heightPages; i++)
	{
		ptr	= &LcdBuff[(DISP_WITH_PIX * (i + *line))];
		j = *offset;
		register U8 k = font->charInfo[ascii].widthBits;
		while (k)
		{
			ptr[j++] = *code++;
			k--;
		}
		while ((j < DISP_WITH_PIX) && (k++ < font->spacePixels))
			ptr[j++] = 0x00;
		LcdFlags |= (0x0001 << (i + *line));
	}
	*offset = (j >= DISP_WITH_PIX)?	0 : j;
}



void printChar(P_U8 lineToPrint, U8 asciiCode, FONT f)
{
//	U8 i;

	if (asciiCode < 0x20)
	{
//		AsciiComand(asciiCode, &Arial_7x5_Info);
	}
	else
	{
		if (asciiCode > 0x80)
			asciiCode = SwitchCode(asciiCode);

		switch (f)
		{
		case font_7x5:
			memcpy(lineToPrint, (const void*)font7x5[asciiCode], sizeof(font7x5[asciiCode]));
			break;
		case font_ss16:
			break;
		case font_Seg24:
			break;
		}

	}
}
/*

		if (LcdWrite >= (sizeof(LcdBuff) - (CHAR_7x5_WIDTH + SPACE)))	// LcdBuff boundary reached
		{																// flush Current String from LcdBuff
			FlushStr();
			LcdSent = LcdWrite = 0;
		}
		if (Column > (DISP_WITH_PIX - CHAR_7x5_WIDTH))					// ROW boundary reached
		{																// fill row & enter to next row
			while (Column < DISP_WITH_PIX)
			{
				LcdBuff[LcdWrite++] = 0;
				Column++;
			}
			LcdEnter();
		}

		for (i = 0; i < CHAR_7x5_WIDTH; i++)
			LcdBuff[LcdWrite++] = font7x5[ch][i];

		if (Column > (DISP_WITH_PIX - (CHAR_7x5_WIDTH + SPACE)))		// no more characters possible in current row
		{
			Column += CHAR_7x5_WIDTH;
			FlushStr();
		}
		else
		{
			i = SPACE;
			while (i)
			{
				LcdBuff[LcdWrite++] = 0;
				i--;
			}
			Column += (CHAR_7x5_WIDTH + SPACE);
		}
	}
}
*/
/*
void LcdString_old(U8 line, U8 position, char* str)
{
	P_U8 ptr	= &LcdBuff[(DISP_WITH_PIX * line)];
	P_U8 lineEnd = ptr + DISP_WITH_PIX;
	ptr += position;

	while (*str)
	{
		P_U8 nextChar = ptr + CHAR_7x5_WIDTH;
		if (nextChar > lineEnd)
		{
			while (ptr < lineEnd)
				*ptr++ = 0x00;
			LcdFlags |= (0x0001 << line);
			if (++line >= DISP_HIGH_BYTES)
			{
				ptr = LcdBuff;
				line = 0;
			}
			else
				ptr	= &LcdBuff[(DISP_WITH_PIX * line)];

			lineEnd = ptr + DISP_WITH_PIX;
			nextChar = ptr + CHAR_7x5_WIDTH;
		}
		printChar(ptr, *str++, font_7x5);
		ptr = nextChar;
		U8 i = CHAR_7x5_SPACE;
		while (i)
		{
			*ptr++ = 0x00;
			i--;
		}
	}
	LcdFlags |= (0x0001 << line);
	_SPI1IE	= 1;				// Enable SPI1 Interrupt
}
*/

void LcdString(U8 line, U8 position, char* str, FONT f)
{
//	if (f == font_7x5)
//		return LcdString_old(line,position,str);

	while (*str)
		Lcd_Char(&line, &position, (U8)*str++, f);
	_SPI1IE	= 1;				// Enable SPI1 Interrupt
}



/*
void LcdString(U8 line, U8 position, char* str, FONT f)
{
	if (f == font_7x5)
		return LcdString_old(line,position,str);

	const FONT_INFO* font;
	switch (f)
	{
	case font_7x5:	font = &SansSerif_16_Info;	break;
	case font_ss16:	font = &SansSerif_16_Info;	break;
	}

	U8	*ptr	= &LcdBuff[(DISP_WITH_PIX * line)];
	U8	*lineEnd = ptr + DISP_WITH_PIX;
	ptr += position;

	while (*str)
	{
		register U8 i, j, ch = (U8)*str;
		register U8 charSize = font->charInfo[ch].widthBits;
		register U8 lineCnt	= font->heightPages;
		if ((ptr + charSize) > lineEnd)
		{
			for (i = 0; i < lineCnt; i++)
			{
				while (ptr < lineEnd)
					*ptr++ = 0x00;
				LcdFlags |= (0x0001 << line);

				if (++line >= DISP_HIGH_BYTES)
				{
					ptr = LcdBuff;
					line = 0;
				}
				else
					ptr	= &LcdBuff[(DISP_WITH_PIX * line)];

				lineEnd = ptr + DISP_WITH_PIX;
				nextChar = ptr + CHAR_7x5_WIDTH;
			}
		}
	}
}
*/
void LcdData(U8 line, U8 position, char* str, U8 len)
{
	FONT f = font_ss16;
	while (len)
	{
		Lcd_Char(&line, &position, (U8)*str++, f);
		len--;
	}
	_SPI1IE	= 1;				// Enable SPI1 Interrupt
/*
	P_U8 ptr	= &LcdBuff[(DISP_WITH_PIX * line)];
	P_U8 lineEnd = ptr + DISP_WITH_PIX;
	ptr += position;

	while (len)
	{
		P_U8 nextChar = ptr + CHAR_7x5_WIDTH;
		if (nextChar > lineEnd)
		{
			while (ptr < lineEnd)
				*ptr++ = 0x00;
			LcdFlags |= (0x0001 << line);
			if (++line >= DISP_HIGH_BYTES)
			{
				ptr = LcdBuff;
				line = 0;
			}
			else
				ptr	= &LcdBuff[(DISP_WITH_PIX * line)];

			lineEnd = ptr + DISP_WITH_PIX;
			nextChar = ptr + CHAR_7x5_WIDTH;
		}
		printChar(ptr, *str++, font_7x5);
		ptr = nextChar;
		U8 i = CHAR_7x5_SPACE;
		while (i)
		{
			*ptr++ = 0x00;
			i--;
		}
		len--;
	}
	LcdFlags |= (0x0001 << line);
	_SPI1IE	= 1;				// Enable SPI1 Interrupt
*/
}

void LcdBcdString(U8 line, U8 position, P_U8 str, U8 len)
{
	P_U8 ptr	= &LcdBuff[(DISP_WITH_PIX * line) + position];

	U8 i = 0;
	while (i < len)
	{
		U8 a;
		if (i & 0x01)
			a = *str++ & 0x0F;
		else
			a = *str >> 4;

		if (a == 0x0F)
			return;
		else if (a <= 0x09)
			a += '0';
		else if (a == 0x0A)
			a = ' ';
		else if (a == 0x0B)
			a = '.';
		else if (a == 0x0C)
			a = ',';
		else // (a == 0x0F)
			return;

		memcpy(ptr, (const void*)font7x5[a], CHAR_7x5_WIDTH);
		ptr += CHAR_7x5_WIDTH;
		a = CHAR_7x5_SPACE;
		while (a)
		{
			*ptr++ = 0x00;
			a--;
		}

		i++;
	}
	LcdFlags |= (0x0001 << line);
	_SPI1IE	= 1;				// Enable SPI1 Interrupt
}

void LcdBmp	(const U8* bmp)
{
	memcpy((void*)LcdBuff, bmp, sizeof(LcdBuff));
	LcdFlags = 0b0000000011111111;
	_SPI1IE	= 1;				// Enable SPI1 Interrupt
}

#endif
