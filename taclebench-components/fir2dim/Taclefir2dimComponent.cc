/*
 * TacleBinarySearchComponent.cc
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#include <taclebench-components/fir2dim/Taclefir2dimComponent.h>
#include <taclebench-components/common/common.h>

#include "fir2dim.h"

#include <eventhandling/EventHandler.h>

BEGIN_OSC_IMPLEMENTATION(Taclefir2dimComponent, Taclefir2dimComponent)

Taclefir2dimComponent Taclefir2dimComponent::mInstance;

SECTION_CRITICAL_DATA OSC* Taclefir2dimComponent::trigger_deps[] = {nullptr};
SECTION_CRITICAL_DATA DEFINE_TRIGGER_AFFINITY(Taclefir2dimComponent, main, Taclefir2dimComponent::mInstance, trigger_deps, TACLE_BENCH_PRIORITY, TACLE_BENCH_AFFINITY);

Taclefir2dimComponent::Taclefir2dimComponent() {


}

DEFINE_TRIGGER_FUNC(Taclefir2dimComponent,main) {
#ifdef TACLE_BENCH_MULTI_RUN_COUNT
	for (uint32_t run = 0; run < TACLE_BENCH_MULTI_RUN_COUNT; run++)
#endif
	main();
#ifdef TACLE_BENCH_AUTOREPEAT
	EventHandling::EventHandler::pInstance.callOSCTrigger(&trigger_main, 0);
#endif
}

END_OSC_IMPLEMENTATION
