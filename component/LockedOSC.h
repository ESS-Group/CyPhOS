/*
 * LockedOSC.h
 *
 *  Created on: 09.04.2015
 *      Author: hendrik
 */

#ifndef COMPONENT_LOCKEDOSC_H_
#define COMPONENT_LOCKEDOSC_H_

#include <common/types.h>
#include <component/OSC.h>

#define LOCKED_OSC_STACK_SIZE 128 // 128*4bytes

#define LOCKED_OSC_PREFIX(NAME) NS_OSC_##NAME##_OSC

#define BEGIN_DECLARE_LOCKED_OSC(NAME,CLASS_NAME) \
		extern void *__NS_OSC_##NAME##_OSC_start; \
		extern void *__NS_OSC_##NAME##_OSC_end; \
		namespace NS_OSC_##NAME##_OSC { \
class CLASS_NAME : public OSC { \
			MICROHEAP_DECLARATION \
			public: \
			void *getOSCStart(); \
			void *getOSCEnd(); \
			Spinlock *getLock();


#define END_DECLARE_LOCKED_OSC }

#define BEGIN_LOCKED_OSC_IMPLEMENTATION(NAME,CLASS_NAME) \
		namespace NS_OSC_##NAME##_OSC{ \
		void *CLASS_NAME::getOSCStart() { \
			return &__NS_OSC_##NAME##_OSC_start; \
		} \
		void *CLASS_NAME::getOSCEnd() { \
			return &__NS_OSC_##NAME##_OSC_end; \
		}

#define END_LOCKED_OSC_IMPLEMENTATION }

#define MEMBER_OF_LOCKED_OSC_BEGIN(NAME) \
		namespace NS_OSC_##NAME##_OSC {

#define MEMBER_OF_LOCKED_OSC_END }

#define INCLUDE_DECLARATION_IN_LOCKED_OSC(OSC_NAME,VAR_TYPE,VAR_NAME) \
		VAR_TYPE LOCKED_OSC_VAR_##OSC_NAME##_LOCKED_OSC_##VAR_NAME

class LockedOSC : public OSC {
public:

private:


};



#endif /* COMPONENT_LOCKEDOSC_H_ */
