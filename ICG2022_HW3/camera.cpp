#include "camera.h"
using namespace std;


Camera::Camera(const float aspectRatio)
{
	// Default camera pose and parameters.
	position = glm::vec3(0.0f, 0.0f, 3.0f);
	target = glm::vec3(0.0f, 0.0f, 0.0f);
	focus = glm::vec3(0.0f, 0.0f, 0.0f);
	lastPos = glm::vec2(0.0f, 0.0f);

	distance = 5.0f;
	pitch = 0.0f;
	yaw = 0.0f;
	fovy = 45.0f;
	nearPlane = 0.1f;
	farPlane = 1000.0f;

	buttonStates = vector<bool>(5, false);
	UpdateView(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
	UpdateProjection(fovy, aspectRatio, nearPlane, farPlane);
}

Camera::~Camera()
{}

void Camera::UpdateView()
{
	position = focus - GetForward() * distance;
	glm::quat orientation = GetDirection();
	viewMatrix = glm::inverse(translate(glm::mat4(1.0f), position) * glm::toMat4(orientation));
}

void Camera::UpdateView(const glm::vec3 newPos, const glm::vec3 newTarget, const glm::vec3 up)
{
	position = newPos;
	target = newTarget;
	viewMatrix = glm::lookAt(position, target, up);
}

void Camera::UpdateProjection(const float fovyInDegree, const float aspectRatio, const float zNear, const float zFar)
{
	fovy = fovyInDegree;
	nearPlane = zNear;
	farPlane = zFar;
	projMatrix = glm::perspective(glm::radians(fovyInDegree), aspectRatio, nearPlane, farPlane);
}

void Camera::ProcessMouseInput(int button, int state, int x, int y)
{
	buttonStates = vector<bool>(5, false);
	if (state == GLUT_DOWN)
	{
		buttonStates[button] = true;
		if (button == 3)
		{
			distance -= 0.5f;
			distance = max(distance, 0.0f);
			UpdateView();
		}
		else if (button == 4)
		{
			distance += 0.5f;
			distance = min(distance, 50.0f);
			UpdateView();
		}
		lastPos = glm::vec2(x * 1.0f, y * 1.0f);
	}
}

void Camera::ProcessMouseMotion(int x, int y)
{
	glm::vec2 curPos = { x * 1.0f, y * 1.0f };
	glm::vec2 delta = (curPos - lastPos);
	if (buttonStates[GLUT_LEFT_BUTTON])
	{
		focus += -GetRight() * delta.x * distance * movementSpeed;
		focus += GetUp() * delta.y * distance * movementSpeed;
		UpdateView();
	}
	else if (buttonStates[GLUT_RIGHT_BUTTON])
	{
		yaw += delta.x * rotationSpeed;
		pitch += delta.y * rotationSpeed;
		UpdateView();
	}
	lastPos = curPos;
}