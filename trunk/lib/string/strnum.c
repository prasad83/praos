#include <string.h>

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
    numstring[index] = (number%10) + '0';
    number = number/10;
		++index;
  }
  numstring[index] = '\0';

	numlength = strlen(numstring);
                                                                                                               
  return strrev(numstring);
}
