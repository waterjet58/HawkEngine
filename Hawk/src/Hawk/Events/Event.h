#pragma once

#include "Hawk/Core.h"
#include "Hawk/Log.h"

#include <string>
#include <sstream>
#include <functional>

namespace Hawk {

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLoseFocuse, WindowMoved,
		ApplicationTick, ApplicationUpdate, ApplicationRender,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication	= Bit(0),
		EventCategoryInput			= Bit(1),
		EventCategoryKeyboard		= Bit(2),
		EventCategoryMouse			= Bit(3),
		EventCategoryMouseButton	= Bit(4)
	};

	//Macro used by The Cherno in Game engine series to setup GetStaticType/GetEventType/GetName for the Application/Key/Mouse events
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
									virtual EventType GetEventType() const override { return GetStaticType(); }\
									virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	class  Event
	{
		friend class EventDispatcher;
	public:
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}

		bool handled = false;
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;

	private:
		Event& _event;

	public:
		EventDispatcher(Event& event)
			: _event(event)
		{
		}

		template<typename T, typename F>
		bool Dispatch(const F& func)
		{
			if (_event.GetEventType() == T::GetStaticType())
			{
				_event.handled |= func(static_cast<T&>(_event));
				return true;
			}
			return false;
		}
	
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}
