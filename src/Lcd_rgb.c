#include	"Lcd.h"

#if (LCD == LCD_RGB)

#include	"Lcd_Fonts.h"
#include	"Ports.h"
#include	"Timers.h"
#include	"Spi_i2c.h"
#include	"Adc.h"
#include	<string.h>
#include	<stdio.h>

#define	LcdSetLineFlag(LINE)	LcdFlags[(LINE >> 4)] |= (0x0001 << (LINE & 0x0F))
#define	LINE_WITH_BYTES			(DISP_WITH_PIX * DISP_PIX_BYTES)
#define	LINE_WITH_WORDS			 DISP_WITH_PIX
#define	LCD_BUFSIZE_BYTES		(LINE_WITH_BYTES * DISP_HIGH_PIX)
#define	LCD_BUFSIZE_WORDS		(LINE_WITH_WORDS * DISP_HIGH_PIX)
#define	F_TRANSPARENT			Lcd_Flags.flag_0

static	union
{
	U8	bytes[LCD_BUFSIZE_BYTES];
	U16	words[LCD_BUFSIZE_WORDS];
} LcdBuff;
//static	U8	LcdBuff[((DISP_WITH_PIX * DISP_HIGH_PIX) * DISP_PIX_BYTES)];
static	U16	LcdFlags[4] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};		// 4 x 16 bits = 64 lines
const	U8*	DataPtr;
static	U8	LcdState;
static	U16	SpiDataCounter = 0;
U16* const	LcdBuffEnd	= &LcdBuff.words[(LCD_BUFSIZE_WORDS - 1)];

static	U16		ColorPrint, ColorBkGr;
static	FLAGS	Lcd_Flags;

const FONT_INFO* Fonts[] =
{
	&Arial_7x5_Info,
	&SansSerif_16_Info,
	&Segoe_24_Info
};


inline	S8	Lcd_NextDataLine (void)
{
	static	 U8	word = 0, bit = 0;
	register U8	b = bit;
	register S8	i;

	for (i = 0; i < 5; i++)								// test all 4 words
	{
		register U16 w = LcdFlags[word];				// w = bits of current word
		if (w)
		{
			register U8  j;
			register U16 test = (0x0001 << b);			// bit to test
			for (j = b; test; j++, test <<= 1)			// start from previously tested / written line
			{
				if (w & test)
				{
					i = (word * 16) + j;
					LcdFlags[word] -= test;				// clear flag of current line
					bit = j + 1;
					if (bit > 15)
					{
						bit = 0;
						if (++word >= 4)
							word = 0;
					}
					return i;							// Line number	= (word * 16) + j)
				}
			}
			b = 0;
		}
		if (++word >= 4)
			word = 0;
	}
	return -1;
}

//	*********************************************************************
//	*							SPI1 INTERRUPT							*
//	*********************************************************************
void __attribute__((interrupt, no_auto_psv)) _SPI1Interrupt(void)
{
	_SPI1IF	= 0;							// Clear SPI1 Interrupt Flag
	SPI1STATbits.SPIROV = 0;				// Clear Receive Overflow Flag

	if (!SpiDataCounter)
	{
		LCD_CS_OFF();
		if (LcdState == LCD_STATE_LINE)
		{
			LCD_DAT();
			LCD_CS_ON();
			SpiDataCounter = (DISP_WITH_PIX * DISP_PIX_BYTES);
			LcdState = LCD_STATE_ADDRESS;
		}
		else if (LcdState == LCD_STATE_ADDRESS)
		{
			register S8 line = Lcd_NextDataLine();
			if (line < 0)
			{
				_SPI1IE	= 0;				// Disable SPI1 Interrupt
				_SPI1IF	= 1;				// Set SPI1 Interrupt Flag
				return;
			}
			register U8* ptr = &LcdBuff.bytes[(line * LINE_WITH_BYTES)];
			if (DataPtr == ptr)
			{
				LCD_DAT();
				LCD_CS_ON();
				SpiDataCounter = LINE_WITH_BYTES;
			}
			else
			{
				LCD_CMD();
				LCD_CS_ON();
				SPI1BUF = SET_COLUMN_ADDRESS;
				SPI1BUF = 0;				// X position
				SPI1BUF = (DISP_WITH_PIX - 1);
				SPI1BUF = SET_ROW_ADDRESS;
				SPI1BUF = line;				// Y position
				SPI1BUF = (DISP_HIGH_PIX - 1);
				DataPtr = ptr;
				LcdState = LCD_STATE_LINE;
				return;
			}
		}
		else	// if (LcdState == LCD_STATE_INIT)
		{
			LCD_CMD();
			LCD_CS_ON();
			DataPtr = LcdInitSequence;
			while (DataPtr[SpiDataCounter] != 0x11)
				SpiDataCounter++;
			LcdState = LCD_STATE_ADDRESS;
		}
	}
	do
	{
		SPI1BUF = *DataPtr++;
		if (!--SpiDataCounter)
			return;
	} while (!SPI1STATbits.SPITBF);
}

void LcdInit(void)
{
//	LCD_PORT_INIT();
	LCD_POW_ON();
	LCD_BL_OFF();
	LCD_RESET();
	DelayMs(1);
	LcdState		= LCD_STATE_INIT;
	Lcd_Flags.word	= 0;
	LCD_reset();
	Spi1_Init();

	ColorPrint	= BLACK;
	ColorBkGr	= WHITE;
	F_TRANSPARENT = 1;

	LcdClear();
	LCD_BL_ON();
}

