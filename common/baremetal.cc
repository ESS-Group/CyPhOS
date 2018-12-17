/*
 * baremetal.cc
 *
 *  Created on: 20.03.2015
 *      Author: hendrik
 */

#include <common/types.h>

#include <common/debug.h>

extern "C" {
	void abort(void);
	void *memcpy(void *dest, const void *src, size_t n);
	void *memset(void *s, int c, size_t n);
	int raise (int sig);
	void __cxa_pure_virtual(void);
}

void abort(void) {
}

void *memcpy(void *dest, const void *src, size_t n)
{
    char* dst8 = (char*)dest;
    char* src8 = (char*)src;

    while (n--) {
        *dst8++ = *src8++;
    }
    return dest;
}

void *memset(void *s, int c, size_t n) {
	char *dest = (char*)s;
	while(n--) {
		*dest++ = c;
	}
	return s;
}

int raise (int sig)
{
	return 0;
}


void __cxa_pure_virtual(void) {
	// FIXME
	DEBUG_STREAM("PUREVIRTUAL","Pure virtual function called??? __cxa_pure_virtual() triggered");
}
