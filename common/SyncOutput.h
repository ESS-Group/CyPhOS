/*
 * SyncOutput.h
 *
 *  Created on: 14.11.2016
 *      Author: hendrik
 */

#ifndef DRIVER_SYNCOUTPUT_H_
#define DRIVER_SYNCOUTPUT_H_


#include <driver/ConsoleBuffer.h>
#include <driver/GenericIO.h>

#include <eventhandling/EventHandler.h>

#define SYNC_OUTPUT_STREAM(TAG,...) { using namespace OSC_PREFIX(ConsoleBuffer); \
		uint32_t line = OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance.startLine(false) << TAG << ':' <<  __VA_ARGS__ << finishOutput; \
		EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(InputOutput)::GenericIO::trigger_printLine,(dword_t)line); \
	\
	}

#define SYNC_OUTPUT_STREAM_CHAR(c) { using namespace OSC_PREFIX(ConsoleBuffer); \
		uint32_t line = OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance.startLine(false) << (c) << finishOutput; \
		EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(InputOutput)::GenericIO::trigger_printLine,(dword_t)line); \
	\
	}

#define SYNC_OUTPUT_STREAM_RAW(...) { using namespace OSC_PREFIX(ConsoleBuffer); \
		uint32_t line = OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance.startLine(false) << __VA_ARGS__ << finishOutput; \
		EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(InputOutput)::GenericIO::trigger_printLine,(dword_t)line); \
	\
	}

#define SYNC_OUTPUT_STREAM_START(...) { using namespace OSC_PREFIX(ConsoleBuffer); \
		OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance.startLine(false) << __VA_ARGS__ ; \
	\
	}

#define SYNC_OUTPUT_STREAM_CONTINUE(...) { using namespace OSC_PREFIX(ConsoleBuffer); \
		OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance << __VA_ARGS__ ; \
	\
	}

#define SYNC_OUTPUT_STREAM_END { using namespace OSC_PREFIX(ConsoleBuffer); \
		uint32_t line = OSC_PREFIX(ConsoleBuffer)::finishOutput(OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance); \
		EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(InputOutput)::GenericIO::trigger_printLine,(dword_t)line); \
	\
	}


#ifdef CONFIG_DEBUG
#define LOG_OUTPUT_STREAM(TAG,...) { using namespace OSC_PREFIX(ConsoleBuffer); \
		uint32_t line = OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance.startLine(true) << TAG << ':' <<  __VA_ARGS__ << finishOutput; \
		EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(InputOutput)::GenericIO::trigger_printLine,(dword_t)line); \
	\
	}

#define LOG_OUTPUT_STREAM_CHAR(c) { using namespace OSC_PREFIX(ConsoleBuffer); \
		uint32_t line = OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance.startLine(false) << TAG << ':' << (c) << finishOutput; \
		EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(InputOutput)::GenericIO::trigger_printLine,(dword_t)line); \
	\
	}

#define LOG_OUTPUT_STREAM_RAW(TAG,...) { using namespace OSC_PREFIX(ConsoleBuffer); \
		uint32_t line = OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance.startLine(true) << TAG << ':' << __VA_ARGS__ << finishOutput; \
		EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(InputOutput)::GenericIO::trigger_printLine,(dword_t)line); \
	\
	}

#define LOG_OUTPUT_STREAM_START(TAG,...) { using namespace OSC_PREFIX(ConsoleBuffer); \
		OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance.startLine(true) << TAG << ':' << __VA_ARGS__ ; \
	\
	}

#define LOG_OUTPUT_STREAM_CONTINUE(TAG,...) { using namespace OSC_PREFIX(ConsoleBuffer); \
		OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance << TAG << ':' << __VA_ARGS__ ; \
	\
	}

#define LOG_OUTPUT_STREAM_END { using namespace OSC_PREFIX(ConsoleBuffer); \
		uint32_t line = OSC_PREFIX(ConsoleBuffer)::finishOutput(OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance); \
		EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(InputOutput)::GenericIO::trigger_printLine,(dword_t)line); \
	\
	}
#else

#endif

#endif /* DRIVER_SYNCOUTPUT_H_ */
