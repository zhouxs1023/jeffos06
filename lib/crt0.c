#include <sys/syscall.h>

// Prototype for the main function (created in user code
void main(void);
extern void __libc_init_memory(unsigned int top_of_binary,
						unsigned int start_bss);

extern char __bss_start[];
extern char _end[];


void _start(void)
{

    // Call main in user code
	__libc_init_memory((unsigned int) _end, (unsigned int) __bss_start);
    main();
	terminate_process(0);
}
