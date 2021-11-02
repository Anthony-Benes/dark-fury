#include "engpch.h"
#include "Engine/Renderer/OrthographicCamera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Engine {

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float zFar, float zNear)
		: Camera(glm::ortho(left, right, bottom, top, -zNear, -zFar)) {
		ENG_PROFILE_FUNCTION();

		m_ViewProjectionMatrix = m_Projection * m_ViewMatrix;
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top, float zFar, float zNear)
	{
		ENG_PROFILE_FUNCTION();

		m_Projection = glm::ortho(left, right, bottom, top, -zNear, -zFar);
		m_ViewProjectionMatrix = m_Projection * m_ViewMatrix;
	}

	void OrthographicCamera::SetProjection(float zoom, float aspectRatio, float farZ, float nearZ)
	{
		ENG_PROFILE_FUNCTION();

		m_Projection = glm::ortho( (-aspectRatio * zoom), (aspectRatio * zoom), -zoom, zoom, -nearZ, -farZ);
		m_ViewProjectionMatrix = m_Projection * m_ViewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		ENG_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_Projection * m_ViewMatrix;
	}

}