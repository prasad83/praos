#include <stdio.h>
#include <malloc.h>
#include <string.h>


/**
 * export MALLOC_CHECK_=0 or 1 or 2 at command line 
 * to check the working of free and allocate
 */
int main(int argc,char **argv,char **envp) {
	char *p1, *p2, *p3;
	int index = 0;

	p1 = (char *) malloc(10);
	p2 = (char *) malloc(10);
	//p3 = (char *) malloc(10);
	
	printf("ALLOCATE: p1 start %u %u %d%d%d\n",p1, *(p1-4), *(p1+10), *(p1+11), *(p1+12));
	printf("ALLOCATE: p2 start %u %u %d%d%d\n",p2, *(p2-4), *(p2+10), *(p2+11), *(p2+12));
	printf("ALLOCATE: p3 start %u %u\n",p3, *(p3-4));

	strncpy(p1,"This is longer than 10\n",13); /* Corrput the memory */
	p3 = (char *) malloc(10);
	free(p2);
	printf("FREE: p2 start %u\n",p2);
	
	p2 = (char *) malloc(19);
	printf("ALLOCATE: p2 start %u\n",p2);
	

	return 0;
}
