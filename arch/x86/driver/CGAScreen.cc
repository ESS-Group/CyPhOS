/*
 * CGAScreen.cc
 *
 *  Created on: 25.05.2018
 *      Author: hendrik
 */

#include <arch/x86/driver/CGAScreen.h>

MEMBER_OF_OSC_BEGIN(InputOutput)

// Singleton instance
CGAScreen CGAScreen::mInstance;

CGAScreen::CGAScreen() : mIndexPort(0x3d4), mDataPort(0x3d5) {
//	setPosition(0,0);
//	print("CyPhOS",6, 15);
//	setPosition(0,1);
//	print("Blabla",6);
//	__asm__ __volatile__ ("	    movl $0x4f524f45, (0xb8000)\n"
//    "movl $0x4f3a4f52, (0xb8004)\n"
//    "movl $0x4f204f20, (0xb8008)\n"
//    "movb %al, (0xb800a)\n"
//    "hlt\n");
}

void CGAScreen::setPosition(int32_t x, int32_t y) {
	int pos;
	int high, low;

	if (x < 0 || x >= cColumns || y < 0 || y > cRows)
		return;

	pos = y * cColumns + x;
	low = pos & 0xff;
	high = (pos & 0x3f00) >> 8;
	mIndexPort.writeByte(14);
	mDataPort.writeByte(high);
	mIndexPort.writeByte(15);
	mDataPort.writeByte(low);
}

void CGAScreen::getPosition(int32_t& x, int32_t& y) {
	int high, low;
	int pos;

	mIndexPort.writeByte(14);
	high = mDataPort.readByte();
	mIndexPort.writeByte(15);
	low = mDataPort.readByte();
	pos = (high & 0x3f) << 8 | (low & 0xff);
	x = pos % cColumns;
	y = pos / cColumns;
}

void CGAScreen::show(int32_t x, int32_t y, char character, uint8_t attrib) {
	int pos;

	if (x < 0 || x >= cColumns || y < 0 || y > cRows)
		return;

	pos = (y * cColumns + x) * 2;
	*(char*) (cCGAStart + pos) = character;
	*(unsigned char*) (cCGAStart + pos + 1) = attrib;

}

void CGAScreen::print(char* string, int n, unsigned char attrib) {
	int x, y;
	char* pos;

	getPosition(x, y);
	pos = (char*) cCGAStart + (y * cColumns + x) * 2;

	while (n) {
		switch (*string) {
		case '\n':
			x = 0;
			y++;
			pos = (char*) cCGAStart + (y * cColumns + x) * 2;
			break;
		case '\b':
			if (x > 0) {
				x--;
			}
			break;
		default:
			*pos = *string;
			*(unsigned char*) (pos + 1) = attrib;
			pos += 2;
			x++;
			if (x >= cColumns) {
				x = 0;
				y++;
				pos = (char*) cCGAStart + (y * cColumns + x) * 2;
			}
			break;
		}
		string++;
		if (y >= cRows) {
			scrollup();
			y--;
			pos = (char*) cCGAStart + (y * cColumns + x) * 2;
		}
		n--;
	}
	setPosition(x, y);
}

void CGAScreen::scrollup() {
	long *dstptr = (long*) cCGAStart;
	long *srcptr = (long*) (cCGAStart + cColumns * 2);
	int counter;

	counter = ((cRows - 1) * cColumns * 2) / sizeof(long);

	while (counter > 0) {
		*(dstptr++) = *(srcptr++);
		counter--;
	}

	counter = (cColumns * 2) / sizeof(long);
	while (counter > 0) {
		*(dstptr++) = ((((cStandardAttribute << 8) | ' ') << 8) | cStandardAttribute) << 8 | ' ';
		counter--;
	}
}

unsigned char attribute(CGAScreen::CGAColor_t bg, CGAScreen::CGAColor_t fg, bool blink) {
	return (blink ? 0x80 : 0) | ((bg & 0x7) << 4) | (fg & 0xf);
}

MEMBER_OF_OSC_END

