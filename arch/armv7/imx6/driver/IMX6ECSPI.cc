/*
 * IMX6ECSPI.cc
 *
 *  Created on: 05.12.2016
 *      Author: hendrik
 */

#include <common/memreg.h>

#include "IMX6ECSPI.h"

#include <driver/IMX6ECSPI.h>


#include <common/debug.h>

#define TAG "IMX6SPI"

MEMBER_OF_OSC_BEGIN(MCP2515SPI)

/**
 * Configures the IOMUX of the IMX6 SoC.
 */
void IMX6ECSPI::configurePorts() {
	/*
	 * Configure the SPI Clock output,
	 * this needs two stages of output configuration.
	 * First configure the Select Input Register
	 * (IOMUXC_ECSPI1_CSPI_CLK_IN_SELECT_INPUT) that configures the daisy-chaining.
	 * Then configure the EIM_DATA16 output register that configures the output port
	 * that is physically connected to the JP4 ECSPI1 port of the wandboard.
	 */

	/* Set ECSPI1_SCLK */
	dword_t regTmp = READ_REGISTER(IOMUXC_ECSPI1_CSPI_CLK_IN_SELECT_INPUT);
	regTmp &= ~(0x3);
	WRITE_REGISTER(IOMUXC_ECSPI1_CSPI_CLK_IN_SELECT_INPUT, regTmp);

	regTmp = READ_REGISTER(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA16);
	regTmp &= ~(0x7);
	regTmp |= (0x1);
	WRITE_REGISTER(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA16, regTmp);

	/* Set ECSPI1_MISO */
	regTmp = READ_REGISTER(IOMUXC_ECSPI1_MISO_SELECT_INPUT);
	regTmp &= ~(0x3);
	WRITE_REGISTER(IOMUXC_ECSPI1_MISO_SELECT_INPUT, regTmp);

	regTmp = READ_REGISTER(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA17);
	regTmp &= ~(0x7);
	regTmp |= (0x1);
	WRITE_REGISTER(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA17, regTmp);

	/* Set ECSPI1_MOSI */
	regTmp = READ_REGISTER(IOMUXC_ECSPI1_MOSI_SELECT_INPUT);
	regTmp &= ~(0x3);
	WRITE_REGISTER(IOMUXC_ECSPI1_MOSI_SELECT_INPUT, regTmp);

	regTmp = READ_REGISTER(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA18);
	regTmp &= ~(0x7);
	regTmp |= (0x1);
	WRITE_REGISTER(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA18, regTmp);

	/* Set ECSPI1_SS0 */
	regTmp = READ_REGISTER(IOMUXC_ECSPI1_SS0_SELECT_INPUT);
	regTmp &= ~(0x3);
	WRITE_REGISTER(IOMUXC_ECSPI1_SS0_SELECT_INPUT, regTmp);

	regTmp = READ_REGISTER(IOMUXC_SW_MUX_CTL_PAD_EIM_EB2_B);
	regTmp &= ~(0x7);
	regTmp |= (0x1);
	WRITE_REGISTER(IOMUXC_SW_MUX_CTL_PAD_EIM_EB2_B, regTmp);

	/*
	 * Configures the Input/Output pins of the SoC. See i.MX6 p. 2190pp
	 * for more information about these values.
	 */
	dword_t outputPortValue = 0x4061;
	dword_t inputPortValue = 0x0041;

	WRITE_REGISTER(IOMUXC_SW_PAD_CTL_PAD_EIM_DATA16, outputPortValue);
	WRITE_REGISTER(IOMUXC_SW_PAD_CTL_PAD_EIM_DATA18, outputPortValue);
	WRITE_REGISTER(IOMUXC_SW_PAD_CTL_PAD_EIM_DATA17, inputPortValue);
	WRITE_REGISTER(IOMUXC_SW_PAD_CTL_PAD_EIM_EB2_B, outputPortValue);


#ifdef CONFIG_DEBUG_SPI
	/* Print registers in debug mode */
	DEBUG_STREAM(TAG,"IOMUXC_ECSPI1_CSPI_CLK_IN_SELECT_INPUT: " << hex << READ_REGISTER(IOMUXC_ECSPI1_CSPI_CLK_IN_SELECT_INPUT));
	DEBUG_STREAM(TAG,"IOMUXC_SW_MUX_CTL_PAD_EIM_DATA16: " << hex << READ_REGISTER(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA16));

	DEBUG_STREAM(TAG,"IOMUXC_ECSPI1_MISO_SELECT_INPUT: " << hex << READ_REGISTER(IOMUXC_ECSPI1_MISO_SELECT_INPUT));
	DEBUG_STREAM(TAG,"IOMUXC_SW_MUX_CTL_PAD_EIM_DATA17: " << hex << READ_REGISTER(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA17));

	DEBUG_STREAM(TAG,"IOMUXC_ECSPI1_MOSI_SELECT_INPUT: " << hex << READ_REGISTER(IOMUXC_ECSPI1_MOSI_SELECT_INPUT));
	DEBUG_STREAM(TAG,"IOMUXC_SW_MUX_CTL_PAD_EIM_DATA18: " << hex << READ_REGISTER(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA18));

	DEBUG_STREAM(TAG,"IOMUXC_ECSPI1_SS0_SELECT_INPUT: " << hex << READ_REGISTER(IOMUXC_ECSPI1_SS0_SELECT_INPUT));
	DEBUG_STREAM(TAG,"IOMUXC_SW_MUX_CTL_PAD_EIM_EB2_B: " << hex << READ_REGISTER(IOMUXC_SW_MUX_CTL_PAD_EIM_EB2_B));

	DEBUG_STREAM(TAG,"IOMUXC_SW_PAD_CTL_PAD_EIM_DATA16: " << hex << READ_REGISTER(IOMUXC_SW_PAD_CTL_PAD_EIM_DATA16));
	DEBUG_STREAM(TAG,"IOMUXC_SW_PAD_CTL_PAD_EIM_DATA17: " << hex << READ_REGISTER(IOMUXC_SW_PAD_CTL_PAD_EIM_DATA17));
	DEBUG_STREAM(TAG,"IOMUXC_SW_PAD_CTL_PAD_EIM_DATA18: " << hex << READ_REGISTER(IOMUXC_SW_PAD_CTL_PAD_EIM_DATA18));
	DEBUG_STREAM(TAG,"IOMUXC_SW_PAD_CTL_PAD_EIM_EB2_B: " << hex << READ_REGISTER(IOMUXC_SW_PAD_CTL_PAD_EIM_EB2_B));
#endif
}

