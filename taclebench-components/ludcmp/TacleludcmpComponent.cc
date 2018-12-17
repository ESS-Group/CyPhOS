/*
 * TacleBinarySearchComponent.cc
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#include <taclebench-components/ludcmp/TacleludcmpComponent.h>
#include <taclebench-components/common/common.h>

#include "ludcmp.h"

#include <eventhandling/EventHandler.h>

BEGIN_OSC_IMPLEMENTATION(TacleludcmpComponent, TacleludcmpComponent)

TacleludcmpComponent TacleludcmpComponent::mInstance;

SECTION_CRITICAL_DATA OSC* TacleludcmpComponent::trigger_deps[] = {nullptr};
SECTION_CRITICAL_DATA DEFINE_TRIGGER_AFFINITY(TacleludcmpComponent, main, TacleludcmpComponent::mInstance, trigger_deps, TACLE_BENCH_PRIORITY, TACLE_BENCH_AFFINITY);

TacleludcmpComponent::TacleludcmpComponent() {


}

DEFINE_TRIGGER_FUNC(TacleludcmpComponent,main) {
#ifdef TACLE_BENCH_MULTI_RUN_COUNT
	for (uint32_t run = 0; run < TACLE_BENCH_MULTI_RUN_COUNT; run++)
#endif
	main();
#ifdef TACLE_BENCH_AUTOREPEAT
	EventHandling::EventHandler::pInstance.callOSCTrigger(&trigger_main, 0);
#endif
}

END_OSC_IMPLEMENTATION
