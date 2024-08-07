#pragma once

#include "Ember/Core/Base.h"
#include "Ember/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Ember {
	class EMBER_API OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_WindowHandle;
	};
}