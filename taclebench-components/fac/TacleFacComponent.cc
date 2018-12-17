/*
 * TacleBinarySearchComponent.cc
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#include <taclebench-components/fac/TacleFacComponent.h>
#include <taclebench-components/fac/fac.h>
#include <taclebench-components/common/common.h>

#include <eventhandling/EventHandler.h>


BEGIN_OSC_IMPLEMENTATION(Tacle_FacComponent, Tacle_FacComponent)

Tacle_FacComponent Tacle_FacComponent::mInstance;

SECTION_CRITICAL_DATA OSC* Tacle_FacComponent::trigger_deps[] = {nullptr};
SECTION_CRITICAL_DATA DEFINE_TRIGGER_AFFINITY(Tacle_FacComponent, main, Tacle_FacComponent::mInstance, trigger_deps, TACLE_BENCH_PRIORITY, TACLE_BENCH_AFFINITY);

Tacle_FacComponent::Tacle_FacComponent() {


}

DEFINE_TRIGGER_FUNC(Tacle_FacComponent,main) {
#ifdef TACLE_BENCH_MULTI_RUN_COUNT
	for (uint32_t run = 0; run < TACLE_BENCH_MULTI_RUN_COUNT; run++)
#endif
	main();
#ifdef TACLE_BENCH_AUTOREPEAT
	EventHandling::EventHandler::pInstance.callOSCTrigger(&trigger_main, 0);
#endif
}

END_OSC_IMPLEMENTATION
