/*
 * TacleBinarySearchComponent.cc
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#include <taclebench-components/pm/TaclepmComponent.h>
#include <taclebench-components/common/common.h>

#include "pm.h"

#include <eventhandling/EventHandler.h>

BEGIN_OSC_IMPLEMENTATION(TaclepmComponent, TaclepmComponent)

TaclepmComponent TaclepmComponent::mInstance;

SECTION_CRITICAL_DATA OSC* TaclepmComponent::trigger_deps[] = {nullptr};
SECTION_CRITICAL_DATA DEFINE_TRIGGER_AFFINITY(TaclepmComponent, main, TaclepmComponent::mInstance, trigger_deps, TACLE_BENCH_PRIORITY, TACLE_BENCH_AFFINITY);

TaclepmComponent::TaclepmComponent() {


}

DEFINE_TRIGGER_FUNC(TaclepmComponent,main) {
#ifdef TACLE_BENCH_MULTI_RUN_COUNT
	for (uint32_t run = 0; run < TACLE_BENCH_MULTI_RUN_COUNT; run++)
#endif
	main();
#ifdef TACLE_BENCH_AUTOREPEAT
	EventHandling::EventHandler::pInstance.callOSCTrigger(&trigger_main, 0);
#endif
}

END_OSC_IMPLEMENTATION
