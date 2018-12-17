/*
 * greeting.h
 *
 *  Created on: 22.04.2016
 *      Author: hendrik
 */

#ifndef DEBUG_GREETING_H_
#define DEBUG_GREETING_H_

#include <common/version.h>
#include <common/debug.h>

#ifndef HARDWARE_PLATFORM
#define HARDWARE_PLATFORM "undefined"
#error "No HARDWARE_PLATFORM defined"
#endif

#ifndef HARDWARE_BOARD
#define HARDWARE_BOARD "undefined"
#error "No HARDWARE_BOARD defined"
#endif

extern const char cyphos_greeting[];

void inline printGreeting()
{
	DEBUG_STREAM("WELCOME",cyphos_greeting);
}



#endif /* DEBUG_GREETING_H_ */
