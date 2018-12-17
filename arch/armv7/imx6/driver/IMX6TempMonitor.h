/*
 * IMX6TempMonitor.h
 *
 *  Created on: 26.02.2018
 *      Author: hendrik
 */


#include <component/OSC.h>
#include <eventhandling/Trigger.h>

#ifndef ARCH_ARMV7_IMX6_DRIVER_IMX6TEMPMONITOR_H_
#define ARCH_ARMV7_IMX6_DRIVER_IMX6TEMPMONITOR_H_


#define TEMPSENSE_CTR0 0x20C8180
#define TEMPSENSE_CTR0_SET 0x20C8184
#define TEMPSENSE_CTR0_CLR 0x20C8188
#define TEMPSENSE_CTR0_TOG 0x20C818C

#define TEMPSENSE_CTR1 0x20C8190
#define TEMPSENSE_CTR1_SET 0x20C8194
#define TEMPSENSE_CTR1_CLR 0x20C8198
#define TEMPSENSE_CTR1_TOG 0x20C819C

#define TEMPMON_TEMPSENSE_POWER_DOWN (0x1)
#define TEMPMON_TEMPSENSE_MEASURE_TEMP (0x2)
#define TEMPMON_TEMPSENSE_FINISHED (0x2)
#define TEMPMON_TEMPSENSE_TEMPCNT (0xFFF00)
#define TEMPMON_TEMPSENSE_ALARMCNT (0xFFF00000)

/**
 * Register containing fused calibration data
 */
#define OCOTP_ANA1 0x021BC4E0

BEGIN_DECLARE_OSC(IMX6TempMonitor, IMX6TempMonitor)

public:
	IMX6TempMonitor();

	static IMX6TempMonitor mInstance;

	/**
	 * Dependencies for the triggers
	 */
	static OSC* trigger_deps[];
	static OSC* trigger_deps_output[];

	DECLARE_TRIGGER(initialize);
	DECLARE_TRIGGER(interrupt);

	DECLARE_TRIGGER(dumpTemps);

private:
	/**
	 * Warning temperature at which an interrupt is set (Unit: Degree celsius)
	 */
	static constexpr uint8_t cWarningTemp = 70;

	void dumpRegisters();
	void dumpCalibration();

	uint32_t mRoomCount;
	uint32_t mHotCount;
	uint32_t mHotTemp;
};

END_DECLARE_OSC
#endif /* ARCH_ARMV7_IMX6_DRIVER_IMX6TEMPMONITOR_H_ */
