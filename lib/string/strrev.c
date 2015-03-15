#include <string.h>

char *strrev(char *src) {
	if(strlen(src) < 1) return src;

	int lastIndex = strlen(src)-1;
	int firstIndex = 0;

	while(firstIndex < lastIndex) {
		char firstChar = src[firstIndex];
		src[firstIndex] = src[lastIndex];
		src[lastIndex] = firstChar;
		++firstIndex; --lastIndex;
	}
	return src;
}
	
