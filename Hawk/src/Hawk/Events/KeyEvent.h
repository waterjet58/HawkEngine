#pragma once

#include "Event.h"

namespace Hawk {

	class KeyEvent : public Event
	{
	protected:
		KeyEvent(int keycode)
			: _keyCode(keycode) {}

		int _keyCode;

	public:
		int GetKeyCode() const { return _keyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	};

	class KeyPressedEvent : public KeyEvent
	{
	private:
		bool _isRepeat;
	public:
		KeyPressedEvent(int keycode, bool isRepeat = false)
			: KeyEvent(keycode), _isRepeat(isRepeat) {}

		bool IsRepeat() const { return _isRepeat; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << _keyCode << " (repeat = " << _isRepeat << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << _keyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << _keyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	};

}