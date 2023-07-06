#pragma once

#include "Event.h"

namespace Hawk {

	class  MouseMovedEvent : public Event
	{
	private:
		float _mouseX, _mouseY;

	public:
		MouseMovedEvent(float x, float y)
		{
			_mouseX = x;
			_mouseY = y;
		}

		inline float getX() const { return _mouseX; }
		inline float getY() const { return _mouseY; }

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	};

	class  MouseScrolledEvent : public Event 
	{
	private: 
		float _xOffset, _yOffset;
	public:
		MouseScrolledEvent(const float xOffset, const float yOffset)
		{
			_xOffset = xOffset;
			_yOffset = yOffset;
		}

		float getXOffset() const { return _xOffset; }
		float getYOffset() const { return _yOffset; }

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	};

	class  MouseButtonEvent : public Event
	{
	protected:
		MouseButtonEvent(int button)
		{
			_button = button;
		}

		int _button;

	public:
		inline int GetMouseButton() const { return _button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	};

	class  MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button)
			: MouseButtonEvent(button) {}

	public:
		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button)
			: MouseButtonEvent(button) {}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}