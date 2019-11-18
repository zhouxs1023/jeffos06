#include <types.h>
#include <conio.h>
#include "mem.h"
#include "status.h"
#include "helper.h"
#include "ktrace.h"

unsigned long nPagesFree;
unsigned long nMemMax;
unsigned char PMAT[2048];
uint32 flat;

extern uint32 _cr3;
extern uint32 *KernelPageTable;
extern task* CurrentTask;

int
brk(uint32 addr)
{
	uint32 i;
	aspace *a = CurrentTask->Parent->addr;
	uint32 VirtMem;
	uint32 PhysMem;

	addr /= 4096;

	if(addr > 512)
		return -1;

	for(i = 0; i <= addr; i++)
	{
		if(!(a->ptab[i]))
		{
			VirtMem = (uint32)MmAllocPagesLo(a, 1, 7, &PhysMem);
			kunmap_Page(a, VirtMem, &PhysMem, 1);
			aspace_map(a, (PhysMem / 4096), i, 1, 7);
		}
	}

	if(i != 0)
		return 0;

	return -1;
}

STATUS
MmMappedToPhysical(IN aspace *pdir, IN PVIRTUAL virt, OUT PPHYSICAL *phys) {
	unsigned long i;

	if(virt >= (PVIRTUAL)0x80000000) {
		/* High page table */
		/* 
		 * Find out the physical memory address of a paged memory address
		 */
		i = (((uint32)virt) - 0x80000000) / 4096;
		*phys = (PPHYSICAL) (pdir->high[i] & 0xfffff000);
	} else {
		i = ((uint32) virt) / 4096;
		*phys = (PPHYSICAL) (pdir->ptab[i] & 0xfffff000);
	}

//	kprintf("Mapped memory: %x is physical address: %x\n", virt, *phys);

	return STATUS_SUCCESS;

}


PVIRTUAL
MmAllocPagesRaw(IN aspace *pdir,
		IN uint32 npages, 
		IN uint32 flags, 
		OUT PPHYSICAL *phys,
		IN uint32 args ) {
	PVIRTUAL virt;
	uint32 i;
	uint32 j;

	// Allocate npages of physical memory, map them into memory, and return
	// a pointer to the location of the physical memory as well

	// A simple and crude search for 'npages' of contigous free physical 
	// memory
	/*
	 * NOTE: What the heck is 16384??? (Max number of pages it seems)
	 */
	for(i = 0; i < (16384 - npages); i++) {
		for(j = 0; j < npages; j++) {
			if(kget_PageBit(i + j)) {
				i += j;
				break;
			}
		}
		if(j == npages) {
			for(j = 0; j < npages; j++)
				kset_PageBit(i + j);
			i = i * 4096;
			goto mapmemory;
		}
	}
	kprintf("kmem: ERROR, there is not enough memory left to allocate %d pages\n", npages);
	return 0;

mapmemory:
	// i stores the beginning location of the physical memory to map in
	if(phys != NULL)
		*phys = (PPHYSICAL) i;


	// Debugging statement
//	kprintf("[allocated real memory of: 0x%x  0x%x pages", i, npages);

	if( args == 0)
	{
		// return the mapped in pointer of the memory
		kmap_KernelPage(pdir, (PPHYSICAL)i, &virt, npages, flags);
	} else if( args == 1 )
	{
		// Map in physical memory to low pages
		kmap_LowPage(pdir, (PPHYSICAL)i, &virt, npages, flags);
	}

//	kprintf(" at: 0x%x]\n", virt);


	return virt;
}

			
STATUS
MmFreePages(IN aspace *pdir,
			IN PVIRTUAL virt, 
			IN uint32 npages) {
	uint32 i;
	uint32 startbit;

	// Free npages of memory from the mapped in address starting at memp
//kprintf("Freeing: virt: 0x%x #%d pages\n", virt, npages);

	// Sanity checks
	i = (uint32)virt;
	if(i % 4096) {
		kprintf("kmem_free: Error trying to deallocate non 4k boundaried memory at: %x\n", virt);
		return STATUS_GENERAL_ERROR;
	}

	if(!virt) {
		kprintf("kmem_free: Error trying to deallocate null pointer\n");
		return STATUS_GENERAL_ERROR;
	}


	// First unmap the memory from the page tables
	kunmap_Page(pdir, virt, (PPHYSICAL *)&startbit, npages);
	startbit = startbit / 4096;
	for(i = 0; i < npages; i++) {
		if(kget_PageBit(startbit + i))
			kunset_PageBit(startbit + i);
		else {
			kprintf("kmem_free: Trying to unset memory which is not set\n");
			return STATUS_GENERAL_ERROR;
		}
	}
	return 0;
}

STATUS
kmap_LowPage(IN aspace *pdir,
				IN PPHYSICAL phys, 
				OUT PVIRTUAL *virt, 
				IN uint32 npages, 
				IN uint32 flags) {
	uint32 i;
	uint32 j;

//kprintf(" Phys pdir: 0x%x ", ((pdir->pdir[0] - 4096) / 4096));

	/* Map the physical memory pointer memp into the next free pages */
	for(i = 0; i < (1024 - npages); i++) {
		for(j = 0; j < npages; j++) {
			if(pdir->ptab[i + j]) {
				i += j;
				break;
			}
		}
		if(j == npages) {
			for(j = 0; j < npages; j++) {
				pdir->ptab[i + j] = (((uint32)phys) + (4096 * j)) | flags;
			}
			goto done;
		}
	}
	kprintf("kmem_MapPage: ERROR, no space left in kernel memory to map physical memory into.\n");
	// Halt the kernel
	KeHalt();
	return STATUS_GENERAL_ERROR;

done:
	*virt =  (PVIRTUAL)(4096 * i);
	return STATUS_SUCCESS;
}

