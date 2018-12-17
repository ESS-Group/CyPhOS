/*
 * IMX6TempMonitor.cc
 *
 *  Created on: 26.02.2018
 *      Author: hendrik
 */

#include <arch/armv7/imx6/driver/IMX6TempMonitor.h>

#include <driver/ConsoleBuffer.h>

#include <driver/GenericTimer.h>

/* Include interrupt controller to control interrupt flags */
#include <driver/ARMGIC.h>
#include <interrupts/InterruptDispatcher.h>

#include <common/SyncOutput.h>

#include <common/memreg.h>

#define TAG "IMX6TempMonitor"

/* Include the i.MX6 Clock/Power management unit */
#include <arch/armv7/imx6/driver/IMX6ClockManagement.h>

BEGIN_OSC_IMPLEMENTATION(IMX6TempMonitor, IMX6TempMonitor)


/**
 * Static singleton instance
 */
IMX6TempMonitor IMX6TempMonitor::mInstance;

/**
 * Instances of static trigger objects
 */
SECTION_CRITICAL_DATA DEFINE_TRIGGER(IMX6TempMonitor, initialize, IMX6TempMonitor::mInstance, trigger_deps_output,EventHandling::Trigger::cMIN_Priority);
SECTION_CRITICAL_DATA DEFINE_TRIGGER(IMX6TempMonitor, interrupt, IMX6TempMonitor::mInstance, trigger_deps,EventHandling::Trigger::cMIN_Priority + 50);

SECTION_CRITICAL_DATA DEFINE_TRIGGER(IMX6TempMonitor, dumpTemps, IMX6TempMonitor::mInstance, trigger_deps,EventHandling::Trigger::cMIN_Priority + 300);

/**
 * Instance of trigger dependencies
 */
SECTION_CRITICAL_DATA OSC* IMX6TempMonitor::trigger_deps[] = { nullptr };
SECTION_CRITICAL_DATA OSC* IMX6TempMonitor::trigger_deps_output[] = {&OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance, nullptr };

IMX6TempMonitor::IMX6TempMonitor() {
	// TODO Auto-generated constructor stub

}

DEFINE_TRIGGER_FUNC(IMX6TempMonitor,initialize) {
	LOG_OUTPUT_STREAM(TAG, "Initializing the IMX6 temperature monitoring\n");

	// Enable interrupt
	InterruptDispatcher::pInstance.registerEvent(HW_INT_TEMP_MON, &trigger_interrupt);


	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.setInterruptTarget(HW_INT_TEMP_MON, 0xF);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.configureInterruptHandling(HW_INT_TEMP_MON, true);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.clearPending(HW_INT_TEMP_MON);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(HW_INT_TEMP_MON);

	// Read calibration data from fused register
	dword_t calData = READ_REGISTER(OCOTP_ANA1);

	mRoomCount = (calData >> 20);
	mHotCount = (calData & 0xFFF00) >> 8;
	mHotTemp = (calData & 0xFF);

	dword_t tmpReg;

	// Programm the temperature monitor
	tmpReg = READ_REGISTER(TEMPSENSE_CTR0_CLR);
	tmpReg &= ~(TEMPMON_TEMPSENSE_POWER_DOWN);
	tmpReg |= TEMPMON_TEMPSENSE_MEASURE_TEMP;

	tmpReg &= ~(TEMPMON_TEMPSENSE_ALARMCNT);

	// Calculate the correct value for the alert temperature (p. 5187)
	uint32_t calcVal = mHotCount + (-cWarningTemp + mHotTemp) / ((float)(mHotTemp - 25.0) / (float)(mRoomCount - mHotCount));
	LOG_OUTPUT_STREAM(TAG,"Alarm temperature: " << dec << (uint16_t)cWarningTemp << " dC, alarm count val: " << calcVal << endl);

	tmpReg |= (calcVal << 20);

	WRITE_REGISTER(TEMPSENSE_CTR0, tmpReg);


	dumpRegisters();
	dumpCalibration();


}

DEFINE_TRIGGER_FUNC(IMX6TempMonitor,interrupt) {
	DEBUG_STREAM(TAG, "TEMPERATURE ALERT!!!");

	// FIXME add proper throttling/dethrottling
	IMX6ClockManagement::mInstance.setCoreFrequencyVoltage(IMX6ClockManagement::mFrequencies[0]);
}

void IMX6TempMonitor::dumpRegisters() {
	LOG_OUTPUT_STREAM_START(TAG,"Monitoring registers:" << endl);

	LOG_OUTPUT_STREAM_CONTINUE(TAG,"CAL data:" << hex << READ_REGISTER(OCOTP_ANA1) << endl);


	LOG_OUTPUT_STREAM_CONTINUE(TAG,"CTR0: " << READ_REGISTER(TEMPSENSE_CTR0) << endl);
	LOG_OUTPUT_STREAM_CONTINUE(TAG,"CTR1: " << READ_REGISTER(TEMPSENSE_CTR1) << endl);

	LOG_OUTPUT_STREAM_END;
}

void IMX6TempMonitor::dumpCalibration() {
	LOG_OUTPUT_STREAM_START(TAG,"Calibration:" << endl);

	LOG_OUTPUT_STREAM_CONTINUE(TAG,"ROOM_COUNT: " << dec << mRoomCount << endl);
	LOG_OUTPUT_STREAM_CONTINUE(TAG,"HOT_COUNT: " << dec << mHotCount << endl);
	LOG_OUTPUT_STREAM_CONTINUE(TAG,"HOT_TEMP: " << dec << mHotTemp << endl);
	LOG_OUTPUT_STREAM_END;

}

DEFINE_TRIGGER_FUNC(IMX6TempMonitor,dumpTemps) {
	uint32_t ctr0 = READ_REGISTER(TEMPSENSE_CTR0);

	if (!(ctr0 & TEMPMON_TEMPSENSE_FINISHED)) {
		SYNC_OUTPUT_STREAM(TAG,"Invalid measurement" << endl);
		return;
	}

	uint32_t nMes = (READ_REGISTER(TEMPSENSE_CTR0) & TEMPMON_TEMPSENSE_TEMPCNT) >> 8;
	SYNC_OUTPUT_STREAM(TAG, "TEMP_CNT: " << dec << nMes << endl);

	// Calculate the correct temperature value (IMX6 trm p. 5187)
	uint32_t temp = (uint32_t)(mHotTemp - (nMes - mHotCount) * ((float)(mHotTemp - 25.0) / (float)(mRoomCount - mHotCount)));
	SYNC_OUTPUT_STREAM(TAG, "Measured temperature: " << temp << endl);
}
END_OSC_IMPLEMENTATION
