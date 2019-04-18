#ifndef _SHADOW_H_
#define _SHADOW_H_

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Types.h"

// ----------------------------------------------------------------------------------------------------------------------------

class Shadow
{
public:
	Shadow();
	~Shadow();

	void Initialize(Particle* points, int pointsCount);
	void Update();

    float m_PointSize;
    float m_LineWidth;

	Particle m_ShadowPoints[POINTS_MAX_COUNT];

private:
	int m_PointsCount;
	Particle* m_Points;

	glm::vec4 m_GroundPlane;
	glm::vec3 m_LightDirection;
	glm::mat4 m_ShadowProjectionMatrix;
};

#endif // _SHADOW_H_
