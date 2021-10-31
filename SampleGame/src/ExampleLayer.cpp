#include "ExampleLayer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

ExampleLayer::ExampleLayer()
	: Engine::Layer("Example"),
	m_CameraController(1280.0f / 720.0f, true) {
	m_VertexArray = Engine::VertexArray::Create();
	float vertices[4 * 5] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};
	Engine::Ref<Engine::VertexBuffer> vBuff = Engine::VertexBuffer::Create(vertices, sizeof(vertices));
	Engine::BufferLayout layout = {
		{Engine::ShaderDataType::Float3, "a_Position"},
		{Engine::ShaderDataType::Float2, "a_TexCoord"}
	};
	vBuff->SetLayout(layout);
	m_VertexArray->AddVertexBuffer(vBuff);
	uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
	Engine::Ref<Engine::IndexBuffer> iBuff = Engine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
	m_VertexArray->SetIndexBuffer(iBuff);

	m_ShaderLibrary.Load("assets/shaders/Color.glsl");
	m_ShaderLibrary.Load("assets/shaders/Texture2D.glsl");

	m_TextureCB = Engine::Texture2D::Create("assets/textures/Checkerboard.png");
	m_TextureLogo = Engine::Texture2D::Create("assets/textures/ChernoLogo.png");

	m_ShaderLibrary.Get("Texture2D")->Bind();
	m_ShaderLibrary.Get("Texture2D")->SetInt("u_Texture", m_TexSlot);
}

void ExampleLayer::OnUpdate(Engine::Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	Engine::Renderer::BeginScene(m_CameraController.GetCamera());

	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f));

	auto texture = m_ShaderLibrary.Get("Texture2D");

	m_TextureCB->Bind();
	Engine::Renderer::Submit(texture, m_VertexArray, scale);
	m_TextureLogo->Bind();
	Engine::Renderer::Submit(texture, m_VertexArray, glm::translate(glm::mat4(1.0f), {0.0f, 0.0f, 0.1f}) * scale);

	Engine::Renderer::EndScene();
}

void ExampleLayer::OnEvent(Engine::Event& event)
{
	m_CameraController.OnEvent(event);
}