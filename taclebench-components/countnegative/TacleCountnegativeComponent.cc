/*
 * TacleBinarySearchComponent.cc
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#include <taclebench-components/countnegative/TacleCountnegativeComponent.h>
#include <taclebench-components/countnegative/countnegative.h>
#include <taclebench-components/common/common.h>

#include <eventhandling/EventHandler.h>


BEGIN_OSC_IMPLEMENTATION(Tacle_CountnegativeComponent, Tacle_CountnegativeComponent)

Tacle_CountnegativeComponent Tacle_CountnegativeComponent::mInstance;

SECTION_CRITICAL_DATA OSC* Tacle_CountnegativeComponent::trigger_deps[] = {nullptr};
SECTION_CRITICAL_DATA DEFINE_TRIGGER_AFFINITY(Tacle_CountnegativeComponent, main, Tacle_CountnegativeComponent::mInstance, trigger_deps, TACLE_BENCH_PRIORITY, TACLE_BENCH_AFFINITY);

Tacle_CountnegativeComponent::Tacle_CountnegativeComponent() {


}

DEFINE_TRIGGER_FUNC(Tacle_CountnegativeComponent,main) {
#ifdef TACLE_BENCH_MULTI_RUN_COUNT
	for (uint32_t run = 0; run < TACLE_BENCH_MULTI_RUN_COUNT; run++)
#endif
	main();
#ifdef TACLE_BENCH_AUTOREPEAT
	EventHandling::EventHandler::pInstance.callOSCTrigger(&trigger_main, 0);
#endif
}

END_OSC_IMPLEMENTATION
