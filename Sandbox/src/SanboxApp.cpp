#include "Ember.h"

#include "imgui.h"

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

		std::shared_ptr<Ember::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Ember::VertexBuffer::Create(vertices, sizeof(vertices)));
		Ember::BufferLayout layout = {
			{ Ember::ShaderDataType::Float3, "a_Position" },
			{ Ember::ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<Ember::IndexBuffer> indexBuffer;
		indexBuffer.reset(Ember::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(Ember::VertexArray::Create());

		float squareVertices[3 * 4] = {
			-0.75f, -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};

		std::shared_ptr<Ember::VertexBuffer> squareVB;
		squareVB.reset(Ember::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ Ember::ShaderDataType::Float3, "a_Position" }
		});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Ember::IndexBuffer> squareIB;
		squareIB.reset(Ember::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);	
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

		m_Shader.reset(new Ember::Shader(vertexSrc, fragmentSrc));

		std::string blueShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);	
			}
		)";

		std::string blueShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

		m_BlueShader.reset(new Ember::Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
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

		Ember::Renderer::Submit(m_BlueShader, m_SquareVA);
		Ember::Renderer::Submit(m_Shader, m_VertexArray);

		Ember::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
	}

	void OnEvent(Ember::Event& event) override
	{
	}

private:
	std::shared_ptr<Ember::Shader> m_Shader;
	std::shared_ptr<Ember::VertexArray> m_VertexArray;

	std::shared_ptr<Ember::Shader> m_BlueShader;
	std::shared_ptr<Ember::VertexArray> m_SquareVA;

	Ember::OrthographicCamera m_Camera;

	glm::vec3 m_CameraPosition;
	float m_CameraRotation;
	float m_CameraTranslationSpeed = 4.0f;
	float m_CameraRotationSpeed = 180.0f;
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
