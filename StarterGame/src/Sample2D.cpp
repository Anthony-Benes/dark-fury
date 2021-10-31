#include "Sample2D.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static const uint32_t s_MapWidth = 24;
static const char* s_MapTiles =
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWDDDDDDWWWWWWWWWW"
"WWWWWDDDDDDDDDDDDWWWWWWW"
"WWWWDDDDDDDDDDDDDDDDWWWW"
"WWWDDDDDWWWDDDDDDDDDDWWW"
"WWDDDDDDWWWDDDDDDDDDDDWW"
"WDDDDDDDDWDDDDDDDDDDDDDW"
"WWDDDDDDDDDDDDDDDDDDDDWW"
"WWWDDDDDDDDDDDDDDDDWWWWW"
"WWWWWDDDDDDDDDDDDWWWWWWW"
"WWWWWWWDDDDDDDWWWWWWWWWW"
"WWWWWWWWDDDDWWWWWWWWWWWW"
"WWWWWWWWWDDWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW";

Sample2D::Sample2D()
	: Engine::Layer("2D Sample"),
	m_CameraController(1280.0f / 720.0f) {}

void Sample2D::OnAttach()
{
	ENG_PROFILE_FUNCTION();

	m_TextureAtlas = Engine::Texture2D::Create("assets/textures/RPGpack_sheet_2X.png");
	s_TextureMap['D'] = Engine::SubTexture2D::CreateFromCoords(m_TextureAtlas, {6, 11}, {128, 128});
	s_TextureMap['W'] = Engine::SubTexture2D::CreateFromCoords(m_TextureAtlas, { 11, 11 }, { 128, 128 });
	s_TextureMap['!'] = Engine::SubTexture2D::CreateFromCoords(m_TextureAtlas, { 1, 3 }, { 128, 128 });

	m_MapWidth = s_MapWidth;
	m_MapHeight = strlen(s_MapTiles) / m_MapWidth;

	m_CameraController.SetZoomLevel(5.0f);
}

void Sample2D::OnUpdate(Engine::Timestep ts)
{
	m_CameraController.OnUpdate(ts);
	Engine::Renderer2D::ResetStats();

	Engine::Renderer2D::BeginScene(m_CameraController.GetCamera());

	for (uint32_t y = 0; y < m_MapHeight; y++) {
		for (uint32_t x = 0; x < m_MapWidth; x++) {
			char tileType = s_MapTiles[x + y * m_MapWidth];
			Engine::Ref<Engine::SubTexture2D> texture;
			if (s_TextureMap.find(tileType) != s_TextureMap.end()) {
				texture = s_TextureMap[tileType];
			}
			else {
				texture = s_TextureMap['!'];
			}
			Engine::Renderer2D::DrawQuad({ x - (m_MapWidth / 2.0f), (m_MapHeight / 2.0f) - y }, { 1.0f, 1.0f }, texture);
		}
	}

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
	ImGui::End();
}

void Sample2D::OnEvent(Engine::Event& event)
{
	m_CameraController.OnEvent(event);
}
