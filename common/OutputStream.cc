/*
 * DebugStream.cc
 *
 *  Created on: 04.05.2015
 *      Author: hendrik
 */


#include "OutputStream.h"

OutputStream::OutputStream() {
	mBase = 10;
	mBufferPosition = 0;
}

void OutputStream::flush() {
	writeToDevice();
	mBufferPosition = 0;
}

void OutputStream::putc(char c) {
	mCharBuffer[mBufferPosition++] = c;
	if (mBufferPosition == DEBUG_BUFFER_SIZE || (c == '\n')) {
		flush();
		mBufferPosition = 0;
	}
}

// Output methods
OutputStream& OutputStream::operator <<(char c) {
	putc(c);
	return *this;
}

OutputStream& OutputStream::operator <<(unsigned char c) {
	return *this << (char) c;
}

OutputStream& OutputStream::operator <<(const char *string) {
	while (*string) {
		putc(*(string++));
	}
	return *this;
}



OutputStream& OutputStream::operator <<(int16_t ival) {
	return *this << (int64_t) ival;
}

OutputStream& OutputStream::operator <<(int32_t ival) {
	return *this << (int64_t) ival;
}

// Print signed number with "-" if negative
OutputStream& OutputStream::operator <<(int64_t ival) {
	if (ival < 0) {
		putc('-');
		ival = -ival;
	}
	// print the remaining number
	return *this << (uint64_t) ival;
}


OutputStream& OutputStream::operator <<(uint16_t ival) {
	return *this << (uint64_t) ival;
}

OutputStream& OutputStream::operator <<(uint32_t ival) {
	return *this << (uint64_t) ival;
}

OutputStream& OutputStream::operator << (uint64_t ival) {
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

// Print a memory address in hexadecimal format
OutputStream& OutputStream::operator << (void* ptr)
 {
   int oldbase = mBase;
   mBase = 16;
   *this << (uintptr_t) ptr;
   mBase = oldbase;
   return *this;
 }

// Call the manipulator functions
OutputStream& OutputStream::operator << (OutputStream& (*f) (OutputStream&))
 {
   return f(*this);
 }

// Newline
OutputStream& endl(OutputStream& os) {
	os << '\n';
	return os;
}

// BIN: switches the base to binary.
OutputStream& bin(OutputStream& os) {
	os.mBase = 2;
	return os;
}

// OCT: switches the base to octal.
OutputStream& oct(OutputStream& os) {
	os.mBase = 8;
	return os;
}

// DEC: switches the base to decimal.
OutputStream& dec(OutputStream& os) {
	os.mBase = 10;
	return os;
}

// HEX: switches the base to hexadecimal.
OutputStream& hex(OutputStream& os) {
	os.mBase = 16;
	return os;
}

// Flushes the buffer
OutputStream& flush(OutputStream& os) {
	os.flush();
	return os;
}
