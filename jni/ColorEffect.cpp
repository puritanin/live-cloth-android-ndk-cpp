#include "ColorEffect.h"

// ----------------------------------------------------------------------------------------------------------------------------

ColorEffect::ColorEffect()
{
}

ColorEffect::~ColorEffect()
{
}

void ColorEffect::Initialize(Particle* points, int sizeX, int sizeY, glm::vec4 baseColor)
{
	srand(time(NULL));

	m_Mode = MODE1;

	m_Points = points;
	m_SizeX = sizeX;
	m_SizeY = sizeY;
	m_PointsCount = m_SizeX * m_SizeY;

	m_BaseColor = baseColor;

	for (int i = 0; i < TOUCHES_MAX_COUNT; i++) {
	    m_Touches[i] = false;
	}

	for (int j = 0; j < m_SizeY; j++)
	{
		for (int i = 0; i < m_SizeX; i++)
		{
			int index = j * m_SizeX + i;

			m_ColorPoints[index].targetPoint = &m_Points[index];
			m_ColorPoints[index].x = i;
			m_ColorPoints[index].y = j;

			if (j % KEY_POINT_INTERVAL == 0 && i % KEY_POINT_INTERVAL == 0)
			{
				// key point
				m_ColorPoints[index].t = 0;
				m_ColorPoints[index].direction = RANDOM(0.0f, 2.0f) < 1.0f ? 1.0f : -1.0f;
				m_ColorPoints[index].beginValue = 0.5f - RANDOM(0.0f, 0.3f) * m_ColorPoints[index].direction;
				m_ColorPoints[index].endValue = 0.5f + RANDOM(0.0f, 0.3f) * m_ColorPoints[index].direction;
				m_ColorPoints[index].value = m_ColorPoints[index].beginValue;

				_keyIndices[_keyIndicesCount] = index;
				_keyIndicesCount++;
			}
			else
			{
				// mid point
				int keyX = i / KEY_POINT_INTERVAL;
				int keyY = j / KEY_POINT_INTERVAL;

				m_ColorPoints[index].localX = (float)(i - keyX * KEY_POINT_INTERVAL) / KEY_POINT_INTERVAL;
				m_ColorPoints[index].localY = (float)(j - keyY * KEY_POINT_INTERVAL) / KEY_POINT_INTERVAL;

				int x0 = keyX * KEY_POINT_INTERVAL;
				int y0 = keyY * KEY_POINT_INTERVAL;
				int indexSafe = y0 * m_SizeX + x0;
				m_ColorPoints[index].keyIndex0 = indexSafe;

				// warning: key indices 1,2,3 maybe greater than size of points array

				x0 = (keyX + 1) * KEY_POINT_INTERVAL;
				y0 = keyY * KEY_POINT_INTERVAL;
				int indexNotSafe = y0 * m_SizeX + x0;
				m_ColorPoints[index].keyIndex1 = (x0 < m_SizeX && y0 < m_SizeY) ? indexNotSafe : indexSafe;

				x0 = keyX * KEY_POINT_INTERVAL;
				y0 = (keyY + 1) * KEY_POINT_INTERVAL;
				indexNotSafe = y0 * m_SizeX + x0;
				m_ColorPoints[index].keyIndex2 = (x0 < m_SizeX && y0 < m_SizeY) ? indexNotSafe : indexSafe;

				x0 = (keyX + 1) * KEY_POINT_INTERVAL;
				y0 = (keyY + 1) * KEY_POINT_INTERVAL;
				indexNotSafe = y0 * m_SizeX + x0;
				m_ColorPoints[index].keyIndex3 = (x0 < m_SizeX && y0 < m_SizeY) ? indexNotSafe : indexSafe;

				_midIndices[_midIndicesCount] = index;
				_midIndicesCount++;
			}
		}
	}
}

