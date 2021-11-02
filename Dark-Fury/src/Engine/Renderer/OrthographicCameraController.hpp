#pragma once

#include "Engine/Renderer/OrthographicCamera.hpp"
#include "Engine/Core/Timestep.hpp"

#include "Engine/Events/ApplicationEvent.hpp"
#include "Engine/Events/MouseEvent.hpp"

namespace Engine {

	struct OrthographicCameraBounds {
		float Left;
		float Right;
		float Bottom;
		float Top;
		float GetWidth() { return Right - Left; }
		float GetHeight() { return Top - Bottom; }
		void ReSize(float aspectRatio, float zoom) {
			Left = -aspectRatio * zoom;
			Right = aspectRatio * zoom;
			Bottom = -zoom;
			Top = zoom;
		}
	};

	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, bool rotation = false, float zFar = -1.0f, float zNear = 1.0f);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		void OnResize(float width, float height);

		const OrthographicCameraBounds& GetBounds() const { return m_Bounds; }
		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }

		float GetZoomLevel() const { return m_ZoomLevel; }
		void SetZoomLevel(float level) {
			m_ZoomLevel = level; CalculateView(); 
		}
	private:
		void CalculateView();

		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		glm::vec2 m_ZBounds;
		OrthographicCameraBounds m_Bounds;
		OrthographicCamera m_Camera;

		bool m_Rotation;

		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f; //In degrees, in the anti-clockwise direction
		float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
	};

}