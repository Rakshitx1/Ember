#include "Ember.h"

#include "imgui.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ExampleLayer : public Ember::Layer
{
	public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f), m_CameraRotation(0.0f)
	{
		m_VertexArray.reset(Ember::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		Ember::Ref<Ember::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Ember::VertexBuffer::Create(vertices, sizeof(vertices)));
		Ember::BufferLayout layout = {
			{ Ember::ShaderDataType::Float3, "a_Position" },
			{ Ember::ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		Ember::Ref<Ember::IndexBuffer> indexBuffer;
		indexBuffer.reset(Ember::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(Ember::VertexArray::Create());

		float squareVertices[3 * 4] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};

		Ember::Ref<Ember::VertexBuffer> squareVB;
		squareVB.reset(Ember::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ Ember::ShaderDataType::Float3, "a_Position" }
		});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Ember::Ref<Ember::IndexBuffer> squareIB;
		squareIB.reset(Ember::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

		m_Shader.reset(Ember::Shader::Create(vertexSrc, fragmentSrc));

		std::string flatShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		std::string flatShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

		m_FlatColorShader.reset(Ember::Shader::Create(flatShaderVertexSrc, flatShaderFragmentSrc));
	}

	void OnUpdate(Ember::Timestep ts) override
	{
		EMBER_INFO("Delta time: {0}s ({1}ms)", ts.GetSeconds(), ts.GetMilliseconds());
		
		if (Ember::Input::IsKeyPressed(EMBER_KEY_LEFT))
			m_CameraPosition.x -= m_CameraTranslationSpeed * ts;
		else if (Ember::Input::IsKeyPressed(EMBER_KEY_RIGHT))
			m_CameraPosition.x += m_CameraTranslationSpeed * ts;
		
		if (Ember::Input::IsKeyPressed(EMBER_KEY_UP))
			m_CameraPosition.y += m_CameraTranslationSpeed * ts;
		else if (Ember::Input::IsKeyPressed(EMBER_KEY_DOWN))
			m_CameraPosition.y -= m_CameraTranslationSpeed * ts;

		if (Ember::Input::IsKeyPressed(EMBER_KEY_A))
			m_CameraRotation += m_CameraRotationSpeed * ts;
		else if (Ember::Input::IsKeyPressed(EMBER_KEY_D))
			m_CameraRotation -= m_CameraRotationSpeed * ts;

		Ember::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
		Ember::RenderCommand::Clear();

		m_Camera.setPosition(m_CameraPosition);
		m_Camera.setRotation(m_CameraRotation);

		Ember::Renderer::BeginScene(m_Camera);

		static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		std::dynamic_pointer_cast<Ember::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<Ember::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		for(int y = 0; y < 20; y++)
		{
			for(int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Ember::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		Ember::Renderer::Submit(m_Shader, m_VertexArray);

		Ember::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(Ember::Event& event) override
	{
	}

private:
	Ember::Ref<Ember::Shader> m_Shader;
	Ember::Ref<Ember::VertexArray> m_VertexArray;

	Ember::Ref<Ember::Shader> m_FlatColorShader;
	Ember::Ref<Ember::VertexArray> m_SquareVA;

	Ember::OrthographicCamera m_Camera;

	glm::vec3 m_CameraPosition;
	float m_CameraRotation;
	float m_CameraTranslationSpeed = 4.0f;
	float m_CameraRotationSpeed = 180.0f;

	glm::vec3 m_SquareColor = {0.2f, 0.3f, 0.8f};
};

class Sandbox : public Ember::Application
{
	public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}
};

Ember::Application* Ember::CreateApplication()
{
	return new Sandbox();
}
