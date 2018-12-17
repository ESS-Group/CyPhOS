/*
 * GenericTimer.h
 *
 *  Created on: 07.10.2016
 *      Author: hendrik
 */

#ifndef DRIVER_GENERICTIMER_H_
#define DRIVER_GENERICTIMER_H_

#include <eventhandling/TimerEvent.h>
#include <eventhandling/Trigger.h>
#include <component/InterruptOSC.h>

#include <common/Queue.h>

#include <sync/Spinlock.h>

MEMBER_OF_OSC_BEGIN(Timer)


class GenericTimer {
	friend class EventHandling::EventHandler;

public:
	GenericTimer();
	/**
	 * Enqueues an timer event for the given OSC and destination method destFunc in an us interval.
	 * @param osc OSC to be called on timer run down
	 * @param destFunc Method to be called after run down
	 * @param interval Interval in us to trigger the event periodically
	 * @return TimerEvent that was enqueued. Can be used to cancel the event.
	 */
	EventHandling::TimerEvent *setPeriodicEvent(EventHandling::Trigger *trigger, uint32_t interval);

	/**
	 * Enqueues an timer event for the given OSC and destination method destFunc after an us delay.
	 * @param osc OSC to be called on timer run down
	 * @param destFunc Method to be called after run down
	 * @param delay Delay in us to trigger the event
	 * @return TimerEvent that was enqueued. Can be used to cancel the event.
	 */
	EventHandling::TimerEvent *setOneshotEvent(EventHandling::Trigger *trigger, uint32_t delay);

	void initializeTimer();

	/**
	 * Returns a used event back to the pool of available TimerEvents
	 * @param event Event to requeue to the pool.
	 */
	void returnOneshotEventToPool(EventHandling::TimerEvent *event);

	/**
	 * Returns a used event back to the pool of available TimerEvents
	 * @param event Event to requeue to the pool.
	 */
	void returnPeriodicEventToPool(EventHandling::TimerEvent *event);

	/**
	 * Cancels a scheduled periodic event and inserts it back to the pool of available events
	 * @param event Periodic event to cancel
	 */
	void cancelPeriodicTimerEvent(EventHandling::TimerEvent *event);

	/**
	 * Cancels a scheduled oneshot event and inserts it back to the pool of available events
	 * @param event Oneshot event to cancel
	 */
	void cancelOneshotTimerEvent(EventHandling::TimerEvent *event);


	static GenericTimer *pInstance;

	/**
	 * Returns the system uptime in milliseconds
	 * @return Uptime in ms
	 */
	uint64_t getTimeInMS();

	void dumpTimerStatus();

	static OSC* trigger_Interrupt_Deps[];
	static EventHandling::Trigger trigger_Interrupt;
protected:
	/**
	 * Size of the pool of available timers to be queued
	 */
	static constexpr uint32_t cTimerPoolSize = 32;

	/**
	 * Interval of the base hardware timer to be used in us.
	 */
	static constexpr uint32_t cTimerInterval = 1000;

	virtual void setInterrupt(bool enable) = 0;
	virtual void setPeriodic(uint64_t us) = 0;
	virtual void setOneshot(uint64_t us) = 0;

	virtual uint64_t getHardwareTicks() = 0;

	void handleTick();

	uint32_t mTimerInterval;

private:
	/**
	 * Pool of TimerEvents that are available to be enqueued either periodically or in oneshots
	 */
	EventHandling::TimerEvent mTimerConnections[cTimerPoolSize * 2];

	/**
	 * Secure the TimerEvent pool
	 */
	Spinlock mOneshotPoolLock;
	Spinlock mPeriodicPoolLock;

	/**
	 * Timed events that are actually enqueued to be triggered (oneshot)
	 */
	Queue<EventHandling::TimerEvent> mOneshotQueue;

	Spinlock mOneshotLock;

	/**
	 * Timed events that are actually enqueued to be triggered (oneshot)
	 */
	Queue<EventHandling::TimerEvent> mPeriodicQueue;

	Spinlock mPeriodicLock;

	/**
	 * Pool of TimerEvents used for enqueueing later.
	 */
	Queue<EventHandling::TimerEvent> mOneshotEventPool;
	Queue<EventHandling::TimerEvent> mPeriodicEventPool;

	EventHandling::TimerEvent *getOneshotEventFromPool();
	EventHandling::TimerEvent *getPeriodicEventFromPool();

	uint64_t mTicks;

	void dumpOneshotQueue();
};


MEMBER_OF_OSC_END
#endif /* DRIVER_GENERICTIMER_H_ */