/**
 * Selects a channel that is currently active. This sets the chip select
 * signal for the channel.
 * @param channel Channel (0-3) to enable
 */
void IMX6ECSPI::selectChannel(uint8_t channel) {
	if ( SPI_MAX_CHANNEL_COUNT <= channel ) {
		return;
	}

	/* Read the current control register */
	dword_t regTmp = READ_REGISTER(ECSPIX_CONREG(mECSPIBase));
	regTmp |= (channel) << ECSPI_CONREG_CHANNEL_SELECT;
	/* Write the register which selects the channel */
	WRITE_REGISTER(ECSPIX_CONREG(mECSPIBase), regTmp);
}

/**
 * Selects the burst length of SPI transfers.
 * @param burstLength Length from 0-4096 bits.
 */
void IMX6ECSPI::selectBurstLength(uint16_t burstLength) {
	if ( burstLength >= SPI_MAX_BURST_LENGTH ) {
		return;
	}

	/* Read the current control register */
	dword_t regTmp = READ_REGISTER(ECSPIX_CONREG(mECSPIBase));
	regTmp &= ~(SPI_BURST_LENGTH_MASK << ECSPI_CONREG_BURST_LENGTH);
	regTmp |= (burstLength - 1) << ECSPI_CONREG_BURST_LENGTH;

	/* Write the control register with the new burst value */
	WRITE_REGISTER(ECSPIX_CONREG(mECSPIBase), regTmp);
}

/**
 * Enables the master or slave mode for the given channel.
 * @param channel Channel to select master/slave mode for
 * @param master Channel (0-3)
 */
