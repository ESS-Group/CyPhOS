/*
 * TacleBinarySearchComponent.cc
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#include <taclebench-components/bitcount/TacleBitcountComponent.h>
#include <taclebench-components/bitcount/bitcount.h>
#include <taclebench-components/common/common.h>

#include <eventhandling/EventHandler.h>


BEGIN_OSC_IMPLEMENTATION(Tacle_BitcountComponent, Tacle_BitcountComponent)

Tacle_BitcountComponent Tacle_BitcountComponent::mInstance;

SECTION_CRITICAL_DATA OSC* Tacle_BitcountComponent::trigger_deps[] = {nullptr};
SECTION_CRITICAL_DATA DEFINE_TRIGGER_AFFINITY(Tacle_BitcountComponent, main, Tacle_BitcountComponent::mInstance, trigger_deps, TACLE_BENCH_PRIORITY, TACLE_BENCH_AFFINITY);

Tacle_BitcountComponent::Tacle_BitcountComponent() {


}

DEFINE_TRIGGER_FUNC(Tacle_BitcountComponent,main) {
#ifdef TACLE_BENCH_MULTI_RUN_COUNT
	for (uint32_t run = 0; run < TACLE_BENCH_MULTI_RUN_COUNT; run++)
#endif
	main();
#ifdef TACLE_BENCH_AUTOREPEAT
	EventHandling::EventHandler::pInstance.callOSCTrigger(&trigger_main, 0);
#endif
}

END_OSC_IMPLEMENTATION
