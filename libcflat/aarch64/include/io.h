#ifndef _ASMARM_IO_H_
#define _ASMARM_IO_H_
#include "libcflat.h"

#define __iomem
#define __force

#define __bswap16 bswap16
static inline u16 bswap16(u16 val)
{
	u16 ret;
	asm volatile("rev16 %0, %1" : "=r" (ret) :  "r" (val));
	return ret;
}

#define __bswap32 bswap32
static inline u32 bswap32(u32 val)
{
	u32 ret;
	asm volatile("rev %0, %1" : "=r" (ret) :  "r" (val));
	return ret;
}

#define __raw_readb __raw_readb
static inline u8 __raw_readb(const volatile void __iomem *addr)
{
	u8 val;
	asm volatile("ldrb %w1, %w0"
		     : "+Qo" (*(volatile u8 __force *)addr),
		       "=r" (val));
	return val;
}

#define __raw_readw __raw_readw
static inline u16 __raw_readw(const volatile void __iomem *addr)
{
	u16 val;
	asm volatile("ldrh %w1, %x0"
		     : "+Q" (*(volatile u16 __force *)addr),
		       "=r" (val));
	return val;
}

#define __raw_readl __raw_readl
static inline u32 __raw_readl(const volatile void __iomem *addr)
{
	u32 val;
	asm volatile("ldr %x1, %x0"
		     : "+Qo" (*(volatile u32 __force *)addr),
		       "=r" (val));
	return val;
}

#define __raw_writeb __raw_writeb
static inline void __raw_writeb(u8 val, volatile void __iomem *addr)
{
	asm volatile("strb %w1, %w0"
		     : "+Qo" (*(volatile u8 __force *)addr)
		     : "r" (val));
}

#define __raw_writew __raw_writew
static inline void __raw_writew(u16 val, volatile void __iomem *addr)
{
	asm volatile("strh %w1, %x0"
		     : "+Q" (*(volatile u16 __force *)addr)
		     : "r" (val));
}

#define __raw_writel __raw_writel
static inline void __raw_writel(u32 val, volatile void __iomem *addr)
{
	asm volatile("str %x1, %x0"
		     : "+Qo" (*(volatile u32 __force *)addr)
		     : "r" (val));
}

#include "asm-generic/io.h"

#endif /* _ASMARM_IO_H_ */
