#include "task.h"

aspace *MmCreatePageTable();

#define MmAllocPages(pdir, npages, flags, phys) MmAllocPagesRaw(pdir, npages, flags, phys, 0)
#define MmAllocPagesLo(pdir, npages, flags, phys) MmAllocPagesRaw(pdir, npages, flags, phys, 1)

int brk(uint32 addr);

PVIRTUAL MmAllocPagesRaw(IN aspace *pdir, 
					IN uint32 npages, 
					IN uint32 flags, 
					OUT PPHYSICAL *phys,
					IN uint32 args);

STATUS MmFreePages(	IN aspace *pdir,
					IN PVIRTUAL virt,
					IN uint32 npages);

STATUS kmap_KernelPage(	IN aspace *pdir,
						IN PPHYSICAL phys,
						OUT PVIRTUAL *virt,
						IN uint32 npages,
						IN uint32 flags);

STATUS kmap_LowPage(	IN aspace *pdir,
						IN PPHYSICAL phys,
						OUT PVIRTUAL *virt,
						IN uint32 npages,
						IN uint32 flags);

STATUS kunmap_Page(	IN aspace *pdir,
					IN PVIRTUAL virt,
					OUT PPHYSICAL *phys,
					IN uint32 npages);

uint32 kget_PageBit(IN uint32 BitNum);

STATUS kset_PageBit(IN uint32 BitNum);

STATUS kunset_PageBit(IN uint32 BitNum);

STATUS MmMappedToPhysical(	IN aspace *pdir, 
							IN PVIRTUAL virt, 
							OUT PPHYSICAL *phys);

STATUS MmMemCount(OUT uint32 *memcount);

void aspace_map(aspace *a, uint32 phys, uint32 virt, uint32 len, uint32 flags);

/*
 * kmem_alloc flags
 */
#define KMEM_KERNEL	0x00000001
#define KMEM_USER	0x00000002
