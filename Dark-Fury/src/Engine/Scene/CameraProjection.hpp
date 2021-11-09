#pragma once

#include <glm/glm.hpp>

namespace Engine {
	static const float DefaultFarFloat = 100.0f;

	class CameraProjection
	{
	public:
		enum class ProjectionType  { Orthographic = 0, Perspective = 1 };

		CameraProjection() : m_Projection(glm::mat4(1.0f)) {
			CameraProjection(1280, 720);
		}
		CameraProjection(uint32_t width, uint32_t height, float zFar = -10.0f, float zNear = 0.0f);
		CameraProjection(float aspectRatio, float fov, float zNear, float zFar, ProjectionType projType = ProjectionType::Perspective);
		~CameraProjection() = default;

		const ProjectionType& GetProjectionType() const { return m_ProjType; }
		void SetProjectionType(ProjectionType projType);

		void SetViewportSize(uint32_t width, uint32_t height);

		void SetViewOrthographic(float width, float height, float zFar = MAX_FLOAT, float zNear = 0.0f);

		void SetViewPerspective(float aspectRatio, float fov, float zNear = 0.01f, float zFar = DefaultFarFloat);

		const glm::mat4& GetProjectionMatrix() const { return m_Projection; }

		void SetFOV(float newFOV) {
			if (m_ProjType == ProjectionType::Orthographic) { m_Zoom = newFOV; }
			else { m_FOV = newFOV; }
			RecalculateProjection();
		}
		void SetFOVPerspective(float newFOV) { m_FOV = newFOV; RecalculateProjection(); }
		void SetFOVOrthographic(float newFOV) { m_Zoom = newFOV; RecalculateProjection(); }
		const float& GetFOV() const { return m_ProjType == ProjectionType::Orthographic ? m_Zoom : m_FOV; }
		const float& GetFOVPerspective() const { return m_FOV; }
		const float& GetFOVOrthographic() const { return m_Zoom; }
		void SetNearClip(float nearClip) {
			if (m_ProjType == ProjectionType::Orthographic) { m_NearOrtho = nearClip; }
			else { m_NearPsp = nearClip; }
			RecalculateProjection();
		}
		void SetNearClipPerspective(float nearClip) { m_NearPsp = nearClip; RecalculateProjection(); }
		void SetNearClipOrthographic(float nearClip) { m_NearOrtho = nearClip; RecalculateProjection(); }
		const float& GetNearClip() const { return m_ProjType == ProjectionType::Orthographic ? m_NearOrtho : m_NearPsp; }
		const float& GetNearClipPerspective() const { return m_NearPsp; }
		const float& GetNearClipOrthographic() const { return m_NearOrtho; }
		void SetFarClip(float farClip) {
			if (m_ProjType == ProjectionType::Orthographic) { m_FarOrtho = farClip; }
			else { m_FarPsp = farClip; }
			RecalculateProjection();
		}
		void SetFarClipPerspective(float farClip) { m_FarPsp = farClip; RecalculateProjection(); }
		void SetFarClipOrthographic(float farClip) { m_FarOrtho = farClip; RecalculateProjection(); }
		const float& GetFarClip() const { return m_ProjType == ProjectionType::Orthographic ? m_FarOrtho : m_FarPsp; }
		const float& GetFarClipPerspective() const { return m_FarPsp; }
		const float& GetFarClipOrthographic() const { return m_FarOrtho; }

	private:
		void RecalculateProjection();

		glm::mat4 m_Projection;
		ProjectionType m_ProjType;
		float m_AspectRatio;

		float m_FOV = 45.0f;
		float m_NearPsp = 0.01f;
		float m_FarPsp = 100.0f;

		float m_Zoom = 1.0f;
		float m_NearOrtho = 1.0f;
		float m_FarOrtho = -1.0f;
	};
}