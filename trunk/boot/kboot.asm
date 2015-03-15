;KERNEL START UP CODE (32-BIT)

%include "asm.inc"

SECTION .text
BITS 32

GLOBAL entry
entry:
  ;Check if data segment is loaded properly
  mov eax,[ds_magic]
  cmp eax,DS_MAGIC_NUMBER
  je ds_ok
  ;Display a blinking white-on-red(0x4F) '!' (0x21) and freeze
  mov word [0B8000h], 4F21h  
  jmp short $

ds_ok:
  ;Stop using bootloader GDT, load new GDT
  lgdt [gdt_ptr]
  
  ;Update the segment registers
  mov ax,LINEAR_DATA_SELECT
  mov ds,ax
  mov es,ax
  mov ss,ax
  mov fs,ax
  mov gs,ax
  jmp LINEAR_CODE_SELECT:start

start:
  ;zero the C language BSS 
  ;'bss' and 'end' are defined in the linker script file
  EXTERN BSS, end
  mov edi,bss
  mov ecx,end
  sub ecx,edi
  xor eax,eax  ;zero the contents of eax
  rep stosb    ;copy AH to [ES:DI] ECX times

  ;Setup the Stack pointer
  mov esp,stack

  ;Setup interrupt handlers, then load IDT register
  mov ecx, (idt_end - idt) >> 3 ;number of exception handlers
  mov edi, idt
  mov esi, isr0
  
do_idt:
  mov eax, esi    ;EAX=offset of entry point
  mov [edi], ax   ;Set low 16 bits of gate offset
  shr eax, 16
  mov [edi+6],ax  ;Set high 16 bits of gate offset
  add edi, 8      ;8 bytes/interrupt gate
	add esi,(isr1-isr0) ;bytes/stub
	loop do_idt

	lidt [idt_ptr]

	;GRUB 0.90 leaves the NT bits set in EFLAGS which causes EXCEPTION 10
	;for first IRET attempt using TSS-base task-switch.Let us avoid it:
	push dword 2
	popf

IMP main
	call main	;Call C code
	jmp $			;freeze

;Multiboot header for GRUB bootloader. This must be in the first 8K
;of the kernel file. Let us use aout kludge format of executable that works with ELF

EXTERN code,bss,end

ALIGN 4
mboot:
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd MULTIBOOT_CHECKSUM
	;aout kludge, these must be PHYSICAL addresses
	dd mboot
	dd code
	dd bss
	dd end
	dd entry

;INTERRUPT/EXCEPTION HANDLERS
IMP fault

%macro INTR_MACRO 1	;(byte offset from start of the stub)
isr%1:
	push byte 0	;(0) fake error code
	db 6Ah			;(1) exception
	db %1				;(2) number
	push gs			;(3) push segment registers
	push fs			;(5)
	push es			;(7)
	push ds			;(9) 
	pusha				;(10) push GP (general purpose) registers
	
		mov ax,LINEAR_DATA_SELECT	;(11) put known-good values...
		mov ds,eax							;(15)...in segment registers
		mov es,eax							;(17)		
		mov fs,eax							;(19)
		mov gs,eax							;(21)
		mov eax,esp							;(23)
		push eax								;(25) push pointer to regs_t
.1:
	;setvect() changes the operand of the CALL instruction at run-time,
	;so we need its location = 27 bytes from start of stub. We also want
	;the CALL to use absolute addressing instead of EIP-relative,so:
	mov eax, fault	;(26)
	call eax				;(31)
	jmp all_ints		;(33)
%endmacro					;(38)


%macro INTR_EC_MACRO 1
isr%1:
	nop		;error code already pushed
	nop		;nop+nop=same length as push byte

	db 6Ah ;(1) exception
	db %1	 ;(2) number
	push gs			;(3) push segment registers
	push fs			;(5)
	push es			;(7)
	push ds			;(9) 
	pusha				;(10) push GP (general purpose) registers
	
		mov ax,LINEAR_DATA_SELECT	;(11) put known-good values...
		mov ds,eax							;(15)...in segment registers
		mov es,eax							;(17)		
		mov fs,eax							;(19)
		mov gs,eax							;(21)
		mov eax,esp							;(23)
		push eax								;(25) push pointer to regs_t
