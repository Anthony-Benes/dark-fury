#include "engpch.h"
#include "Engine/Scene/CameraProjection.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Engine {

	CameraProjection::CameraProjection(uint32_t width, uint32_t height, float zFar, float zNear)
		: CameraProjection(((float)width / (float)height), ((float)height / 2.0f), zNear, zFar, ProjectionType::Orthographic) {}

	CameraProjection::CameraProjection(float aspectRatio, float fov, float zNear, float zFar, ProjectionType projType)
		: m_Projection(glm::mat4(1.0f)),
		  m_ProjType(projType),
		  m_AspectRatio(aspectRatio) {
		if (projType == ProjectionType::Orthographic) {
			m_Zoom = fov;
			m_NearOrtho = zNear;
			m_FarOrtho = zFar;
			SetViewOrthographic((m_AspectRatio * m_Zoom * 2.0f), (m_Zoom * 2.0f), m_FarOrtho, m_NearOrtho);
		}
		else {
			m_FOV = fov;
			m_NearPsp = zNear;
			m_FarPsp = zFar;
			SetViewPerspective(m_AspectRatio, m_FOV, m_NearPsp, m_FarPsp);
		}
	}

	void CameraProjection::SetProjectionType(ProjectionType projType)
	{
		if (projType != m_ProjType) {
			m_ProjType = projType;
			RecalculateProjection();
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
		m_Zoom = height / 2.0f;
		m_NearOrtho = zNear == MAX_FLOAT ? m_NearOrtho : zNear;
		m_FarOrtho = zFar == MAX_FLOAT ? m_FarOrtho : zFar;
		RecalculateProjection();
	}

	void CameraProjection::SetViewPerspective(float aspectRatio, float fov, float zNear, float zFar)
	{
		m_ProjType = ProjectionType::Perspective;
		m_AspectRatio = aspectRatio;
		m_FOV = fov;
		m_NearPsp = zNear == MAX_FLOAT ? m_NearPsp : zNear;
		m_FarPsp = zFar == MAX_FLOAT ? m_FarPsp : zFar;
		RecalculateProjection();
	}

	void CameraProjection::RecalculateProjection()
	{
		if (m_ProjType == ProjectionType::Orthographic) {
			if (m_FarOrtho == MAX_FLOAT) { m_FarOrtho = -DefaultFarFloat; }
			float xHalf = m_Zoom * m_AspectRatio;
			m_Projection = glm::ortho(-xHalf, xHalf, -m_Zoom, m_Zoom, -m_NearOrtho, -m_FarOrtho);
		}
		else {
			m_Projection = glm::perspective(DEG_TO_RAD(m_FOV), m_AspectRatio, m_NearPsp, m_FarPsp);
		}
	}

}