#include <stdio.h>

char *revstr(char *str) {
	if(strlen(str) < 2) return str;

	int lastIndex = strlen(str)-1;
	int firstIndex = 0;
	
	while(firstIndex < lastIndex) {
		char ch = str[firstIndex];
		str[firstIndex] = str[lastIndex];
		str[lastIndex] = ch;
		++firstIndex; --lastIndex;
	}
	printf("RETURN: %s",str);
	return str;
}
		

#define MAX_NUM_STRLEN 15
char numstring[MAX_NUM_STRLEN];
char numlength = 0;
                                                                                                               
const char *unum2str(unsigned number) {
  int index = 0;
  numlength = 0;
  if(number == 0) {
    numstring[0] = '0';
    numstring[1] = '\0';
    return numstring;
  }
  while(number != 0 && index-1 < MAX_NUM_STRLEN) {
    numstring[index++] = (number%10) + '0';
    number = number/10;
  }
  numstring[index] = '\0';
	printf("index: %d",index);
  numlength = strlen(numstring);
  return revstr(numstring);
}

int main() {
	char numstr[10];
	
	int i = 405;

	int index  = 0;
	
	printf("\n%d --> %s\n",i,unum2str(i));
	return 0;
}


	
