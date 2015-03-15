#include <echo.h>

static short echo_char = 1;

short getecho(void) { return echo_char; }
short setecho(short value) {	
	short old_echo_char = echo_char;
	echo_char = (value == 0)? 0:1;
	return old_echo_char;
}

