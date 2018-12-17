/*
 * DummyStream.h
 *
 *  Created on: 04.05.2015
 *      Author: hendrik
 */

#ifndef DEBUG_DUMMYSTREAM_H_
#define DEBUG_DUMMYSTREAM_H_

#include <debug/DummyStream.h>

class DummyStream
{
public:
    // print simple chars
    DummyStream& operator << (char c) {}
    DummyStream& operator << (unsigned char c) {}

    //  print null-terminated char arrays
    DummyStream& operator << (const char* string) {}

    //  Print numbers in format to base
    DummyStream& operator << (short ival) {}
    DummyStream& operator << (unsigned short ival) {}
    DummyStream& operator << (int ival) {}
    DummyStream& operator << (unsigned int ival) {}
    DummyStream& operator << (long ival) {}
    DummyStream& operator << (unsigned long ival) {}
    DummyStream& operator << (unsigned long long ival) {}

    // Print pointer like a hexadecimal number
    DummyStream& operator << (void* ptr);

    // Stream manipulators
    DummyStream& operator << (DummyStream& (*f) (DummyStream&));
};



#endif /* DEBUG_DUMMYSTREAM_H_ */
