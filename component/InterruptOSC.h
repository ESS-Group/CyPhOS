/*
 * InterruptOSC.h
 *
 *  Created on: 02.03.2016
 *      Author: hendrik
 */

#ifndef COMPONENT_INTERRUPTOSC_H_
#define COMPONENT_INTERRUPTOSC_H_

#include <component/OSC.h>

#define BEGIN_DECLARE_INTERRUPTOSC(NAME,CLASS_NAME) \
		extern void *__NS_OSC_##NAME##_OSC_start; \
		extern void *__NS_OSC_##NAME##_OSC_text_end; \
		extern void *__NS_OSC_##NAME##_OSC_end; \
		namespace NS_OSC_##NAME##_OSC { \
class CLASS_NAME : public InterruptOSC { \
			public: \
			void *getOSCStart(); \
			void *getOSCTextEnd(); \
			void *getOSCEnd(); \
			Spinlock *getLock();

#define BEGIN_DECLARE_INTERRUPTOSC_INHERIT(NAME,CLASS_NAME,INHERITANCE) \
		extern void *__NS_OSC_##NAME##_OSC_start; \
		extern void *__NS_OSC_##NAME##_OSC_text_end; \
		extern void *__NS_OSC_##NAME##_OSC_end; \
		namespace NS_OSC_##NAME##_OSC { \
class CLASS_NAME : public InterruptOSC, INHERITANCE { \
			public: \
			void *getOSCStart(); \
			void *getOSCTextEnd(); \
			void *getOSCEnd(); \
			Spinlock *getLock();



class InterruptOSC: public OSC {

public:
	virtual void handleInterrupt(dword_t arg) = 0;
};

#endif /* COMPONENT_INTERRUPTOSC_H_ */
