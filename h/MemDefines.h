#ifndef	_mem_defines_
#define	_mem_defines_

#include	"Typedef.h"

#define	MEM_SPI_TIMEOUT			10						// Time out after last received byte to save data & stop spi

#define	MAX_DEVICE_COUNT		6						// Max memory devices to access by Chip Select

#define	MEM_SECTORS_MP25P40		8						// 4Mbit	512kB
#define	MEM_SECTORS_MP25P32		64						// 32Mbit	4MB
#define	MEM_SECTORS_MP25P64		128						// 64Mbit	8MB		sector size = 64kB	(65 536B)
//#define	MEM_SECTORS_S25FL512S	256						// 512Mbit	64MB	sector size = 256kB (262 144B) page size 512B
#define	MEM_SECTORS_S25FL512S	1024					// 512Mbit	64MB	sector size = 256kB (262 144B) page size 512B

#define	MEM_PAGE_SIZE			0x100					// size of 1 page in bytes					256	bytes
#define	MEM_SECTOR_SIZE			0x10000					// (MEM_PAGE_SIZE * 256 Pages in Sector)	64	kB
#define	MEM_PAGE_SIZE_512		0x200					// size of 1 page in bytes					512	bytes
#define	MEM_SECTOR_SIZE_512		0x40000					// (MEM_PAGE_SIZE_512 * 512 Pages in Sector)256	kB
#define	MEM_SIZE_512			0x4000000				// (MEM_PAGE_SIZE_512 * 512 * 256 Sectors)
//#define	MEM_SIZE			MEM_PAGE_SIZE * MEM_SECTOR_SIZE * MEM_SECTORS	// sectors * 256 pages * 256 bytes

#define	MEM_CMD_WREN			0x06					// Write Enable
#define	MEM_CMD_WRDI			0x04					// Write Disable
#define	MEM_CMD_RDID			0x9F					// Read Identification
#define	MEM_CMD_RDSR			0x05					// Read Status Register
#define	MEM_CMD_WRSR			0x01					// Write Status Register
#define	MEM_CMD_READ			0x03					// Read Data bytes
#define	MEM_CMD_4READ			0x13					// Read Data bytes (4-byte Address)
#define	MEM_CMD_FREAD			0x0B					// Read Data bytes at higher speed
#define	MEM_CMD_PP				0x02					// Page Program
#define	MEM_CMD_PW				0x20					// Page Write	(modify page - automatic erase/write of page)
#define	MEM_CMD_4PP				0x12					// Page Program (4-byte Address)
#define	MEM_CMD_SE				0xD8					// Sector Erase
#define	MEM_CMD_SSE				0x20					// SUBSECTOR Erase
#define	MEM_CMD_4SE				0xDC					// Sector Erase 256 kB (4-byte Address)
#define	MEM_CMD_BE				0xC7					// Bulk Erase
#define	MEM_CMD_DP				0xB9					// Deep Power-down
#define	MEM_CMD_RES				0xAB					// Release from Deep Powerdown,(and Read Electronic Signature)
#define	MEM_STATUS_WIP			0x01					// Write In Progress  (WIP) bit
#define	MEM_STATUS_WEL			0x02					// Write Enable Latch (WEL) bit

#define	SERVICE_WORD_SIZE_BYTES	(sizeof(U16))			// Service word size = sizeof(U16)
#define	ROW_SIZE_SIZE_BYTES		(sizeof(U16))			// Printer row size = sizeof(U16)
#define	SECTOR_SIZE_BYTES		(sizeof(U16))			// Sector size = sizeof(U16)
#define	PAGE_BITS_SIZE			(256 / 8)				// 1 bit per Page (256 pages / 8bit in byte)
#define	ADDR_MIN_SIZE			(2)						// at least 2 bytes must be saved to avoid coruuption of service sector

#define	ROW_SIZE_ADDR			(MEM_SECTOR_SIZE		- ROW_SIZE_SIZE_BYTES)	// 65536 - 2	= 65 534	0xFFFE
#define	SERVICE_SECTOR_ADDR		(MEM_SECTOR_SIZE		- sizeof(ServiceData))	// 65536 - ServiceData
#define	SERVICE_ADDR_NULL		(SERVICE_SECTOR_ADDR	- (sizeof(U32)))		// ServiceData - 4
#define	SERVICE_ADDR_ADDR		(SERVICE_ADDR_NULL		- (sizeof(U32)))		// ServiceData - 8

//#define	SERVICE_WORD_ADDR		(MEM_SECTOR_SIZE	- SERVICE_WORD_SIZE_BYTES)	// 65536 - 2	= 65 534	0xFFFE
//#define	ROW_SIZE_ADDR			(SERVICE_WORD_ADDR	- ROW_SIZE_SIZE_BYTES)		// 65534 - 2	= 65 532	0xFFFC
//#define	SERVICE_SECTOR_ADDR		(ROW_SIZE_ADDR		- SECTOR_SIZE_BYTES)		// 65532 - 2	= 65 530	0xFFFA
//#define	SERVICE_PAGE_ADDR		(SERVICE_SECTOR_ADDR - PAGE_BITS_SIZE)			// 65530 - 32	= 65 598	0xFFDA
//#define	SERVICE_ADDR_ADDR		(SERVICE_PAGE_ADDR	- ADDR_MIN_SIZE)			// 65 598 - 2	= 65 496	0xFFD8
//#define	SERVICE_SECTOR_PAGE		(SERVICE_SECTOR_ADDR >> 8)

#endif	// _mem_defines_