void IMX6ECSPI::selectChannelMode(uint8_t channel, bool master) {
	if ( SPI_MAX_CHANNEL_COUNT <= channel ) {
		return;
	}

	/* Read the current control register */
	dword_t regTmp = READ_REGISTER(ECSPIX_CONREG(mECSPIBase));
	/* Reset bit for the selected channel*/
	regTmp &= ~(0x1 << (ECSPI_CONREG_CHANNEL_MODE + channel));

	/* If master mode should be enabled set the right bit */
	if (master) {
		regTmp |= (0x1 << (ECSPI_CONREG_CHANNEL_MODE + channel));
	}
	/* Write the register which selects the channel */
	WRITE_REGISTER(ECSPIX_CONREG(mECSPIBase), regTmp);
}

/**
 * Writes to the SPI interface and simultaneously
 * @param length Length of the SPI burst in bits
 * @return The number of data words (32bit) read from the input
 */
uint32_t IMX6ECSPI::writeOutputBuffer(uint16_t length) {
	/* Set the controller burst length to the requested output length */
	selectBurstLength(length);

	uint16_t mWritePtr = 0;

	mReadBufferRPtr = 0;
	mReadBufferWPtr = 0;

	/* Write the output buffer to the TXFIFO */
	while ( length != 0 && !(READ_REGISTER(ECSPIX_STATREG(mECSPIBase)) & (0x1 << ECSPI_STATREG_TF)) ) {
		/* Write the data word in the TXFIFO */
		WRITE_REGISTER(ECSPIX_TXDATA(mECSPIBase), mOutputBuffer[mWritePtr]);

		/* Reduce the length to submit by 32 bit */
		if ( length < 32 ) {
			length = 0;
		} else {
			length -= 32;
		}
		mWritePtr++;
	}
#ifdef CONFIG_DEBUG_SPI
	DEBUG_STREAM(TAG, "Wrote: " << dec << mWritePtr << " words to spi");
	DEBUG_STREAM(TAG, "----------------Wrote: " << hex << mOutputBuffer[0]);
#endif

	dumpRegisters();

	/* Set the XCH-bit to start transmission */
	dword_t regTmp = READ_REGISTER(ECSPIX_CONREG(mECSPIBase));
	regTmp |= (0x1 << ECSPI_CONREG_XCH);

#ifdef CONFIG_DEBUG_SPI
	DEBUG_STREAM(TAG, "Writing CONREG: " << hex << regTmp);
#endif

	WRITE_REGISTER(ECSPIX_CONREG(mECSPIBase), regTmp);

#ifdef CONFIG_DEBUG_SPI
	DEBUG_STREAM(TAG, "Transmission started");
#endif
	dumpRegisters();

	/* Wait for the XCH bit to be set to 0 again (end of transmission) */
	while ( READ_REGISTER(ECSPIX_CONREG(mECSPIBase)) & (0x1 << ECSPI_CONREG_XCH)) {	}

	/* Check how many words are available in the FIFO queue */
	uint16_t readSize = 0;

	/* Empty the RXFIFO queue.
	 * Read until no words are left */
	while ( READ_REGISTER(ECSPIX_STATREG(mECSPIBase)) & (0x1 << ECSPI_STATREG_RR) ) {
		mInputBuffer[mReadBufferWPtr] = READ_REGISTER(ECSPIX_RXDATA(mECSPIBase));
#ifdef CONFIG_DEBUG_SPI
	DEBUG_STREAM(TAG, "Read word from SPI: " << hex << mInputBuffer[mReadBufferWPtr]);
	dumpRegisters();
#endif
		readSize++;
		mReadBufferWPtr = (mReadBufferWPtr + 1) % SPI_INPUT_BUFFER_SIZE;
	}

#ifdef CONFIG_DEBUG_SPI
	DEBUG_STREAM(TAG, "Read: " << dec << readSize << " words from spi");
#endif

	dumpRegisters();

	return readSize;
}

/**
 * Returns one dataword from the SPI controller.
 * @return 32bit dataword from the drivers receive buffer.
 */
uint32_t IMX6ECSPI::readInputBuffer() {
	uint32_t dataWord = mInputBuffer[mReadBufferRPtr];
	mReadBufferRPtr = (mReadBufferRPtr + 1) % SPI_INPUT_BUFFER_SIZE;
	return dataWord;
}

/**
 * Resets the SPI controller and reenables it.
 */