void LcdClear(void)
{
	memset((void*)LcdBuff.bytes,	0x00,	sizeof(LcdBuff));
	memset((void*)LcdFlags,	0xFF,	sizeof(LcdFlags));
	_SPI1IE	= 1;				// Enable SPI1 Interrupt
}

void LcdSetColor(COLOR color)
{
	register U16 i;
	for (i = 0; i < LCD_BUFSIZE_WORDS; i++)
		LcdBuff.words[i] = color;
	memset((void*)LcdFlags,	0xFF,	sizeof(LcdFlags));
	_SPI1IE	= 1;				// Enable SPI1 Interrupt
}

void LcdBmp	(const U8* bmp)
{
	memcpy((void*)LcdBuff.bytes,	bmp,	sizeof(LcdBuff));
	memset((void*)LcdFlags,	0xFF,	sizeof(LcdFlags));
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
}

// line		0-7
// offset	0-95
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

	register U8		i, j, k;
	register U8		chWith	= font->charInfo[ascii].widthBits;		// charcter with in pixels
	register U8		with	= DISP_WITH_PIX - *offset;				// remain pix till end of line
	register U8		row		= *line * 8;							// number of pixel row
	register U16*	ptr		= LcdBuff.words;

	ptr	+= ((row * LINE_WITH_WORDS) + *offset);

	if (chWith > with)
	{
		return;
	}

	if (with > font->spacePixels)
		with = font->spacePixels;
	with	+= chWith;
	*offset	+= with;

	const U8* code = &font->data[font->charInfo[ascii].offset];
	for (i = 0; i < font->heightPages; i++)							// high in bytes
	{
		for (j = 0x01; j; j <<= 1)									// high in pix (8 per byte)
		{
			if (&ptr[chWith] > LcdBuffEnd)
				return;
			if (F_TRANSPARENT)
			{
				for (k = 0; k < chWith; k++)						// with
					if (code[k] & j)
						ptr[k] = ColorPrint;					// draw char pixels
			}
			else
			{
				for (k = 0; k < chWith; k++)						// with
					ptr[k] = (code[k] & j)?	 ColorPrint : ColorBkGr;			// draw char pixels

				for (; k < with; k++)									// with + spaces
					ptr[k] = ColorBkGr;									// draw spaces
			}

			LcdSetLineFlag(row);									// set flag to refresh line, increment line
			row++;
			ptr += DISP_WITH_PIX;
		}
		code += chWith;
	}
}

void LcdString(U8 line, U8 position, char* str, FONT f)
{
	while (*str)
		Lcd_Char(&line, &position, (U8)*str++, f);
	_SPI1IE	= 1;				// Enable SPI1 Interrupt
}

void	LcdData		(U8 line, U8 position, char* str, U8 len)
{
	FONT f = font_ss16;
	while (len)
	{
		Lcd_Char(&line, &position, (U8)*str++, f);
		len--;
	}
	_SPI1IE	= 1;				// Enable SPI1 Interrupt
}

void LcdDrawLineHor (U8 line, U8 position, U8 len, COLOR color)
{
//	ColorPrint = color;
	register U16*	ptr = &LcdBuff.words[(line * LINE_WITH_WORDS) + position];
	while (len--)
		*ptr++ = color;
	LcdSetLineFlag(line);
	_SPI1IE	= 1;				// Enable SPI1 Interrupt
}

void LcdRgbTest	(void)
{
	static U16		tic = 0;
	static COLOR	color = BLACK, color1 = BLACK;
	static U8		line  = 63, pos = 0;
	static char		str[22] = "Battery";
	static U16		bat_last = 0;
	
	if (bat_last != Battery)
	{
		bat_last = Battery;
		sprintf(str, "Bat: %u,%03uV  ", Battery / 1000, Battery % 1000);
	}
//	LcdSetColor(GREY);
	if (GetTimeSinceMs(tic) >= 25)
	{
		tic = GetTicsMs();

		LcdDrawLineHor(line, pos, 64, color);	// clear previous line
		if (line >= 63)
		{
			line	= 0;
			pos		= 0;
//			LcdSetColor(color);
		}
		else
		{
			line++;
			if (!(line & 0x01))
				pos++;
		}
		if (!(line & 0x07))
		{
			switch (line >> 3)
			{
			case 0:	color = RED;	color1	= BLACK;							break;
			case 1:	color = YELLOW;	LcdString(5, 5, "En?elîts", font_Seg24);	break;
			case 2:	color = PINK;	LcdString(0, 48, str, font_ss16);			break;
			case 3:	color = WHITE;												break;
			case 4:	color = CYAN;												break;
			case 5:	color = GREEN;	LcdString(2, 0, "Mazais", font_Seg24);		break;
			case 6:	color = BLUE;	color1	= WHITE;							break;
			case 7:	color = BROWN;												break;
			default:color = BLACK;												break;
			}
//			LcdSetColor(color);
//			LcdString(0, 48, DEVICE, font_ss16);
		}

		LcdDrawLineHor(line, pos, 64, color1);		// draw next line
//		LcdStri00000ng(2, 48, DEVICE, font_ss16);
	}
}

#endif	// LCD_RGB
