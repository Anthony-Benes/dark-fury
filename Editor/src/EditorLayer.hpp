#pragma once
#include "Engine.h"

#include "Panels/SceneHierarchyPanel.hpp"

namespace Engine {

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		~EditorLayer() = default;

		void OnAttach() override;
		void OnDetach() override { ENG_PROFILE_FUNCTION(); }
		void OnUpdate(Timestep ts) override;
		void OnImGuiRender() override;
		void OnEvent(Event& event) override;
	private:
		OrthographicCameraController m_CameraController;

		Ref<Framebuffer> m_Framebuffer;

		bool m_ViewFocused = false;
		bool m_ViewHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		Ref<Texture2D> m_TextureCB;

		Ref<Scene> m_ActiveScene;
		Entity m_FirstSquare;
		Entity m_SecondSquare;
		Entity m_Camera;

		// Panels
		SceneHierarchyPanel m_ScenePanel;
	};

}