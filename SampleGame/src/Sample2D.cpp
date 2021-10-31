#include "Sample2D.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sample2D::Sample2D()
	: Engine::Layer("2D Sample"),
	m_CameraController(1280.0f / 720.0f),
	m_ShapeColor({ 0.3f, 0.8f, 0.2f, 1.0f }),
	m_TexTint({ 1.0f, 1.0f, 1.0f, 1.0f }) {}

void Sample2D::OnAttach()
{
	ENG_PROFILE_FUNCTION();

	m_Texture = Engine::Texture2D::Create("assets/textures/Checkerboard.png");
}
static float rotation = 0.0f;
void Sample2D::OnUpdate(Engine::Timestep ts)
{
	m_CameraController.OnUpdate(ts);
	Engine::Renderer2D::ResetStats();

	rotation += 0.5f;

	Engine::Renderer2D::BeginScene(m_CameraController.GetCamera());

	Engine::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.2f, 0.3f, 0.8f, 1.0f });
	Engine::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, m_ShapeColor);
	Engine::Renderer2D::DrawRotatedQuad({ -1.5f, -1.0f, 0.0f }, { 1.0f, 1.0f }, DEG_TO_RAD(rotation), m_Texture, 10.0f, m_TexTint);
	Engine::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_Texture, 10.0f, m_TexTint);

	Engine::Renderer2D::EndScene();
}

void Sample2D::OnImGuiRender()
{
	ImGui::Begin("Settings");

	auto stats = Engine::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	ImGui::ColorEdit4("Shape Color", glm::value_ptr(m_ShapeColor));
	ImGui::ColorEdit4("Texture Tint", glm::value_ptr(m_TexTint));
	ImGui::End();
}

void Sample2D::OnEvent(Engine::Event& event)
{
	m_CameraController.OnEvent(event);
}
