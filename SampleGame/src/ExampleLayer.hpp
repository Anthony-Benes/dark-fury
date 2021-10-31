#pragma once
#include "Engine.h"

class ExampleLayer : public Engine::Layer
    {
    public:
        ExampleLayer();

        void OnUpdate(Engine::Timestep ts);
        void OnEvent(Engine::Event& event);
	private:
		Engine::ShaderLibrary m_ShaderLibrary;
		Engine::Ref<Engine::VertexArray> m_VertexArray;

		Engine::Ref<Engine::Texture2D> m_TextureCB;
		Engine::Ref<Engine::Texture2D> m_TextureLogo;
		uint32_t m_TexSlot = 0;

		Engine::OrthographicCameraController m_CameraController;
    };