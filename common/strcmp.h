/*
 * strcmp.h
 *
 *  Created on: 10.06.2018
 *      Author: hendrik
 */

#ifndef COMMON_STRCMP_H_
#define COMMON_STRCMP_H_

#include <common/types.h>

#include <common/debug.h>
//
//#ifdef TAG
//#undef TAG
//#endif
//#define TAG "STRING"

inline bool strncmp(char *str1, char *str2, uint32_t len) {
	if (*str1 != *str2) {
		return false;
	}
	while (len != 0 && *str1 != 0 && *str2 != 0) {
//		DEBUG_STREAM(TAG,"Comparing: " << *str1 << " with: " << *str2);
		// Compare strings
		if (*str1 != *str2) {
			return false;
		}
		str1++;
		str2++;
		len--;
	}
	return true;
}

#undef TAG

#endif /* COMMON_STRCMP_H_ */

