#include <x86.h>
void outportb(unsigned port, unsigned val) {
	__asm__ __volatile__(
		"outb %b0, %w1"
		:
		: "a"(val), "d"(port)
	);
}		
