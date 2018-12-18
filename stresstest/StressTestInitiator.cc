/*
 * StressTestInitiator.cc
 *
 *  Created on: 09.03.2017
 *      Author: hendrik
 */

#include <stresstest/StressTestInitiator.h>

#include <eventhandling/EventHandler.h>
#include <driver/GenericTimer.h>

#include <common/SyncOutput.h>

#define TAG "StressTestInit"
#ifndef CONFIG_STRESSTEST_OUTPUT
#include <stresstest/StressTestComponent_generic_no_single_measure_0.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_1.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_2.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_3.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_4.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_5.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_6.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_7.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_8.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_9.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_10.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_11.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_12.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_13.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_14.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_15.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_16.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_17.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_18.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_19.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_20.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_21.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_22.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_23.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_24.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_25.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_26.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_27.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_28.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_29.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_30.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_31.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_32.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_33.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_34.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_35.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_36.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_37.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_38.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_39.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_40.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_41.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_42.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_43.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_44.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_45.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_46.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_47.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_48.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_49.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_50.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_51.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_52.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_53.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_54.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_55.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_56.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_57.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_58.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_59.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_60.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_61.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_62.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_63.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_64.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_65.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_66.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_67.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_68.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_69.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_70.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_71.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_72.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_73.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_74.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_75.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_76.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_77.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_78.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_79.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_80.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_81.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_82.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_83.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_84.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_85.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_86.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_87.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_88.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_89.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_90.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_91.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_92.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_93.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_94.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_95.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_96.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_97.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_98.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_99.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_100.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_101.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_102.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_103.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_104.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_105.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_106.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_107.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_108.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_109.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_110.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_111.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_112.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_113.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_114.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_115.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_116.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_117.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_118.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_119.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_120.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_121.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_122.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_123.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_124.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_125.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_126.h>
#include <stresstest/StressTestComponent_generic_no_single_measure_127.h>
#else
#include <stresstest/StressTestComponent_generic_0.h>
#include <stresstest/StressTestComponent_generic_1.h>
#include <stresstest/StressTestComponent_generic_2.h>
#include <stresstest/StressTestComponent_generic_3.h>
#include <stresstest/StressTestComponent_generic_4.h>
#include <stresstest/StressTestComponent_generic_5.h>
#include <stresstest/StressTestComponent_generic_6.h>
#include <stresstest/StressTestComponent_generic_7.h>
#include <stresstest/StressTestComponent_generic_8.h>
#include <stresstest/StressTestComponent_generic_9.h>
#include <stresstest/StressTestComponent_generic_10.h>
#include <stresstest/StressTestComponent_generic_11.h>
#include <stresstest/StressTestComponent_generic_12.h>
#include <stresstest/StressTestComponent_generic_13.h>
#include <stresstest/StressTestComponent_generic_14.h>
#include <stresstest/StressTestComponent_generic_15.h>
#include <stresstest/StressTestComponent_generic_16.h>
#include <stresstest/StressTestComponent_generic_17.h>
#include <stresstest/StressTestComponent_generic_18.h>
#include <stresstest/StressTestComponent_generic_19.h>
#include <stresstest/StressTestComponent_generic_20.h>
#include <stresstest/StressTestComponent_generic_21.h>
#include <stresstest/StressTestComponent_generic_22.h>
#include <stresstest/StressTestComponent_generic_23.h>
#include <stresstest/StressTestComponent_generic_24.h>
#include <stresstest/StressTestComponent_generic_25.h>
#include <stresstest/StressTestComponent_generic_26.h>
#include <stresstest/StressTestComponent_generic_27.h>
#include <stresstest/StressTestComponent_generic_28.h>
#include <stresstest/StressTestComponent_generic_29.h>
#include <stresstest/StressTestComponent_generic_30.h>
#include <stresstest/StressTestComponent_generic_31.h>
#endif


BEGIN_OSC_IMPLEMENTATION(StressTestInitiator,StressTestInitiator)

StressTestInitiator StressTestInitiator::mInstance;

