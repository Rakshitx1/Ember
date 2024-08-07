#include "Emberpch.h"

#include "Ember/Renderer/OrthographicCameraController.h"
#include "Ember/Core/Input.h"

namespace Ember {

	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		: m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation)
	{
	}

	void OrthographicCameraController::OnUpdate(Timestep ts)
	{
		EMBER_PROFILE_FUNCTION();
		
		if (Input::IsKeyPressed(EMBER_KEY_A))
			m_CameraPosition.x -= m_CameraTranslationSpeed * ts;
		else if (Input::IsKeyPressed(EMBER_KEY_D))
			m_CameraPosition.x += m_CameraTranslationSpeed * ts;
		
		if (Input::IsKeyPressed(EMBER_KEY_W))
			m_CameraPosition.y += m_CameraTranslationSpeed * ts;
		else if (Input::IsKeyPressed(EMBER_KEY_S))
			m_CameraPosition.y -= m_CameraTranslationSpeed * ts;

		if(m_Rotation)
		{
			if (Input::IsKeyPressed(EMBER_KEY_Q))
				m_CameraRotation += m_CameraRotationSpeed * ts;
			else if (Input::IsKeyPressed(EMBER_KEY_E))
				m_CameraRotation -= m_CameraRotationSpeed * ts;

			if(m_CameraRotation > 180.0f)
				m_CameraRotation -= 360.0f;
			else if(m_CameraRotation <= -180.0f)
				m_CameraRotation += 360.0f;

			m_Camera.setRotation(m_CameraRotation);
		}

		m_Camera.setPosition(m_CameraPosition);
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		EMBER_PROFILE_FUNCTION();
		
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(EMBER_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(EMBER_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	void OrthographicCameraController::OnResize(float width, float height)
	{
		m_AspectRatio = width / height;
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		EMBER_PROFILE_FUNCTION();
		
		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);

		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		EMBER_PROFILE_FUNCTION();
		
		OnResize((float)e.GetWidth(), (float)e.GetHeight());
		return false;
	}

}
