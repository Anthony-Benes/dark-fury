#pragma once
#include "Engine.h"

class EditorLayer : public Engine::Layer
{
public:
	EditorLayer();
	~EditorLayer() = default;

	void OnAttach() override;
	void OnDetach() override { ENG_PROFILE_FUNCTION(); }
	void OnUpdate(Engine::Timestep ts) override;
	void OnImGuiRender() override;
	void OnEvent(Engine::Event& event) override;
private:
	Engine::OrthographicCameraController m_CameraController;

	Engine::Ref<Engine::Framebuffer> m_Framebuffer;

	bool m_ViewFocused = false;
	bool m_ViewHovered = false;
	glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	glm::vec4 m_SquareColor;
	Engine::Ref<Engine::Texture2D> m_TextureCB;

	Engine::Ref<Engine::Scene> m_ActiveScene;
	Engine::Entity m_SquareEntity;
	Engine::Entity m_Camera;

	bool useOrtho = false;
};