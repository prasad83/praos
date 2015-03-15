int main(int argc,char *argv[]) {
	if(argc < 2) {
		printf("Usage: %s <character>\n",argv[0]);
		return;
	}
	printf("Char %c value is %d (hex=%x)\n", *argv[1],*argv[1],*argv[1]);
	return 0;
}
