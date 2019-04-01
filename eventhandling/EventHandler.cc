/*
 * EventHandler.cc
 *
 *  Created on: 26.03.2015
 *      Author: hendrik
 */
#include <eventhandling/EventHandler.h>

/* Debugging */
#include <common/debug.h>
#define TAG "EVENTHANDLER"

/* Include arm helper (for interrupt bit) */
#include <common/cpu.h>

#include <common/QueueElement.h>

/* Include cache management for prefetching */
#include <cachemanagement/GenericCacheManagement.h>

#include "StackPool.h"

#include <common/SyncOutput.h>

#ifdef CONFIG_SCHEDULER_MULTI_FIFO
#include <eventhandling/MultiFifoStrategy.h>
MultiFifoStrategy MultiFifoStrategy::pInstance;
#endif

#ifdef CONFIG_SCHEDULER_SINGLE_FIFO
#include <eventhandling/FifoStrategy.h>
FifoStrategy FifoStrategy::pInstance;
#endif

#ifdef CONFIG_SCHEDULER_PRIORITY_MULTI
#include <eventhandling/MultiQueuePriorityStrategy.h>
MultiQueuePriorityStrategy MultiQueuePriorityStrategy::pInstance;
#endif

#ifdef CONFIG_SCHEDULER_PRIORITY
#include <eventhandling/SingleQueuePriorityStrategy.h>
SingleQueuePriorityStrategy SingleQueuePriorityStrategy::pInstance;
#endif

#include <driver/GenericTimer.h>

/* Include the SystemMode class for entering/leaving the mode */
#include <core/SystemMode.h>

#ifdef CONFIG_PROFILING
#include <benchmarking/ResultContainer.h>

#include <common/cyclecounter.h>
#endif

#include <driver/GenericMMU.h>

#ifdef CONFIG_X86_DISCARD_SMI_PROFILING
#include <common/msr.h>
#endif

#ifdef CONFIG_PREEMPT
/**
 * Preemption currently is not implemented with the new locking Event/Trigger
 * system
 */
#error "Preemption currently not correctly implemented"
#endif

#ifdef CONFIG_PROFILING

/**
 * Define Trigger objects that are included for profiling.
 */
static EventHandling::Trigger* profiling_included_triggers[] = {
	//&OSC_PREFIX(Timer)::GenericTimer::trigger_Interrupt, // Example use. You really should not include the timer interrupt, as it is called very often
	// IMPORTANT, nullptr terminate the array
	nullptr
};

#if defined(CONFIG_PROFILING) & defined(CONFIG_X86_DISCARD_SMI_PROFILING)
	/**
	 * Saves SMI Count before profiling to compare
	 */
	static uint32_t sSMIDiscards[NR_CPUS];
#if defined(CONFIG_PROFILING_EXECUTION) | defined(CONFIG_PROFILING_PRELOAD) | defined(CONFIG_PROFILING_PRELOAD_OVERHEAD) | defined(CONFIG_PROFILING_WRITEBACK)
	static uint32_t sSMICounts[NR_CPUS];
#endif
#endif


#ifdef CONFIG_PROFILING_EXECUTION
/**
 * Cycle counter value before the execution of a trigger function
 */
static cycle_t sTimerStartValue[NR_CPUS];
#endif
#endif


void startDispatching() {
	EventHandling::EventHandler::pInstance.dispatching();
}

void finishEventHandler(void *stackPointer) {
	EventHandling::EventHandler::pInstance.eventTaskFinished(stackPointer);
}

void printMemory(void *addr, uint32_t range) {
	printMemoryDescending(addr, range);
}

void contextSwitchOSC(EventHandling::EventTask *con, dword_t arg) {
	cpu_t cpuID = getCPUID();
	EventHandling::EventHandler::pInstance.pCPUState[cpuID] = EventHandling::EventHandler::FINALIZING_JUMP;
	void (OSC::*func)(dword_t) = con->trigger->pDestinationFunc;

#ifdef CONFIG_DISPATCHER_DEBUG
	DEBUG_STREAM(TAG,"Activating OSC:" << (void*)con->trigger->pDestinationOSC << "with argument: " << hex << arg);
	DEBUG_STREAM(TAG, "contextSwitchOSC Eventtask: " << con);
#endif

#ifdef CONFIG_PROFILING_EXECUTION
	if (profiling_trigger_included(con->trigger)) {

#ifdef CONFIG_X86_DISCARD_SMI_PROFILING
		// Save SMI count to compare later
		sSMICounts[cpuID] = readMSR(COMMON_MSR_SMI_COUNT);
#endif
		RESET_READ_CYCLE_COUNTER(sTimerStartValue[cpuID]);
	}
#endif
	EventHandling::EventHandler::pInstance.pCPUState[cpuID] = EventHandling::EventHandler::EXECUTING_TRIGGER;

	(con->trigger->pDestinationOSC->*func)(arg);
}

