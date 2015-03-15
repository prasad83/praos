#include <string.h>

char *strncpy(char *dst, const char *src, size_t length) {
	char *start_dst = dst;
	size_t count = 0;
	while(count++ < length && *src != '\0') *dst++ = *src++;
	while(count++ < length)	*dst++ = '\0';

	return start_dst;
}
