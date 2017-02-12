#ifndef	_spi_i2c_h_
#define	_spi_i2c_h_

#include	"Typedef.h"

#define	SPI1_TX_FULL			SPI1STATbits.SPITBF		// SPIx Transmit Buffer Full
#define	SPI1_RX_FULL			SPI1STATbits.SPIRBF		// SPIx Receive Buffer Full
#define	SPI1_RX_FIFO_EMPTY		SPI1STATbits.SRXMPT		// SPIx Receive FIFO Empty
#define	SPI1_RX_SPIROV			SPI1STATbits.SPIROV		// SPIx Receive Overflow Flag
#define	SPI1_SHIFT_EMPTY		SPI1STATbits.SRMPT		// SPIx Shift Register (SPIxSR) Empty	(Enhanced Buffer mode)
#define	SPI1_FIFO_COUNT			SPI1STATbits.SPIBEC		// SPIx SPIx Buffer Element Count		(Enhanced Buffer mode)

#define	SPI2_TX_FULL			SPI2STATbits.SPITBF		// SPIx Transmit Buffer Full
#define	SPI2_RX_FULL			SPI2STATbits.SPIRBF		// SPIx Receive Buffer Full
#define	SPI2_RX_FIFO_EMPTY		SPI2STATbits.SRXMPT		// SPIx Receive FIFO Empty
#define	SPI2_RX_SPIROV			SPI2STATbits.SPIROV		// SPIx Receive Overflow Flag
#define	SPI2_SHIFT_EMPTY		SPI2STATbits.SRMPT		// SPIx Shift Register (SPIxSR) Empty	(Enhanced Buffer mode)
#define	SPI2_FIFO_COUNT			SPI2STATbits.SPIBEC		// SPIx SPIx Buffer Element Count		(Enhanced Buffer mode)

#define	SPI3_TX_FULL			SPI3STATbits.SPITBF		// SPIx Transmit Buffer Full
#define	SPI3_RX_FULL			SPI3STATbits.SPIRBF		// SPIx Receive Buffer Full
#define	SPI3_RX_FIFO_EMPTY		SPI3STATbits.SRXMPT		// SPIx Receive FIFO Empty
#define	SPI3_RX_SPIROV			SPI3STATbits.SPIROV		// SPIx Receive Overflow Flag
#define	SPI3_SHIFT_EMPTY		SPI3STATbits.SRMPT		// SPIx Shift Register (SPIxSR) Empty	(Enhanced Buffer mode)
#define	SPI3_FIFO_COUNT			SPI3STATbits.SPIBEC		// SPIx SPIx Buffer Element Count		(Enhanced Buffer mode)

#define	SPI_STAT_CLEAR_MASK		0b0000011110100011		// SPIxSTAT Register MASK
#define	SPI_STAT_CLEAR_VAL		0b0000000010100000		// SPIxSTAT Register Value if SPI ready and clear
							//	  |||||||||||||||+	--	SPIRBF: SPIx Receive Buffer Full Status
							//	  ||||||||||||||+-	--	SPITBF: SPIx Transmit Buffer Full Status bit
							//	  |||||||||||+++--	--	SISEL<2:0>: SPIx Buffer Interrupt Mode bits (valid in Enhanced Buffer mode) 101=Interrupt when the last bit is shifted out of SPIxSR; now the transmit is complete
							//	  ||||||||||+-----	--	SRXMPT: Receive FIFO Empty bit (valid in Enhanced Buffer mode)
							//	  |||||||||+------	--	SPIROV: Receive Overflow Flag
							//	  ||||||||+-------	--	SRMPT: Shift Register (SPIxSR) Empty bit (valid in Enhanced Buffer mode)
							//	  |||||+++--------	--	SPIBEC<2:0>: SPIx Buffer Element Count bits (valid in Enhanced Buffer mode)
							//	  |||++-----------	--	Unimplemented
							//	  ||+-------------	--	SPISIDL: Stop in Idle Mode
							//	  |+--------------	--	Unimplemented
							//	  +---------------	--	SPIEN: SPIx Enable
#define	SPI1_BUSY()				((SPI1STAT & SPI_STAT_CLEAR_MASK) != SPI_STAT_CLEAR_VAL)
#define	SPI2_BUSY()				((SPI2STAT & SPI_STAT_CLEAR_MASK) != SPI_STAT_CLEAR_VAL)
#define	SPI3_BUSY()				((SPI3STAT & SPI_STAT_CLEAR_MASK) != SPI_STAT_CLEAR_VAL)

#define	IRQ_SPI_1				void __attribute__((interrupt, no_auto_psv)) _SPI1Interrupt(void)	\
								{	_SPI1IF	= 0;	SPI1STATbits.SPIROV = 0;
#define	IRQ_SPI_2				void __attribute__((interrupt, no_auto_psv)) _SPI2Interrupt(void)	\
								{	_SPI2IF	= 0;	SPI2STATbits.SPIROV = 0;
#define	IRQ_SPI_3				void __attribute__((interrupt, no_auto_psv)) _SPI3Interrupt(void)	\
								{	_SPI3IF	= 0;	SPI3STATbits.SPIROV = 0;
/*
typedef struct
{
	union
	{
		U16		reg;
		struct
		{
			U16	SPIRBF	:1;
			U16	SPITBF	:1;
			U16	SISEL	:3;
			U16	SRXMPT	:1;
			U16	SPIROV	:1;
			U16	SRMPT	:1;
			U16	SPIBEC	:3;
			U16			:2;
			U16	SPISIDL	:1;
			U16			:1;
			U16	SPIEN	:1;
		};
		struct
		{
			U16			:2;
			U16	SISEL0	:1;
			U16	SISEL1	:1;
			U16	SISEL2	:1;
			U16			:3;
			U16	SPIBEC0	:1;
			U16	SPIBEC1	:1;
			U16	SPIBEC2	:1;
		};
	};
} SPI_STATBITS;

typedef struct
{
	union
	{
		U8		byte;
		struct
		{
			U8	WIP		:1;
			U8	WEL		:1;
			U8	BP0		:1;
			U8	BP1		:1;
			U8	BP2		:1;
			U8	E_ERR	:1;
			U8	P_ERR	:1;
			U8	SRWD	:1;
		};
	};
} SPI_STATUS;
*/
void Spi1_Init			(void);
void Spi1Clear			(void);
U8	 Spi1Byte			(U8 data);
void Spi2_Init			(void);
void Spi2Clear			(void);
U8	 Spi2Byte			(U8 data);
void Spi3_Init			(void);
void Spi3Clear			(void);
U8	 Spi3Byte			(U8 data);

U8	I2c2Test			(U8 data);
U8	I2c2WriteByte		(U8 slave, U8 address, U8 data);
U8	I2c2Write			(U8 slave, U8 address, P_U8 data, U8 len);
//U8	I2c2Write		(U8 slave, U8 address, const P_U8 data, U8 len)
U8	I2c2Read			(U8 slave, P_U8 data, U8 len);
U8	I2c2ReadReverse		(U8 slave, P_U8 data, U8 len);
U8	I2c2ReadFrom		(U8 slave, U8 address, P_U8 data, U8 len);
U8	I2c2ReadReverseFrom	(U8 slave, U8 address, P_U8 data, U8 len);

#endif //_spi_i2c_h_