SECTION_CRITICAL_DATA OSC* StressTestInitiator::trigger_deps[] = {nullptr};

SECTION_CRITICAL_DATA EventHandling::Trigger StressTestInitiator::trigger_Start = {(OSC*)&StressTestInitiator::mInstance,(void (OSC::*)(dword_t))&StressTestInitiator::func_Start,
				(OSC**)&StressTestInitiator::trigger_deps, EventHandling::Trigger::cMIN_Priority + 2400};

SECTION_CRITICAL_DATA EventHandling::Trigger StressTestInitiator::trigger_Periodic = {(OSC*)&StressTestInitiator::mInstance,(void (OSC::*)(dword_t))&StressTestInitiator::func_Periodic,
				(OSC**)&StressTestInitiator::trigger_deps, EventHandling::Trigger::cMIN_Priority + 2401};

StressTestInitiator::StressTestInitiator() {
}

void StressTestInitiator::func_Start(dword_t arg) {
	SYNC_OUTPUT_STREAM(TAG,"Starting stress test with array size: " << dec << OSC_PREFIX(StressTestComponent0)::StressTestComponent0::cArraySize * 4 << " bytes" << endl);
	SYNC_OUTPUT_STREAM(TAG,"Access count: " << dec << OSC_PREFIX(StressTestComponent0)::StressTestComponent0::cAccessCount << endl);
#ifndef CONFIG_STRESSTEST_OUTPUT
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent0)::StressTestComponent0::event_Continue, &OSC_PREFIX(StressTestComponent1)::StressTestComponent1::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent1)::StressTestComponent1::event_Continue, &OSC_PREFIX(StressTestComponent2)::StressTestComponent2::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent2)::StressTestComponent2::event_Continue, &OSC_PREFIX(StressTestComponent3)::StressTestComponent3::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent3)::StressTestComponent3::event_Continue, &OSC_PREFIX(StressTestComponent4)::StressTestComponent4::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent4)::StressTestComponent4::event_Continue, &OSC_PREFIX(StressTestComponent5)::StressTestComponent5::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent5)::StressTestComponent5::event_Continue, &OSC_PREFIX(StressTestComponent6)::StressTestComponent6::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent6)::StressTestComponent6::event_Continue, &OSC_PREFIX(StressTestComponent7)::StressTestComponent7::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent7)::StressTestComponent7::event_Continue, &OSC_PREFIX(StressTestComponent8)::StressTestComponent8::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent8)::StressTestComponent8::event_Continue, &OSC_PREFIX(StressTestComponent9)::StressTestComponent9::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent9)::StressTestComponent9::event_Continue, &OSC_PREFIX(StressTestComponent10)::StressTestComponent10::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent10)::StressTestComponent10::event_Continue, &OSC_PREFIX(StressTestComponent11)::StressTestComponent11::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent11)::StressTestComponent11::event_Continue, &OSC_PREFIX(StressTestComponent12)::StressTestComponent12::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent12)::StressTestComponent12::event_Continue, &OSC_PREFIX(StressTestComponent13)::StressTestComponent13::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent13)::StressTestComponent13::event_Continue, &OSC_PREFIX(StressTestComponent14)::StressTestComponent14::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent14)::StressTestComponent14::event_Continue, &OSC_PREFIX(StressTestComponent15)::StressTestComponent15::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent15)::StressTestComponent15::event_Continue, &OSC_PREFIX(StressTestComponent16)::StressTestComponent16::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent16)::StressTestComponent16::event_Continue, &OSC_PREFIX(StressTestComponent17)::StressTestComponent17::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent17)::StressTestComponent17::event_Continue, &OSC_PREFIX(StressTestComponent18)::StressTestComponent18::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent18)::StressTestComponent18::event_Continue, &OSC_PREFIX(StressTestComponent19)::StressTestComponent19::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent19)::StressTestComponent19::event_Continue, &OSC_PREFIX(StressTestComponent20)::StressTestComponent20::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent20)::StressTestComponent20::event_Continue, &OSC_PREFIX(StressTestComponent21)::StressTestComponent21::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent21)::StressTestComponent21::event_Continue, &OSC_PREFIX(StressTestComponent22)::StressTestComponent22::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent22)::StressTestComponent22::event_Continue, &OSC_PREFIX(StressTestComponent23)::StressTestComponent23::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent23)::StressTestComponent23::event_Continue, &OSC_PREFIX(StressTestComponent24)::StressTestComponent24::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent24)::StressTestComponent24::event_Continue, &OSC_PREFIX(StressTestComponent25)::StressTestComponent25::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent25)::StressTestComponent25::event_Continue, &OSC_PREFIX(StressTestComponent26)::StressTestComponent26::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent26)::StressTestComponent26::event_Continue, &OSC_PREFIX(StressTestComponent27)::StressTestComponent27::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent27)::StressTestComponent27::event_Continue, &OSC_PREFIX(StressTestComponent28)::StressTestComponent28::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent28)::StressTestComponent28::event_Continue, &OSC_PREFIX(StressTestComponent29)::StressTestComponent29::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent29)::StressTestComponent29::event_Continue, &OSC_PREFIX(StressTestComponent30)::StressTestComponent30::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent30)::StressTestComponent30::event_Continue, &OSC_PREFIX(StressTestComponent31)::StressTestComponent31::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent31)::StressTestComponent31::event_Continue, &OSC_PREFIX(StressTestComponent32)::StressTestComponent32::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent32)::StressTestComponent32::event_Continue, &OSC_PREFIX(StressTestComponent33)::StressTestComponent33::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent33)::StressTestComponent33::event_Continue, &OSC_PREFIX(StressTestComponent34)::StressTestComponent34::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent34)::StressTestComponent34::event_Continue, &OSC_PREFIX(StressTestComponent35)::StressTestComponent35::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent35)::StressTestComponent35::event_Continue, &OSC_PREFIX(StressTestComponent36)::StressTestComponent36::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent36)::StressTestComponent36::event_Continue, &OSC_PREFIX(StressTestComponent37)::StressTestComponent37::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent37)::StressTestComponent37::event_Continue, &OSC_PREFIX(StressTestComponent38)::StressTestComponent38::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent38)::StressTestComponent38::event_Continue, &OSC_PREFIX(StressTestComponent39)::StressTestComponent39::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent39)::StressTestComponent39::event_Continue, &OSC_PREFIX(StressTestComponent40)::StressTestComponent40::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent40)::StressTestComponent40::event_Continue, &OSC_PREFIX(StressTestComponent41)::StressTestComponent41::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent41)::StressTestComponent41::event_Continue, &OSC_PREFIX(StressTestComponent42)::StressTestComponent42::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent42)::StressTestComponent42::event_Continue, &OSC_PREFIX(StressTestComponent43)::StressTestComponent43::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent43)::StressTestComponent43::event_Continue, &OSC_PREFIX(StressTestComponent44)::StressTestComponent44::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent44)::StressTestComponent44::event_Continue, &OSC_PREFIX(StressTestComponent45)::StressTestComponent45::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent45)::StressTestComponent45::event_Continue, &OSC_PREFIX(StressTestComponent46)::StressTestComponent46::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent46)::StressTestComponent46::event_Continue, &OSC_PREFIX(StressTestComponent47)::StressTestComponent47::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent47)::StressTestComponent47::event_Continue, &OSC_PREFIX(StressTestComponent48)::StressTestComponent48::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent48)::StressTestComponent48::event_Continue, &OSC_PREFIX(StressTestComponent49)::StressTestComponent49::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent49)::StressTestComponent49::event_Continue, &OSC_PREFIX(StressTestComponent50)::StressTestComponent50::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent50)::StressTestComponent50::event_Continue, &OSC_PREFIX(StressTestComponent51)::StressTestComponent51::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent51)::StressTestComponent51::event_Continue, &OSC_PREFIX(StressTestComponent52)::StressTestComponent52::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent52)::StressTestComponent52::event_Continue, &OSC_PREFIX(StressTestComponent53)::StressTestComponent53::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent53)::StressTestComponent53::event_Continue, &OSC_PREFIX(StressTestComponent54)::StressTestComponent54::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent54)::StressTestComponent54::event_Continue, &OSC_PREFIX(StressTestComponent55)::StressTestComponent55::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent55)::StressTestComponent55::event_Continue, &OSC_PREFIX(StressTestComponent56)::StressTestComponent56::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent56)::StressTestComponent56::event_Continue, &OSC_PREFIX(StressTestComponent57)::StressTestComponent57::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent57)::StressTestComponent57::event_Continue, &OSC_PREFIX(StressTestComponent58)::StressTestComponent58::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent58)::StressTestComponent58::event_Continue, &OSC_PREFIX(StressTestComponent59)::StressTestComponent59::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent59)::StressTestComponent59::event_Continue, &OSC_PREFIX(StressTestComponent60)::StressTestComponent60::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent60)::StressTestComponent60::event_Continue, &OSC_PREFIX(StressTestComponent61)::StressTestComponent61::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent61)::StressTestComponent61::event_Continue, &OSC_PREFIX(StressTestComponent62)::StressTestComponent62::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent62)::StressTestComponent62::event_Continue, &OSC_PREFIX(StressTestComponent63)::StressTestComponent63::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent63)::StressTestComponent63::event_Continue, &OSC_PREFIX(StressTestComponent64)::StressTestComponent64::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent64)::StressTestComponent64::event_Continue, &OSC_PREFIX(StressTestComponent65)::StressTestComponent65::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent65)::StressTestComponent65::event_Continue, &OSC_PREFIX(StressTestComponent66)::StressTestComponent66::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent66)::StressTestComponent66::event_Continue, &OSC_PREFIX(StressTestComponent67)::StressTestComponent67::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent67)::StressTestComponent67::event_Continue, &OSC_PREFIX(StressTestComponent68)::StressTestComponent68::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent68)::StressTestComponent68::event_Continue, &OSC_PREFIX(StressTestComponent69)::StressTestComponent69::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent69)::StressTestComponent69::event_Continue, &OSC_PREFIX(StressTestComponent70)::StressTestComponent70::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent70)::StressTestComponent70::event_Continue, &OSC_PREFIX(StressTestComponent71)::StressTestComponent71::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent71)::StressTestComponent71::event_Continue, &OSC_PREFIX(StressTestComponent72)::StressTestComponent72::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent72)::StressTestComponent72::event_Continue, &OSC_PREFIX(StressTestComponent73)::StressTestComponent73::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent73)::StressTestComponent73::event_Continue, &OSC_PREFIX(StressTestComponent74)::StressTestComponent74::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent74)::StressTestComponent74::event_Continue, &OSC_PREFIX(StressTestComponent75)::StressTestComponent75::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent75)::StressTestComponent75::event_Continue, &OSC_PREFIX(StressTestComponent76)::StressTestComponent76::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent76)::StressTestComponent76::event_Continue, &OSC_PREFIX(StressTestComponent77)::StressTestComponent77::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent77)::StressTestComponent77::event_Continue, &OSC_PREFIX(StressTestComponent78)::StressTestComponent78::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent78)::StressTestComponent78::event_Continue, &OSC_PREFIX(StressTestComponent79)::StressTestComponent79::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent79)::StressTestComponent79::event_Continue, &OSC_PREFIX(StressTestComponent80)::StressTestComponent80::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent80)::StressTestComponent80::event_Continue, &OSC_PREFIX(StressTestComponent81)::StressTestComponent81::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent81)::StressTestComponent81::event_Continue, &OSC_PREFIX(StressTestComponent82)::StressTestComponent82::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent82)::StressTestComponent82::event_Continue, &OSC_PREFIX(StressTestComponent83)::StressTestComponent83::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent83)::StressTestComponent83::event_Continue, &OSC_PREFIX(StressTestComponent84)::StressTestComponent84::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent84)::StressTestComponent84::event_Continue, &OSC_PREFIX(StressTestComponent85)::StressTestComponent85::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent85)::StressTestComponent85::event_Continue, &OSC_PREFIX(StressTestComponent86)::StressTestComponent86::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent86)::StressTestComponent86::event_Continue, &OSC_PREFIX(StressTestComponent87)::StressTestComponent87::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent87)::StressTestComponent87::event_Continue, &OSC_PREFIX(StressTestComponent88)::StressTestComponent88::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent88)::StressTestComponent88::event_Continue, &OSC_PREFIX(StressTestComponent89)::StressTestComponent89::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent89)::StressTestComponent89::event_Continue, &OSC_PREFIX(StressTestComponent90)::StressTestComponent90::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent90)::StressTestComponent90::event_Continue, &OSC_PREFIX(StressTestComponent91)::StressTestComponent91::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent91)::StressTestComponent91::event_Continue, &OSC_PREFIX(StressTestComponent92)::StressTestComponent92::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent92)::StressTestComponent92::event_Continue, &OSC_PREFIX(StressTestComponent93)::StressTestComponent93::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent93)::StressTestComponent93::event_Continue, &OSC_PREFIX(StressTestComponent94)::StressTestComponent94::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent94)::StressTestComponent94::event_Continue, &OSC_PREFIX(StressTestComponent95)::StressTestComponent95::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent95)::StressTestComponent95::event_Continue, &OSC_PREFIX(StressTestComponent96)::StressTestComponent96::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent96)::StressTestComponent96::event_Continue, &OSC_PREFIX(StressTestComponent97)::StressTestComponent97::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent97)::StressTestComponent97::event_Continue, &OSC_PREFIX(StressTestComponent98)::StressTestComponent98::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent98)::StressTestComponent98::event_Continue, &OSC_PREFIX(StressTestComponent99)::StressTestComponent99::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent99)::StressTestComponent99::event_Continue, &OSC_PREFIX(StressTestComponent100)::StressTestComponent100::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent100)::StressTestComponent100::event_Continue, &OSC_PREFIX(StressTestComponent101)::StressTestComponent101::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent101)::StressTestComponent101::event_Continue, &OSC_PREFIX(StressTestComponent102)::StressTestComponent102::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent102)::StressTestComponent102::event_Continue, &OSC_PREFIX(StressTestComponent103)::StressTestComponent103::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent103)::StressTestComponent103::event_Continue, &OSC_PREFIX(StressTestComponent104)::StressTestComponent104::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent104)::StressTestComponent104::event_Continue, &OSC_PREFIX(StressTestComponent105)::StressTestComponent105::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent105)::StressTestComponent105::event_Continue, &OSC_PREFIX(StressTestComponent106)::StressTestComponent106::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent106)::StressTestComponent106::event_Continue, &OSC_PREFIX(StressTestComponent107)::StressTestComponent107::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent107)::StressTestComponent107::event_Continue, &OSC_PREFIX(StressTestComponent108)::StressTestComponent108::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent108)::StressTestComponent108::event_Continue, &OSC_PREFIX(StressTestComponent109)::StressTestComponent109::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent109)::StressTestComponent109::event_Continue, &OSC_PREFIX(StressTestComponent110)::StressTestComponent110::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent110)::StressTestComponent110::event_Continue, &OSC_PREFIX(StressTestComponent111)::StressTestComponent111::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent111)::StressTestComponent111::event_Continue, &OSC_PREFIX(StressTestComponent112)::StressTestComponent112::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent112)::StressTestComponent112::event_Continue, &OSC_PREFIX(StressTestComponent113)::StressTestComponent113::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent113)::StressTestComponent113::event_Continue, &OSC_PREFIX(StressTestComponent114)::StressTestComponent114::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent114)::StressTestComponent114::event_Continue, &OSC_PREFIX(StressTestComponent115)::StressTestComponent115::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent115)::StressTestComponent115::event_Continue, &OSC_PREFIX(StressTestComponent116)::StressTestComponent116::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent116)::StressTestComponent116::event_Continue, &OSC_PREFIX(StressTestComponent117)::StressTestComponent117::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent117)::StressTestComponent117::event_Continue, &OSC_PREFIX(StressTestComponent118)::StressTestComponent118::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent118)::StressTestComponent118::event_Continue, &OSC_PREFIX(StressTestComponent119)::StressTestComponent119::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent119)::StressTestComponent119::event_Continue, &OSC_PREFIX(StressTestComponent120)::StressTestComponent120::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent120)::StressTestComponent120::event_Continue, &OSC_PREFIX(StressTestComponent121)::StressTestComponent121::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent121)::StressTestComponent121::event_Continue, &OSC_PREFIX(StressTestComponent122)::StressTestComponent122::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent122)::StressTestComponent122::event_Continue, &OSC_PREFIX(StressTestComponent123)::StressTestComponent123::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent123)::StressTestComponent123::event_Continue, &OSC_PREFIX(StressTestComponent124)::StressTestComponent124::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent124)::StressTestComponent124::event_Continue, &OSC_PREFIX(StressTestComponent125)::StressTestComponent125::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent125)::StressTestComponent125::event_Continue, &OSC_PREFIX(StressTestComponent126)::StressTestComponent126::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent126)::StressTestComponent126::event_Continue, &OSC_PREFIX(StressTestComponent127)::StressTestComponent127::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent127)::StressTestComponent127::event_Continue, &OSC_PREFIX(StressTestComponent0)::StressTestComponent0::trigger_doStress);
#else
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent0)::StressTestComponent0::event_Continue, &OSC_PREFIX(StressTestComponent1)::StressTestComponent1::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent1)::StressTestComponent1::event_Continue, &OSC_PREFIX(StressTestComponent2)::StressTestComponent2::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent2)::StressTestComponent2::event_Continue, &OSC_PREFIX(StressTestComponent3)::StressTestComponent3::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent3)::StressTestComponent3::event_Continue, &OSC_PREFIX(StressTestComponent4)::StressTestComponent4::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent4)::StressTestComponent4::event_Continue, &OSC_PREFIX(StressTestComponent5)::StressTestComponent5::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent5)::StressTestComponent5::event_Continue, &OSC_PREFIX(StressTestComponent6)::StressTestComponent6::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent6)::StressTestComponent6::event_Continue, &OSC_PREFIX(StressTestComponent7)::StressTestComponent7::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent7)::StressTestComponent7::event_Continue, &OSC_PREFIX(StressTestComponent8)::StressTestComponent8::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent8)::StressTestComponent8::event_Continue, &OSC_PREFIX(StressTestComponent9)::StressTestComponent9::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent9)::StressTestComponent9::event_Continue, &OSC_PREFIX(StressTestComponent10)::StressTestComponent10::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent10)::StressTestComponent10::event_Continue, &OSC_PREFIX(StressTestComponent11)::StressTestComponent11::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent11)::StressTestComponent11::event_Continue, &OSC_PREFIX(StressTestComponent12)::StressTestComponent12::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent12)::StressTestComponent12::event_Continue, &OSC_PREFIX(StressTestComponent13)::StressTestComponent13::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent13)::StressTestComponent13::event_Continue, &OSC_PREFIX(StressTestComponent14)::StressTestComponent14::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent14)::StressTestComponent14::event_Continue, &OSC_PREFIX(StressTestComponent15)::StressTestComponent15::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent15)::StressTestComponent15::event_Continue, &OSC_PREFIX(StressTestComponent16)::StressTestComponent16::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent16)::StressTestComponent16::event_Continue, &OSC_PREFIX(StressTestComponent17)::StressTestComponent17::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent17)::StressTestComponent17::event_Continue, &OSC_PREFIX(StressTestComponent18)::StressTestComponent18::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent18)::StressTestComponent18::event_Continue, &OSC_PREFIX(StressTestComponent19)::StressTestComponent19::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent19)::StressTestComponent19::event_Continue, &OSC_PREFIX(StressTestComponent20)::StressTestComponent20::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent20)::StressTestComponent20::event_Continue, &OSC_PREFIX(StressTestComponent21)::StressTestComponent21::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent21)::StressTestComponent21::event_Continue, &OSC_PREFIX(StressTestComponent22)::StressTestComponent22::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent22)::StressTestComponent22::event_Continue, &OSC_PREFIX(StressTestComponent23)::StressTestComponent23::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent23)::StressTestComponent23::event_Continue, &OSC_PREFIX(StressTestComponent24)::StressTestComponent24::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent24)::StressTestComponent24::event_Continue, &OSC_PREFIX(StressTestComponent25)::StressTestComponent25::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent25)::StressTestComponent25::event_Continue, &OSC_PREFIX(StressTestComponent26)::StressTestComponent26::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent26)::StressTestComponent26::event_Continue, &OSC_PREFIX(StressTestComponent27)::StressTestComponent27::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent27)::StressTestComponent27::event_Continue, &OSC_PREFIX(StressTestComponent28)::StressTestComponent28::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent28)::StressTestComponent28::event_Continue, &OSC_PREFIX(StressTestComponent29)::StressTestComponent29::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent29)::StressTestComponent29::event_Continue, &OSC_PREFIX(StressTestComponent30)::StressTestComponent30::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent30)::StressTestComponent30::event_Continue, &OSC_PREFIX(StressTestComponent31)::StressTestComponent31::trigger_doStress);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent31)::StressTestComponent31::event_Continue, &OSC_PREFIX(StressTestComponent0)::StressTestComponent0::trigger_doStress);