#ifdef CONFIG_PROFILING

int profiling_trigger_included(EventHandling::Trigger *trigger) {
	EventHandling::Trigger **compareTrigger = profiling_included_triggers;
	while ( nullptr != (*compareTrigger) ) {
		if ((*compareTrigger) == trigger) {
			return 1;
		}
		compareTrigger++;
	}
	return 0;
}

#endif

extern volatile bool exceptionActive;

namespace EventHandling {

EventHandler EventHandler::pInstance;

EventHandler::EventHandler() :
		pLastEventID(0) {
	for (uint32_t i = 0; i < NR_CPUS; i++) {
		pActiveEventTask[i] = nullptr;
		pStackPointers[i] = 0;
	}
	pEventDispatchCounter = 0;

	/* Enqueue all EventTasks for now, so getting them for emitEvent is quicker */
	for (uint32_t i = 0; i < cEventTaskPoolSize; i++) {
		pEventTasks[i].pEnqueued = false;
		pEventTasks[i].event = nullptr;
		pEventTasks[i].trigger = nullptr;
		pEventTaskPoolQueue.enqueue(&pEventTasks[i]);
	}

	/* Enqueue all EventTasks (interrupt handling) for now, so getting them for emitEvent is quicker */
	for (uint32_t i = 0; i < cInterruptTaskPoolSize; i++) {
		pInterruptTasks[i].pEnqueued = false;
		pInterruptTasks[i].event = nullptr;
		pInterruptTasks[i].trigger = nullptr;
		pInterruptTaskPoolQueue.enqueue(&pInterruptTasks[i]);
	}
#ifdef CONFIG_SCHEDULER_SINGLE_FIFO
	pScheduling = &FifoStrategy::pInstance;
#elif defined CONFIG_SCHEDULER_MULTI_FIFO
	pScheduling = &MultiFifoStrategy::pInstance;
#elif defined CONFIG_SCHEDULER_PRIORITY_MULTI
	pScheduling = &MultiQueuePriorityStrategy::pInstance;
#elif defined CONFIG_SCHEDULER_PRIORITY
	pScheduling = &SingleQueuePriorityStrategy::pInstance;
#else
#error "No scheduling defined"
	pScheduling = nullptr;
#endif

	pDummyEvent.pType = Event::DUMMY_CALL_EVENT;
}

void EventHandler::readyTask(EventTask *event) {
	// Submit the task to the scheduler
	pScheduling->enqueueEvent(event);

#ifdef CONFIG_DISPATCHER_DEBUG
	DEBUG_STREAM(TAG,"EventTask: " << hex << event << " enqueued");
#endif

	// Notify possible idling cores
	SEND_WAKE;
}

void EventHandler::eventTaskFinished(void *stackPointer) {
#if defined(CONFIG_PROFILING_EXECUTION)
	/*
	 * If profiling is enabled save runtime for the Trigger here
	 */
	cycle_t after;
	READ_CYCLE_COUNTER(after);
#endif
	byte_t cpuID = getCPUID();
	pCPUState[cpuID] = RETURNED_TO_HANDLER;

	EventTask *eventtask = pActiveEventTask[cpuID];
#ifdef CONFIG_DISPATCHER_DEBUG
	DEBUG_STREAM(TAG,"Finished event task: " << hex << eventtask);
#endif

#if defined(CONFIG_PROFILING_EXECUTION) || defined(CONFIG_PROFILING_WRITEBACK)
	bool profileTrigger = profiling_trigger_included(eventtask->trigger);
#endif

#ifdef CONFIG_PROFILING_EXECUTION
	if (profileTrigger) {
#ifdef CONFIG_X86_DISCARD_SMI_PROFILING
		// Compare SMI count. If it differs an SMI occurred during measurement -> discard
		if (sSMICounts[cpuID] != readMSR(COMMON_MSR_SMI_COUNT)) {
			sSMIDiscards[cpuID]++;
		} else {
#endif
		ResultContainer::pInstance[cpuID].enterResult(TRIGGER_EXECUTION_TIME,CYCLE_COUNTER, (after - sTimerStartValue[cpuID]), (uintptr_t)eventtask->trigger,eventtask->trigger->mSequenceNo);
#ifdef CONFIG_X86_DISCARD_SMI_PROFILING
		}
#endif
	}
#endif

	/*
	 * Reset the stackpointer of the event here. Especially important
	 * for interrupt events, as they are not recreated every time.
	 */
	eventtask->pStackpointer = nullptr;

#ifdef CONFIG_CACHE_CONTROL

#ifdef CONFIG_PROFILING_WRITEBACK
#ifdef CONFIG_X86_DISCARD_SMI_PROFILING
		// Save SMI count to compare later
		sSMICounts[cpuID] = readMSR(COMMON_MSR_SMI_COUNT);
#endif
	// FIXME move profiling to CacheManagment
#endif
	cycle_t cycles_evict = 0;

	pCPUState[cpuID] = WRITEBACK;
	// Call the cache management to evict the component from the cache
	CacheManagement::GenericCacheManagement::sInstance->evictOSC(eventtask, &cycles_evict);
	pCPUState[cpuID] = WRITEBACK_COMPLETE;
#ifdef CONFIG_PROFILING_WRITEBACK
	if (profileTrigger) {
#ifdef CONFIG_X86_DISCARD_SMI_PROFILING
		// Compare SMI count. If it differs an SMI occurred during measurement -> discard
		if (sSMICounts[cpuID] != readMSR(COMMON_MSR_SMI_COUNT)) {
			sSMIDiscards[cpuID]++;
		} else {
#endif
		ResultContainer::pInstance[cpuID].enterResult(OSC_WRITEBACK,CYCLE_COUNTER, cycles_evict, (uintptr_t)eventtask->trigger,eventtask->trigger->mSequenceNo);
#ifdef CONFIG_X86_DISCARD_SMI_PROFILING
		}
#endif
	}
#endif

#endif

#ifdef CONFIG_PROFILING
	eventtask->trigger->mSequenceNo++;
#endif

	/*
	 * Unlock the component and all its dependencies
	 */
	unlockTask(eventtask);

	pCPUState[cpuID] = UNLOCKED_TASK;

	/*
	 * Clean up the event and requeue it as a free event if it was a SW event
	 * Check if SW event or interrupt event
	 */
	if (eventtask->event->pType == Event::INTERRUPT_EVENT) {
		// Reueue the event to the pool of available events
		requeueInterruptEventTask(eventtask);
	} else if (eventtask->event->pType == Event::TIMER_EVENT && ((TimerEvent*)eventtask->event)->mTimerEventType == TimerEvent::ONESHOT ) {
		// Oneshot timer events need to be requeued by the GenericTimer component
		OSC_PREFIX(Timer)::GenericTimer::pInstance->returnOneshotEventToPool((TimerEvent*)eventtask->event);
		requeueEventTask(eventtask);
	} else {
		requeueEventTask(eventtask);
	}

	// The event handler is completely finished here, restart the dispatcher loop.
	activateDispatching();
}

/**
 * This should activate the new OSC.
 * Steps needed:
 * 	- Prefetch the new OSC (and dependent) to cache ways
 *  - Change stackpointer before activation
 *  - Change return address to some function in operating system
 *  - Get THIS-address for OSC
 *  - Jump to OSC trigger-function
 */
[[noreturn]] void EventHandler::activateOSC(EventTask *event) {
	// Get the CPUID
	cpu_t cpuID = getCPUID();

#ifdef CONFIG_DISPATCHER_DEBUG
	DEBUG_STREAM(TAG,"Activate OSC: " << hex << event->trigger->pDestinationOSC);
#endif

#ifdef CONFIG_CACHE_CONTROL

	OSC *destOSC = event->trigger->pDestinationOSC;
#if defined(CONFIG_PROFILING_PRELOAD) || defined(CONFIG_PROFILING_PRELOAD_OVERHEAD)

#ifdef CONFIG_X86_DISCARD_SMI_PROFILING
		// Save SMI count to compare later
		sSMICounts[cpuID] = readMSR(COMMON_MSR_SMI_COUNT);
#endif

#endif
	cycle_t cycles_preload;
#ifdef CONFIG_PROFILING_PRELOAD_OVERHEAD
	cycle_t cycles_preload_before;
	RESET_READ_CYCLE_COUNTER(cycles_preload_before);

#endif

	pCPUState[cpuID] = PRELOADING;
	// Call the cache management to preload the OSC to the cache
	CacheManagement::GenericCacheManagement::sInstance->preloadOSC(destOSC,event->trigger,&cycles_preload);
	pCPUState[cpuID] = PRELOADED;
#ifdef CONFIG_PROFILING_PRELOAD_OVERHEAD
	READ_CYCLE_COUNTER(cycles_preload);
	cycles_preload -= cycles_preload_before;
#endif


#if defined(CONFIG_PROFILING_PRELOAD) || defined(CONFIG_PROFILING_PRELOAD_OVERHEAD)
	if (profiling_trigger_included(event->trigger)) {
#ifdef CONFIG_X86_DISCARD_SMI_PROFILING
		// Compare SMI count. If it differs an SMI occurred during measurement -> discard
		if (sSMICounts[cpuID] != readMSR(COMMON_MSR_SMI_COUNT)) {
			sSMIDiscards[cpuID]++;
		} else {
#endif
		ResultContainer::pInstance[cpuID].enterResult(OSC_PRELOAD, CYCLE_COUNTER, cycles_preload, (uintptr_t)event->trigger, event->trigger->mSequenceNo);
#ifdef CONFIG_X86_DISCARD_SMI_PROFILING
		}
#endif
	}
#endif
#endif

	// Get the stackpointer of this OSC
	pStackPointers[cpuID] = (uintptr_t)event->pStackpointer;

#ifdef CONFIG_DISPATCHER_DEBUG
	DEBUG_STREAM(TAG,"Activate OSC: " << hex << event->trigger->pDestinationOSC << " with stack: " << pStackPointers[cpuID]);
	DEBUG_STREAM(TAG, "activateOSC Eventtask: " << event);
#endif

	// Increase global dispatched event counter
	pEventDispatchCounter++;
	// Increase local dispatched event counter
	pCoreDispatchCount[cpuID]++;
	pCPUState[cpuID] = SETTING_UP_OSC_JUMP;
	// CAUTION: This will not return
	dispatchOSCMemberEvent_asm((void*)pStackPointers[cpuID], event, event->mArg);
}


void EventHandler::dispatching() {
	// Never leave this loop, at least not directly
	cpu_t cpuNR = getCPUID();

	EventTask *task = nullptr;

	// Check if a stack was used from the pool before. Return it if so.
	if (pStackPointers[cpuNR] != 0) {
		// Return the used stack to the available StackPool
		StackPool::pInstance.setStackFree((void*)pStackPointers[cpuNR]);
		pStackPointers[cpuNR] = 0;
	}

#ifdef CONFIG_DISPATCHER_DEBUG
	DEBUG_STREAM(TAG, "Start dispatching with scheduler: " << hex << pScheduling);
#endif

#ifdef CONFIG_MEASURE_IDLE
	uint64_t idleStart = OSC_PREFIX(Timer)::GenericTimer::pInstance->getHardwareTicks();
	if (pIdleRefvalue[cpuNR] + pIdleResetVal < idleStart) {
		pIdleRefvalue[cpuNR] = idleStart;
		pIdleTime[cpuNR] = 0;
	}
#endif

	// Allow interrupts inside the dispatch loop, as no other work is executed at the moment
	Core::SystemMode::mInstance.enableInterrupts();

	/*
	 * Main event loop. From here the CPU will dispatch incoming events.
	 * If no events are available, it will sleep until woken again.
	 */
	while (1) {
		pCPUState[cpuNR] = WAITING;
		// Stop event dispatching if a exception occurred on some CPU
		if(exceptionActive) {
			// Hang here, the system will most likely not work correctly after an exception on one CPU anyway.
			while(1);
		}

		// CAUTION: This needs to be synchronized in some way because the interrupt "handler" also enqueues events.
		Core::SystemMode::mInstance.disableInterrupts();

		task = pScheduling->getEvent(cpuNR);

		if (task != pActiveEventTask[cpuNR]) {
			DEBUG_STREAM(TAG,"Trying to activate non locked task");
			while(1){}
		}

		// Check if an event is available to be dispatched
		if (nullptr != task) {
			pCPUState[cpuNR] = DISPATCHING;
#ifdef CONFIG_MEASURE_IDLE
			uint64_t idleStop = OSC_PREFIX(Timer)::GenericTimer::pInstance->getHardwareTicks();
			pIdleTime[cpuNR] += (idleStop - idleStart);
#endif

			// Activate the event task
			activateOSC(task);
		}

		Core::SystemMode::mInstance.enableInterrupts();

		// Allow the CPU to sleep for interrupts & exceptions
		WAIT_EVENT;
	}
}



uint32_t EventHandler::getEventCount() {
	return pEventDispatchCounter;
}

uint32_t EventHandler::getPendingSWEvents() {
	return pScheduling->getPendingEvents();
}

uint32_t EventHandler::getFreeSWEvents() {
	return pEventTaskPoolQueue.size();
}

EventTask *EventHandler::getTask() {
	// Get First element of queue
	pEventTasksLock.lock();
	EventTask *task = (EventTask*) pEventTaskPoolQueue.dequeueFirst();
	pEventTasksLock.unlock();
	if (task) {
		task->pTimestamp = pLastEventID++;
	} else {
		DEBUG_STREAM(TAG,"NO FREE TASK");
		for (uint32_t i = 0; i < cEventTaskPoolSize; i++) {
			DEBUG_STREAM(TAG,"Task: " << dec << i << " Trigger: " << hex << pEventTasks[i].trigger);
		}
	}
	return task;
}

EventTask *EventHandler::getInterruptTask() {
	// Get First element of queue
	pInterruptTasksLock.lock();
	EventTask *task = (EventTask*) pInterruptTaskPoolQueue.dequeueFirst();
	pInterruptTasksLock.unlock();
	if (task) {
		task->pTimestamp = pLastEventID++;
	}
	return task;
}

void EventHandler::subscribeEvent(Event *event, Trigger *trigger) {
	event->mSubscriberLock.lock();

	// Add trigger from the subscriber list
	event->addSubscriber(trigger);

#ifdef CONFIG_DISPATCHER_DEBUG
	DEBUG_STREAM(TAG,"Trigger: " << hex << trigger << "<--" << event);
#endif
	event->mSubscriberLock.unlock();
	return;
}

void EventHandler::unsubscribeEvent(Event *event, Trigger *trigger) {
	event->mSubscriberLock.lock();

	// Remove trigger from the subscriber list
	event->mSubscribers.remove(trigger);

#ifdef CONFIG_DISPATCHER_DEBUG
	DEBUG_STREAM(TAG, "Unsubscribed from event: " << hex << event);
#endif

	event->mSubscriberLock.unlock();
	return;
}

void EventHandler::eventTriggered(Event *event, dword_t arg) {
#ifdef CONFIG_DISPATCHER_DEBUG
	DEBUG_STREAM(TAG,"Event: " << hex << event << " was triggered with argument: " << hex << arg);
#endif

	/*
	 * For every subscribed Trigger create an EventTask and enqueue it to the
	 * schedulers list.
	 */
	event->mSubscriberLock.lock();

#ifdef CONFIG_DISPATCHER_DEBUG
	DEBUG_STREAM(TAG,"Event: " << hex << event << " subscribers: " << dec << event->mSubscribers.size());
#endif
	Trigger *trigger = (Trigger*) event->mSubscribers.get(0);
	while (trigger != nullptr) {
#ifdef CONFIG_DISPATCHER_DEBUG
		DEBUG_STREAM(TAG,"Activating Trigger: " << hex << trigger);
#endif
		EventTask *newCon = getTask();

		if (newCon == nullptr) {
#ifdef CONFIG_DISPATCHER_DEBUG
			DEBUG_STREAM(TAG,"NO TASK READY");
#endif
			event->mSubscriberLock.unlock();
			return;
		}
		newCon->event = event;
		newCon->trigger = trigger;
		newCon->mArg = arg;
		readyTask(newCon);

		trigger = (Trigger*) trigger->getNext();
	}
	event->mSubscriberLock.unlock();
}

void EventHandler::interruptEventTriggered(Event *event, dword_t arg) {
	event->mSubscriberLock.lock();
#ifdef CONFIG_DISPATCHER_DEBUG
	DEBUG_STREAM(TAG,"Interrupt event: " << hex << event << " was triggered with argument: " << hex << arg);
#endif

	/*
	 * For every subscribed Trigger create an EventTask and enqueue it to the
	 * schedulers list.
	 */
	Trigger *trigger = (Trigger*) event->mSubscribers.get(0);
	while (trigger != nullptr) {
		EventTask *newCon = (EventTask*) getInterruptTask();
		if (newCon == nullptr) {
			event->mSubscriberLock.unlock();
			return;
		}
		newCon->event = event;
		newCon->trigger = trigger;
		newCon->mArg = arg;

#ifdef CONFIG_DISPATCHER_DEBUG
		DEBUG_STREAM(TAG,"Event activating subscribed OSC: " << hex << trigger->pDestinationOSC);
#endif
		readyTask(newCon);

		trigger = (Trigger*) trigger->getNext();
	}

	event->mSubscriberLock.unlock();
}

/* Function to return used event connections */
void EventHandler::requeueEventTask(EventTask *task) {
	/*
	 * No check for con==nullptr for performance reasons.
	 * Not needed because it is a private function with only one call.
	 */
	task->pStackpointer = nullptr;
	task->pEnqueued = false;
	task->event = nullptr;
	task->trigger = nullptr;
	pEventTasksLock.lock();
	pEventTaskPoolQueue.enqueueFast(task);
	pEventTasksLock.unlock();
}

/* Function to return used event connections */
void EventHandler::requeueInterruptEventTask(EventTask *task) {
	/*
	 * No check for con==nullptr for performance reasons.
	 * Not needed because it is a private function with only one call.
	 */
	task->pStackpointer = nullptr;
	task->pEnqueued = false;
	task->event = nullptr;
	task->trigger = nullptr;
	pInterruptTasksLock.lock();
	pInterruptTaskPoolQueue.enqueueFast(task);
	pInterruptTasksLock.unlock();
}

void EventHandler::dumpDispatchStatistic() {
#ifdef CONFIG_MEASURE_IDLE
	uint64_t currentTicks = OSC_PREFIX(Timer)::GenericTimer::pInstance->getHardwareTicks();
#endif

	LOG_OUTPUT_STREAM_START(TAG, "EventHandler statistics:" << endl);
	LOG_OUTPUT_STREAM_CONTINUE(TAG, "Scheduler: " << hex << pScheduling << endl);
	for (cpu_t cpu = 0; cpu < NR_CPUS; cpu++) {
		LOG_OUTPUT_STREAM_CONTINUE(TAG, "CPU " << dec << (uint32_t) cpu << ": " << pCoreDispatchCount[cpu] << endl);
#ifdef CONFIG_MEASURE_IDLE
		LOG_OUTPUT_STREAM_CONTINUE(TAG, "CPU " << dec << (uint32_t)cpu << " utilization: " << dec << 100 - ((100*pIdleTime[cpu])/(currentTicks-pIdleRefvalue[cpu])) << endl);
#endif
	}
	LOG_OUTPUT_STREAM_CONTINUE(TAG, "Sum of dispatched events: " << dec << pEventDispatchCounter << endl);
	LOG_OUTPUT_STREAM_END;
}

void EventHandler::callOSCTrigger(Trigger *trigger, dword_t arg) {
	// Get free connection from src OSC
	EventTask *con = EventHandler::pInstance.getTask();
	// Prepare the connection for the event dispatcher
	if (nullptr != con) {
		con->trigger = trigger;
		con->mArg = arg;

		// Set dummy event
		con->event = &pDummyEvent;

		// Register this event with the EventHandler
		readyTask(con);
	}
}

bool EventHandler::tryTaskLock(EventTask *task) {
//	GenericMMU::sInstance->flushTLB();
#ifdef CONFIG_DEBUG_LOCKING
	DEBUG_STREAM(TAG,"Try locking EventTask: " << hex << task << " with OSC: " << task->trigger->pDestinationOSC);
//	DEBUG_STREAM(TAG,"Lock status: " << task->trigger->pDestinationOSC->getLock()->getStatus());
#endif

	// Try to lock the main OSC first
	if (task->trigger->pDestinationOSC == 0) {
		DEBUG_STREAM(TAG,"FAIL! OSC is null, task: " << hex << task << " trigger: " << task->trigger);
		while(1){}
	}

	pActiveTaskLock.lock();
//	DEBUG_STREAM(TAG,"Try locking EventTask: " << hex << task << " with OSC: " << task->trigger->pDestinationOSC);

	// Check for task's OSC and all its dependencies if the component is already in use
	for (cpu_t cpu = 0; cpu < NR_CPUS; cpu++) {
		if (pActiveEventTask[cpu] == nullptr) {
			// No task active on CPU -> skip
			continue;
		}

		// Check task's OSC with other CPUs main OSC
		if (task->trigger->pDestinationOSC == pActiveEventTask[cpu]->trigger->pDestinationOSC) {
			pActiveTaskLock.unlock();
			return false;
		}
		// Check other CPU's dependencies
		OSC **otherDep = pActiveEventTask[cpu]->trigger->pDeps;
		while (*otherDep != nullptr) {
			if(*otherDep == task->trigger->pDestinationOSC) {
				pActiveTaskLock.unlock();
				return false;
			}
			otherDep++;
		}


		// Check this task's dependencies
		OSC **dep = task->trigger->pDeps;
		while (*dep != nullptr) {
			if ((*dep) == pActiveEventTask[cpu]->trigger->pDestinationOSC) {
				pActiveTaskLock.unlock();
				return false;
			}

			// Check other CPU's dependencies
			otherDep = pActiveEventTask[cpu]->trigger->pDeps;
			while (*otherDep != nullptr) {
				if(*otherDep == *dep) {
					pActiveTaskLock.unlock();
					return false;
				}
				otherDep++;
			}
			dep++;
		}
	}

	pActiveEventTask[getCPUID()] = task;
#ifdef CONFIG_DISPATCHER_DEBUG
	DEBUG_STREAM(TAG,"Locked task: " << hex << task << " with OSC: " << task->trigger->pDestinationOSC);
#endif

//	DEBUG_STREAM(TAG,"Success");
	pActiveTaskLock.unlock();
	return true;
}

void EventHandler::unlockTask(EventTask *task) {
	pActiveTaskLock.lock();

	cpu_t cpuID = getCPUID();
	EventTask *activeTask = pActiveEventTask[cpuID];
	if (activeTask != task) {
		DEBUG_STREAM(TAG,"Trying to unlock a foreign task!!!, Something is wrong, captain!");
	}

	pActiveEventTask[cpuID] = nullptr;
#ifdef CONFIG_DISPATCHER_DEBUG
#endif
//	DEBUG_STREAM(TAG,"Unlocked task: " << hex << task << " with OSC: " << task->trigger->pDestinationOSC);

	pActiveTaskLock.unlock();
}

void EventHandler::printCurrentEventTask() {
	DEBUG_STREAM(TAG,"Running on: " << dec << (uint32_t)getCPUID());
	for (cpu_t cpu = 0; cpu < NR_CPUS; cpu++) {
		EventTask *task = pActiveEventTask[cpu];
		DEBUG_STREAM(TAG,"CPU: " << dec << (uint32_t)cpu);
		DEBUG_STREAM(TAG,"State:" << dec << pCPUState[cpu]);
		DEBUG_STREAM(TAG,"EventTask: " << hex << task);
		if (task == nullptr) {
			continue;
		}
		DEBUG_STREAM(TAG,"EventTask stack: " << task->pStackpointer);
		DEBUG_STREAM(TAG,"EventTask trigger: " << task->trigger);
		DEBUG_STREAM(TAG,"EventTask trigger OSC: " << task->trigger->pDestinationOSC);
		DEBUG_STREAM(TAG,"EventTask event: " << task->event);
	}
	CacheManagement::GenericCacheManagement::sInstance->printCacheWayInformation();
}

#ifdef CONFIG_X86_DISCARD_SMI_PROFILING
void EventHandler::printSMIDiscards() {
	for (cpu_t cpu = 0; cpu < NR_CPUS; cpu++) {
		DEBUG_STREAM(TAG,"SMIs discarded on CPU " << (uint32_t)cpu << ": " << dec << sSMIDiscards[cpu]);
	}
}
#endif

}