STATUS
kmap_KernelPage(IN aspace *pdir,
				IN PPHYSICAL phys, 
				OUT PVIRTUAL *virt, 
				IN uint32 npages, 
				IN uint32 flags) {
	uint32 i;
	uint32 j;

//kprintf(" Phys pdir: 0x%x ", ((pdir->pdir[0] - 4096) / 4096));

	/* Map the physical memory pointer memp into the next free pages */
	for(i = 0; i < (1024 - npages); i++) {
		for(j = 0; j < npages; j++) {
			if(pdir->high[i + j]) {
				i += j;
				break;
			}
		}
		if(j == npages) {
			for(j = 0; j < npages; j++) {
				pdir->high[i + j] = (((uint32)phys) + (4096 * j)) | flags;
			}
			goto done;
		}
	}
	kprintf("kmem_MapPage: ERROR, no space left in kernel memory to map physical memory into.\n");
	// Halt the kernel
	KeHalt();
	return STATUS_GENERAL_ERROR;

done:
//kprintf("Memory mapping starting at offset: 0x%x\n", i * 4096);
	*virt =  (PVIRTUAL)(0x80000000 + (4096 * i));
	return STATUS_SUCCESS;
}


STATUS
kunmap_Page(IN aspace *pdir,
			IN PVIRTUAL virt, 
			OUT PPHYSICAL *phys,
			IN uint32 npages) {
	uint32 i;
	uint32 j;

	if(virt >= (PVIRTUAL)0x80000000) {
		i = (((uint32)virt) - 0x80000000) / 4096;
		*phys = (PPHYSICAL) (pdir->high[i] & 0xFFFFF000);

		for(j = 0; j < npages; j++) {
			if(!pdir->high[i + j]) {
				kprintf("kunmap_Page: ERROR attempting to unmap KERNEL page table which is already 0\n");
				return STATUS_GENERAL_ERROR;
			}
			pdir->high[i + j] = 0;
		}
	} else {
		i = ((uint32)virt) / 4096;
		*phys = (PPHYSICAL) (pdir->ptab[i] & 0xFFFFF000);
		for(j = 0; j < npages; j++) {
			if(!pdir->ptab[i + j]) {
				kprintf("kunmap_Page: ERROR attempting to unmap USER page table which is already 0\n");
				return STATUS_GENERAL_ERROR;
			}
			pdir->ptab[i + j] = 0;
		}
	}

	return STATUS_SUCCESS;
}


uint32
kget_PageBit(IN uint32 BitNum) {
	uint32 Bit;
	uint32 b = 0;

	b |= (1 << (BitNum % 8));
	Bit = PMAT[BitNum >> 3];
	Bit &= b;

	if(Bit)
		return 1;
	else
		return 0;

}

STATUS
kset_PageBit(uint32 BitNum) {
        uint32 b = 0;

        b |= (1 << (BitNum % 8));    // Find offset into byte to place bit
        PMAT[BitNum >> 3] |= b;       // AND in UnSet BIT

		return STATUS_SUCCESS;
}                          


STATUS
kunset_PageBit(IN uint32 BitNum) {
        uint32 b = 0;

        b |= (1 << (BitNum % 8));    // Find offset into byte to place bit
        PMAT[BitNum >> 3] &= ~b;       // AND in UnSet BIT
//kprintf("kunset: BitNum: %x &= b: %x  PMAT[%d]: %x\n", BitNum, ~b, BitNum >> 3,PMAT[BitNum >> 3]);

		return STATUS_SUCCESS;
}                                   


aspace *
MmCreatePageTable( void ) {
	uint32 i;
	PPHYSICAL phys;
	aspace *pdir;

	pdir = MmAllocPages((aspace *)KernelPageTable, 2, 3, &phys);
	if(pdir == NULL) {
		kprintf("Failure to allocate memory for page table\n");
		return 0;
	}
	

	for(i = 0; i < 1024; i++) {
		pdir->pdir[i] = 0;
		pdir->ptab[i] = 0;
	}

	/* Map page table into page directory */
	pdir->pdir[0] = ((uint32) phys + 4096) | 7;
	/* Map kernels high page table into page directory */
	pdir->pdir[512] = (_cr3 + 2 * 4096) | 3;

	pdir->refcount = 0;

	return pdir;
}

void
aspace_map(aspace *a, uint32 phys, uint32 virt, uint32 len, uint32 flags) {
	uint32 i;
	uint32 *b = 0;

//kprintf("aspace_map called. a: 0x%x  phys: 0x%x  virt: 0x%x  \n      len: 0x%x  flags: 0x%x\n",
//	a, phys, virt, len, flags);

	for(i = 0; i < len; i++)
		a->ptab[virt + i] = ((phys + i) * 4096) | flags;

//b = virt * 4096;
//kprintf("virt[0] = 0x%x\n", b[0]);
//kprintf("value at a->ptab[%x] = 0x%x\n", virt + i, ((phys + i) * 4096) | flags);
}


