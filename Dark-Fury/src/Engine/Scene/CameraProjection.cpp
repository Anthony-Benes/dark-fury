#include "engpch.h"
#include "Engine/Scene/CameraProjection.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Engine {

	CameraProjection::CameraProjection(uint32_t width, uint32_t height, float zFar, float zNear)
		: CameraProjection(((float)width / (float)height), ((float)height / 2.0f), zNear, zFar, ProjectionType::Orthographic) {}

	CameraProjection::CameraProjection(float aspectRatio, float fov, float zNear, float zFar, ProjectionType projType)
		: m_Projection(glm::mat4(1.0f)),
		  m_ProjType(projType),
		  m_AspectRatio(aspectRatio),
		  m_FOVorZoom(fov),
		  m_NearClip(zNear),
		  m_FarClip(zFar) {
		if (projType == ProjectionType::Orthographic) {
			SetViewOrthographic((m_AspectRatio * m_FOVorZoom * 2.0f), (m_FOVorZoom * 2.0f), m_FarClip, m_NearClip);
		}
		else {
			SetViewPerspective(m_AspectRatio, m_FOVorZoom, m_NearClip, m_FarClip);
		}
	}

	void CameraProjection::SetViewportSize(uint32_t width, uint32_t height)
	{
		ENG_CORE_ASSERT(width > 0 && height > 0);
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}

	void CameraProjection::SetViewOrthographic(float width, float height, float zFar, float zNear)
	{
		m_ProjType = ProjectionType::Orthographic;
		m_AspectRatio = width / height;
		m_FOVorZoom = height / 2.0f;
		m_NearClip = zNear == MAX_FLOAT ? m_NearClip : zNear;
		m_FarClip = zFar == MAX_FLOAT ? m_FarClip : zFar;
		RecalculateProjection();
	}

	void CameraProjection::SetViewPerspective(float aspectRatio, float fov, float zNear, float zFar)
	{
		m_ProjType = ProjectionType::Perspective;
		m_AspectRatio = aspectRatio;
		m_FOVorZoom = fov;
		m_NearClip = zNear == MAX_FLOAT ? m_NearClip : zNear;
		m_FarClip = zFar == MAX_FLOAT ? m_FarClip : zFar;
		RecalculateProjection();
	}

	void CameraProjection::RecalculateProjection()
	{
		if (m_ProjType == ProjectionType::Orthographic) {
			if (m_FarClip == MAX_FLOAT) { m_FarClip = -DefaultFarFloat; }
			float xHalf = m_FOVorZoom * m_AspectRatio;
			m_Projection = glm::ortho(-xHalf, xHalf, -m_FOVorZoom, m_FOVorZoom, -m_NearClip, -m_FarClip);
		}
		else {
			m_Projection = glm::perspective(DEG_TO_RAD(m_FOVorZoom), m_AspectRatio, m_NearClip, m_FarClip);
		}
	}

}