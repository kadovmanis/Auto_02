#ifndef _FONTS_DB_H_
#define _FONTS_DB_H_

#include	"Typedef.h"

// This structure describes a single character's display information
typedef struct
{
	const U8		widthBits;		// width, in bits (or pixels), of the character
	const U16		offset;			// offset of the character's bitmap, in bytes, into the the FONT_INFO's data array
	
} CH_INFO;	

// Describes a single font
typedef struct
{
	const U8 		heightPages;	// height, in pages (8 pixels), of the font's characters
	const U8 		startChar;		// the first character in the font (e.g. in charInfo and data)
	const U8 		endChar;		// the last character in the font
	const U8		spacePixels;	// number of pixels that a space character takes up
	const CH_INFO*	charInfo;		// pointer to array of char information
	const U8*		data;			// pointer to generated array of character visual representation
		
} FONT_INFO;	

/* Font data for Arial 7x5 pixels */
extern const FONT_INFO	Arial_7x5_Info;
extern const CH_INFO	Arial_7x5_Descriptors[];
extern const U8			Arial_7x5_Bitmaps[];
/* Font data for Microsoft Sans Serif 11pt */
extern const FONT_INFO	SansSerif_16_Info;
extern const CH_INFO	SansSerif_16_Descriptors[];
extern const U8			SansSerif_16_Bitmaps[];
/* Font data for Segoe Print 12pt */
extern const FONT_INFO	Segoe_24_Info;
extern const CH_INFO	segoe_24_Descriptors[];
extern const U8			segoe_24_Bitmaps[];

/*
ÂÈÇÌÎÍÏÒÐÛÞ
âèçìîíïòðûþ
 !"#$%&'()*+,-./0123456789:;<=>?@
ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`
abcdefghijklmnopqrstuvwxyz{|}~
*/
#endif	// _FONTS_DB_H_
