/*
 * TacleBinarySearchComponent.cc
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#include <taclebench-components/matrix1/Taclematrix1Component.h>
#include <taclebench-components/common/common.h>

#include "matrix1.h"

#include <eventhandling/EventHandler.h>

BEGIN_OSC_IMPLEMENTATION(Taclematrix1Component, Taclematrix1Component)

Taclematrix1Component Taclematrix1Component::mInstance;

SECTION_CRITICAL_DATA OSC* Taclematrix1Component::trigger_deps[] = {nullptr};
SECTION_CRITICAL_DATA DEFINE_TRIGGER_AFFINITY(Taclematrix1Component, main, Taclematrix1Component::mInstance, trigger_deps, TACLE_BENCH_PRIORITY, TACLE_BENCH_AFFINITY);

Taclematrix1Component::Taclematrix1Component() {


}

DEFINE_TRIGGER_FUNC(Taclematrix1Component,main) {
#ifdef TACLE_BENCH_MULTI_RUN_COUNT
	for (uint32_t run = 0; run < TACLE_BENCH_MULTI_RUN_COUNT; run++)
#endif
	main();
#ifdef TACLE_BENCH_AUTOREPEAT
	EventHandling::EventHandler::pInstance.callOSCTrigger(&trigger_main, 0);
#endif
}

END_OSC_IMPLEMENTATION
