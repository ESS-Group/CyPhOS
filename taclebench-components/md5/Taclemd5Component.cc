/*
 * TacleBinarySearchComponent.cc
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#include <taclebench-components/md5/Taclemd5Component.h>
#include <taclebench-components/common/common.h>

#include "md5.h"

#include <eventhandling/EventHandler.h>

BEGIN_OSC_IMPLEMENTATION(Taclemd5Component, Taclemd5Component)

Taclemd5Component Taclemd5Component::mInstance;

SECTION_CRITICAL_DATA OSC* Taclemd5Component::trigger_deps[] = {nullptr};
SECTION_CRITICAL_DATA DEFINE_TRIGGER_AFFINITY(Taclemd5Component, main, Taclemd5Component::mInstance, trigger_deps, TACLE_BENCH_PRIORITY, TACLE_BENCH_AFFINITY);

Taclemd5Component::Taclemd5Component() {


}

DEFINE_TRIGGER_FUNC(Taclemd5Component,main) {
#ifdef TACLE_BENCH_MULTI_RUN_COUNT
	for (uint32_t run = 0; run < TACLE_BENCH_MULTI_RUN_COUNT; run++)
#endif
	main();
#ifdef TACLE_BENCH_AUTOREPEAT
	EventHandling::EventHandler::pInstance.callOSCTrigger(&trigger_main, 0);
#endif
}

END_OSC_IMPLEMENTATION
