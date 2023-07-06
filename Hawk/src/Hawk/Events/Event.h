#pragma once

#include "Hawk/Core.h"
#include "Hawk/Log.h"

#include <string>
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

	protected:
		bool m_Handled = false;
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;

	private:
		Event& m_Event;

	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	
	};


}
