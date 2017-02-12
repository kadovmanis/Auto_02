#ifndef		__CONFIG_BITS_H__
#define		__CONFIG_BITS_H__

// CONFIG1
#ifdef	BOOT_LOADER
	#pragma config WDTPS	= PS32768		// Watchdog Timer Postscaler (1:32,768)
	#pragma config FWPSA	= PR128			// WDT Prescaler (Prescaler ratio of 1:128)
	#pragma config WINDIS	= OFF			// Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
	#pragma config FWDTEN	= OFF			// Watchdog Timer Enable (Watchdog Timer is disabled)
	#pragma config ICS		= PGx2			// Comm Channel Select (Emulator functions are shared with PGEC2/PGED2)
	#pragma config GWRP		= OFF			// General Code Segment Write Protect (Writes to program memory are allowed)
	#pragma config GCP		= OFF			// General Code Segment Code Protect (Code protection is disabled)
	#pragma config JTAGEN	= OFF			// JTAG Port Enable (JTAG port is disabled)
#else
	#pragma config WDTPS	= PS32768		// Watchdog Timer Postscaler (1:32,768)
	#pragma config FWPSA	= PR128			// WDT Prescaler (Prescaler ratio of 1:128)
	#pragma config WINDIS	= OFF			// Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
	#pragma config FWDTEN	= OFF			// Watchdog Timer Enable (Watchdog Timer is disabled)
	#pragma config ICS		= PGx2			// Comm Channel Select (Emulator functions are shared with PGEC2/PGED2)
	#pragma config GWRP		= OFF			// General Code Segment Write Protect (Writes to program memory are allowed)
	#pragma config GCP		= ON			// General Code Segment Code Protect (Code protection is disabled)
	#pragma config JTAGEN	= OFF			// JTAG Port Enable (JTAG port is disabled)
#endif
//	_CONFIG2( IESO_ON			& PLLDIV_NODIV		& PLL96MHZ_OFF	& FNOSC_FRC			& FCKSM_CSECMD	& OSCIOFNC_ON	& IOL1WAY_OFF	& POSCMOD_NONE	)
#pragma config POSCMOD	= NONE				// Primary Oscillator Select (HS oscillator mode selected)
#pragma config IOL1WAY	= OFF				// IOLOCK One-Way Set Enable bit (Write RP Registers Once)
#pragma config OSCIOFNC	= ON				// Primary Oscillator Output Function (OSCO functions as port I/O (RC15))
#pragma config FCKSM	= CSECMD			// Clock Switching and Monitor (Both Clock Switching and Fail-safe Clock Monitor are disabled)
#pragma config FNOSC	= FRC				// Oscillator Select (Primary oscillator (XT, HS, EC) with PLL module (XTPLL,HSPLL, ECPLL))
#pragma config PLL96MHZ	= OFF				// 96MHz PLL Disable (Enabled)
#pragma config PLLDIV	= NODIV				// USB 96 MHz PLL Prescaler Select bits (Oscillator input divided by 3 (12MHz input))
#pragma config IESO		= ON				// Internal External Switch Over Mode (IESO mode (Two-speed start-up)disabled)

//	_CONFIG3( WPEND_WPENDMEM	& WPCFG_WPCFGDIS	& WPDIS_WPDIS	& ALTPMP_ALPMPDIS	& WUTSEL_FST	& SOSCSEL_EC	& WPFP_WPFP255					)
// CONFIG3
#pragma config WPFP		= WPFP255			// Write Protection Flash Page Segment Boundary (Highest Page (same as page 170))
#pragma config SOSCSEL	= EC				// 
#pragma config WUTSEL	= FST				// 
#pragma config ALTPMP	= ALPMPDIS			// 
#pragma config WPDIS	= WPDIS				// Segment Write Protection Disable bit (Segmented code protection disabled)
#pragma config WPCFG	= WPCFGDIS			// Configuration Word Code Page Protection Select bit (Last page(at the top of program memory) and Flash configuration words are not protected)
#pragma config WPEND	= WPENDMEM			// Segment Write Protection End Page Select bit (Write Protect from WPFP to the last page of memory)
//	_CONFIG4( 0xFFFF )

/** CONFIGURATION Bits **********************************************/

// CONFIG3
//#pragma config WPFP = WPFP511           // Write Protection Flash Page Segment Boundary (Highest Page (same as page 170))
//#pragma config WPDIS = WPDIS            // Segment Write Protection Disable bit (Segmented code protection disabled)
//#pragma config WPCFG = WPCFGDIS         // Configuration Word Code Page Protection Select bit (Last page(at the top of program memory) and Flash configuration words are not protected)
//#pragma config WPEND = WPENDMEM         // Segment Write Protection End Page Select bit (Write Protect from WPFP to the last page of memory)







#endif

