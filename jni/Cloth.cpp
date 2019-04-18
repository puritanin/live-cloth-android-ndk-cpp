#include "Cloth.h"

// ----------------------------------------------------------------------------------------------------------------------------

Cloth::Cloth()
{
}

Cloth::~Cloth()
{
}

void Cloth::Initialize(glm::vec4 gravity, float friction)
{
    m_PointSize = 3.0f;
    m_LineWidth = 2.0f;

	m_Gravity = gravity;
	m_Friction = friction;

    // touches
    for (int i = 0; i < TOUCHES_MAX_COUNT; i++) {
        m_Touches[i] = -1;
    }

	// points
	m_ClothParticlesCount = 0;
	float y = 1.3f;
	for (int j = 0; j < CLOTH_SIZE_Y; j++)
	{
		float x = -1.2f;
		for (int i = 0; i < CLOTH_SIZE_X; i++)
		{
			// vertex
			m_ClothParticles[m_ClothParticlesCount].vertex = glm::vec4(x, y, 0.0f, 1.0f);
			m_ClothLastParticles[m_ClothParticlesCount] = glm::vec4(x, y, 0.0f, 1.0f);
			// color
			m_ClothParticles[m_ClothParticlesCount].color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			m_ClothParticlesCount++;
			x += 0.1f;
		}
		y -= 0.1f;
	}

	// lines
	m_ClothLinesCount = 0;
	for (int j = 0; j < CLOTH_SIZE_Y; j++)
	{
		for (int i = 0; i < CLOTH_SIZE_X - 1; i++)
		{
			m_ClothLines[m_ClothLinesCount++] = j * CLOTH_SIZE_X + i;
			m_ClothLines[m_ClothLinesCount++] = j * CLOTH_SIZE_X + i + 1;
		}
	}
	for (int j = 0; j < CLOTH_SIZE_Y - 1; j++)
	{
		for (int i = 0; i < CLOTH_SIZE_X; i++)
		{
			m_ClothLines[m_ClothLinesCount++] = j * CLOTH_SIZE_X + i;
			m_ClothLines[m_ClothLinesCount++] = (j + 1) * CLOTH_SIZE_X + i;
		}
	}

	// constraints
	m_ConstraintsCount = 0;
	for (int i = 0; i < m_ClothLinesCount; i += 2)
	{
		m_Constraints[m_ConstraintsCount].aPoint = &m_ClothParticles[m_ClothLines[i]].vertex;
		m_Constraints[m_ConstraintsCount].bPoint = &m_ClothParticles[m_ClothLines[i + 1]].vertex;
		m_Constraints[m_ConstraintsCount].distance = 0.98f * glm::distance(*m_Constraints[m_ConstraintsCount].aPoint, *m_Constraints[m_ConstraintsCount].bPoint);
		m_Constraints[m_ConstraintsCount].stiffness = 0.8f;
		m_ConstraintsCount++;
	}

	// pins
	m_PinsCount = 0;
	for (int i = 0; i < CLOTH_SIZE_X; i += 4)
	{
		m_Pins[m_PinsCount].aPoint = &m_ClothParticles[i].vertex;
		m_Pins[m_PinsCount].position = m_ClothParticles[i].vertex;
		m_PinsCount++;
	}
}

void Cloth::Update(float frameTime)
{
	// points
	for (int i = 0; i < m_ClothParticlesCount; i++)
	{
		glm::vec4 velocity = m_ClothParticles[i].vertex - m_ClothLastParticles[i];
		velocity = velocity * m_Friction;
		// save last good state
		m_ClothLastParticles[i] = m_ClothParticles[i].vertex;
		// gravity
		m_ClothParticles[i].vertex = m_ClothParticles[i].vertex + m_Gravity;
		// inertia
		m_ClothParticles[i].vertex = m_ClothParticles[i].vertex + velocity;
	}

	// touches
    for (int i = 0; i < TOUCHES_MAX_COUNT; i++)
    {
        if (m_Touches[i] != -1) {
            *m_TouchPoints[i] = m_TouchPositions[i];
        }
    }

	// constraints
	float step = 1.0f / 5;
	for (int n = 0; n < 5; n++)
	{
		for (int i = 0; i < m_ConstraintsCount; i++)
		{
			glm::vec4 normal = *m_Constraints[i].aPoint - *m_Constraints[i].bPoint;
			float m = glm::dot(normal, normal);
			float scalar = ((m_Constraints[i].distance * m_Constraints[i].distance - m) / m) * m_Constraints[i].stiffness * step;
			normal = normal * scalar;
			*m_Constraints[i].aPoint = *m_Constraints[i].aPoint + normal;
			*m_Constraints[i].bPoint = *m_Constraints[i].bPoint - normal;
		}
	}

	// pins
	for (int i = 0; i < m_PinsCount; i++)
	{
		*m_Pins[i].aPoint = m_Pins[i].position;
	}

	// ground restrict
	for (int i = 0; i < m_ClothParticlesCount; i++)
	{
		if (m_ClothParticles[i].vertex.y < -1.3f) m_ClothParticles[i].vertex.y = -1.3f;
	}
}

int Cloth::FindNearestIndex(glm::vec3 nearPoint, glm::vec3 farPoint)
{
	int nearestIndex = -1;
	float minDistance = 99.0f;

	for (int i = 0; i < m_ClothParticlesCount; i++)
	{
		glm::vec3 v1 = glm::vec3(m_ClothParticles[i].vertex) - nearPoint;
		glm::vec3 v2 = glm::vec3(m_ClothParticles[i].vertex) - farPoint;
		glm::vec3 v3 = farPoint - nearPoint;

		float d = glm::length(glm::cross(v1, v2)) / glm::length(v3);
		if (d < minDistance) {
			minDistance = d;
			nearestIndex = i;
		}
	}

	return minDistance < 0.1f ? nearestIndex : -1;
}

void Cloth::UpdateTouchPosition(int index, glm::vec3 nearPoint, glm::vec3 farPoint)
{
	// lerp = x * (1 - a) + (y * a)
	m_TouchPositions[index] = glm::vec4(nearPoint * (1 - 0.5f) + farPoint * 0.5f, 1);
}
