/*
 * InputOutputAggregator.h
 *
 *  Created on: 19.06.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_DRIVER_INPUTOUTPUTAGGREGATOR_H_
#define ARCH_X86_DRIVER_INPUTOUTPUTAGGREGATOR_H_

#include <driver/GenericIO.h>

MEMBER_OF_OSC_BEGIN(InputOutput)


class InputOutputAggregator : public GenericIO {
	friend class UART;
	friend class LegacyKeyboard;
public:
	InputOutputAggregator();

	void putc(char c);
	void handleInputInterrupt(dword_t arg);
private:
	static InputOutputAggregator pInstance;
};

MEMBER_OF_OSC_END

#endif /* ARCH_X86_DRIVER_INPUTOUTPUTAGGREGATOR_H_ */
