#pragma once

#include "Engine/Renderer/Camera.hpp"

#include <glm/glm.hpp>

namespace Engine {

	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera(float fov, float aspectRatio, float farZ, float nearZ = 0.1f);

		virtual void SetProjection(float fov, float aspectRatio, float farZ, float nearZ = 0.1f) override;

		const glm::vec3& GetPosition() const override { return m_Position; }
		void SetPosition(const glm::vec3& position) override { m_Position = position; RecalculateViewMatrix(); }

		const glm::vec2& GetRotation() const { return m_Rotation; }
		void SetRotation(const glm::vec2& rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }
	private:
		void RecalculateViewMatrix();
	private:
		float m_aspectRatio = 0.0f;
		float m_FOV = 90.0f;
		glm::vec3 m_Position = { 0.0f, 0.0f, 10.0f };
		glm::vec2 m_Rotation = { 0.0f, 0.0f };
	};

}
