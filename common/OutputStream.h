/*
 * DebugStream.h
 *
 *  Created on: 04.05.2015
 *      Author: hendrik
 */

#ifndef DEBUG_OUTPUTSTREAM_H_
#define DEBUG_OUTPUTSTREAM_H_

#include <common/types.h>


class OutputStream {

public:
	OutputStream();

	uint8_t mBase;

    // print simple chars
    OutputStream& operator << (char c);
    OutputStream& operator << (unsigned char c);

    //  print null-terminated char arrays
    OutputStream& operator << (const char* string);

    //  Print numbers in format to base
//    DebugStream& operator << (int8_t);
	OutputStream& operator << (int16_t);
	OutputStream& operator << (int32_t);
	OutputStream& operator << (int64_t);
//    DebugStream& operator << (uint8_t);
    OutputStream& operator << (uint16_t);
    OutputStream& operator << (uint32_t);
    OutputStream& operator << (uint64_t);

    // Print pointer like a hexadecimal number
    OutputStream& operator << (void* ptr);

    // Stream manipulators
    OutputStream& operator << (OutputStream& (*f) (OutputStream&));



	void flush();
protected:
	static constexpr uint32_t cBUFFER_SIZE = 256;
	char mCharBuffer[cBUFFER_SIZE];
	uint32_t mBufferPosition;


	void putc(char c);

	~OutputStream(){};

	virtual void writeToDevice() = 0;

private:
	OutputStream(const OutputStream &copy); // prevent copies
};

// Newline
OutputStream& endl (OutputStream& os);

// BIN: switches the base to binary.

OutputStream& bin (OutputStream& os);

// OCT: switches the base to octal.

OutputStream& oct (OutputStream& os);

// DEC: switches the base to decimal.

OutputStream& dec (OutputStream& os);

// HEX: switches the base to hexadecimal.

OutputStream& hex (OutputStream& os);

// flush the buffer
OutputStream& flush (OutputStream& os);

#endif /* DEBUG_OUTPUTSTREAM_H_ */
