//
// Portions of this code are from OpenBlt, by Brian Swetland
//

#ifndef _MEMORY_H_
#define _MEMORY_H_

#define KM16   0
#define KM32   1
#define KM64   2
#define KM128  3
#define KM256  4
#define KM512  5
#define KM1024 6

void *kmalloc(int size);
void *kmallocB(int size);
void kfree(int size, void *block);
void kfreeB(int size, void *block);
void memory_init(void);
void memory_status(void);

#define kmalloc16() kmalloc(KM16)
#define kmalloc32() kmalloc(KM32)
#define kmalloc64() kmalloc(KM64)
#define kfree16(v)  kfree(KM16,v)
#define kfree32(v)  kfree(KM32,v)
#define kfree64(v)  kfree(KM64,v)

#endif

