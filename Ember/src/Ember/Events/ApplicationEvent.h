#pragma once

#include "Ember/Events/Event.h"

namespace Ember 
{

	class EMBER_API WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height)
				: m_Width(width), m_Height(height) {}

		inline uint32_t GetWidth() const { return m_Width; }
		inline uint32_t GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		uint32_t m_Width, m_Height;
	};

	class EMBER_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class EMBER_API WindowMinimizedEvent : public Event
	{
	public:
		WindowMinimizedEvent() {}

		EVENT_CLASS_TYPE(WindowMinimized)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class EMBER_API WindowRestoredEvent : public Event
	{
	public:
		WindowRestoredEvent() {}

		EVENT_CLASS_TYPE(WindowRestored)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class EMBER_API AppTickEvent : public Event
	{
	public:
	AppTickEvent() {}

	EVENT_CLASS_TYPE(AppTick)
	EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class EMBER_API AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() {}

	EVENT_CLASS_TYPE(AppUpdate)
	EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class EMBER_API AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() {}

	EVENT_CLASS_TYPE(AppRender)
	EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

}