/*
 * CGAScreen.h
 *
 *  Created on: 25.05.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_DRIVER_CGASCREEN_H_
#define ARCH_X86_DRIVER_CGASCREEN_H_

#include <common/types.h>

#include <common/IOPort.h>

#include <component/OSC.h>

MEMBER_OF_OSC_BEGIN(InputOutput)

class CGAScreen {
public:
	typedef enum {
		BLACK,
		BLUE,
		GREEN,
		CYAN,
		RED,
		MAGENTA,
		BROWN,
		LIGHT_GREY,
		DARK_GREY,
		LIGHT_BLUE,
		LIGHT_GREEN,
		LIGHT_CYAN,
		LIGHT_RED,
		LIGHT_MAGENTA,
		YELLOW,
		WHITE
	} CGAColor_t;

	static constexpr uint8_t cStandardAttribute = BLACK << 4 | LIGHT_GREY;
	static constexpr uint8_t cRows = 25;
	static constexpr uint8_t cColumns = 80;

	static constexpr uintptr_t cCGAStart = 0xb8000;

	void setPosition(int32_t x, int32_t y);
	void getPosition(int32_t& x, int32_t& y);

	void show(int32_t x, int32_t y, char character, uint8_t attrib = cStandardAttribute);
	void print(char* string, int32_t n, uint8_t attrib = cStandardAttribute);

	void scrollup();

	CGAScreen();
	static CGAScreen mInstance;

private:

	IOPort mIndexPort;
	IOPort mDataPort;
};

MEMBER_OF_OSC_END

#endif /* ARCH_X86_DRIVER_CGASCREEN_H_ */
