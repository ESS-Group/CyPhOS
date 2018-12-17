/*
 * TacleBenchInitiator.cc
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#include <taclebench-components/TacleBenchInitiator.h>

#include <common/SyncOutput.h>

#include <taclebench-components/common/common.h>

#include <eventhandling/EventHandler.h>

#define TAG "TacleBenchInitiator"

#ifdef TACLE_BINARYSEARCH
#include "binarysearch/TaclebinarysearchComponent.h"
#endif
#ifdef TACLE_BITCOUNT
#include "bitcount/TacleBitcountComponent.h"
#endif
#ifdef TACLE_BITONIC
#include "bitonic/TacleBitonicComponent.h"
#endif
#ifdef TACLE_BSORT
#include "bsort/TacleBsortComponent.h"
#endif
#ifdef TACLE_COMPLEX_UPDATES
#include "complex_updates/TacleComplexUpdatesComponent.h"
#endif
#ifdef TACLE_COUNTNEGATIVE
#include "countnegative/TacleCountnegativeComponent.h"
#endif
#ifdef TACLE_FAC
#include "fac/TacleFacComponent.h"
#endif
#ifdef TACLE_FFT
#include "fft/TacleFftComponent.h"
#endif
#ifdef TACLE_FILTERBANK
#include "filterbank/TaclefilterbankComponent.h"
#endif
#ifdef TACLE_FIR2DIM
#include "fir2dim/Taclefir2dimComponent.h"
#endif
#ifdef TACLE_IIR
#include "iir/TacleiirComponent.h"
#endif
#ifdef TACLE_INSERTSORT
#include "insertsort/TacleinsertsortComponent.h"
#endif
#ifdef TACLE_JFDCTINT
#include "jfdctint/TaclejfdctintComponent.h"
#endif
#ifdef TACLE_LMS
#include "lms/TaclelmsComponent.h"
#endif
#ifdef TACLE_LUDCMP
#include "ludcmp/TacleludcmpComponent.h"
#endif
#ifdef TACLE_MATRIX1
#include "matrix1/Taclematrix1Component.h"
#endif
#ifdef TACLE_MD5
#include "md5/Taclemd5Component.h"
#endif
#ifdef TACLE_MINVER
#include "minver/TacleminverComponent.h"
#endif
#ifdef TACLE_PM
#include "pm/TaclepmComponent.h"
#endif
#ifdef TACLE_PRIME
#include "prime/TacleprimeComponent.h"
#endif
#ifdef TACLE_QUICKSORT
#include "quicksort/TaclequicksortComponent.h"
#endif
#ifdef TACLE_RECURSION
#include "recursion/TaclerecursionComponent.h"
#endif
#ifdef TACLE_SHA
#include "sha/TacleshaComponent.h"
#endif
#ifdef TACLE_ST
#include "st/TaclestComponent.h"
#endif


BEGIN_OSC_IMPLEMENTATION(TacleBenchInitiator, TacleBenchInitiator)

TacleBenchInitiator TacleBenchInitiator::mInstance;

SECTION_CRITICAL_DATA OSC* TacleBenchInitiator::trigger_deps[] = {&OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance,nullptr};

SECTION_CRITICAL_DATA DEFINE_TRIGGER(TacleBenchInitiator, startBenchmarks, TacleBenchInitiator::mInstance, trigger_deps, TACLE_BENCH_PRIORITY + 10);

TacleBenchInitiator::TacleBenchInitiator() {

}

DEFINE_TRIGGER_FUNC(TacleBenchInitiator,startBenchmarks) {

	// Call all enabled tacle components here
#ifdef TACLE_BINARYSEARCH
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(TaclebinarysearchComponent)::TaclebinarysearchComponent::trigger_main, 0);
#endif
#ifdef TACLE_BITCOUNT
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(Tacle_BitcountComponent)::Tacle_BitcountComponent::trigger_main, 0);
#endif
#ifdef TACLE_BITONIC
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(Tacle_BitonicComponent)::Tacle_BitonicComponent::trigger_main, 0);
#endif
#ifdef TACLE_BSORT
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(Tacle_BsortComponent)::Tacle_BsortComponent::trigger_main, 0);
#endif
#ifdef TACLE_COMPLEX_UPDATES
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(Tacle_ComplexUpdatesComponent)::Tacle_ComplexUpdatesComponent::trigger_main, 0);
#endif
#ifdef TACLE_COUNTNEGATIVE
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(Tacle_CountnegativeComponent)::Tacle_CountnegativeComponent::trigger_main, 0);
#endif
#ifdef TACLE_FAC
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(Tacle_FacComponent)::Tacle_FacComponent::trigger_main, 0);
#endif
#ifdef TACLE_FFT
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(Tacle_FftComponent)::Tacle_FftComponent::trigger_main, 0);
#endif
#ifdef TACLE_FILTERBANK
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(TaclefilterbankComponent)::TaclefilterbankComponent::trigger_main, 0);
#endif
#ifdef TACLE_FIR2DIM
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(Taclefir2dimComponent)::Taclefir2dimComponent::trigger_main, 0);
#endif
#ifdef TACLE_IIR
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(TacleiirComponent)::TacleiirComponent::trigger_main, 0);
#endif
#ifdef TACLE_INSERTSORT
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(TacleinsertsortComponent)::TacleinsertsortComponent::trigger_main, 0);
#endif
#ifdef TACLE_JFDCINT
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(TaclejfdctintComponent)::TaclejfdctintComponent::trigger_main, 0);
#endif
#ifdef TACLE_LMS
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(TaclelmsComponent)::TaclelmsComponent::trigger_main, 0);
#endif
#ifdef TACLE_LUDCMP
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(TacleludcmpComponent)::TacleludcmpComponent::trigger_main, 0);
#endif
#ifdef TACLE_MATRIX1
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(Taclematrix1Component)::Taclematrix1Component::trigger_main, 0);
#endif
#ifdef TACLE_MD5
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(Taclemd5Component)::Taclemd5Component::trigger_main, 0);
#endif
#ifdef TACLE_MINVER
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(TacleminverComponent)::TacleminverComponent::trigger_main, 0);
#endif
#ifdef TACLE_PM
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(TaclepmComponent)::TaclepmComponent::trigger_main, 0);
#endif
#ifdef TACLE_PRIME
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(TacleprimeComponent)::TacleprimeComponent::trigger_main, 0);
#endif
#ifdef TACLE_QUICKSORT
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(TaclequicksortComponent)::TaclequicksortComponent::trigger_main, 0);
#endif
#ifdef TACLE_RECURSION
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(TaclerecursionComponent)::TaclerecursionComponent::trigger_main, 0);
#endif
#ifdef TACLE_SHA
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(TacleshaComponent)::TacleshaComponent::trigger_main, 0);
#endif
#ifdef TACLE_ST
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(TaclestComponent)::TaclestComponent::trigger_main, 0);
#endif
}

END_OSC_IMPLEMENTATION
