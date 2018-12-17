#ifndef ARCH_ARMV7_IMX6_DRIVER_IMX6ECSPI_H_
#define ARCH_ARMV7_IMX6_DRIVER_IMX6ECSPI_H_
/*
 * IMX6ECSPI.h
 *
 *  Created on: 05.12.2016
 *      Author: hendrik
 */

#include <component/OSC.h>

#include <eventhandling/Event.h>
#include <eventhandling/Trigger.h>
#include <eventhandling/TimerEvent.h>

#define ECSPI1_BASE 0x2008000
#define ECSPI2_BASE 0x200C000
#define ECSPI3_BASE 0x2010000
#define ECSPI4_BASE 0x2014000
#define ECSPI5_BASE 0x2018000

#define ECSPIX_RXDATA(ECSPI_BASE) ((byte_t*)ECSPI_BASE + 0x0)
#define ECSPIX_TXDATA(ECSPI_BASE) ((byte_t*)ECSPI_BASE + 0x4)
#define ECSPIX_CONREG(ECSPI_BASE) ((byte_t*)ECSPI_BASE + 0x8)
#define ECSPIX_CONFIGREG(ECSPI_BASE) ((byte_t*)ECSPI_BASE + 0xC)
#define ECSPIX_INTREG(ECSPI_BASE) ((byte_t*)ECSPI_BASE + 0x10)
#define ECSPIX_DMAREG(ECSPI_BASE) ((byte_t*)ECSPI_BASE + 0x14)
#define ECSPIX_STATREG(ECSPI_BASE) ((byte_t*)ECSPI_BASE + 0x18)
#define ECSPIX_PERIODREG(ECSPI_BASE) ((byte_t*)ECSPI_BASE + 0x1C)
#define ECSPIX_TESTREG(ECSPI_BASE) ((byte_t*)ECSPI_BASE + 0x20)
#define ECSPIX_MSGDATA(ECSPI_BASE) ((byte_t*)ECSPI_BASE + 0x40)

/* Input/Output configuration registers */
#define IOMUXC_ECSPI1_CSPI_CLK_IN_SELECT_INPUT 0x20E07F4
#define IOMUXC_SW_MUX_CTL_PAD_EIM_DATA16 0x20E0090

#define IOMUXC_ECSPI1_MISO_SELECT_INPUT 0x20E07F8
#define IOMUXC_SW_MUX_CTL_PAD_EIM_DATA17 0x20E0094

#define IOMUXC_ECSPI1_MOSI_SELECT_INPUT 0x20E07FC
#define IOMUXC_SW_MUX_CTL_PAD_EIM_DATA18 0x20E0098

#define IOMUXC_ECSPI1_SS0_SELECT_INPUT 0x20E0800
#define IOMUXC_SW_MUX_CTL_PAD_EIM_EB2_B 0x20E008C

#define IOMUXC_SW_PAD_CTL_PAD_EIM_DATA16 0x20E03A4
#define IOMUXC_SW_PAD_CTL_PAD_EIM_DATA17 0x20E03A8
#define IOMUXC_SW_PAD_CTL_PAD_EIM_DATA18 0x20E03AC
#define IOMUXC_SW_PAD_CTL_PAD_EIM_EB2_B 0x20E03A0


#define ECSPI_CONREG_EN 0x0
#define ECSPI_CONREG_XCH 0x2
#define ECSPI_CONREG_SMC 0x3
#define ECSPI_CONREG_CHANNEL_MODE 0x4
#define ECSPI_CONREG_CHANNEL_SELECT 0x12
#define ECSPI_CONREG_DRCTL 0x10
#define ECSPI_CONREG_BURST_LENGTH 0x14
#define ECSPI_CONREG_PRE_DIVIDER 0xC

#define ECSPI_CONFIGREG_SCLK_PHA 0x0
#define ECSPI_CONFIGREG_SCLK_POL 0x4
#define ECSPI_CONFIGREG_SCLK_CTL 0x14
#define ECSPI_CONFIGREG_DATA_CTL 0x10

#define ECSPI_STATREG_TF 0x2
#define ECSPI_STATREG_RR 0x3

#define SPI_MAX_CHANNEL_COUNT 4
#define SPI_MAX_BURST_LENGTH 4096
#define SPI_BURST_LENGTH_MASK 0xFFF

#define SPI_CLOCK_MAX_PREDIVIDER_VALUE 16
#define SPI_CLOCK_PREDIVIDER_MASK 0xF

#define SPI_OUTPUT_BUFFER_SIZE 32
#define SPI_INPUT_BUFFER_SIZE 32

MEMBER_OF_OSC_BEGIN(MCP2515SPI)

class IMX6ECSPI {

private:
	enum EIM_D16_PAD {
		EIM_DATA16 = 0,
		ECSPI1_SCLK = 1,
		IPU1_DI0_PIN05 = 2,
		IPU2_CSI1_DATA18 = 3,
		HDMI_TX_DDC_SDA = 4,
		GPIO3_IO16 = 5,
		I2C2_SDA = 6
	};

	enum EIM_D17_PAD {
		EIM_DATA17 = 0,
		ECSPI1_MISO = 1,
		IPU1_DI0_PIN06 = 2,
		IPU2_CSI1_PIXCLK = 3,
		DCIC1_OUT = 4,
		GPIO3_IO17 = 5,
		I2C3_SCL = 6
	};

	enum EIM_D18_PAD {
		EIM_DATA18 = 0,
		ECSPI1_MOSI = 1,
		IPU1_DI0_PIN07 = 2,
		IPU2_CSI1_DATA17 = 3,
		IPU_DI1_D0_CS= 4,
		GPIO3_IO18 = 5,
		I2C3_SDA = 6
	};

	enum EIM_EB2_PAD {
		EIM_EB2_B = 0,
		ECSPI1_SS0 = 1,
		IPU2_CSI1_DATA19 = 3,
		HDMI_TX_DDC_SCL = 4,
		GPIO2_IO30 = 5,
		I2C2_SCL = 6,
		SRC_BOOT_CFG30 = 7
	};

	void configurePorts();

	uint16_t mReadBufferRPtr;
	uint16_t mReadBufferWPtr;

	uint32_t mInputBuffer[SPI_INPUT_BUFFER_SIZE];


protected:
	uintptr_t mECSPIBase;

	void selectChannel(uint8_t channel);

	void selectChannelMode(uint8_t channel, bool master);

	void selectClockPhaseOperation(uint8_t channel, bool mode);

	void selectClockPolarity(uint8_t channel, bool activeLow);

	void selectClockIdle(uint8_t channel, bool high);
	void selectDataIdle(uint8_t channel, bool low);

	void selectBurstLength(uint16_t burstLength);

	void setClockPreDivider(uint16_t divider);

	uint32_t writeOutputBuffer(uint16_t length);

	uint32_t readInputBuffer();

	uint32_t mOutputBuffer[SPI_OUTPUT_BUFFER_SIZE];

	void dumpRegisters();

public:
	void startSPI();
	void enableLoopback(bool enable);
};

MEMBER_OF_OSC_END

#endif /* ARCH_ARMV7_IMX6_DRIVER_IMX6ECSPI_H_ */