void IMX6ECSPI::startSPI() {
#ifdef CONFIG_DEBUG_SPI
	DEBUG_STREAM(TAG, "Start SPI with base address: " << hex << mECSPIBase);
#endif
	/* Enable the SPIs clock */
	IMX6ClockManagement::mInstance.enableSPIClock(1, true);

	/* Reset the controller first */
	dword_t regTmp = READ_REGISTER(ECSPIX_CONREG(mECSPIBase));
	/* Clear the enable bit */
	regTmp &= ~(0x1);
	WRITE_REGISTER(ECSPIX_CONREG(mECSPIBase), regTmp);

	dumpRegisters();


	/* Reenable the SPI controller */
	regTmp = READ_REGISTER(ECSPIX_CONREG(mECSPIBase));
	regTmp |= (0x1) << ECSPI_CONREG_EN;
	/* Disable Data ready control */
	regTmp &= ~(0x3 << ECSPI_CONREG_DRCTL);
	/* Disable automatic transmission start,
	 * clear SMC bit */
	regTmp &= ~(0x1 << ECSPI_CONREG_SMC);

#ifdef CONFIG_DEBUG_SPI
	DEBUG_STREAM(TAG, "Writeing CONREG: " << hex << regTmp);
#endif
	/* Write the new conreg value */
	WRITE_REGISTER(ECSPIX_CONREG(mECSPIBase), regTmp);

#ifdef CONFIG_DEBUG_SPI
	DEBUG_STREAM(TAG, "CONREG: " << hex << READ_REGISTER(ECSPIX_CONREG(mECSPIBase)));
#endif

	/* Setup the ports */
	configurePorts();

	dumpRegisters();
}

/**
 * Selects the clock phase control for a given channel. Check i.MX6 reference p. 979
 * for more information.
 * @param channel Channel to set the value for
 * @param mode
 */
void IMX6ECSPI::selectClockPhaseOperation(uint8_t channel, bool mode) {
	if ( SPI_MAX_CHANNEL_COUNT <= channel ) {
		return;
	}

	/* Read the current config register */
	dword_t regTmp = READ_REGISTER(ECSPIX_CONFIGREG(mECSPIBase));
	/* Reset bit for the selected channel*/
	regTmp &= ~(0x1 << (ECSPI_CONFIGREG_SCLK_PHA + channel));

	if (mode) {
		regTmp |= (0x1 << (ECSPI_CONFIGREG_SCLK_PHA + channel));
	}
	/* Write the register which selects the channel */
	WRITE_REGISTER(ECSPIX_CONFIGREG(mECSPIBase), regTmp);
}

/**
 * Selects the clock polarity for a given channel. Check i.MX6 reference p. 979
 * for more information.
 * @param channel Channel to set the value for
 * @param activeLow
 */
void IMX6ECSPI::selectClockPolarity(uint8_t channel, bool activeLow) {
	if ( SPI_MAX_CHANNEL_COUNT <= channel ) {
		return;
	}

	/* Read the current config register */
	dword_t regTmp = READ_REGISTER(ECSPIX_CONFIGREG(mECSPIBase));
	/* Reset bit for the selected channel*/
	regTmp &= ~(0x1 << (ECSPI_CONFIGREG_SCLK_POL + channel));

	if (activeLow) {
		regTmp |= (0x1 << (ECSPI_CONFIGREG_SCLK_POL + channel));
	}
	/* Write the register which selects the channel */
	WRITE_REGISTER(ECSPIX_CONFIGREG(mECSPIBase), regTmp);
}

/**
 * Selects if the clock should be high or low during idling.
 * @param channel Channel to set the value for.
 * @param high True if clock should be set to high during idling.
 */
void IMX6ECSPI::selectClockIdle(uint8_t channel, bool high) {
	if ( SPI_MAX_CHANNEL_COUNT <= channel ) {
		return;
	}

	/* Read the current config register */
	dword_t regTmp = READ_REGISTER(ECSPIX_CONFIGREG(mECSPIBase));
	/* Reset bit for the selected channel*/
	regTmp &= ~(0x1 << (ECSPI_CONFIGREG_SCLK_CTL + channel));

	if (high) {
		regTmp |= (0x1 << (ECSPI_CONFIGREG_SCLK_CTL + channel));
	}
	/* Write the register which selects the channel */
	WRITE_REGISTER(ECSPIX_CONFIGREG(mECSPIBase), regTmp);
}

