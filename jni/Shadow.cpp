#include "Shadow.h"

// ----------------------------------------------------------------------------------------------------------------------------

Shadow::Shadow()
{
}

Shadow::~Shadow()
{
}

void Shadow::Initialize(Particle* points, int pointsCount)
{
    m_PointSize = 2.0f;
    m_LineWidth = 1.0f;

	m_Points = points;
	m_PointsCount = pointsCount;

	m_GroundPlane = glm::vec4(glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)), 1.3f);
	m_LightDirection = glm::normalize(glm::vec3(-0.5f, -1.5f, -1.0f));

	m_ShadowProjectionMatrix = glm::mat4(0.0f);

	m_ShadowProjectionMatrix[0][0] = m_GroundPlane.y * m_LightDirection.y + m_GroundPlane.z * m_LightDirection.z;
	m_ShadowProjectionMatrix[0][1] = -m_GroundPlane.x * m_LightDirection.y;
	m_ShadowProjectionMatrix[0][2] = -m_GroundPlane.x * m_LightDirection.z;
	m_ShadowProjectionMatrix[0][3] = 0.0f;

	m_ShadowProjectionMatrix[1][0] = -m_GroundPlane.y * m_LightDirection.x;
	m_ShadowProjectionMatrix[1][1] = m_GroundPlane.x * m_LightDirection.x + m_GroundPlane.z * m_LightDirection.z;
	m_ShadowProjectionMatrix[1][2] = -m_GroundPlane.y * m_LightDirection.z;
	m_ShadowProjectionMatrix[1][3] = 0.0f;

	m_ShadowProjectionMatrix[2][0] = -m_GroundPlane.z * m_LightDirection.x;
	m_ShadowProjectionMatrix[2][1] = -m_GroundPlane.z * m_LightDirection.y;
	m_ShadowProjectionMatrix[2][2] = m_GroundPlane.x * m_LightDirection.x + m_GroundPlane.y * m_LightDirection.y;
	m_ShadowProjectionMatrix[2][3] = 0.0f;

	m_ShadowProjectionMatrix[3][0] = -m_GroundPlane.w * m_LightDirection.x;
	m_ShadowProjectionMatrix[3][1] = -m_GroundPlane.w * m_LightDirection.y;
	m_ShadowProjectionMatrix[3][2] = -m_GroundPlane.w * m_LightDirection.z;
	m_ShadowProjectionMatrix[3][3] = m_GroundPlane.x * m_LightDirection.x + m_GroundPlane.y * m_LightDirection.y + m_GroundPlane.z * m_LightDirection.z;
}

void Shadow::Update()
{
	for (int i = 0; i < m_PointsCount; i++)
	{
		m_ShadowPoints[i].vertex = m_ShadowProjectionMatrix * m_Points[i].vertex;
		m_ShadowPoints[i].vertex = m_ShadowPoints[i].vertex / m_ShadowPoints[i].vertex.w;

		float distance = glm::distance(m_Points[i].vertex, m_ShadowPoints[i].vertex);
		
		distance /= 2.5f;
		if (distance > 1.0f) distance = 1.0f;

		m_ShadowPoints[i].color = glm::vec4(glm::vec3(0.3f + 0.55f * distance), 1.0f);
	}
}
