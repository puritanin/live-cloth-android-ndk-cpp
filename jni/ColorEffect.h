#ifndef _COLOREFFECT_H_
#define _COLOREFFECT_H_

#include <stdlib.h>
#include <time.h>

#include <glm/glm.hpp>

#include "Types.h"

// ----------------------------------------------------------------------------------------------------------------------------

#define KEY_POINT_INTERVAL		5

// ----------------------------------------------------------------------------------------------------------------------------

class ColorEffect
{
public:
	ColorEffect();
	~ColorEffect();

	void Initialize(Particle* points, int sizeX, int sizeY, glm::vec4 baseColor);
	void Update();
	void SetMode1();
	void SetMode2(bool touched, int index, int x, int y);

private:
	int m_PointsCount;
	Particle* m_Points;

	int m_SizeX;
	int m_SizeY;

	glm::vec4 m_BaseColor;

	ColorPoint m_ColorPoints[POINTS_MAX_COUNT];

	bool m_Touches[TOUCHES_MAX_COUNT];
    glm::vec2 m_TouchPositions[TOUCHES_MAX_COUNT];

    enum ModeState { MODE1, MODE2 };
	ModeState m_Mode;

	int _keyIndicesCount;
	int _keyIndices[POINTS_MAX_COUNT];
	int _midIndicesCount;
	int _midIndices[POINTS_MAX_COUNT];
};

#endif // _COLOREFFECT_H_