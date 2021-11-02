#pragma once

#include "Engine/Renderer/Camera.hpp"

#include <glm/glm.hpp>

namespace Engine {

	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top, float zFar = -1.0f, float zNear = 1.0f);

		virtual void SetProjection(float left, float right, float bottom, float top, float zFar, float zNear = 1.0f) override;
		virtual void SetProjection(float zoom, float aspectRatio, float farZ = -1.0f, float nearZ = 1.0f) override;

		const glm::vec3& GetPosition() const override { return m_Position; }
		void SetPosition(const glm::vec3& position) override { m_Position = position; RecalculateViewMatrix(); }

		float GetRotation() const { return m_Rotation; }
		void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }
	private:
		void RecalculateViewMatrix();
	private:
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		float m_Rotation = 0.0f;
	};

}
