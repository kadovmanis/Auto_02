#ifndef	_memory_h_
#define	_memory_h_

#include	"Typedef.h"

void MemInit			(void);
void Mem_SetWriteAddress(U32 addr);
void Mem_SetReadAddress	(U32 addr);
U32	 Mem_GetWriteAddress(void);
U32	 Mem_GetReadAddress	(void);
U16	 Mem_GetType		(void);

inline U8	MemStatus	(void);
//void Mem_WriteByte		(U8 data);
inline S8	Mem_WriteByte	(U8 data);
inline void	Mem_WriteByte_1	(U8 data);
//U8	 Mem_ReadByte		(void);
inline S8	Mem_ReadByte	(U8* ptr);
//void Mem_WriteEnd		(void);

void Mem_PageErase		(void);

#endif	// _memory_h_

