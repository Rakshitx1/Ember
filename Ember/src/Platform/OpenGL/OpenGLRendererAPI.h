#pragma once

#include "Ember/Core/Base.h"
#include "Ember/Renderer/RendererAPI.h"

namespace Ember {

	class EMBER_API OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void init() override;

		virtual void setClearColor(const glm::vec4& color) override;
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		
		virtual void clear() override;

		virtual void drawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0 ) override;
		
	};

}
