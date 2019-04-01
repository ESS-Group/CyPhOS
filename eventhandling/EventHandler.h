/*

 * EventHandler.h
 *
 *  Created on: 26.03.2015
 *      Author: hendrik
 */

#ifndef EVENTHANDLING_EVENTHANDLER_H_
#define EVENTHANDLING_EVENTHANDLER_H_

#include <common/Queue.h>
#include <common/cpu.h>

#include <component/OSC.h>

#include <sync/Spinlock.h>
#include <eventhandling/Event.h>


#include <eventhandling/EventTask.h>
#include <eventhandling/TimerEvent.h>
#include <eventhandling/EventPriorityQueue.h>

#include <eventhandling/SchedulingStrategy.h>

#include <eventhandling/Trigger.h>
#ifndef __ASSEMBLY__

/* Prototype for ASM functions */
extern "C" {
/*
 * Called after an event is handled so that the EventHandler can do some cleanup work.
 * Called from ASM so this needs direct c-name mangling.
 */
void finishEventHandler(void *stackPointer);
/**
 * This function is already running in the context of the new OSC. It resolves
 * the C++-FunctionPtr and calls the member function. The return address is set to
 * the dispatcher loop.
 */
void contextSwitchOSC(EventHandling::EventTask *con, dword_t arg);
/**
 * This function activates the dispatcher loop. It is a global c++-function but needs c-name mangling because it is called after
 * the ASM function activateDispatching reset the core.
 */
void startDispatching();
/**
 * This function kills your context. It resets the stack and disables interrupts
 */
[[noreturn]] void activateDispatching();

/**
 * This function will not return. It changes the context to the new stackPointer and then
 * calls the member function of the OSC that got intercepted before.
 */
[[noreturn]] void dispatchOSCMemberEvent_asm(void *stackPointer, EventHandling::EventTask *eventcon, dword_t arg);


void printMemory(void *addr, uint32_t range);

#ifdef CONFIG_PROFILING
/**
 * Checks if a Trigger object is excluded from being profiled.
 * @param trigger Trigger pointer that should be checked for exclusion
 * @return 1 if excluded, 0 if included
 */
int profiling_trigger_included(EventHandling::Trigger *trigger);
#endif
}

namespace EventHandling {

class EventHandler {
public:
	enum STATE {
		PRELOADING,
		PRELOADED,
		WRITEBACK,
		EXECUTING_TRIGGER,
		WAITING,
		DISPATCHING,
		SETTING_UP_OSC_JUMP,
		FINALIZING_JUMP,
		RETURNED_TO_HANDLER,
		WRITEBACK_COMPLETE,
		UNLOCKED_TASK
	};
	EventHandler();

	/**
	 * This is the main event loop. It should only be called from the initialization code of the OS
	 * or after the execution of an event.
	 */
	void dispatching();

	/**
	 * Statistics of total handled events. Not secured for multicore access,
	 * which makes this only a imprecise value.
	 */
	uint32_t getEventCount();

	/**
	 * This function returns a free EventTask for SW events.
	 */
	EventTask *getTask();

	/**
	 * This function returns a free EventTask object for interrupts.
	 */
	EventTask *getInterruptTask();

	/**
	 * Requeues a used EventTask
	 * @param task EventTask object to requeue
	 */
	void requeueEventTask(EventTask *con);

	/**
	 * Requeues a used interrupt EventTask
	 * @param task EventTask object to requeue
	 */
	void requeueInterruptEventTask(EventTask *task);

	/**
	 * Method to finish the execution of a EventTask execution.
	 * @param stackPointer returned by OSC trigger
	 */
	void eventTaskFinished(void *stackPointer);

	/* Statistic functions */

	/* Return the no of pending events ready to execute */
	uint32_t getPendingSWEvents();

	/* Returns the number of available events in the event pool. */
	uint32_t getFreeSWEvents();

	/**
	 * Subscribes an Trigger to the given event
	 *
	 * IMPORTANT: One trigger can only be subscribed to ONE event at a time.
	 * 			  Trying to subscribe to multiple events won't work and will corrupt the data structures
	 *
	 * @param event Event that should be subscribed to
	 * @param trigger Trigger that is subscribing to the Event.
	 */
	void subscribeEvent(Event *event, Trigger *trigger);

	/**
	 * Unsubscribes an Trigger for the given event
	 *
	 * @param event Event that should be unsubscribed from
	 * @param trigger Trigger that is unsubscribing from the Event.
	 */
	void unsubscribeEvent(Event *event, Trigger *trigger);

