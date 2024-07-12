#include "Emberpch.h"

#include "Ember/Renderer/Renderer2D.h"
#include "Ember/Renderer/VertexArray.h"
#include "Ember/Renderer/Shader.h"

#include <glm/gtc/matrix_transform.hpp>
#include "Renderer2D.h"

namespace Ember{

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Colour;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;
	};

	struct Renderer2DData
	{
		// Per draw call
		static const uint32_t MaxQuads = 100000;				// TODO: Test this
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; 		// TODO: RenderCaps

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		glm::vec4 QuadVertexPosition[4];

		Renderer2D::Statistics Stats;
	};

	static Renderer2DData s_Data;

	void Renderer2D::Init()
	{
		EMBER_PROFILE_FUNCTION();

		s_Data.QuadVertexArray = VertexArray::Create();

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float, "a_TexIndex" },
			{ ShaderDataType::Float, "a_TilingFactor" }
		});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
		{
			samplers[i] = i;
		}

		s_Data.TextureShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetIntArray("u_TextureSlots", samplers, s_Data.MaxTextureSlots);

		// Set Texture slot 0 to white texture
		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		s_Data.QuadVertexPosition[0] = {-0.5, -0.5, 0.0, 1.0};
		s_Data.QuadVertexPosition[1] = { 0.5, -0.5, 0.0, 1.0};
		s_Data.QuadVertexPosition[2] = { 0.5,  0.5, 0.0, 1.0};
		s_Data.QuadVertexPosition[3] = {-0.5,  0.5, 0.0, 1.0};
	}

	void Renderer2D::Shutdown()
	{
		EMBER_PROFILE_FUNCTION();
	}

	void Renderer2D::BeginScene(const OrthographicCamera &camera)
	{
		EMBER_PROFILE_FUNCTION();
		
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::EndScene()
	{
		EMBER_PROFILE_FUNCTION();

		uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

		Flush();
	}

    void Renderer2D::Flush()
    {
		if(s_Data.QuadIndexCount == 0)
		{
			return;
		}
		
		// Bind textures
		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
		{
			s_Data.TextureSlots[i]->Bind(i);
		}

		RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);

		s_Data.Stats.DrawCalls++;
    }

	///////////////////////////////////////////////////////////////////////////////
	///									COLOURED QUAD							///
	///////////////////////////////////////////////////////////////////////////////

    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4& colour)
	{
		DrawQuad({position.x, position.y, 0.0f}, size, colour);
	}

	void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4& colour)
	{
		EMBER_PROFILE_FUNCTION();

		if(s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			StartNewBatch();
		}

		const float textureIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) 
							* glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

		for(int i = 0; i < 4; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPosition[i];
			s_Data.QuadVertexBufferPtr->Colour = colour;
			s_Data.QuadVertexBufferPtr->TexCoord = s_Data.QuadVertexPosition[i] + 0.5f;
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	///////////////////////////////////////////////////////////////////////////////
	///									TEXTURED QUAD							///
	///////////////////////////////////////////////////////////////////////////////

	void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, const float& tilingFactor, const glm::vec4& colour)
	{
		DrawQuad({position.x, position.y, 0.0f}, size, texture, tilingFactor, colour);
	}

	void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture, const float& tilingFactor, const glm::vec4& colour)
	{
		EMBER_PROFILE_FUNCTION();

		if(s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			StartNewBatch();
		}

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if(s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
			{
				StartNewBatch();
			}

			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
							* glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

		for(int i = 0; i < 4; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPosition[i];
			s_Data.QuadVertexBufferPtr->Colour = colour;
			s_Data.QuadVertexBufferPtr->TexCoord = s_Data.QuadVertexPosition[i] + 0.5f;
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	///////////////////////////////////////////////////////////////////////////////
	///									SUBTEXTURE QUAD							///
	///////////////////////////////////////////////////////////////////////////////

	void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<SubTexture2D> &subTexture, const float& tilingFactor, const glm::vec4& colour)
	{
		DrawQuad({position.x, position.y, 0.0f}, size, subTexture, tilingFactor, colour);
	}

	void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<SubTexture2D> &subTexture, const float& tilingFactor, const glm::vec4& colour)
	{
		EMBER_PROFILE_FUNCTION();

		const glm::vec2* textureCoords = subTexture->GetTexCoords();
		const Ref<Texture2D> texture = subTexture->GetTexture();

		if(s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			StartNewBatch();
		}

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if(s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
			{
				StartNewBatch();
			}

			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
							* glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

		for(int i = 0; i < 4; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPosition[i];
			s_Data.QuadVertexBufferPtr->Colour = colour;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	///////////////////////////////////////////////////////////////////////////////
	///								ROTATED COLOURED QUAD						///
	///////////////////////////////////////////////////////////////////////////////

	void Renderer2D::DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, const float& rotation, const glm::vec4& colour)
	{
		DrawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, colour);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, const float& rotation, const glm::vec4& colour)
	{
		EMBER_PROFILE_FUNCTION();

		if(s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			StartNewBatch();
		}

		const float textureIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) 
							* glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f}) 
							* glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

		for(int i = 0; i < 4; i++)
		{
		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPosition[i];
		s_Data.QuadVertexBufferPtr->Colour = colour;
		s_Data.QuadVertexBufferPtr->TexCoord = s_Data.QuadVertexPosition[i] + 0.5f;;
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	///////////////////////////////////////////////////////////////////////////////
	///								ROTATED TEXTURED QUAD						///
	///////////////////////////////////////////////////////////////////////////////

	void Renderer2D::DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, const float& rotation, const Ref<Texture2D> &texture, const float& tilingFactor, const glm::vec4& colour)
	{
		DrawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, texture, tilingFactor, colour);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, const float& rotation, const Ref<Texture2D> &texture, const float& tilingFactor, const glm::vec4& colour)
	{
		EMBER_PROFILE_FUNCTION();

		if(s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			StartNewBatch();
		}

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if(s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
			{
				StartNewBatch();
			}

			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) 
							* glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f}) 
							* glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

		for(int i = 0; i < 4; i++)
		{
		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPosition[i];
		s_Data.QuadVertexBufferPtr->Colour = colour;
		s_Data.QuadVertexBufferPtr->TexCoord = s_Data.QuadVertexPosition[i] + 0.5f;
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	///////////////////////////////////////////////////////////////////////////////
	///							ROTATED SUBTEXTURE QUAD							///
	///////////////////////////////////////////////////////////////////////////////

	void Renderer2D::DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, const float &rotation, const Ref<SubTexture2D> &subTexture, const float& tilingFactor, const glm::vec4& colour)
	{
		DrawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, subTexture, tilingFactor, colour);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, const float &rotation, const Ref<SubTexture2D> &subTexture, const float& tilingFactor, const glm::vec4& colour)
	{
		EMBER_PROFILE_FUNCTION();

		const glm::vec2* textureCoords = subTexture->GetTexCoords();
		const Ref<Texture2D> texture = subTexture->GetTexture();

		if(s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			StartNewBatch();
		}

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if(s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
			{
				StartNewBatch();
			}

			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
							* glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f})
							* glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

		for(int i = 0; i < 4; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPosition[i];
			s_Data.QuadVertexBufferPtr->Colour = colour;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}

	void Ember::Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

    void Renderer2D::StartNewBatch()
    {
		EndScene();

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureSlotIndex = 1;
    }
}
