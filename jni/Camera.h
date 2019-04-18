#ifndef _CAMERA_H_
#define _CAMERA_H_

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// ----------------------------------------------------------------------------------------------------------------------------

class Camera
{
public:
	Camera();
	~Camera();

	void Initialize(float viewAngle);
	void SetProjection(float angle, int width, int height, float nearPlane, float farPlane);
    void ChangeViewAngle(float dAngle);

	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;
	glm::mat4 m_ModelViewProjectionMatrix;

private:
	void CalculateViewMatrix();

	int _width, _height;
	float _viewAngleValue;
};

#endif // _CAMERA_H_