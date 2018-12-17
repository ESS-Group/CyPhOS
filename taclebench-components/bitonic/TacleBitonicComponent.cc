/*
 * TacleBinarySearchComponent.cc
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#include <taclebench-components/bitonic/TacleBitonicComponent.h>
#include <taclebench-components/bitonic/bitonic.h>
#include <taclebench-components/common/common.h>

#include <eventhandling/EventHandler.h>


BEGIN_OSC_IMPLEMENTATION(Tacle_BitonicComponent, Tacle_BitonicComponent)

Tacle_BitonicComponent Tacle_BitonicComponent::mInstance;

SECTION_CRITICAL_DATA OSC* Tacle_BitonicComponent::trigger_deps[] = {nullptr};
SECTION_CRITICAL_DATA DEFINE_TRIGGER_AFFINITY(Tacle_BitonicComponent, main, Tacle_BitonicComponent::mInstance, trigger_deps, TACLE_BENCH_PRIORITY, TACLE_BENCH_AFFINITY);

Tacle_BitonicComponent::Tacle_BitonicComponent() {


}

DEFINE_TRIGGER_FUNC(Tacle_BitonicComponent,main) {
#ifdef TACLE_BENCH_MULTI_RUN_COUNT
	for (uint32_t run = 0; run < TACLE_BENCH_MULTI_RUN_COUNT; run++)
#endif
	main();
#ifdef TACLE_BENCH_AUTOREPEAT
	EventHandling::EventHandler::pInstance.callOSCTrigger(&trigger_main, 0);
#endif
}

END_OSC_IMPLEMENTATION
