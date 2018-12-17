/*
 * IMX6GPIO.cc
 *
 *  Created on: 05.12.2016
 *      Author: hendrik
 */

#include <common/memreg.h>

#include <interrupts/InterruptDispatcher.h>

#include "IMX6GPIO.h"

#include <driver/IMX6GPIO.h>

#include <common/SyncOutput.h>

/* Include GIC driver for interrupt control */
#include <driver/ARMGIC.h>

#include <common/debug.h>


#define TAG "IMX6GPIO"

BEGIN_OSC_IMPLEMENTATION(GPIO, IMX6GPIO)

SECTION_CRITICAL_DATA OSC* IMX6GPIO::trigger_Interrupt_Deps[] = { nullptr };
SECTION_CRITICAL_DATA EventHandling::Trigger IMX6GPIO::trigger_Interrupt((OSC*) &IMX6GPIO::mInstance, (void (OSC::*)(dword_t))&IMX6GPIO::handleInputInterrupt, (OSC**)&IMX6GPIO::trigger_Interrupt_Deps,EventHandling::Trigger::cMIN_Priority + 200);

SECTION_CRITICAL_DATA EventHandling::Event IMX6GPIO::EventGas;
SECTION_CRITICAL_DATA EventHandling::Event IMX6GPIO::EventBreak;

#ifdef CONFIG_IMX6_CAN_INTERRUPT
SECTION_CRITICAL_DATA EventHandling::Event IMX6GPIO::EventCAN;
#endif

IMX6GPIO IMX6GPIO::mInstance;

IMX6GPIO::IMX6GPIO(){

	configurePorts();
#ifdef CONFIG_IMX6_GPIO
	InterruptDispatcher::pInstance.registerEvent(HW_INT_GPIO4_0_15, &trigger_Interrupt);
#endif

#ifdef CONFIG_IMX6_CAN_INTERRUPT
	InterruptDispatcher::pInstance.registerEvent(HW_INT_GPIO3_16_31, &trigger_Interrupt);
#endif
}

/**
 * Configures the GPIO Ports
 */
void IMX6GPIO::configurePorts() {
#ifdef CONFIG_IMX6_GPIO
	/* Set GPIO1_IO05 */
	dword_t regTmp = READ_REGISTER(IOMUXC_SW_MUX_CTL_PAD_GPIO05);
	regTmp &= ~(0x7);
	regTmp |= (0x5);
	WRITE_REGISTER(IOMUXC_SW_MUX_CTL_PAD_GPIO05, regTmp);

	/* Set GPIO7_IO11 */
	regTmp = READ_REGISTER(IOMUXC_SW_MUX_CTL_PAD_GPIO16);
	regTmp &= ~(0x7);
	regTmp |= (0x5);
	WRITE_REGISTER(IOMUXC_SW_MUX_CTL_PAD_GPIO16, regTmp);

	/* Set GPIO4_IO12 */
	regTmp = READ_REGISTER(IOMUXC_SW_MUX_CTL_PAD_KEY_COL3);
	regTmp &= ~(0x7);
	regTmp |= (0x5);
	WRITE_REGISTER(IOMUXC_SW_MUX_CTL_PAD_KEY_COL3, regTmp);

	/* Set GPIO4_IO13 */
	regTmp = READ_REGISTER(IOMUXC_SW_MUX_CTL_PAD_KEY_ROW3);
	regTmp &= ~(0x7);
	regTmp |= (0x5);
	WRITE_REGISTER(IOMUXC_SW_MUX_CTL_PAD_KEY_ROW3, regTmp);

	/*
	 * Configure IOMUXC control registers
	*/
	dword_t inputPortValue = 0x18041;

	WRITE_REGISTER(IOMUXC_SW_PAD_CTL_PAD_GPIO05, inputPortValue);
	WRITE_REGISTER(IOMUXC_SW_PAD_CTL_PAD_GPIO16, inputPortValue);
	WRITE_REGISTER(IOMUXC_SW_PAD_CTL_PAD_KEY_COL3, inputPortValue);
	WRITE_REGISTER(IOMUXC_SW_PAD_CTL_PAD_KEY_ROW3, inputPortValue);

	/* Configure GPIO direction registers as input*/
	regTmp = READ_REGISTER(GPIO1_GDIR);
	/*reset bit 5*/
	regTmp &= ~(1 << 5);
	WRITE_REGISTER(GPIO1_GDIR, regTmp);

	regTmp = READ_REGISTER(GPIO7_GDIR);
	/*reset bit 11*/
	regTmp &= ~(1 << 11);
	WRITE_REGISTER(GPIO7_GDIR, regTmp);

	regTmp = READ_REGISTER(GPIO4_GDIR);
	/*reset bit 12 and 13*/
	regTmp &= ~(3 << 12);
	WRITE_REGISTER(GPIO4_GDIR, regTmp);
#endif

#ifdef CONFIG_IMX6_CAN_INTERRUPT
	// Configure PAD EIM_DATA27 to GPIO3_IO27
	dword_t regTmp = READ_REGISTER(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA27);
	regTmp &= ~(0x7);
	regTmp |= (0x5);
	WRITE_REGISTER(IOMUXC_SW_MUX_CTL_PAD_EIM_DATA27, regTmp);

	/*
	 * Configure IOMUXC control registers
	*/
	dword_t inputPortValue = 0x18041;

	WRITE_REGISTER(IOMUXC_SW_PAD_CTL_PAD_EIM_DATA27, inputPortValue);
#endif
}

