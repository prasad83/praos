#include <stdio.h>
#include <stdarg.h>


static void mputc(const unsigned char ch) {
	printf("%c",ch);
}

static void mputnum(long number,int base) {
	unsigned char numbuf[15];
	int numlen = 0;
	int index = 0;
	
	int remainder = 0;
	
	numlen = 0;
	do {
		remainder = (number % base);
		if(remainder > 9 && base == 16) {
			remainder = 'A' + (remainder-10);
		} else {
			remainder = remainder + '0';
		}
		numbuf[numlen++] = remainder;
		number = number / base;
	} while(number != 0);
	if(base == 16) { numbuf[numlen++] = 'X'; numbuf[numlen++]='0'; }
	for(index = numlen-1; index >= 0; --index)
		mputc(numbuf[index]);	
}

static void mputstr(const unsigned char *prntstr) {
	while(*prntstr) mputc(*prntstr++);
}

static void parse_printf(const char *fmt, va_list args) {
	/* Character to be printed */
	unsigned char prntch;
	/* String to be printed */
	unsigned char *prntstr;
	/* Number to be printed */
	long number;

	while(*fmt != '\0') {
		switch(*fmt) {
			case '%':
				if(*(fmt+1) == 'c') {
					prntch = (unsigned char) va_arg(args,int);
					mputc(prntch);
					fmt++;
					break;
				}
				if(*(fmt+1) == 'd') {
					number = (long) va_arg(args,int);
					mputnum(number,10);
					fmt++;
					break;
				}
				if(*(fmt+1) == 'u') {
					number = (long) va_arg(args,unsigned);
					mputnum(number,10);
					fmt++;
					break;
				}
				if(*(fmt+1) == 'x') {
					number = (long) va_arg(args,int);
					mputnum(number,16);
					fmt++;
					break;
				}
				if(*(fmt+1) == 's') {
					prntstr = (unsigned char *) va_arg(args,char *);
					mputstr(prntstr);
					fmt++;
					break;
				}
			default:
				mputc(*fmt);
				break;
		}
		fmt++;
	}
}

void mprintf(const char *fmt, ...){
	va_list args;

	va_start(args,fmt);
	parse_printf(fmt,args);
	va_end(args);
}

int main() {
	mprintf("This is '%c'<-(char), %d <-(int), %s <-(str), %u <-(unsigned) data\n",
	'a',12345,"'Some word here'",0xF3);
	mprintf("%x <- (hex)\n",0xF3);
	return 0;
}
