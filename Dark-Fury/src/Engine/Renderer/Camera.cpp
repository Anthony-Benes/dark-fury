#include "engpch.h"
#include "Engine/Renderer/Camera.hpp"

namespace Engine {
	Camera::Camera(CameraComponent cameraView, TransformComponent cameraTransform)
		: m_Transform(cameraTransform),
		  m_Projection(cameraView) {
		UpdateViewProjectionMatrix();
	}

	Camera::Camera(const glm::vec3& translation, CameraComponent cameraView)
		: m_Transform(translation),
		  m_Projection(cameraView) {
		UpdateViewProjectionMatrix();
	}

	Camera::Camera(uint32_t width, uint32_t height, float zFar, float zNear)
		: m_Transform(),
		  m_Projection(width, height, zFar, zNear) {
		UpdateViewProjectionMatrix();
	}

	Camera::Camera(float aspectRatio, float fov, float zNear, float zFar, CameraProjection::ProjectionType pType)
		: m_Transform(),
		  m_Projection(aspectRatio, fov, zNear, zFar, pType) {
		UpdateViewProjectionMatrix();
	}

	Camera::Camera(const glm::vec3& translation, uint32_t width, uint32_t height, float zFar, float zNear)
		: m_Transform(translation),
		  m_Projection(width, height, zFar, zNear) {
		UpdateViewProjectionMatrix();
	}

	Camera::Camera(const glm::vec3& translation, float aspectRatio, float fov, float zNear, float zFar, CameraProjection::ProjectionType pType)
		: m_Transform(translation),
		  m_Projection(aspectRatio, fov, zNear, zFar, pType) {
		UpdateViewProjectionMatrix();
	}

	Camera::Camera(TransformComponent cameraTransform, uint32_t width, uint32_t height, float zFar, float zNear)
		: m_Transform(cameraTransform),
		  m_Projection(width, height, zFar, zNear) {
		UpdateViewProjectionMatrix();
	}

	Camera::Camera(TransformComponent cameraTransform, float aspectRatio, float fov, float zNear, float zFar, CameraProjection::ProjectionType pType)
		: m_Transform(cameraTransform),
		  m_Projection(aspectRatio, fov, zNear, zFar, pType) {
		UpdateViewProjectionMatrix();
	}

	void Camera::SetViewOrthographic(float width, float height, float zFar, float zNear) {
		m_Projection.Camera.SetViewOrthographic(width, height, zFar, zNear);
		UpdateViewProjectionMatrix();
	}

	void Camera::SetViewPerspective(float aspectRatio, float fov, float zNear, float zFar) {
		m_Projection.Camera.SetViewPerspective(aspectRatio, fov, zNear, zFar);
		UpdateViewProjectionMatrix();
	}

	void Camera::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_Projection.Camera.SetViewportSize(width, height);
	}

	void Camera::UpdateViewProjectionMatrix() {
		m_ViewProjectionMatrix = GetProjectionMatrix() * GetViewMatrix();
	}

}