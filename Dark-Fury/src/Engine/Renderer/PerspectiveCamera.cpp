#include "engpch.h"
#include "Engine/Renderer/PerspectiveCamera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Engine {

	PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float farZ, float nearZ)
		: Camera(glm::perspective(DEG_TO_RAD(fov), aspectRatio, nearZ, farZ)),
		m_aspectRatio(aspectRatio),
		m_FOV(fov) {
		ENG_PROFILE_FUNCTION();

		RecalculateViewMatrix();
	}

	void PerspectiveCamera::SetProjection(float fov, float aspectRatio, float farZ, float nearZ)
	{
		ENG_PROFILE_FUNCTION();
		m_aspectRatio = aspectRatio;
		m_FOV = fov;
		m_Projection = glm::perspective(DEG_TO_RAD(fov), m_aspectRatio, nearZ, farZ);
		m_ViewProjectionMatrix = m_Projection * m_ViewMatrix;
	}

	void PerspectiveCamera::RecalculateViewMatrix()
	{
		ENG_PROFILE_FUNCTION();

		glm::vec3 direction;
		direction.x = sin(DEG_TO_RAD(m_Rotation.y)) * cos(DEG_TO_RAD(m_Rotation.x));
		direction.y = sin(DEG_TO_RAD(m_Rotation.x));
		direction.z = -1 * cos(DEG_TO_RAD(m_Rotation.y)) * cos(DEG_TO_RAD(m_Rotation.x));
		direction = glm::normalize(direction);

		m_ViewMatrix = glm::lookAt(m_Position, m_Position + direction, glm::vec3(0, 1, 0));
		m_ViewProjectionMatrix = m_Projection * m_ViewMatrix;
	}

}