.SUFFIXES: .asm

MAKEFILE=Makefile
MAKEDEP=$(MAKEFILE)
INCLUDE=../include
NASM=nasm -f elf -i$(INCLUDE)/
CFLAGS= -ansi -g -Wall -W -O2 -nostdinc -fno-builtin -Wbad-function-cast  -Wmissing-declarations -Wmissing-prototypes 
CC=gcc $(CFLAGS) -I$(INCLUDE)

X86=x86/inportb.o x86/outportb.o x86/enable.o x86/disable.o x86/interrupt.o x86/keyboard.o
STDIO=stdio/stdio.o stdio/mprintf.o
CONIO=conio/conio.o
VIDEO=video/cursor.o video/scroll.o video/echo.o
MM=mm/palloc.o
GLOBALS=globals/globals.o
STRING=string/strlen.o string/strrev.o string/strnum.o string/memfunc.o string/strcpy.o

OBJS=$(GLOBALS) $(X86) $(STDIO) $(CONIO) $(VIDEO) $(STRING) $(MM)

all: libc.a

clean:
	rm -f libc.a $(OBJS)

# Some implicit rules
.asm.o:
	$(NASM) -o$@ $<
.c.o:
	$(CC) -c -o$@ $<

# Some explicit rules
$(LIBC): $(MAKEFILE)
	make -f $(MAKEFILE)

x86/inportb.o: x86/inportb.c $(MAKEDEP)
x86/enable.o: x86/enable.c $(MAKEDEP)
x86/disable.o: x86/disable.c $(MAKEDEP)
x86/outportb.o: x86/outportb.c $(MAKEDEP)
x86/interrupt.o: x86/interrupt.c $(MAKEDEP)
x86/keyboard.o: x86/keyboard.c $(MAKEDEP)

stdio/stdio.o: stdio/stdio.c $(MAKEDEP)
stdio/mprint.o: stdio/mprint.c $(MAKEDEP)

conio/conio.o: conio/conio.c $(MAKEDEP)

video/cursor.o: video/cursor.c $(MAKEDEP)
video/scroll.o: video/scroll.c $(MAKEDEP)
video/echo.o: video/echo.c $(MAKEDEP)

globals/globals.o: globals/globals.c $(MAKEDEP)

mm/palloc.o: mm/palloc.c $(MAKEDEP)

string/strcpy.o: string/strcpy.c $(MAKEDEP)
string/strlen.o: string/strlen.c $(MAKEDEP)
string/strrev.o: string/strrev.c $(MAKEDEP)
string/strnum.o: string/strnum.c $(MAKEDEP)
string/memfunc.o: string/memfunc.c $(MAKEDEP)

libc.a: $(OBJS) $(MAKEDEP)
	ar rcs libc.a $(OBJS)
