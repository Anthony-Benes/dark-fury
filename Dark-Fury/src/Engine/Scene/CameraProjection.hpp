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

		const ProjectionType& GetType() const { return m_ProjType; }

		void SetViewportSize(uint32_t width, uint32_t height);

		void SetViewOrthographic(float width, float height, float zFar = MAX_FLOAT, float zNear = 0.0f);

		void SetViewPerspective(float aspectRatio, float fov, float zNear = 0.01f, float zFar = DefaultFarFloat);

		const glm::mat4& GetProjectionMatrix() const { return m_Projection; }

	private:
		void RecalculateProjection();

		glm::mat4 m_Projection;
		ProjectionType m_ProjType;
		float m_AspectRatio;
		float m_FOVorZoom;
		float m_NearClip;
		float m_FarClip;
	};
}