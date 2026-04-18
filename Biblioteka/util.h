#ifndef UTIL_H
#define UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

	void util_clear_line(void);

	int util_read_uint(const char* prompt, unsigned* out);

	int util_read_token(const char* prompt, char* buffer, int bufferSize);

#ifdef __cplusplus
}
#endif

#endif #pragma once