void IMX6GPIO::configureInterrupts() {
#ifdef CONFIG_IMX6_GPIO
	/* set ICR12 and ICR13 to rising edge*/
	dword_t regTmp = READ_REGISTER(GPIO4_ICR1);
	/*reset bit 27-24 */
	regTmp &= ~(0xf << 24);
	regTmp |= 0xa << 24;
	//regTmp |= 0x5 << 24;
	WRITE_REGISTER(GPIO4_ICR1, regTmp);

	/*enable GPIO12 and GPIO13 interrupts*/
	regTmp = READ_REGISTER(GPIO4_IMR);
	regTmp |= 3 << 12;
	//regTmp |= 1 << 12;
	WRITE_REGISTER(GPIO4_IMR, regTmp);

	/*clear ISR*/
	regTmp = READ_REGISTER(GPIO4_ISR);
	/*set bit 12 and 13*/
	regTmp |= (3 << 12);
	WRITE_REGISTER(GPIO4_ISR, regTmp);

//	/* set ICR5 to rising edge*/
//	dword_t regTmp = READ_REGISTER(GPIO1_ICR1);
//	regTmp &= ~(0x3 << 10);
//	regTmp |= 0x2 << 10;
//	WRITE_REGISTER(GPIO1_ICR1, regTmp);
//
//	/*enable GPIO5 interrupts*/
//	regTmp = READ_REGISTER(GPIO1_IMR);
//	regTmp |= 1 << 5;
//	WRITE_REGISTER(GPIO1_IMR, regTmp);


//	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.setInterruptTarget(HW_INT_GPIO1_INT5, 0xF);
//	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(HW_INT_GPIO1_INT5);
//	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.configureInterruptHandling(HW_INT_GPIO1_INT5, true);
//
//	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.clearPending(HW_INT_GPIO1_INT5);
//	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(HW_INT_GPIO1_INT5);

	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.setInterruptTarget(HW_INT_GPIO4_0_15, 0xF);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(HW_INT_GPIO4_0_15);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.configureInterruptHandling(HW_INT_GPIO4_0_15, true);

	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.clearPending(HW_INT_GPIO4_0_15);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(HW_INT_GPIO4_0_15);

#endif

#ifdef CONFIG_IMX6_CAN_INTERRUPT
	/* set ICR27 to falling edge (MCP2515 documentation p. 49)*/
	dword_t regTmp = READ_REGISTER(GPIO3_ICR2);
	/*reset bit 23-22 */
//	regTmp &= ~(0x3 << 22);
	regTmp |= 0x3 << 22;
	WRITE_REGISTER(GPIO3_ICR2, regTmp);

	/*enable GPIO3_27 interrupts*/
	regTmp = READ_REGISTER(GPIO3_IMR);
	regTmp |= 3 << 27;
	WRITE_REGISTER(GPIO3_IMR, regTmp);

	/*clear ISR*/
	regTmp = READ_REGISTER(GPIO3_ISR);
	/*set bit 12 and 13*/
	regTmp |= (3 << 27);
	WRITE_REGISTER(GPIO3_ISR, regTmp);

	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.setInterruptTarget(HW_INT_GPIO3_16_31, 0xF);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(HW_INT_GPIO3_16_31);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.configureInterruptHandling(HW_INT_GPIO3_16_31, true);

	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.clearPending(HW_INT_GPIO3_16_31);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(HW_INT_GPIO3_16_31);
#endif
}

uint8_t IMX6GPIO::readGPIO(dword_t port, uint8_t pin){
	if(port != GPIO1_DR && port != GPIO4_DR && port != GPIO7_DR && port != GPIO3_DR){
		return GPIO_WRONG_PARAMETER;
	}
	if(pin > 31){
		return GPIO_WRONG_PARAMETER;
	}
	return (READ_REGISTER(port) & (1 << pin)) >> pin;
}

void IMX6GPIO::handleInputInterrupt(){
#if 0
	dword_t regTmp = READ_REGISTER(GPIO4_ISR);
	if((regTmp & (1 << BREAK_SIGNAL1_PIN) )== 0){
		//SYNC_OUTPUT_STREAM(TAG, "Bremse" << endl);
		EventHandling::EventHandler::pInstance.eventTriggered(&EventBreak, readGPIO(BREAK_SIGNAL2_PORT,BREAK_SIGNAL2_PIN));
	}
	if((regTmp & (1 << ACCELERATE_SIGNAL1_PIN) )== 0){
		//SYNC_OUTPUT_STREAM(TAG, "Gas" << endl);
		EventHandling::EventHandler::pInstance.eventTriggered(&EventGas, readGPIO(ACCELERATE_SIGNAL2_PORT,ACCELERATE_SIGNAL2_PIN));
	}
	/*reset bit 12 and 13*/
	regTmp |= (3 << 12);
	WRITE_REGISTER(GPIO4_ISR, regTmp);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.clearPending(HW_INT_GPIO4_0_15);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(HW_INT_GPIO4_0_15);
#endif

#ifdef CONFIG_IMX6_CAN_INTERRUPT
	dword_t regTmp = READ_REGISTER(GPIO3_ISR);

	/*reset bit 27 */
	regTmp |= (3 << 27);
	WRITE_REGISTER(GPIO3_ISR, regTmp);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.clearPending(HW_INT_GPIO3_16_31);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(HW_INT_GPIO3_16_31);

	EventHandling::EventHandler::pInstance.eventTriggered(&EventCAN, 0);
#endif

}
END_OSC_IMPLEMENTATION
