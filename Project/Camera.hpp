#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
	public:
	glm::vec3 viewLookAt;
        glm::vec3 viewCameraPos;
	glm::vec3 viewUp;

	Camera() 
		: viewLookAt(glm::vec3()),
                  viewCameraPos(glm::vec3()),
                  viewUp(glm::vec3()) {}

	Camera(glm::vec3 viewLookAt, glm::vec3 viewCameraPos, glm::vec3 viewUp)
		: viewLookAt(viewLookAt),
		  viewCameraPos(viewCameraPos),
		  viewUp(viewUp) {}

	glm::mat4 getViewMatrix() {
		return glm::lookAt(viewCameraPos, viewLookAt, viewUp);
	}
};
	
