#pragma once
#include "Engine.h"

class Sample2D : public Engine::Layer
{
public:
	Sample2D();
	~Sample2D() = default;

	void OnAttach() override;
	void OnDetach() override { ENG_PROFILE_FUNCTION(); }
	void OnUpdate(Engine::Timestep ts) override;
	void OnImGuiRender() override;
	void OnEvent(Engine::Event& event) override;
private:
	Engine::OrthographicCameraController m_CameraController;
	Engine::Ref<Engine::Texture2D> m_Texture;

	glm::vec4 m_ShapeColor;
	glm::vec4 m_TexTint;
};