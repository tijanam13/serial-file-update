#define _CRT_SECURE_NO_WARNINGS
#include "util.h"
#include <stdio.h>

int util_read_uint(const char* prompt, unsigned* out) {
	if (prompt) printf("%s", prompt);
	unsigned tmp;
	if (scanf("%u", &tmp) != 1) {
		util_clear_line();
		return 0;
	}
	*out = tmp;
	util_clear_line();
	return 1;
}

int util_read_token(const char* prompt, char* buffer, int bufferSize) {
	if (prompt) printf("%s", prompt);
	if (scanf("%s", buffer) != 1) {
		util_clear_line();
		return 0;
	}
	buffer[bufferSize - 1] = '\0';
	util_clear_line();
	return 1;
}

void util_clear_line(void) {
	int c;
	while ((c = getchar()) != EOF && c != '\n');
}