void ColorEffect::Update()
{
	switch(m_Mode)
	{
		case MODE1:
			for (int i = 0; i < _keyIndicesCount; i++)
			{
				int index = _keyIndices[i];
				m_ColorPoints[index].t += 0.0167f; // [1/60]

				if (m_ColorPoints[index].t > 1.0f)
				{
					m_ColorPoints[index].t = 0;
					m_ColorPoints[index].direction = -m_ColorPoints[index].direction;
					m_ColorPoints[index].beginValue = m_ColorPoints[index].endValue;
					m_ColorPoints[index].endValue = 0.5f + RANDOM(0.0f, 0.3f) * m_ColorPoints[index].direction;
				}

				float brightness = (m_ColorPoints[index].endValue - m_ColorPoints[index].beginValue) * m_ColorPoints[index].t + m_ColorPoints[index].beginValue;
				m_ColorPoints[index].value = brightness;

				m_Points[index].color = m_BaseColor * brightness;
			}

			for (int i = 0; i < _midIndicesCount; i++)
			{
				int index = _midIndices[i];
				int index0 = m_ColorPoints[index].keyIndex0;
				int index1 = m_ColorPoints[index].keyIndex1;
				int index2 = m_ColorPoints[index].keyIndex2;
				int index3 = m_ColorPoints[index].keyIndex3;

				float brightness01 = glm::mix(m_ColorPoints[index0].value, m_ColorPoints[index1].value, m_ColorPoints[index].localX);
				float brightness23 = glm::mix(m_ColorPoints[index2].value, m_ColorPoints[index3].value, m_ColorPoints[index].localX);
				float brightness = glm::mix(brightness01, brightness23, m_ColorPoints[index].localY);
				m_ColorPoints[index].value = brightness;

				m_Points[index].color = m_BaseColor * brightness;
			}
			break;

		case MODE2:
			for (int i = 0; i < m_PointsCount; i++)
			{
				if (m_ColorPoints[i].t < 1.0f)
				{
					m_ColorPoints[i].t += 0.0333f; // [1/30]

					float brightness = (m_ColorPoints[i].endValue - m_ColorPoints[i].beginValue) * m_ColorPoints[i].t + m_ColorPoints[i].beginValue;
					m_ColorPoints[i].value = brightness;

					m_Points[i].color = m_BaseColor * brightness;
				}
			}
			break;

		default:
			break;
	}
}

void ColorEffect::SetMode1()
{
	if (m_Mode == MODE1) return;
	m_Mode = MODE1;

	for (int i = 0; i < _keyIndicesCount; i++)
	{
		int index = _keyIndices[i];

		m_ColorPoints[index].beginValue = m_ColorPoints[index].value;
		m_ColorPoints[index].direction = m_ColorPoints[index].beginValue < 0.5f ? 1.0f : -1.0f;
		m_ColorPoints[index].endValue = 0.5f + RANDOM(0.0f, 0.3f) * m_ColorPoints[index].direction;
		m_ColorPoints[index].t = 0;
	}
}

void ColorEffect::SetMode2(bool touched, int index, int x, int y)
{
	m_Mode = MODE2;

	if (touched) {
		m_Touches[index] = true;
		m_TouchPositions[index] = glm::vec2(x, y);
	}
	else {
		m_Touches[index] = false;
	}

	// init
	for (int i = 0; i < _midIndicesCount; i++)
	{
		int index = _midIndices[i];

		m_ColorPoints[index].beginValue = m_ColorPoints[index].value;
		m_ColorPoints[index].endValue = 0;
		m_ColorPoints[index].t = 0;
	}

	for (int i = 0; i < _keyIndicesCount; i++)
	{
		int index = _keyIndices[i];

		m_ColorPoints[index].beginValue = m_ColorPoints[index].value;
		m_ColorPoints[index].endValue = 0;
		m_ColorPoints[index].t = 0;
	}

	// fill
	for (int i = 0; i < TOUCHES_MAX_COUNT; i++)
	{
		if (m_Touches[i])
		{
			glm::vec2 p = m_TouchPositions[i];

			float length1 = sqrt(pow(p.x, 2.0f) + pow(p.y, 2.0f));
			float length2 = sqrt(pow(p.x - (float)m_SizeX, 2.0f) + pow(p.y, 2.0f));
			float length3 = sqrt(pow(p.x, 2.0f) + pow(p.y - (float)m_SizeY, 2.0f));
			float length4 = sqrt(pow(p.x - (float)m_SizeX, 2.0f) + pow(p.y - (float)m_SizeY, 2.0f));
			float lengthMax = MAX(MAX(length1, length2), MAX(length3, length4));

			for (int i = 0; i < _midIndicesCount; i++)
			{
				int index = _midIndices[i];

				float endValue = 1.0f - sqrt(pow(p.x - m_ColorPoints[index].x, 2.0f) + pow(p.y - m_ColorPoints[index].y, 2.0f)) / lengthMax;
				m_ColorPoints[index].endValue = MAX(m_ColorPoints[index].endValue, endValue);
			}

			for (int i = 0; i < _keyIndicesCount; i++)
			{
				int index = _keyIndices[i];

				float endValue = 1.0f - sqrt(pow(p.x - m_ColorPoints[index].x, 2.0f) + pow(p.y - m_ColorPoints[index].y, 2.0f)) / lengthMax;
				m_ColorPoints[index].endValue = MAX(m_ColorPoints[index].endValue, endValue);
			}
		}
	}
}
