/*
 * UARTOutputOSC.cc
 *
 *  Created on: 28.07.2015
 *      Author: hendrik
 */

#include <common/debug.h>
#include <testcomponents/UARTConsole.h>
#define TAG "UARTOutputOSC"

#include "ApplicationPeriodicTest.h"
#include "ApplicationReboot.h"
#include "ApplicationSystemInfo.h"
#include "TLBInfo.h"

#include "ApplicationCacheMissTest.h"
#if defined(CONFIG_CAN) && defined(CONFIG_AUTOLAB)
#include <autolabcomponents/Lights.h>
#include <autolabcomponents/Pedals.h>
#include <autolabcomponents/SteeringColumn.h>
#include <autolabcomponents/GearSelector.h>
#endif

#ifdef CONFIG_CAN
#include "ApplicationCANTest.h"
#include <arch/armv7/imx6/driver/MCP2515SPI.h>
#include <canbenchmark-components/CANSimpleTalkerApplication.h>
#include <canbenchmark-components/CANSimpleReceiver.h>
#endif

#ifndef CONFIG_UART_CONSOLE_DRIVER
#error "No uart console driver defined"
#define CONFIG_UART_CONSOLE_DRIVER DUMMY
#endif

#include <common/SyncOutput.h>

#ifdef CONFIG_PROFILING
#include <benchmarking/ResultPrintApplication.h>
#endif

#include <arch/armv7/imx6/driver/IMX6UartDriver.h>

#include <eventhandling/EventHandler.h>


#include <stresstest/StressTestInitiator.h>

#include "BenchmarkCore0.h"

#ifdef CONFIG_IMX6_TEMPMONITOR
#include <arch/armv7/imx6/driver/IMX6TempMonitor.h>
#endif

#ifdef CONFIG_TACLEBENCH
#include <taclebench-components/TacleBenchInitiator.h>
#endif

#ifdef CONFIG_AMD64
#include <driver/LegacyKeyboard.h>
#endif

/* Static instance */
BEGIN_OSC_IMPLEMENTATION(UARTConsole,UARTConsole)

SECTION_CRITICAL_DATA OSC* UARTConsole::trigger_receiveChar_deps[] = {CONSOLE_BUFFER_DEP,nullptr};
SECTION_CRITICAL_DATA EventHandling::Trigger UARTConsole::trigger_receiveChar = {(OSC*)&UARTConsole::pInstance,(void (OSC::*)(dword_t))&UARTConsole::triggerFunc_handleKey,
				(OSC**)&UARTConsole::trigger_receiveChar_deps, CONFIG_PRIORITY_UART - 1};

UARTConsole UARTConsole::pInstance;

ConsoleApplication::ConsoleApplicationEntry UARTConsole::mApplications[] = {
#ifdef CONFIG_ARMV7
		{&OSC_PREFIX(ApplicationSystemInfo)::ApplicationSystemInfo::trigger_printPMUStats,"pmustats"},

		{&OSC_PREFIX(ApplicationSystemInfo)::ApplicationSystemInfo::trigger_multiCore,"mc"},
		{&OSC_PREFIX(TLBInfo)::TLBInfo::trigger_show,"tlbinfo"},
#endif
		{&OSC_PREFIX(ApplicationSystemInfo)::ApplicationSystemInfo::trigger_verifyCritical,"verify"},
		{&OSC_PREFIX(ApplicationSystemInfo)::ApplicationSystemInfo::trigger_crashTest,"crashtest"},
		{&OSC_PREFIX(ApplicationCacheMissTest)::ApplicationCacheMissTest::trigger_verifyCache,"misstest"},
		{&OSC_PREFIX(ApplicationSystemInfo)::ApplicationSystemInfo::trigger_printEventDispatcherStats,"eventstats"},
		{&OSC_PREFIX(ApplicationSystemInfo)::ApplicationSystemInfo::trigger_printUptime,"uptime"},
		{&OSC_PREFIX(ApplicationSystemInfo)::ApplicationSystemInfo::trigger_floatTest,"float"},
		{&OSC_PREFIX(ApplicationSystemInfo)::ApplicationSystemInfo::trigger_printVersion,"version"},
		{&OSC_PREFIX(ApplicationSystemInfo)::ApplicationSystemInfo::trigger_printCacheManagementStats,"cachestats"},
		{&OSC_PREFIX(ApplicationPeriodicTest)::ApplicationPeriodicTest::trigger_startPeriodic,"startperiodic"},
		{&OSC_PREFIX(ApplicationPeriodicTest)::ApplicationPeriodicTest::trigger_oneshotTest,"oneshot"},
		{&OSC_PREFIX(ApplicationPeriodicTest)::ApplicationPeriodicTest::trigger_stopPeriodic,"stopperiodic"},
		{&OSC_PREFIX(ApplicationPeriodicTest)::ApplicationPeriodicTest::trigger_endlessTest,"endless"},
		{&OSC_PREFIX(ApplicationReboot)::ApplicationReboot::trigger_Reboot,"reboot"},
#ifdef CONFIG_STRESSTEST_COMPONENTS
		{&OSC_PREFIX(StressTestInitiator)::StressTestInitiator::trigger_Start,"stress"},
#endif
#if defined(CONFIG_CAN) && defined(CONFIG_AUTOLAB)
		{&OSC_PREFIX(MCP2515SPI)::MCP2515SPI::trigger_startAutolab,"autolab"},
		{&OSC_PREFIX(ApplicationLights)::ApplicationLights::trigger_start,"lights"},
		{&OSC_PREFIX(ApplicationLights)::ApplicationLights::trigger_stop,"stoplights"},
		{&OSC_PREFIX(ApplicationPedals)::ApplicationPedals::trigger_start,"pedals"},
		{&OSC_PREFIX(ApplicationPedals)::ApplicationPedals::trigger_stop,"stoppedals"},
		{&OSC_PREFIX(ApplicationSteeringColumn)::ApplicationSteeringColumn::trigger_start,"steering"},
		{&OSC_PREFIX(ApplicationSteeringColumn)::ApplicationSteeringColumn::trigger_stop,"stopsteering"},
		{&OSC_PREFIX(ApplicationGearSelector)::ApplicationGearSelector::trigger_start,"gear"},
		{&OSC_PREFIX(ApplicationGearSelector)::ApplicationGearSelector::trigger_stop,"stopgear"},
		{&OSC_PREFIX(MCP2515SPI)::MCP2515SPI::trigger_start,"startcan"},
		{&OSC_PREFIX(MCP2515SPI)::MCP2515SPI::trigger_printRx,"printrx"},
		{&OSC_PREFIX(MCP2515SPI)::MCP2515SPI::trigger_printTx,"printtx"},
		{&OSC_PREFIX(CANSimpleTalkerApplication)::CANSimpleTalkerApplication::trigger_start,"canstalker"},
		{&OSC_PREFIX(CANSimpleReceiver)::CANSimpleReceiver::trigger_start,"cansr"},
#endif
#ifdef CONFIG_IMX6_TEMPMONITOR
		{&OSC_PREFIX(IMX6TempMonitor)::IMX6TempMonitor::trigger_dumpTemps,"temps"},
#endif
#ifdef CONFIG_PROFILING
		{&OSC_PREFIX(ResultPrintApplication)::ResultPrintApplication::trigger_PrintResultContainer,"benchmark"},
#endif
#ifdef CONFIG_TACLEBENCH
		{&OSC_PREFIX(TacleBenchInitiator)::TacleBenchInitiator::trigger_startBenchmarks,"tacle"},
#endif
		{nullptr,"END"}
};