.1:
	;setvect() changes the operand of the CALL instruction at run-time,
	;so we need its location = 27 bytes from start of stub. We also want
	;the CALL to use absolute addressing instead of EIP-relative,so:
	mov eax, fault	;(26)
	call eax				;(31)
	jmp all_ints		;(33)
%endmacro					;(38)

;the vector within the stub (operand of the CALL instruction)
;is at (isr0.1-isr0+1)
all_ints:
		pop eax
	popa		;pop GP (general purpose) registers
	pop ds
	pop es
	pop fs
	pop gs
	add esp,8	;drop exception number and error code
	iret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;name: 		getvect
;action: 	reads interrupt vector
;in:			[EBP+12] = vector number
;out:			vector stored at address given by [EBP+8]
;modifies:EAX,EDX
;minimum CPU: '386+
;C prototype:
;	typedef struct { unsigned access_byte,eip; } vector_t;
;	getvect(vector_t *v,unsigned vect_num);
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

EXP getvect
	push ebp
		mov ebp,esp
		push esi
		push ebx
			mov	esi,[ebp+8]
		
			;get access byte from IDT[1]
			xor ebx,ebx
			mov bl,[ebp+12]
			shl ebx,3
			mov al,[idt+ebx+5]
			mov [esi+0],eax
		
			;get handler address from stub
			mov eax,isr1
			sub eax,isr0	;assume stub size < 256 bytes
			mul byte [ebp+12]	
			mov ebx,eax
			add ebx,isr0
			mov eax,[ebx + (isr0.1 - isr0 + 1)]
			mov [esi+4],eax
		pop ebx
		pop esi
	pop ebp
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;name: 		setvect
;action: 	writes interrupt vector
;in: 			[EBP+12] = vector number
;					vector store at address given by [EBP+8]
;out:			nothing
;modifies:EAX,EDX
;minimum cpu: '386+
;C prototype:
;	typedef struct { unsigned access_byte,eip; } vector_t;
;	setvect(vector_t *v,unsigned vect_num);
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
EXP setvect
	push ebp
		mov ebp,esp
		push esi
		push ebx
			mov esi,[ebp+8]
			
			;store access byte in IDT [1]
			mov eax,[esi+0]
			xor ebx,ebx
			mov bl,[ebp+12]
			shl ebx,3
			mov [idt+ebx+5],al
			
			;store handler address in stub
			mov eax,isr1
			sub eax,isr0	;assume stub size < 256 bytes
			mul byte [ebp+12]
			mov ebx,eax
			add ebx,isr0
			mov eax,[esi+4]
			mov [ebx + (isr0.1 - isr0 + 1)],eax
		pop ebx
		pop esi
	pop ebp
	ret


