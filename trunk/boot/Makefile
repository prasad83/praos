.SUFFIXES: .asm
MAKEFILE=Makefile
MAKEDEP=$(MAKEFILE)
INCLUDE=../include
NASM=nasm -f elf -i$(INCLUDE)/
CFLAGS= -ansi -g -Wall -W -O2 -nostdinc -fno-builtin -Wbad-function-cast  -Wmissing-declarations -Wmissing-prototypes
CC=gcc $(CFLAGS) -I$(INCLUDE)

LDSCRIPT=../kernel.ld
LD=ld -g -T $(LDSCRIPT) -nostdlib
LIBC=../lib/libc.a
OBJS=kboot.o main.o

all: kernel

install: kernel
	cp -f kernel ../iso/kernel

clean:
	rm -f *.o kernel kernel.sym kernel.lst

# Some implicit rules
.asm.o:
	$(NASM) -o$@ $<
.c.o:
	$(CC) -c -o$@ $<

# Some explicit rules
$(LIBC): ../lib/$(MAKEFILE)
	make -C ../lib/ -f $(MAKEFILE)
	
kboot.o: kboot.asm $(MAKEDEP)
main.o: main.c $(MAKEDEP)

kernel: $(OBJS) $(LDSCRIPT) $(LIBC) $(MAKEDEP)
	$(LD) -o$@ $(OBJS) $(LIBC)
#	objdump --source $@ > kernel.lst
#	nm $@ | sort > kernel.sym
	strip $@	