	/**
	 * Initiates the Event. This will create a micro EventTask for every
	 * subscriber of the event and call their trigger function.
	 * @param event Event that should be emitted
	 * @param arg Argument that is provided to the Trigger handling function
	 */
	void eventTriggered(Event *event, dword_t arg);

	/**
	 * Initiates the Interrupt Event. This will create a micro EventTask for every
	 * subscriber of the event and call their trigger function.
	 * @param event Event that should be emitted
	 * @param arg Argument that is provided to the Trigger handling function
	 */
	void interruptEventTriggered(Event *event, dword_t arg);

	/**
	 * Can be used to call a Trigger of a component directly. No Event is necessary
	 * as the EventTask that will result in this call will get a dummy Event assigned.
	 * @param trigger Trigger that should be called.
	 * @param arg Optional argument that is passed
	 */
	void callOSCTrigger(Trigger *trigger, dword_t arg);

	/**
	 * Returns the currently active EventTask handler for the core calling this method.
	 */
	inline EventTask *getSelf() {
		return pActiveEventTask[getCPUID()];
	}

	void dumpDispatchStatistic();

	/**
	 * Tries to lock the OSC of the task that is triggered. This will also lock the dependencies
	 * @param task EventTask with trigger and dependencies set
	 */
	bool tryTaskLock(EventTask *task);

	/**
	 * Unlocks the OSC that was triggered by the task. This will also unlock the dependencies.
	 * @param task EventTask with trigger and dependencies set
	 */
	void unlockTask(EventTask *task);

	/**
	 * Debug method to print information about currently running OSC EventTask. Can be used
	 * by the crash handler of a specific arch.
	 */
	void printCurrentEventTask();

#ifdef CONFIG_X86_DISCARD_SMI_PROFILING
	void printSMIDiscards();
#endif

	/**
	 * Singleton instance
	 */
	static EventHandler pInstance;
	STATE pCPUState[NR_CPUS];
private:
	/* Size of the static event pool */
	static constexpr uint32_t cEventTaskPoolSize = 128;
	static constexpr uint32_t cInterruptTaskPoolSize = 8;

	/**
	 * Queue of EventTasks
	 */
	Queue<EventTask> pEventTaskPoolQueue;

	/**
	 * Queue that contains all available interrupt tasks
	 */
	Queue<EventTask> pInterruptTaskPoolQueue;

	/**
	 * Lock for the EventTask pool.
	 */
	Spinlock pEventTasksLock;

	/**
	 * Lock for the interrupt EventTask pool.
	 */
	Spinlock pInterruptTasksLock;

	/**
	 * Pointer to selected scheduling strategy
	 */
	SchedulingStrategy *pScheduling;

	/**
	 * Array of currently active EventTask. Needed for context saving.
	 */
	EventTask *pActiveEventTask[NR_CPUS];

	Spinlock pActiveTaskLock;

	/**
	 * Activates an OSC for the specific event.
	 * If the event was preempted before then this will resume its context.
	 */
	[[noreturn]]
	void activateOSC(EventTask *event);

	/**
	 * Pool of EventTask objects that are used to run an trigger of an OSC
	 */
	EventTask pEventTasks[cEventTaskPoolSize];

	/**
	 * Pool of interrupt tasks
	 */
	EventTask pInterruptTasks[cInterruptTaskPoolSize];

	/**
	 * Imprecise total event counter.
	 */
	volatile uint32_t pEventDispatchCounter;

	/**
	 * Per CPU counter of dispatched events
	 */
	uint32_t pCoreDispatchCount[NR_CPUS];

	#ifdef CONFIG_MEASURE_IDLE
	volatile uint64_t pIdleTime[NR_CPUS];

	volatile uint64_t pIdleRefvalue[NR_CPUS];

	static constexpr uint64_t pIdleResetVal = 1000000;
	#endif

	/* Private id value for dispatched events, to guarantee the order */
	uint64_t pLastEventID;

	/**
	 * Enqueues an EventTask into the main event queue. This is NOT safe to call from interrupt context.
	 * context.
	 */
	void readyTask(EventTask *event);

	/**
	 * Dummy event used for trigger only calls.
	 */
	Event pDummyEvent;

	/**
	 * Stack pointers used for component trigger execution. The variable is used for deferred stack pointer
	 * release in dispatching().
	 */
	uintptr_t pStackPointers[NR_CPUS];


};

}


#endif

#endif /* EVENTHANDLING_EVENTHANDLER_H_ */
