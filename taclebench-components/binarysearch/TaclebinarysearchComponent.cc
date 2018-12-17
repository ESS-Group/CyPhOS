/*
 * TacleBinarySearchComponent.cc
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#include <taclebench-components/binarysearch/TaclebinarysearchComponent.h>
#include <taclebench-components/common/common.h>

#include "binarysearch.h"

#include <eventhandling/EventHandler.h>

BEGIN_OSC_IMPLEMENTATION(TaclebinarysearchComponent, TaclebinarysearchComponent)

TaclebinarysearchComponent TaclebinarysearchComponent::mInstance;

SECTION_CRITICAL_DATA OSC* TaclebinarysearchComponent::trigger_deps[] = {nullptr};
SECTION_CRITICAL_DATA DEFINE_TRIGGER_AFFINITY(TaclebinarysearchComponent, main, TaclebinarysearchComponent::mInstance, trigger_deps, TACLE_BENCH_PRIORITY, TACLE_BENCH_AFFINITY);

TaclebinarysearchComponent::TaclebinarysearchComponent() {


}

DEFINE_TRIGGER_FUNC(TaclebinarysearchComponent,main) {
#ifdef TACLE_BENCH_MULTI_RUN_COUNT
	for (uint32_t run = 0; run < TACLE_BENCH_MULTI_RUN_COUNT; run++)
#endif
	main();


#ifdef TACLE_BENCH_AUTOREPEAT
	EventHandling::EventHandler::pInstance.callOSCTrigger(&trigger_main, 0);
#endif
}

END_OSC_IMPLEMENTATION
