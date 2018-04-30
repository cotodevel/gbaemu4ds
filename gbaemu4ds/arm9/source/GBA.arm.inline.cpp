#include "GBA.h"
#include "GBAinline.h"
#include "fatfileextract.h"
#include "Util.h"

__attribute__((section(".itcm")))
u32 CPUReadMemory(u32 address)
{
	return CPUReadMemoryreal(address);
}

__attribute__((section(".itcm")))
s16 CPUReadHalfWordSignedoutline(u32 address)
{
	return (s16)CPUReadHalfWordSigned(address);
}

__attribute__((section(".itcm")))
s8 CPUReadByteSigned(u32 address)
{
	return (s8)CPUReadBytereal(address);
}

__attribute__((section(".itcm")))
u32 CPUReadHalfWord(u32 address)
{
	return CPUReadHalfWordreal(address);
}

__attribute__((section(".itcm")))
u8 CPUReadByte(u32 address)
{
	return CPUReadBytereal(address);
}

__attribute__((section(".itcm")))
void CPUWriteMemoryextern(u32 address, u32 value)
{
	CPUWriteMemory(address,value);
}

__attribute__((section(".itcm")))
void CPUWriteHalfWordextern(u32 address, u16 value)
{
	CPUWriteHalfWord(address,value);
}

__attribute__((section(".itcm")))
void CPUWriteByteextern(u32 address, u8 b)
{
	CPUWriteByte(address,b);
}

__attribute__((section(".itcm")))
u8 ichfly_readu8extern(unsigned int pos)
{
	return ichfly_readu8(pos);
}

__attribute__((section(".itcm")))
u16 ichfly_readu16extern(unsigned int pos)
{
	return ichfly_readu16(pos);
}

__attribute__((section(".itcm")))
u32 ichfly_readu32extern(unsigned int pos)
{
	return ichfly_readu32(pos);
}

__attribute__((section(".itcm")))
u32 CPUReadMemorypu(u32 address)
{
	return CPUReadMemoryrealpu(address);
}

__attribute__((section(".itcm")))
u32 CPUReadHalfWordpu(u32 address)
{
	return CPUReadHalfWordrealpu(address);
}

__attribute__((section(".itcm")))
u8 CPUReadBytepu(u32 address)
{
	return CPUReadByterealpu(address);
}

__attribute__((section(".itcm")))
void CPUWriteMemorypuextern(u32 address, u32 value)
{
	CPUWriteMemorypu(address,value);
}

__attribute__((section(".itcm")))
void CPUWriteHalfWordpuextern(u32 address, u16 value)
{
	CPUWriteHalfWordpu(address,value);
}

__attribute__((section(".itcm")))
void CPUWriteBytepuextern(u32 address, u8 b)
{
	CPUWriteBytepu(address,b);
}

__attribute__((section(".itcm")))
s16 CPUReadHalfWordrealpuSignedoutline(u32 address)
{
	return (s16)CPUReadHalfWordrealpuSigned(address);
}

__attribute__((section(".itcm")))
s8 CPUReadByteSignedpu(u32 address)
{
	return (s8)CPUReadByterealpu(address);
}