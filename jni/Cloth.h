#ifndef _CLOTH_H_
#define _CLOTH_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "Types.h"

// ----------------------------------------------------------------------------------------------------------------------------

#define CLOTH_SIZE_X			25
#define CLOTH_SIZE_Y			21

// ----------------------------------------------------------------------------------------------------------------------------

class Cloth
{
public:
	Cloth();
	~Cloth();

	void Initialize(glm::vec4 gravity, float friction);
	void Update(float frameTime);
	int FindNearestIndex(glm::vec3 nearPoint, glm::vec3 farPoint);
	void UpdateTouchPosition(int index, glm::vec3 nearPoint, glm::vec3 farPoint);

	int m_ClothParticlesCount;
	Particle m_ClothParticles[POINTS_MAX_COUNT];

	int m_ClothLinesCount;
	GLushort m_ClothLines[POINTS_MAX_COUNT];

    float m_PointSize;
    float m_LineWidth;

    int m_Touches[TOUCHES_MAX_COUNT];
	glm::vec4 *m_TouchPoints[TOUCHES_MAX_COUNT];
	glm::vec4 m_TouchPositions[TOUCHES_MAX_COUNT];

private:
	glm::vec4 m_Gravity;
	float m_Friction;

	glm::vec4 m_ClothLastParticles[POINTS_MAX_COUNT];

	int m_ConstraintsCount;
	StiffnessConstraint m_Constraints[POINTS_MAX_COUNT];
	int m_PinsCount;
	PinConstraint m_Pins[POINTS_MAX_COUNT];
};

#endif // _CLOTH_H_