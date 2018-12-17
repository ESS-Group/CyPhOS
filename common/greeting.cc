/*
 * greeting.cc
 *
 *  Created on: 07.03.2017
 *      Author: hendrik
 */

#include "greeting.h"

#define xstr(s) str(s)
#define str(s) #s


const char cyphos_greeting[] = {OS_NAME " version: " OS_VERSION "\n"
		"  ______ ____    ____ .______    __    __    ______        _______." "\n"
		" /      |\\   \\  /   / |   _  \\  |  |  |  |  /  __  \\      /       |" "\n"
		"|  ,----' \\   \\/   /  |  |_)  | |  |__|  | |  |  |  |    |   (----`" "\n"
		"|  |       \\_    _/   |   ___/  |   __   | |  |  |  |     \\   \\" "\n"
		"|  `----.    |  |     |  |      |  |  |  | |  `--'  | .----)   |" "\n"
		" \\______|    |__|     | _|      |__|  |__|  \\______/  |_______/" "\n"
#ifdef __GNUC__
	"Compiled "  __DATE__  ":"  __TIME__ "\n"
	"GCC version: "  __VERSION__ "\n"
#endif
	"Platform: "  HARDWARE_PLATFORM "\n"
	"Board: "  HARDWARE_BOARD "\n"
#ifdef __ARM_FEATURE_UNALIGNED
	"Unaligned memory access enabled" "\n"
#endif
	"Git commit:" xstr(GIT_COMMIT) "\n"
};
