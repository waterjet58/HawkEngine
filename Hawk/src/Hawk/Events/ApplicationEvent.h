#pragma once

#include "Event.h"

#include <sstream>

namespace Hawk {

	class HAWK_API WindowResizeEvent : public Event
	{
	private:
		unsigned int m_Width, m_Height;
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
		{
			m_Width = width;
			m_Height = height;
		}

		inline unsigned int GetWidth() const { return m_Width; }
		inline unisgned int GetHeight() const { return m_Height; }

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	
	};
}