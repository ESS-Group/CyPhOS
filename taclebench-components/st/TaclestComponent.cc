/*
 * TacleBinarySearchComponent.cc
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#include <taclebench-components/st/TaclestComponent.h>
#include <taclebench-components/common/common.h>

#include "st.h"

#include <eventhandling/EventHandler.h>

BEGIN_OSC_IMPLEMENTATION(TaclestComponent, TaclestComponent)

TaclestComponent TaclestComponent::mInstance;

SECTION_CRITICAL_DATA OSC* TaclestComponent::trigger_deps[] = {nullptr};
SECTION_CRITICAL_DATA DEFINE_TRIGGER_AFFINITY(TaclestComponent, main, TaclestComponent::mInstance, trigger_deps, TACLE_BENCH_PRIORITY, TACLE_BENCH_AFFINITY);

TaclestComponent::TaclestComponent() {


}

DEFINE_TRIGGER_FUNC(TaclestComponent,main) {
#ifdef TACLE_BENCH_MULTI_RUN_COUNT
	for (uint32_t run = 0; run < TACLE_BENCH_MULTI_RUN_COUNT; run++)
#endif
	main();
#ifdef TACLE_BENCH_AUTOREPEAT
	EventHandling::EventHandler::pInstance.callOSCTrigger(&trigger_main, 0);
#endif
}

END_OSC_IMPLEMENTATION