UARTConsole::UARTConsole() {

	mInputBufferPointer = 0;
}

void UARTConsole::triggerFunc_handleKey(dword_t key) {
	// Enter key
	if ( (char)key == 0xd || (char)key == 0xa ) { /* Return */
		SYNC_OUTPUT_STREAM_START("");
		parseCommand();
		clearBuffer();
		SYNC_OUTPUT_STREAM_END;
	} else if (key >= ' ' ) {
		if ( mInputBufferPointer + 1 < UART_CONSOLE_BUFFER_SIZE ) {
			mInputBuffer[mInputBufferPointer++] = (char)key;
			SYNC_OUTPUT_STREAM_CHAR((char)key );
		} else {
			// Error handling
			clearBuffer();
		}
	} else if (key == 0x8) { /* Backspace */
		if (mInputBufferPointer > 0 ) {
			mInputBufferPointer--;
			mInputBuffer[mInputBufferPointer] = 0;
			SYNC_OUTPUT_STREAM_RAW("\x8\x20\x8");
		}
	}
}

void UARTConsole::clearBuffer() {
	for (uint32_t pos = 0; pos < UART_CONSOLE_BUFFER_SIZE; pos++) {
		mInputBuffer[pos] = 0;
	}
	mInputBufferPointer = 0;
	SYNC_OUTPUT_STREAM_CONTINUE("\n\rCyPhOS > ");
}

bool UARTConsole::compareCommand(const char *cmd1, const char *cmd2) {
	if ( cmd1 == nullptr || cmd2 == nullptr ) {
		return false;
	}

	while ( *cmd1 != 0 && *cmd2 != 0 ) {
		if ( *cmd1 != *cmd2 ) {
			return false;
		}
		cmd1++;
		cmd2++;
	}
	if ( *cmd1 == 0 && *cmd2 == 0 ) {
		return true;
	} else {
		return false;
	}
}

void UARTConsole::parseCommand() {
//	OUTPUT_STREAM << "Parsing command: " << mInputBuffer << endl;
	if ( compareCommand(mInputBuffer,"help") ) {
		printHelp();
		return;
	} else if ( compareCommand(mInputBuffer, "unsub") ) {
		/* Subscription testing */
		EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(InputOutput)::GenericIO::event_charReady, &trigger_receiveChar);
		EventHandling::EventHandler::pInstance.unsubscribeEvent(&OSC_PREFIX(InputOutput)::GenericIO::event_charReady, &trigger_receiveChar);
	}

	ConsoleApplication::ConsoleApplicationEntry *entry = mApplications;
	while ( entry->trigger != nullptr ) {
		if (compareCommand(mInputBuffer, entry->mCommandName) ) {
			EventHandling::EventHandler::pInstance.callOSCTrigger(entry->trigger, 0);
			return;
		}
		entry++;
	}
	SYNC_OUTPUT_STREAM_CONTINUE(endl << "Unknown command: " << mInputBuffer << endl);
}

void UARTConsole::printHelp() {
	SYNC_OUTPUT_STREAM_CONTINUE(endl << "Available commands: " << endl);

	ConsoleApplication::ConsoleApplicationEntry *entry = mApplications;
	while ( entry->trigger != nullptr ) {
		SYNC_OUTPUT_STREAM_CONTINUE(entry->mCommandName << endl);
		entry++;
	}
}

/**
 * Starts the console application.
 */
void UARTConsole::start() {
	DEBUG_STREAM(TAG,"Starting UARTConsole");
	/* Register listener to uart console */
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(InputOutput)::GenericIO::event_charReady, &trigger_receiveChar);
}

END_OSC_IMPLEMENTATION
