/*
 * LegacyKeyboard.h
 *
 *  Created on: 15.06.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_DRIVER_LEGACYKEYBOARD_H_
#define ARCH_X86_DRIVER_LEGACYKEYBOARD_H_

#include <component/OSC.h>
#include <eventhandling/Event.h>
#include <eventhandling/Trigger.h>

#include <common/types.h>
#include <common/IOPort.h>

BEGIN_DECLARE_OSC(InputOutput, LegacyKeyboard)

public:
	enum KEYBOARD_LED_t {
		LED_CAPS_LOCK = 0x4, LED_NUM_LOCK = 0x2, LED_SCROLL_LOCK = 0x1
	};

	enum KEYBOARD_COMMAND_t {
		COMMAND_SET_LED = 0xed, COMMAND_SET_SPEED = 0xf3
	};

	enum KEYBOARD_REPLY_t {
		REPLY_ACK = 0xfa
	};

	enum CONTROL_PORT_STATUS_t {
		CONTROL_PORT_OUTB = 0x01, CONTROL_PORT_INPB = 0x02, CONTROL_PORT_AUXB = 0x20
	};

	enum KEYBOARD_SCAN_CODE_t {
		SCAN_CODE_F1 = 0x3b, SCAN_CODE_DEL = 0x53, SCAN_CODE_UP = 72, SCAN_CODE_DOWN = 80, SCAN_CODE_LEFT = 75, SCAN_CODE_RIGHT = 77, SCAN_CODE_DIV = 8
	};

	LegacyKeyboard();
	uint8_t readKey();

	bool decodeKey(uint8_t code);
	void decodeAscii(uint8_t code);

	static EventHandling::Event sKeyEvent;

	static LegacyKeyboard mInstance;

	void setLED(KEYBOARD_LED_t led, bool enable);

	void setRepeatRate(uint8_t speed, uint8_t delay);

	void setInterruptEnabled(bool enabled);
private:
	enum MODIFIER_t {
		SHIFT = 0x1, ALT_LEFT = 0x2, ALT_RIGHT = 0x4, CTRL_LEFT = 0x8, CTRL_RIGHT = 0x10, CAPS_LOCK = 0x20, NUM_LOCK = 0x40, SCROLL_LOCK = 0x80
	};

	static constexpr uint8_t cControlPortAddress = 0x64;
	static constexpr uint8_t cDataPortAddress = 0x60;

	/*
	 * Modifier key scancodes
	 */
	static constexpr uint8_t cLeftShiftPressed = 0x2A;
	static constexpr uint8_t cRightShiftPressed = 0x36;
	static constexpr uint8_t cLeftAltPressed = 0x38;
	static constexpr uint8_t cLeftControlPressed = 0x1D;
	static constexpr uint8_t cCapsLockPressed = 0x3A;
	static constexpr uint8_t cScrollLockPressed = 0x46;
	static constexpr uint8_t cNumLockPressed = 0x45;
	static constexpr uint8_t cPrefixE0 = 0xE0;
	static constexpr uint8_t cPrefixE1 = 0xE1;

	static constexpr uint8_t cReleaseBit = 0x80;

	/*
	 * Keyboard layout constants
	 */
    static const unsigned char cTabNormal[];
    static const unsigned char cTabShift[];
    static const unsigned char cTabAlt[];
    static const unsigned char cTabAscNum[];
    static const unsigned char cTabScanNum[];

	const IOPort mControlPort;
	const IOPort mDataPort;

	uint8_t mLEDs;


	uint8_t mCurrentPrefix;

	DECLARE_TRIGGER(handleInterrupt);

	static OSC* trigger_Interrupt_Deps[];

	class Key {
	public:
		Key() :
				mAsciiCode(0), mScanCode(0), mModifier(0) {
		}

		bool isValid() {
			return mScanCode != 0;
		}

		void setASCIIChar(char ascii) {
			mAsciiCode = ascii;
		}

		char getASCIIChar() {
			return mAsciiCode;
		}

		void setScanCode(uint8_t scancode) {
			mScanCode = scancode;
		}

		void invalidate() {
			mScanCode = 0;
			mAsciiCode = 0;
			mModifier = 0;
		}

		void setShift(bool pressed) {
			mModifier = pressed ? mModifier | MODIFIER_t::SHIFT : mModifier & ~MODIFIER_t::SHIFT;
		}
		void setAltLeft(bool pressed) {
			mModifier = pressed ? mModifier | MODIFIER_t::ALT_LEFT : mModifier & ~MODIFIER_t::ALT_LEFT;
		}
		void setAltRight(bool pressed) {
			mModifier = pressed ? mModifier | MODIFIER_t::ALT_RIGHT : mModifier & ~MODIFIER_t::ALT_RIGHT;
		}
		void setCtrlLeft(bool pressed) {
			mModifier = pressed ? mModifier | MODIFIER_t::CTRL_LEFT : mModifier & ~MODIFIER_t::CTRL_LEFT;
		}
		void setCtrlRight(bool pressed) {
			mModifier = pressed ? mModifier | MODIFIER_t::CTRL_RIGHT : mModifier & ~MODIFIER_t::CTRL_RIGHT;
		}
		void setCapsLock(bool pressed) {
			mModifier = pressed ? mModifier | MODIFIER_t::CAPS_LOCK : mModifier & ~MODIFIER_t::CAPS_LOCK;
		}
		void setNumLock(bool pressed) {
			mModifier = pressed ? mModifier | MODIFIER_t::NUM_LOCK : mModifier & ~MODIFIER_t::NUM_LOCK;
		}
		void setScrollLock(bool pressed) {
			mModifier = pressed ? mModifier | MODIFIER_t::SCROLL_LOCK : mModifier & ~SCROLL_LOCK;
		}

		bool getShift() {
			return mModifier & MODIFIER_t::SHIFT;
		}
		bool getAltLeft() {
			return mModifier & MODIFIER_t::ALT_LEFT;
		}
		bool getAltRight() {
			return mModifier & MODIFIER_t::ALT_RIGHT;
		}
		bool getCtrlLeft() {
			return mModifier & MODIFIER_t::CTRL_LEFT;
		}
		bool getCtrlRight() {
			return mModifier & MODIFIER_t::CTRL_RIGHT;
		}
		bool getCapsLock() {
			return mModifier & MODIFIER_t::CAPS_LOCK;
		}
		bool getNumLock() {
			return mModifier & MODIFIER_t::NUM_LOCK;
		}
		bool getScrollLock() {
			return mModifier & MODIFIER_t::SCROLL_LOCK;
		}
		bool getAlt() {
			return getAltLeft() | getAltRight();
		}
		bool getCtrl() {
			return getCtrlLeft() | getCtrlRight();
		}

	private:
		char mAsciiCode;
		uint8_t mScanCode;
		uint8_t mModifier;
	};

	Key mCurrentKey;
};

END_DECLARE_OSC

#endif /* ARCH_X86_DRIVER_LEGACYKEYBOARD_H_ */
