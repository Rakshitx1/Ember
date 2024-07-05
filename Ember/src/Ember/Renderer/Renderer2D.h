#pragma once

#include "Ember/Core/Core.h"
#include "Ember/Renderer/OrthographicCamera.h"
#include "Ember/Renderer/RenderCommand.h"

namespace Ember {

	class EMBER_API Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		static void Flush();

		// Primitives
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
	};

}