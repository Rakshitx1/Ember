#pragma once

#include "Core.h"
#include "Window.h"
#include "Ember/LayerStack.h"
#include "Events/Event.h"
#include "Ember/Events/ApplicationEvent.h"

#include "Ember/Core/Timestep.h"

#include "Ember/ImGui/ImGuiLayer.h"

namespace Ember
{
	class EMBER_API Application
	{

	public:
		
		Application();
		
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;

	private:
		static Application* s_Instance;
	
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}
