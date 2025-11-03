#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
	char buf[64];

	write(STDOUT_FILENO, "Starting test...\n", 17);

	memset(buf, 'X', sizeof(buf));
	buf[63] = '\0';

	write(STDOUT_FILENO, "About to call sprintf...\n", 25);
	int ret = sprintf(buf, "hello");
	write(STDOUT_FILENO, "sprintf returned\n", 17);

	printf("string: ret=%d buf='%s'\n", ret, buf);

	memset(buf, 'X', sizeof(buf));
	ret = sprintf(buf, "%.2f", 3.14);

	printf("float: ret=%d buf='%s'\n", ret, buf);

	return 0;
}
