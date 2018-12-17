/*
 * baremetal.h
 *
 *  Created on: 01.04.2016
 *      Author: hendrik
 */

#ifndef COMMON_BAREMETAL_H_
#define COMMON_BAREMETAL_H_

extern "C" {
	void abort(void);
	void *memcpy(void *dest, const void *src, size_t n);
	int raise (int sig);
	void __cxa_pure_virtual(void);
}

#endif /* COMMON_BAREMETAL_H_ */
