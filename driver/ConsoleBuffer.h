/*
 * ConsoleBuffer.h
 *
 *  Created on: 07.02.2017
 *      Author: hendrik
 */

#ifndef DRIVER_CONSOLEBUFFER_H_
#define DRIVER_CONSOLEBUFFER_H_

#include <component/OSC.h>

#include <sync/ComponentLock.h>

#define CONSOLE_BUFFER_DEP (&(OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance))

BEGIN_DECLARE_OSC(ConsoleBuffer,ConsoleBuffer)

public:
	uint32_t mWritePosition;
	uint32_t mBase = 10;

	uint32_t enterLine(const char *string);
	const char *readLine(uint32_t lineNum);

	ConsoleBuffer& startLine(bool cpuID);
	void endLine();

	void freeLine(uint32_t lineNum);

	static ConsoleBuffer mInstance;

	void putc(const char c);

	// print simple chars
	ConsoleBuffer& operator <<(char c);
	ConsoleBuffer& operator <<(unsigned char c);

	//  print null-terminated char arrays
	ConsoleBuffer& operator <<(const char* string);

	//  Print numbers in format to base
	ConsoleBuffer& operator <<(int16_t);
	ConsoleBuffer& operator <<(int32_t);
	ConsoleBuffer& operator <<(int64_t);
	ConsoleBuffer& operator <<(uint16_t);
	ConsoleBuffer& operator <<(uint32_t);
	ConsoleBuffer& operator <<(uint64_t);

	// Print pointer like a hexadecimal number
	ConsoleBuffer& operator <<(void* ptr);

	// Stream manipulators
	ConsoleBuffer& operator <<(ConsoleBuffer& (*f)(ConsoleBuffer&));

	// End of stream
	uint32_t operator <<(uint32_t (*f)(ConsoleBuffer&));
private:
//	ComponentLock mLock;

	constexpr static int cLineLength = 1024;
	constexpr static int cLineCount = 20;

	uint32_t mWriteLinePos;

	uint32_t mReadPosition;

	char mBuffer[cLineCount][cLineLength];
	bool mLineUsed[cLineCount];
};

// Newline
uint32_t finishOutput(ConsoleBuffer& os);
ConsoleBuffer& endl(ConsoleBuffer& os);

// BIN: switches the base to binary.

ConsoleBuffer& bin (ConsoleBuffer& os);

// OCT: switches the base to octal.

ConsoleBuffer& oct (ConsoleBuffer& os);

// DEC: switches the base to decimal.

ConsoleBuffer& dec (ConsoleBuffer& os);

// HEX: switches the base to hexadecimal.

ConsoleBuffer& hex (ConsoleBuffer& os);


END_DECLARE_OSC

#endif /* DRIVER_CONSOLEBUFFER_H_ */
