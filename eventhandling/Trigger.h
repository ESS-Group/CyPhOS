/*
 * Trigger.h
 *
 *  Created on: 14.02.2017
 *      Author: hendrik
 */

#ifndef EVENTHANDLING_TRIGGER_H_
#define EVENTHANDLING_TRIGGER_H_

#include <component/OSC.h>

#include <common/QueueElement.h>


/**
 * Declares a trigger inside the class of an OSC.
 * This trigger needs a definition of the trigger itself and the corresponding function
 * later in the source file. Use DEFINE_TRIGGER and DEFINE_TRIGGER_FUNC for this.
 */
#define DECLARE_TRIGGER(NAME) \
	void triggerFunc_##NAME(dword_t arg); \
	static EventHandling::Trigger trigger_##NAME

/**
 * Defines a new trigger. The trigger needs a declaration before defining it.
 * @param CLASS_NAME name of the class the trigger belongs to
 * @param NAME Name of the trigger
 * @param OSC_INSTANCE OSC instance the trigger belongs to
 * @param DEPS Trigger dependencies. This is a nullptr terminated array that includes all OSCs that are necessary to
 * 				run the trigger function
 */
#define DEFINE_TRIGGER(CLASS_NAME,NAME,OSC_INSTANCE,DEPS,PRIORITY) \
	EventHandling::Trigger CLASS_NAME::trigger_##NAME = {(OSC*)&OSC_INSTANCE,(void (OSC::*)(dword_t))&CLASS_NAME::triggerFunc_##NAME, \
				(OSC**)&CLASS_NAME::DEPS,PRIORITY}


/**
 * Defines a new trigger. The trigger needs a declaration before defining it.
 * @param CLASS_NAME name of the class the trigger belongs to
 * @param NAME Name of the trigger
 * @param OSC_INSTANCE OSC instance the trigger belongs to
 * @param DEPS Trigger dependencies. This is a nullptr terminated array that includes all OSCs that are necessary to
 * 				run the trigger function
 */
#define DEFINE_TRIGGER_AFFINITY(CLASS_NAME,NAME,OSC_INSTANCE,DEPS,PRIORITY,AFFINITY) \
	EventHandling::Trigger CLASS_NAME::trigger_##NAME = {(OSC*)&OSC_INSTANCE,(void (OSC::*)(dword_t))&CLASS_NAME::triggerFunc_##NAME, \
				(OSC**)&CLASS_NAME::DEPS,PRIORITY,AFFINITY}


/**
 * Defines a new trigger function. The trigger needs a declaration before defining it.
 * @param CLASS_NAME name of the class the trigger belongs to
 * @param NAME Name of the trigger the function belongs to
 */
#define DEFINE_TRIGGER_FUNC(CLASS_NAME,NAME) \
		void CLASS_NAME::triggerFunc_##NAME(dword_t arg)


namespace EventHandling {

/**
 * Matching class to Events. Triggers are the input of events and handle
 * Events emitted from other components (or the same, but this would not necessarily
 * makes sense).
 *
 * IMPORTANT: One trigger can only be subscribed to one event. If you need one function to react
 * to multiple events create multiple Trigger objects.
 */
class Trigger: public QueueElement<Trigger> {
public:
	constexpr static priority_t cInvalid_Priority = 0x0;
	constexpr static priority_t cMIN_Priority = 0x1;
	constexpr static priority_t cMAX_Priority = 0xFFFFFFFF;

	/**
	 * OSC the trigger belongs to.
	 */
	OSC *pDestinationOSC;

	/**
	 * Member function pointer that handles the event this trigger is subscribed to.
	 * @param
	 */
	void (OSC::*pDestinationFunc)(dword_t);

	/**
	 * Dependencies this trigger needs to handle the event. This ensures data integrity because only one
	 * processor at the time can access these dependencies (serialized access).
	 */
	OSC **pDeps;

	/**
	 * Simple constructor
	 * @param osc
	 * @param func
	 * @param deps
	 */
	Trigger(OSC *osc, void (OSC::*func)(dword_t), OSC **deps, priority_t priority, cpu_affinity_t affinity = 0xFF) : pDestinationOSC(osc), pDestinationFunc(func),
			pDeps(deps), pPriority(priority), pAffinity(affinity){

#ifdef CONFIG_PROFILING
		mSequenceNo = 0;
#endif
	}

	/**
	 * Priority value of the Trigger
	 */
	priority_t pPriority = cMIN_Priority;

	/**
	 * Processor affinity for the event. Only one processor or all are supported at the moment.
	 */
	cpu_affinity_t pAffinity = 0xFF;



#ifdef CONFIG_PROFILING
	/**
	 * Saves the sequence number of the current trigger execution.
	 * Can be used to identify multiple runs
	 */
	uint32_t mSequenceNo;
#endif
};

}

#endif /* EVENTHANDLING_TRIGGER_H_ */
