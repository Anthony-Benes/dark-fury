#pragma once

#include "Engine/Scene/Components.hpp"
#include "Engine/Scene/CameraProjection.hpp"

#include <glm/glm.hpp>

namespace Engine {

	class Camera
	{
	public:
		Camera(CameraComponent& cameraView, TransformComponent& cameraTransform);
		Camera(const glm::vec3& translation, CameraComponent& cameraView);
		Camera(uint32_t width, uint32_t height, float zFar = -10.0f, float zNear = 0.0f);
		Camera(float aspectRatio, float fov, float zNear, float zFar, CameraProjection::ProjectionType pType = CameraProjection::ProjectionType::Perspective);
		Camera(const glm::vec3& translation, uint32_t width, uint32_t height, float zFar = -10.0f, float zNear = 0.0f);
		Camera(const glm::vec3& translation, float aspectRatio, float fov, float zNear, float zFar, CameraProjection::ProjectionType pType = CameraProjection::ProjectionType::Perspective);
		Camera(TransformComponent& cameraTransform, uint32_t width, uint32_t height, float zFar = -10.0f, float zNear = 0.0f);
		Camera(TransformComponent& cameraTransform, float aspectRatio, float fov, float zNear, float zFar, CameraProjection::ProjectionType pType = CameraProjection::ProjectionType::Perspective);
		~Camera() = default;

		const CameraProjection::ProjectionType& GetProjectionType() const { return m_Projection->Camera.GetProjectionType(); }

		void SetViewOrthographic(float width, float height, float zFar = MAX_FLOAT, float zNear = MAX_FLOAT);
		void SetViewPerspective(float aspectRatio, float fov, float zNear = 0.01f, float zFar = DefaultFarFloat);

		void SetViewportSize(uint32_t width, uint32_t height);
		void UpdateViewProjectionMatrix();

		TransformComponent* GetTransformComponent() { return m_Transform; }
		CameraComponent* GetCameraComponent() { return m_Projection; }

		const glm::mat4& GetViewMatrix() const { return glm::inverse(m_Transform->GetTransform()); }
		const glm::mat4& GetProjectionMatrix() const { return m_Projection->Camera.GetProjectionMatrix(); }
		const glm::mat4& GetViewProjectionMatrix() {
			UpdateViewProjectionMatrix();
			return m_ViewProjectionMatrix;
		}

	protected:
		TransformComponent* m_Transform;
		CameraComponent* m_Projection;
		glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.0f);
	};

}