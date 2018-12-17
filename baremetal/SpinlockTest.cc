#include "MemoryProfiling.h"

#include <common/debug.h>

#define TAG "SpinlockTest"

#include <common/types.h>


#include <sync/barrier.h>


extern "C" {
	void restart_system();
}


/*
 * Entry points used by startup code if baremetal testing is enabled.
 */
void (*baremetal_entry_points[])() = {
	&entry_func_0,
	&entry_func_1,
	&entry_func_2,
	&entry_func_3
};

static Barrier barrier(4);

inline void printText(uint8_t index) {
	while (1) {
		barrier.await();
		if (index == 0) {
			DEBUG_STREAM(TAG,"Barrier 0 ");
		}
		barrier.await();
		if (index == 1) {
			DEBUG_STREAM(TAG,"Barrier 1 ");
		}
		barrier.await();
		if (index == 2) {
			DEBUG_STREAM(TAG,"Barrier 2 ");
		}
		barrier.await();
		if (index == 3) {
			DEBUG_STREAM(TAG,"Barrier 3 ");
		}
	}
}

void entry_func_0() {
	printText(0);
}

void entry_func_1() {
	printText(1);
}

void entry_func_2() {
	printText(2);
}

void entry_func_3() {
	printText(3);
}