#endif


	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent0)::StressTestComponent0::trigger_doStress, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent4)::StressTestComponent4::trigger_doStress, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent8)::StressTestComponent8::trigger_doStress, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent12)::StressTestComponent12::trigger_doStress, 0);

//	OSC_PREFIX(Timer)::GenericTimer::pInstance->setPeriodicEvent(&trigger_Periodic, 1 * 1000 * 1000);
}

void StressTestInitiator::func_Start2(dword_t arg) {
	//EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent1)::StressTestComponent1::trigger_selfCall, 0);
	//OSC_PREFIX(Timer)::GenericTimer::pInstance->setPeriodicEvent(&trigger_Periodic, 1 * 1000 * 1000);
}


void StressTestInitiator::func_Periodic(dword_t arg) {
	//EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent1)::StressTestComponent1::trigger_printOut, 0);
//	DEBUG_STREAM(TAG,"Periodic");
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent0)::StressTestComponent0::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent1)::StressTestComponent1::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent2)::StressTestComponent2::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent3)::StressTestComponent3::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent4)::StressTestComponent4::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent5)::StressTestComponent5::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent6)::StressTestComponent6::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent7)::StressTestComponent7::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent8)::StressTestComponent8::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent9)::StressTestComponent9::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent10)::StressTestComponent10::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent11)::StressTestComponent11::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent12)::StressTestComponent12::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent13)::StressTestComponent13::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent14)::StressTestComponent14::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent15)::StressTestComponent15::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent16)::StressTestComponent16::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent17)::StressTestComponent17::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent18)::StressTestComponent18::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent19)::StressTestComponent19::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent20)::StressTestComponent20::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent21)::StressTestComponent21::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent22)::StressTestComponent22::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent23)::StressTestComponent23::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent24)::StressTestComponent24::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent25)::StressTestComponent25::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent26)::StressTestComponent26::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent27)::StressTestComponent27::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent28)::StressTestComponent28::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent29)::StressTestComponent29::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent30)::StressTestComponent30::trigger_printOut, 0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent31)::StressTestComponent31::trigger_printOut, 0);
}
END_OSC_IMPLEMENTATION