;INTERRUPT/EXCEPTION STUBS
	INTR_MACRO	0			;zero divide (fault)
	INTR_MACRO	1			;debug/single step
	INTR_MACRO	2			;non-maskable interrupt (trap)
	INTR_MACRO	3			;INT3 (trap)
	INTR_MACRO	4			;INTO (trap)
	INTR_MACRO	5			;BOUND (fault)
	INTR_MACRO	6			;invalid opcode (fault)
	INTR_MACRO	7			;coprocessor not available (fault)
	INTR_EC_MACRO	8		;double fault (abort w/error code)
	INTR_MACRO	9			;coproc segment overrun (abort 386/486SX only)
	INTR_EC_MACRO	0Ah	;bad TSS (fault w/error code)
	INTR_EC_MACRO	0Bh	;segment not present (fault w/error code)
	INTR_EC_MACRO	0Ch	;stack fault (fault w/error code0
	INTR_EC_MACRO	0Dh	;GPF (fault w/error code)
	INTR_EC_MACRO	0Eh	;page fault
	INTR_MACRO	0Fh			;reserved
	INTR_MACRO	10h		;FP exception/coprocessor error (trap)
	INTR_MACRO	11h		;aligment check (trap, 486+ only)
	INTR_MACRO	12h		;machine check (Pentium+ only)
	INTR_MACRO	13h
	INTR_MACRO	14h
	INTR_MACRO	15h
	INTR_MACRO	16h
	INTR_MACRO	17h
	INTR_MACRO	18h
	INTR_MACRO	19h
	INTR_MACRO	1Ah
	INTR_MACRO	1Bh
	INTR_MACRO	1Ch
	INTR_MACRO	1Dh
	INTR_MACRO	1Eh
	INTR_MACRO	1Fh

	;isr20 through isr2F are hardware interrupts. The 8259 programmable 
	;interrupt controller (PIC) chips must be reprogrammed to make these work
	INTR_MACRO 20h	;IRQ 0/timer interrupt
	INTR_MACRO 21h	;IRQ 1/keyboard interrupt
	INTR_MACRO 22h	
	INTR_MACRO 23h
	INTR_MACRO 24h
	INTR_MACRO 25h
	INTR_MACRO 26h	;IRQ 6/floppy interrupt
	INTR_MACRO 27h	
	INTR_MACRO 28h	;IRQ 8/real-time clock interrupt
	INTR_MACRO 29h
	INTR_MACRO 2Ah
	INTR_MACRO 2Bh
	INTR_MACRO 2Ch
	INTR_MACRO 2Dh	;IRQ 13/math coprocessor interrupt
	INTR_MACRO 2Eh	;IRQ 14/primary ATA ("IDE") drive interrupt
	INTR_MACRO 2Fh	;IRQ 15/secondary ATA drive interrupt

	;syscall software interrupt	
	INTR_MACRO 30h

	;the other 207 vectors are undefined
	%assign i 31h
	%rep (0FFh -30h)
		INTR_MACRO i
		%assign i (i+1)
	%endrep

;DATA
SECTION .data

ds_magic:
  dd DS_MAGIC_NUMBER

gdt: ;Global Descriptor Table
  ;The GDT contains basic segment descriptors. These segment descriptors
  ;are keeping information about different pairs of the memory.In Real Mode,
  ;one segment is 64kb big followed by the next segment in a 16 byte distance.
  ;In protected mode however, you can decide where to put a segment. Every
  ;segment can be 4Gb.
  dw 0 ;Segment Length 0-15 (low word of segment length)
  dw 0 ;Base Address 0-15   (low word of base address)
  db 0 ;Base Address 16-23  (low byte of high word of base address)
  db 0 ;Flags (segment type and misc flags)
  db 0 ;Access (highest nibble of segment length 16-19)
  db 0 ;Highest byte of base address (24-31)

ldt:  ;Layout Descriptor Table
  ;The LDT is optional and contains segment descriptors, too but these are 
  ;usually used for applications. Operating System could set up the GDT with
  ;it's own system descriptors and for every task a LDT which contains 
  ;the application descriptors.LDT's basically provide different tasks different
  ;memory-layouts.
  dw 0 ;Segment Length 0-15 (low word of segment length)
  dw 0 ;Base Address 0-15   (low word of base address)
  db 0 ;Base Address 16-23  (low byte of high word of base address)
  db 0 ;Flags (segment type and misc flags)
  db 0 ;Access (highest nibble of segment length 16-19)
  db 0 ;Highest byte of base address (24-31)

LINEAR_DATA_SELECT equ $-gdt
  dw 0FFFFh
  dw 0
  db 0
  db 092h ;ring 0,data,expand-up,writable
  db 0CFH ;page-granular (4 Gb limit), 32-bit
  db 0

LINEAR_CODE_SELECT equ $-gdt
  dw 0FFFFh
  dw 0
  db 0
  db 09Ah ;ring 0,code,non-confirming,readable
  db 0CFH ;page-granular (4 Gb limit), 32-bit
  db 0
gdt_end:

gdt_ptr:
  dw gdt_end - gdt - 1
  dd gdt
  

;We shall define now 256 ring 0 interrrupt gates
idt: ;Interrupt Descriptor Table
%rep 256 ;NASM macro to replicate 256 times 
  dw 0               ;Low Word for Handler Offset
  dw LINEAR_CODE_SELECT ;Segment Selector
  db 0               ;Unused in this descriptor
  db 08Eh            ;Flag Byte (Ring 0, '386 interrupt gate)
  dw 0               ;High Word for Handler Offset
%endrep

idt_end:
  
idt_ptr:
  dw idt_end - idt - 1 ;IDT Limit
  dd idt               ;linear address of IDT

;BSS
SECTION .bss
  resd 1024 ;Uninitialized 1024bytes of data
stack:

