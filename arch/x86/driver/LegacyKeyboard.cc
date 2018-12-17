/*
 * LegacyKeyboard.cc
 *
 *  Created on: 15.06.2018
 *      Author: hendrik
 */

#include <arch/x86/driver/LegacyKeyboard.h>

#include <common/debug.h>

#include <eventhandling/EventHandler.h>

#include <interrupts/InterruptDispatcher.h>

#include <interrupt_table.h>

#include <driver/IOAPIC.h>
#include <driver/InputOutputAggregator.h>

#define TAG "Keyboard"

BEGIN_OSC_IMPLEMENTATION_SECONDARY(InputOutput, LegacyKeyboard)

unsigned const char LegacyKeyboard::cTabNormal[] =
{
   0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 225, 39, '\b',
   0, 'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 129, '+', '\n',
   0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 148, 132, '^', 0, '#',
   'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', 0,
   '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
   0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, '<', 0, 0
};

unsigned const char LegacyKeyboard::cTabShift[] =
{
   0, 0, '!', '"', 21, '$', '%', '&', '/', '(', ')', '=', '?', 96, 0,
   0, 'Q', 'W', 'E', 'R', 'T', 'Z', 'U', 'I', 'O', 'P', 154, '*', 0,
   0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 153, 142, 248, 0, 39,
   'Y', 'X', 'C', 'V', 'B', 'N', 'M', ';', ':', '_', 0,
   0, 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '>', 0, 0
};

unsigned const char LegacyKeyboard::cTabAlt[] =
{
   0, 0, 0, 253, 0, 0, 0, 0, '{', '[', ']', '}', '\\', 0, 0,
   0, '@', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '~', 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 230, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '|', 0, 0
};

unsigned const char LegacyKeyboard::cTabAscNum[] =
{
   '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', ','
};
unsigned const char LegacyKeyboard::cTabScanNum[] = {
   8, 9, 10, 53, 5, 6, 7, 27, 2, 3, 4, 11, 51
};


LegacyKeyboard LegacyKeyboard::mInstance;

/**
 * Static trigger dependencies
 */
SECTION_CRITICAL_DATA OSC* LegacyKeyboard::trigger_Interrupt_Deps[] = { nullptr };

/**
 * Static triggers
 */
SECTION_CRITICAL_DATA DEFINE_TRIGGER(LegacyKeyboard, handleInterrupt, LegacyKeyboard::mInstance, trigger_Interrupt_Deps, CONFIG_PRIORITY_UART);

SECTION_CRITICAL_DATA EventHandling::Event LegacyKeyboard::sKeyEvent;

LegacyKeyboard::LegacyKeyboard() :
		mControlPort(cControlPortAddress), mDataPort(cDataPortAddress) {
	InterruptDispatcher::pInstance.registerEvent(HW_INT_KEYBOARD, &trigger_handleInterrupt);

}

uint8_t LegacyKeyboard::readKey() {
	uint32_t control = mControlPort.readByte();
	uint32_t data = mDataPort.readByte();

	DEBUG_STREAM(TAG,"Control:" << hex << control << " ,data: " << data);
	return data;
}

DEFINE_TRIGGER_FUNC(LegacyKeyboard, handleInterrupt) {
//	uint8_t control = mControlPort.readByte();

	uint8_t keyData = mDataPort.readByte();
	if (decodeKey(keyData)) {
		// Valid key -> emit event
		OSC_PREFIX(InputOutput)::InputOutputAggregator::pInstance.handleInputInterrupt(mCurrentKey.getASCIIChar());
	}
}

bool LegacyKeyboard::decodeKey(uint8_t code) {
	// Check if key is preluding to a sequence of keys
	if (code == cPrefixE0 || code == cPrefixE1) {
		mCurrentPrefix = code;
		return false;
	}

	// Check if a key got released
	if (code & cReleaseBit) {
		// Check if released key was part of a modifier
		switch (code) {
			case cLeftShiftPressed:
			case cRightShiftPressed:
				mCurrentKey.setShift(false);
				break;
			case cLeftControlPressed: {
				if (mCurrentPrefix == cPrefixE0) {
					mCurrentKey.setCtrlRight(false);
				} else {
					mCurrentKey.setCtrlLeft(false);
				}
				break;
			}
			case cLeftAltPressed: {
				if (mCurrentPrefix == cPrefixE0) {
					mCurrentKey.setAltRight(false);
				} else {
					mCurrentKey.setAltLeft(false);
				}
				break;
			}
		}

		mCurrentPrefix = 0;
		return false;
	}

	// If this point is reached a key was pressed
	switch (code) {
		case cLeftShiftPressed:
		case cRightShiftPressed:
			mCurrentKey.setShift(true);
			break;
		case cLeftControlPressed: {
			if (mCurrentPrefix == cPrefixE0) {
				mCurrentKey.setCtrlRight(true);
			} else {
				mCurrentKey.setCtrlLeft(true);
			}
			break;
		}
		case cLeftAltPressed: {
			if (mCurrentPrefix == cPrefixE0) {
				mCurrentKey.setAltRight(true);
			} else {
				mCurrentKey.setAltLeft(true);
			}
			break;
		}
		case cCapsLockPressed: {
			bool currentState = mCurrentKey.getCapsLock();
			mCurrentKey.setCapsLock(!currentState);
			setLED(KEYBOARD_LED_t::LED_CAPS_LOCK, !currentState);
			break;
		}
		case cScrollLockPressed: {
			bool currentState = mCurrentKey.getScrollLock();
			mCurrentKey.setScrollLock(!currentState);
			setLED(KEYBOARD_LED_t::LED_SCROLL_LOCK, !currentState);
			break;
		}
		default:
		{
			decodeAscii(code);
			mCurrentPrefix = 0;
			return true;
		}
	}
	return false;
}

void LegacyKeyboard::decodeAscii(uint8_t code) {
	if (code == 53 && mCurrentPrefix == cPrefixE0) {
		mCurrentKey.setASCIIChar('/');
		mCurrentKey.setScanCode(KEYBOARD_SCAN_CODE_t::SCAN_CODE_DIV);
	} else if (mCurrentKey.getNumLock() && !mCurrentPrefix && code >= 71 && code <= 83) {
		mCurrentKey.setASCIIChar(cTabAscNum[code - 71]);
		mCurrentKey.setScanCode(cTabScanNum[code - 71]);
	} else if (mCurrentKey.getAltRight()) {
		mCurrentKey.setASCIIChar(cTabAlt[code]);
		mCurrentKey.setScanCode(code);
	} else if (mCurrentKey.getShift()) {
		mCurrentKey.setASCIIChar(cTabShift[code]);
		mCurrentKey.setScanCode(code);
	} else if (mCurrentKey.getCapsLock()) {
		if ((code >= 16 && code <= 26) || (code >= 30 && code <= 40) || (code >= 44 && code <= 50)) {
			mCurrentKey.setASCIIChar(cTabShift[code]);
			mCurrentKey.setScanCode(code);
		} else {
			mCurrentKey.setASCIIChar(cTabNormal[code]);
			mCurrentKey.setScanCode(code);
		}
	} else {
		mCurrentKey.setASCIIChar(cTabNormal[code]);
		mCurrentKey.setScanCode(code);
	}
}

void LegacyKeyboard::setLED(KEYBOARD_LED_t led, bool enable) {
	uint8_t status, reply;

	mDataPort.writeByte(KEYBOARD_COMMAND_t::COMMAND_SET_LED);

	do {
		status = mControlPort.readByte();
	} while ((status & CONTROL_PORT_STATUS_t::CONTROL_PORT_OUTB) == 0);


	reply = mDataPort.readByte();

	if (reply == KEYBOARD_REPLY_t::REPLY_ACK) {
		if (enable) {
			mLEDs |= led;
		} else {
			mLEDs &= ~led;
		}
		mDataPort.writeByte(mLEDs);
		do {
			status = mControlPort.readByte();
		} while ((status & CONTROL_PORT_STATUS_t::CONTROL_PORT_OUTB) == 0);
		reply = mDataPort.readByte();
	}
}

void LegacyKeyboard::setRepeatRate(uint8_t speed, uint8_t delay) {
	uint8_t status, reply;

	mDataPort.writeByte(KEYBOARD_COMMAND_t::COMMAND_SET_SPEED);

	do {
		status = mControlPort.readByte();
	} while ((status & CONTROL_PORT_STATUS_t::CONTROL_PORT_OUTB) == 0);


	reply = mDataPort.readByte();

	if (reply == KEYBOARD_REPLY_t::REPLY_ACK) {

		mDataPort.writeByte(((delay & 3) << 5) | (speed & 31));
		do {
			status = mControlPort.readByte();
		} while ((status & CONTROL_PORT_STATUS_t::CONTROL_PORT_OUTB) == 0);
		reply = mDataPort.readByte();
	}
}

void LegacyKeyboard::setInterruptEnabled(bool enabled) {
	if (enabled) {
		IOAPIC::mInstances[0].setInterruptMask(IOAPIC::INTERRUPT_INPUT_KEYBOARD, IOAPIC::UNMASKED);
		for (uint16_t i = 0; i <1000;i++) {
			mDataPort.readByte();
		}
		uint8_t status;
		// Empty command buffer
		do {
			status = mControlPort.readByte();
			DEBUG_STREAM(TAG,"Status: " << hex << (uint16_t)status);
			DEBUG_STREAM(TAG,"Data port: " << (uint16_t)mDataPort.readByte());
		}while(status & CONTROL_PORT_STATUS_t::CONTROL_PORT_OUTB);
//		setRepeatRate(0,0);
//		setLED(KEYBOARD_LED_t::LED_NUM_LOCK, true);
		DEBUG_STREAM(TAG,"Keyboard interrupts enabled");
	} else {
		IOAPIC::mInstances[0].setInterruptMask(IOAPIC::INTERRUPT_INPUT_KEYBOARD, IOAPIC::MASKED);
	}
}

END_OSC_IMPLEMENTATION
