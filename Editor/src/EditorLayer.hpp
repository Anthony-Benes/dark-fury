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
	Engine::PerspectiveCamera m_Camera;

	// Temp
	Engine::Ref<Engine::VertexArray> m_SquareVA;
	Engine::Ref<Engine::Shader> m_FlatColorShader;
	Engine::Ref<Engine::Framebuffer> m_Framebuffer;

	Engine::Ref<Engine::Texture2D> m_CheckerboardTexture;

	glm::vec4 m_SquareColor;
};