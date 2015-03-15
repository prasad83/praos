#include <string.h>
unsigned strlen(const char *src) {
	unsigned length = 0;
	while(*src++ != '\0')	++length;
	return length;
}		
