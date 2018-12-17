/*
 * ApplicationPeriodicTest.cc
 *
 *  Created on: 14.11.2016
 *      Author: hendrik
 */

#include "ApplicationPeriodicTest.h"

#include <common/SyncOutput.h>

#include <driver/GenericTimer.h>

#include <driver/ConsoleBuffer.h>

#include "ApplicationReboot.h"

#define TAG "ApplicationPeriodicTest"

BEGIN_OSC_IMPLEMENTATION(ApplicationPeriodicTest,ApplicationPeriodicTest)


ApplicationPeriodicTest ApplicationPeriodicTest::mInstance;


SECTION_CRITICAL_DATA OSC* ApplicationPeriodicTest::trigger_deps[] = {&OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance,nullptr};

SECTION_CRITICAL_DATA DEFINE_TRIGGER(ApplicationPeriodicTest,startPeriodic,ApplicationPeriodicTest::mInstance, trigger_deps, EventHandling::Trigger::cMIN_Priority);
SECTION_CRITICAL_DATA DEFINE_TRIGGER(ApplicationPeriodicTest,stopPeriodic,ApplicationPeriodicTest::mInstance, trigger_deps, EventHandling::Trigger::cMIN_Priority);
SECTION_CRITICAL_DATA DEFINE_TRIGGER(ApplicationPeriodicTest,periodicCall,ApplicationPeriodicTest::mInstance, trigger_deps, EventHandling::Trigger::cMIN_Priority + 500);
SECTION_CRITICAL_DATA DEFINE_TRIGGER(ApplicationPeriodicTest,oneshotTest,ApplicationPeriodicTest::mInstance, trigger_deps, EventHandling::Trigger::cMIN_Priority + 501);
SECTION_CRITICAL_DATA DEFINE_TRIGGER(ApplicationPeriodicTest,oneshotTestTimerTrigger,ApplicationPeriodicTest::mInstance, trigger_deps, EventHandling::Trigger::cMIN_Priority + 502);

SECTION_CRITICAL_DATA DEFINE_TRIGGER(ApplicationPeriodicTest,endlessTest,ApplicationPeriodicTest::mInstance, trigger_deps, EventHandling::Trigger::cMIN_Priority);


ApplicationPeriodicTest::ApplicationPeriodicTest() :
		mCounter(0), mTimerEvent(nullptr) {
}

DEFINE_TRIGGER_FUNC(ApplicationPeriodicTest,startPeriodic) {
	SYNC_OUTPUT_STREAM(TAG, "Started periodic application");
	mTimerEvent = OSC_PREFIX(Timer)::GenericTimer::pInstance->setPeriodicEvent(&ApplicationPeriodicTest::trigger_periodicCall, 1000000);
}

DEFINE_TRIGGER_FUNC(ApplicationPeriodicTest,stopPeriodic) {
	SYNC_OUTPUT_STREAM(TAG, "Stopping periodic application");
    OSC_PREFIX(Timer)::GenericTimer::pInstance->cancelPeriodicTimerEvent(mTimerEvent);
}

DEFINE_TRIGGER_FUNC(ApplicationPeriodicTest,oneshotTest) {
	SYNC_OUTPUT_STREAM(TAG, "Testing oneshot timer" << endl);
	OSC_PREFIX(Timer)::GenericTimer::pInstance->setOneshotEvent(&ApplicationPeriodicTest::trigger_oneshotTestTimerTrigger,2000*1000);
	OSC_PREFIX(Timer)::GenericTimer::pInstance->setOneshotEvent(&ApplicationPeriodicTest::trigger_oneshotTestTimerTrigger,2000*1000);
	OSC_PREFIX(Timer)::GenericTimer::pInstance->setOneshotEvent(&ApplicationPeriodicTest::trigger_oneshotTestTimerTrigger,3500*1000);
	OSC_PREFIX(Timer)::GenericTimer::pInstance->setOneshotEvent(&ApplicationPeriodicTest::trigger_oneshotTestTimerTrigger,4000*1000);
	OSC_PREFIX(Timer)::GenericTimer::pInstance->setOneshotEvent(&ApplicationPeriodicTest::trigger_oneshotTestTimerTrigger,4000*1000);
	OSC_PREFIX(Timer)::GenericTimer::pInstance->setOneshotEvent(&ApplicationPeriodicTest::trigger_oneshotTestTimerTrigger,1000*1000);
}

DEFINE_TRIGGER_FUNC(ApplicationPeriodicTest,oneshotTestTimerTrigger) {
	SYNC_OUTPUT_STREAM(TAG, "Oneshot triggered" << endl);
	//EventHandling::EventHandler::pInstance.callOSCTrigger(&ApplicationPeriodicTest::trigger_oneshotTest,0);
}


DEFINE_TRIGGER_FUNC(ApplicationPeriodicTest,periodicCall) {
	SYNC_OUTPUT_STREAM(TAG,"Counter test:" << dec << mCounter++ << endl);

	// Dirty data array
	for (int i = 0; i < 60000;i++) {
		mTestArray[i] = mCounter + i;
	}
}

DEFINE_TRIGGER_FUNC(ApplicationPeriodicTest,endlessTest) {
	while (1) {
		SYNC_OUTPUT_STREAM(TAG, "DOING NOTHING");
		BUSY_WAITING_LONG
		;
	}
}

END_OSC_IMPLEMENTATION
