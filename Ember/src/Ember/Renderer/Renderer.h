#pragma once

#include "Ember/Core/Base.h"
#include "Ember/Renderer/RenderCommand.h"
#include "Ember/Renderer/Shader.h"
#include "Ember/Renderer/OrthographicCamera.h"

namespace Ember {

	class EMBER_API Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		inline static RendererAPI::API GetAPI() { return RendererAPI::getAPI(); }

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static Scope<SceneData> m_SceneData;
	};

}