#include <x86.h>
void enable(void) {
	__asm__ __volatile__ ("sti" : : );
}