/**
 * Select if the data output pin is low or high when idling.
 * @param channel Channel to set the value for.
 * @param low True if channel should be low during idling.
 */
void IMX6ECSPI::selectDataIdle(uint8_t channel, bool low) {
	if ( SPI_MAX_CHANNEL_COUNT <= channel ) {
		return;
	}

	/* Read the current config register */
	dword_t regTmp = READ_REGISTER(ECSPIX_CONFIGREG(mECSPIBase));
	/* Reset bit for the selected channel*/
	regTmp &= ~(0x1 << (ECSPI_CONFIGREG_DATA_CTL + channel));

	if (low) {
		regTmp |= (0x1 << (ECSPI_CONFIGREG_DATA_CTL + channel));
	}
	/* Write the register which selects the channel */
	WRITE_REGISTER(ECSPIX_CONFIGREG(mECSPIBase), regTmp);
}

/**
 * Enables a loopback to test driver functionality.
 * All output will be written to the RXFIFO register.
 * @param enable
 */
void IMX6ECSPI::enableLoopback(bool enable) {
	dword_t regTmp = READ_REGISTER(ECSPIX_TESTREG(mECSPIBase));
	if (enable) {
		regTmp |= (1 << 31);
	} else {
		regTmp &= ~(1 << 31);
	}
#ifdef CONFIG_DEBUG_SPI
	DEBUG_STREAM(TAG, "Writeing ECSPIX_TESTREG: " << hex << regTmp);
#endif
	WRITE_REGISTER(ECSPIX_TESTREG(mECSPIBase), regTmp);
	dumpRegisters();
}

/**
 * Set the predivider of the SPI output clock
 * @param divider Value to devide the 60MHz reference clock with
 */
void IMX6ECSPI::setClockPreDivider(uint16_t divider) {
	if ( divider > SPI_CLOCK_PREDIVIDER_MASK ) {
		return;
	}

	/* Read the current control register */
	dword_t regTmp = READ_REGISTER(ECSPIX_CONREG(mECSPIBase));
#ifdef CONFIG_DEBUG_SPI
	DEBUG_STREAM(TAG,"CONREG before predivider: " << hex << regTmp);
#endif
	regTmp &= ~(SPI_CLOCK_PREDIVIDER_MASK << ECSPI_CONREG_PRE_DIVIDER);
	regTmp |= (divider - 1) << ECSPI_CONREG_PRE_DIVIDER;
#ifdef CONFIG_DEBUG_SPI
	DEBUG_STREAM(TAG,"CONREG after predivider: " << hex << regTmp);
#endif
	/* Write the control register with the new burst value */
	WRITE_REGISTER(ECSPIX_CONREG(mECSPIBase), regTmp);
}

void IMX6ECSPI::dumpRegisters() {
#ifdef CONFIG_DEBUG_SPI
	DEBUG_STREAM(TAG,"Dumping ECSPI registers with base: " << hex << mECSPIBase);
	DEBUG_STREAM(TAG,"CONREG: " << hex << READ_REGISTER(ECSPIX_CONREG(mECSPIBase)));
	DEBUG_STREAM(TAG,"CONFIGREG: " << hex << READ_REGISTER(ECSPIX_CONFIGREG(mECSPIBase)));
	DEBUG_STREAM(TAG,"INTREG: " << hex << READ_REGISTER(ECSPIX_INTREG(mECSPIBase)));
	DEBUG_STREAM(TAG,"DMAREG: " << hex << READ_REGISTER(ECSPIX_DMAREG(mECSPIBase)));
	DEBUG_STREAM(TAG,"STATREG: " << hex << READ_REGISTER(ECSPIX_STATREG(mECSPIBase)));
	DEBUG_STREAM(TAG,"PERIODREG: " << hex << READ_REGISTER(ECSPIX_PERIODREG(mECSPIBase)));
	DEBUG_STREAM(TAG,"TESTREG: " << hex << READ_REGISTER(ECSPIX_TESTREG(mECSPIBase)));
	DEBUG_STREAM(TAG,"MSGDATA: " << hex << READ_REGISTER(ECSPIX_MSGDATA(mECSPIBase)));
#endif
}
MEMBER_OF_OSC_END
