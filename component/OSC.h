/*
 * OSC.h
 *
 *  Created on: 20.03.2015
 *      Author: hendrik
 */

#ifndef COMPONENT_OSC_H_
#define COMPONENT_OSC_H_

/* Protect the header against asm include */
#ifndef __ASSEMBLY__

#include <common/QueueElement.h>

#include <sync/Spinlock.h>

#include <common/Queue.h>

#include <common/types.h>


#define BEGIN_DECLARE_OSC(NAME,CLASS_NAME) \
		extern void *__NS_OSC_##NAME##_OSC_start; \
		extern void *__NS_OSC_##NAME##_OSC_text_end; \
		extern void *__NS_OSC_##NAME##_OSC_end; \
		namespace NS_OSC_##NAME##_OSC { \
class CLASS_NAME : public OSC { \
			public: \
			void *getOSCStart(); \
			void *getOSCTextEnd(); \
			void *getOSCEnd(); \
			Spinlock *getLock();


#define BEGIN_DECLARE_OSC_INHERIT(NAME,CLASS_NAME,INHERITANCE) \
		extern void *__NS_OSC_##NAME##_OSC_start; \
		extern void *__NS_OSC_##NAME##_OSC_text_end; \
		extern void *__NS_OSC_##NAME##_OSC_end; \
		namespace NS_OSC_##NAME##_OSC { \
class CLASS_NAME : public OSC, INHERITANCE { \
			public: \
			void *getOSCStart(); \
			void *getOSCTextEnd(); \
			void *getOSCEnd(); \
			Spinlock *getLock();

#define OSC_FUNC_T(FUNC_NAME) void (OSC::*FUNC_NAME)(dword_t)

#define DECLARE_OSC_TRIGGER(NAME) \
		static Trigger TRIGGER_##NAME; \
		static void FUNC_##NAME();

#define TRIGGER_FUNC(NAME) FUNC_##NAME


#define END_DECLARE_OSC }

//__NS_OSC_###NAME###_OSC_end describes the end of compiled code, but we want to cache the complete component
#define BEGIN_OSC_IMPLEMENTATION(NAME,CLASS_NAME) \
		namespace NS_OSC_##NAME##_OSC{ \
		SECTION_CRITICAL_DATA Spinlock NS_OSC_##NAME##_LOCK; \
		void *CLASS_NAME::getOSCStart() { \
			return &__NS_OSC_##NAME##_OSC_start; \
		} \
		void *CLASS_NAME::getOSCTextEnd() { \
			return &__NS_OSC_##NAME##_OSC_text_end; \
		} \
		void *CLASS_NAME::getOSCEnd() { \
			return &__NS_OSC_##NAME##_OSC_end; \
		} \
		Spinlock *CLASS_NAME::getLock() { \
			return &NS_OSC_##NAME##_LOCK; \
		}

#define BEGIN_OSC_IMPLEMENTATION_SECONDARY(NAME,CLASS_NAME) \
		namespace NS_OSC_##NAME##_OSC{ \
		extern Spinlock NS_OSC_##NAME##_LOCK; \
		void *CLASS_NAME::getOSCStart() { \
			return &__NS_OSC_##NAME##_OSC_start; \
		} \
		void *CLASS_NAME::getOSCTextEnd() { \
			return &__NS_OSC_##NAME##_OSC_text_end; \
		} \
		void *CLASS_NAME::getOSCEnd() { \
			return &__NS_OSC_##NAME##_OSC_end; \
		} \
		Spinlock *CLASS_NAME::getLock() { \
			return &NS_OSC_##NAME##_LOCK; \
		}

#define END_OSC_IMPLEMENTATION }

#define MEMBER_OF_OSC_BEGIN(NAME) \
		namespace NS_OSC_##NAME##_OSC {

#define MEMBER_OF_OSC_END }

#define OSC_PREFIX(NAME) NS_OSC_##NAME##_OSC

#define SECTION_CRITICAL_DATA __attribute__ ((section (".critical_section")))

// Forward declaration
namespace EventHandling {
	class EventHandler;
}

namespace CacheManagement {
	class GenericCacheManagement;
}

class OSC : public QueueElement<OSC> {
	friend class EventHandling::EventHandler;
	friend class CacheManagement::GenericCacheManagement;
	friend class MemoryAllocator;
public:

	OSC()  {
	}


	virtual void *getOSCStart() = 0;
	virtual void *getOSCTextEnd() = 0;
	virtual void *getOSCEnd() = 0;

	virtual Spinlock *getLock() = 0;




private:

protected:

};

#include <common/types.h>


#endif

#ifdef __ASSEMBLY__
#define OSC_FUNC_ASM(osc_name,func_name) \
	.global NS_OSC_##osc_name##_OSC_##func_name## ; \
	NS_OSC_##osc_name##_OSC_##func_name##:

#define OSC_FUNC_ASM_SYM(osc_name,func_name) \
	NS_OSC_##osc_name##_OSC_##func_name
#else
#define OSC_FUNC_ASM(osc_name,func_name) \
	NS_OSC_##osc_name##_OSC_##func_name
#endif



#endif /* COMPONENT_OSC_H_ */
