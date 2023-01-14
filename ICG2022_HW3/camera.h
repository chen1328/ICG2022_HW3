#ifndef CAMERA_H
#define CAMERA_H

#include "headers.h"
using namespace std;


// Camera Declarations.
class Camera
{
public:
	// Camera Public Methods.
	Camera(const float aspectRatio);
	~Camera();

	glm::vec3   GetCameraPos()  const { return position; }
	glm::mat4x4 GetViewMatrix() const { return viewMatrix; }
	glm::mat4x4 GetProjMatrix() const { return projMatrix; }

	glm::quat GetDirection() const { return glm::quat(glm::vec3(-pitch, -yaw, 0.0f)); }
	glm::vec3 GetForward()   const { return glm::rotate(GetDirection(), glm::vec3(0.0f, 0.0f, -1.0f)); }
	glm::vec3 GetRight()     const { return glm::rotate(GetDirection(), glm::vec3(1.0f, 0.0f, 0.0f)); }
	glm::vec3 GetUp()        const { return glm::rotate(GetDirection(), glm::vec3(0.0f, 1.0f, 0.0f)); }

	void UpdateView();
	void UpdateView(const glm::vec3 newPos, const glm::vec3 newTarget, const glm::vec3 up);
	void UpdateProjection(const float fovyInDegree, const float aspectRatio, const float zNear, const float zFar);

	void ProcessMouseInput(int button, int state, int x, int y);
	void ProcessMouseMotion(int x, int y);

private:
	// Camera Private Data.
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 focus;
	glm::vec2 lastPos;

	float distance;
	float pitch;
	float yaw;
	float fovy; // in degree.
	float aspectRatio;
	float nearPlane;
	float farPlane;

	const float movementSpeed = 0.0008f;
	const float rotationSpeed = 0.008f;
	vector<bool> buttonStates;

	glm::mat4x4 viewMatrix;
	glm::mat4x4 projMatrix;
};

#endif
