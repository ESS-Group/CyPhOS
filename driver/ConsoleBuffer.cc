/*
 * ConsoleBuffer.cc
 *
 *  Created on: 07.02.2017
 *      Author: hendrik
 */

#include <driver/ConsoleBuffer.h>

BEGIN_OSC_IMPLEMENTATION(ConsoleBuffer,ConsoleBuffer)

ConsoleBuffer ConsoleBuffer::mInstance;

uint32_t ConsoleBuffer::enterLine(const char *string) {
//	mLock.lock();

	// Reserve a line
	uint32_t lineNum = (mWritePosition + 1) % cLineCount;
	while (mLineUsed[lineNum] && (lineNum != mWritePosition)) {
		lineNum = (lineNum + 1) % cLineCount;
	}
	mLineUsed[lineNum] = true;
//	mLock.unlock();
	uint32_t linePos = 0;

	// Copy the string to the line buffer and cut of if to long
	while (*string != '\0' && linePos < (cLineLength - 1)) {
		mBuffer[lineNum][linePos] = *string;
		string++;
		linePos++;
	}
	// Forced null termination
	mBuffer[lineNum][linePos] = '\0';

	mWritePosition = lineNum;

	return lineNum;
}

const char *ConsoleBuffer::readLine(uint32_t lineNum) {
	const char *retVal;
//	mLock.lock();
	if (mLineUsed[lineNum]) {
		retVal = mBuffer[lineNum];
	} else {
		retVal = (const char*) "WRONG LINE";
	}
//	mLock.unlock();
	return retVal;
}

void ConsoleBuffer::freeLine(uint32_t lineNum) {
//	mLock.lock();
	mLineUsed[lineNum] = false;
//	mLock.unlock();
}

ConsoleBuffer& ConsoleBuffer::startLine(bool cpuID) {
//	mLock.lock();

	// Reserve a line
	uint32_t lineNum = (mWritePosition + 1) % cLineCount;
	while (mLineUsed[lineNum] && (lineNum != mWritePosition)) {
		lineNum = (lineNum + 1) % cLineCount;
	}
	mLineUsed[lineNum] = true;
	mWriteLinePos = 0;

	mWritePosition = lineNum;

#ifdef CONFIG_FORCE_CPUID_OUTPUT
	if (cpuID) {
		*this << hex << (uint16_t)getCPUID() << ':';
	}
#endif

	return *this;
}

void ConsoleBuffer::endLine() {
	mBuffer[mWritePosition][mWriteLinePos] = '\0';
//	mLock.unlock();
}

void ConsoleBuffer::putc(const char c) {
	if (mWriteLinePos < (cLineLength - 1)) {
		mBuffer[mWritePosition][mWriteLinePos] = c;
		mWriteLinePos++;
	}
#ifdef CONFIG_FORCE_CPUID_OUTPUT
	if(c == '\n') {
		*this << hex << (uint16_t)getCPUID() << ':';
	}
#endif
}

// Output methods
ConsoleBuffer& ConsoleBuffer::operator <<(char c) {
	putc(c);
	return *this;
}

ConsoleBuffer& ConsoleBuffer::operator <<(unsigned char c) {
	return *this << (char) c;
}

// Print a memory address in hexadecimal format
ConsoleBuffer& ConsoleBuffer::operator << (void* ptr)
 {
   int oldbase = mBase;
   mBase = 16;
   *this << (uintptr_t) ptr;
   mBase = oldbase;
   return *this;
 }


ConsoleBuffer& ConsoleBuffer::operator <<(int16_t ival) {
	return *this << (int64_t) ival;
}

ConsoleBuffer& ConsoleBuffer::operator <<(int32_t ival) {
	return *this << (int64_t) ival;
}

// Print signed number with "-" if negative
ConsoleBuffer& ConsoleBuffer::operator <<(int64_t ival) {
	if (ival < 0) {
		putc('-');
		ival = -ival;
	}
	// print the remaining number
	return *this << (uint64_t) ival;
}

ConsoleBuffer& ConsoleBuffer::operator <<(uint16_t ival) {
	return *this << (uint64_t) ival;
}

ConsoleBuffer& ConsoleBuffer::operator <<(uint32_t ival) {
	return *this << (uint64_t) ival;
}

ConsoleBuffer& ConsoleBuffer::operator <<(uint64_t ival) {
	uint64_t div;
	char digit;

	if (mBase == 8)
		putc('0');         // octal numbers get a zero
	else if (mBase == 16) {
		putc('0');         // hexadecimal numbers get a zero and a x
		putc('x');
	}

	// Calculate the biggest exponent of the selected number base which is smaller
	// than the number to print.
	for (div = 1; ival / div >= (uint64_t) mBase; div *= mBase)
		;

	// print digit-wise the number
	for (; div > 0; div /= (uint64_t) mBase) {
		digit = ival / div;
		if (digit < 10)
			putc('0' + digit);
		else
			putc('a' + digit - 10);
		ival %= div;
	}
	return *this;
}

// Call the manipulator functions
uint32_t ConsoleBuffer::operator <<(uint32_t (*f)(ConsoleBuffer&)) {
	return f(*this);
}

ConsoleBuffer& ConsoleBuffer::operator << (ConsoleBuffer& (*f) (ConsoleBuffer&))
 {
   return f(*this);
 }

ConsoleBuffer& ConsoleBuffer::operator <<(const char *string) {
	while (*string) {
		putc((*string++));
	}
	return *this;
}

// BIN: switches the base to binary.
ConsoleBuffer& bin(ConsoleBuffer& os) {
	os.mBase = 2;
	return os;
}

// OCT: switches the base to octal.
ConsoleBuffer& oct(ConsoleBuffer& os) {
	os.mBase = 8;
	return os;
}

// DEC: switches the base to decimal.
ConsoleBuffer& dec(ConsoleBuffer& os) {
	os.mBase = 10;
	return os;
}

// HEX: switches the base to hexadecimal.
ConsoleBuffer& hex(ConsoleBuffer& os) {
	os.mBase = 16;
	return os;
}

// Newline
uint32_t finishOutput(ConsoleBuffer& os) {
	uint32_t lineNum = os.mWritePosition;
	os.endLine();
	return lineNum;
}

ConsoleBuffer& endl(ConsoleBuffer& os) {
	os << '\n';
	return os;
}

END_OSC_IMPLEMENTATION
