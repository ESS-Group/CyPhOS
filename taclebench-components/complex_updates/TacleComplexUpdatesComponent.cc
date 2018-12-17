/*
 * TacleBinarySearchComponent.cc
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#include <taclebench-components/complex_updates/TacleComplexUpdatesComponent.h>
#include <taclebench-components/complex_updates/complex_updates.h>
#include <taclebench-components/common/common.h>

#include <eventhandling/EventHandler.h>


BEGIN_OSC_IMPLEMENTATION(Tacle_ComplexUpdatesComponent, Tacle_ComplexUpdatesComponent)

Tacle_ComplexUpdatesComponent Tacle_ComplexUpdatesComponent::mInstance;

SECTION_CRITICAL_DATA OSC* Tacle_ComplexUpdatesComponent::trigger_deps[] = {nullptr};
SECTION_CRITICAL_DATA DEFINE_TRIGGER_AFFINITY(Tacle_ComplexUpdatesComponent, main, Tacle_ComplexUpdatesComponent::mInstance, trigger_deps, TACLE_BENCH_PRIORITY, TACLE_BENCH_AFFINITY);

Tacle_ComplexUpdatesComponent::Tacle_ComplexUpdatesComponent() {


}

DEFINE_TRIGGER_FUNC(Tacle_ComplexUpdatesComponent,main) {
#ifdef TACLE_BENCH_MULTI_RUN_COUNT
	for (uint32_t run = 0; run < TACLE_BENCH_MULTI_RUN_COUNT; run++)
#endif
	main();
#ifdef TACLE_BENCH_AUTOREPEAT
	EventHandling::EventHandler::pInstance.callOSCTrigger(&trigger_main, 0);
#endif
}

END_OSC_IMPLEMENTATION
