/*
 * TacleBinarySearchComponent.cc
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#include <taclebench-components/insertsort/TacleinsertsortComponent.h>
#include <taclebench-components/common/common.h>

#include "insertsort.h"

BEGIN_OSC_IMPLEMENTATION(TacleinsertsortComponent, TacleinsertsortComponent)

TacleinsertsortComponent TacleinsertsortComponent::mInstance;

SECTION_CRITICAL_DATA OSC* TacleinsertsortComponent::trigger_deps[] = {nullptr};
SECTION_CRITICAL_DATA DEFINE_TRIGGER_AFFINITY(TacleinsertsortComponent, main, TacleinsertsortComponent::mInstance, trigger_deps, TACLE_BENCH_PRIORITY, TACLE_BENCH_AFFINITY);

TacleinsertsortComponent::TacleinsertsortComponent() {


}

DEFINE_TRIGGER_FUNC(TacleinsertsortComponent,main) {
#ifdef TACLE_BENCH_MULTI_RUN_COUNT
	for (uint32_t run = 0; run < TACLE_BENCH_MULTI_RUN_COUNT; run++)
#endif
	main();
#ifdef TACLE_BENCH_AUTOREPEAT
	EventHandling::EventHandler::pInstance.callOSCTrigger(&trigger_main, 0);
#endif
}

END_OSC_IMPLEMENTATION
