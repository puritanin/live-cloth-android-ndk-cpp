#include "Camera.h"

// ----------------------------------------------------------------------------------------------------------------------------

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::Initialize(float viewAngle)
{
	_viewAngleValue = viewAngle;
	CalculateViewMatrix();
}

void Camera::CalculateViewMatrix()
{
	m_ViewMatrix = glm::mat4(1.0f);
	m_ViewMatrix = glm::translate(m_ViewMatrix, glm::vec3(0, 0, -4.0f));
	m_ViewMatrix = glm::rotate(m_ViewMatrix, _viewAngleValue, glm::vec3(0, 1, 0));

	m_ModelViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void Camera::SetProjection(float angle, int width, int height, float nearPlane, float farPlane)
{
	_width = width;
	_height = height;
	m_ProjectionMatrix = glm::perspective(angle, (float)width / (float)height, nearPlane, farPlane);
	m_ModelViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void Camera::ChangeViewAngle(float dAngle)
{
    _viewAngleValue -= dAngle;
    CalculateViewMatrix();
}
