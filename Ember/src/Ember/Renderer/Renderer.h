#pragma once

#include "Ember/Core.h"
#include "RenderCommand.h"
#include "Shader.h"
#include "OrthographicCamera.h"

namespace Ember {

    class EMBER_API Renderer
    {
    public:
        static void BeginScene(OrthographicCamera& camera);
        static void EndScene();

        static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);

        inline static RendererAPI::API GetAPI() { return RendererAPI::getAPI(); }

    private:
        struct SceneData
        {
            glm::mat4 ViewProjectionMatrix;
        };

        static SceneData* m_SceneData;
    };

}