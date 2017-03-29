#ifndef	_lcd_h_
#define	_lcd_h_

#include	"Typedef.h"

typedef enum
{
	font_7x5	= 0,	// 
	font_ss16,			// 
	font_Seg24			// 
} FONT;

// 					0blll0000000000hhh	Green
//					0b0001111100000000	Red
//					0b0000000011111000	Blue
#define COL_R(R)	((R >> 3) << 8) 
#define COL_G(G)	(((G >> 2) << 13) | (G >> 5))
#define COL_B(B)	(B & 0b11111000)
#define RGB(R,G,B)	(COL_R(R) | COL_G(G) | COL_B(B))

typedef enum {
    BLACK     = RGB(  0,  0,  0), // black
    GREY      = RGB(192,192,192), // grey
    WHITE     = RGB(255,255,255), // white
    RED       = RGB(255,  0,  0), // red
    PINK      = RGB(255,192,203), // pink
    YELLOW    = RGB(255,255,  0), // yellow
    GOLDEN    = RGB(255,215,  0), // golden
    BROWN     = RGB(128, 42, 42), // brown
    BLUE      = RGB(  0,  0,255), // blue
    CYAN      = RGB(  0,255,255), // cyan
    GREEN     = RGB(  0,255,  0), // green
    PURPLE    = RGB(160, 32,240), // purple
} COLOR;

#define LCD_CS_ON()		LCD_CS			= 0
#define LCD_CS_OFF()	LCD_CS			= 1
#define LCD_RESET()		LCD_RST			= 0
#define LCD_reset()		LCD_RST			= 1
#define LCD_CMD()		LCD_CD			= 0
#define LCD_DAT()		LCD_CD			= 1
#define LCD_PORT_INIT()	LATD 			|= 0x0F00
#define LCD_POW_ON()	LCD_POWER		= 0
#define LCD_POW_OFF()	LCD_POWER		= 1
#define LCD_BL_ON()		LCD_BACKLIGHT	= 0
#define LCD_BL_OFF()	LCD_BACKLIGHT	= 1

void	LcdInit			(void);
void	LcdClear		(void);
void	LcdSetColor		(COLOR color);
//void	LcdString		(U8 line, U8 position, char* str);
void	LcdString		(U8 line, U8 position, char* str, FONT font);
void	LcdData			(U8 line, U8 position, char* str, U8 len);
void	LcdBcdString	(U8 line, U8 position, P_U8 str, U8 len);

void	LcdBmp			(const U8* bmp);
void	LcdDrawLineHor	(U8 line, U8 position, U8 len, COLOR color);

void	LcdRgbTest		(void);

#endif  // _lcd_h_

