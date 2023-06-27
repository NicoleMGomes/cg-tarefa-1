#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader/Shader.h>
#include <GLFW/glfw3.h>

class Camera
{
public:
	Camera()
	{
	}

	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	float cameraSpeed;
	bool firstMouse;
	float lastX;
	float lastY;
	float yaw;
	float pitch;
	float sensitivity;

	void init(GLuint width, GLuint height, Shader *shader)
	{
		// Inicializa propriedades da câmera
		firstMouse = true;
		cameraPos = glm::vec3(0.0, 0.0, 3.0);
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		cameraSpeed = 0.05f;
		sensitivity = 0.05f;
		pitch = 0.0;
		yaw = -90.0;

		// Atualiza propriedades do shader
		glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
		shader->setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);
		shader->setMat4("view", glm::value_ptr(view));
		shader->setMat4("projection", glm::value_ptr(projection));
	}

	void updateShader(GLuint width, GLuint height, Shader *shader)
	{
		// Atualiza view e posição da câmera
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
		shader->setMat4("projection", glm::value_ptr(projection));
		shader->setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);
		shader->setMat4("view", glm::value_ptr(view));
	}

	void key_callback(GLFWwindow *window)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			cameraPos += cameraSpeed * cameraFront;
		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			cameraPos -= cameraSpeed * cameraFront;
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}
	}

	void mouse_callback(GLFWwindow *window, double xpos, double ypos)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float offsetX = xpos - lastX;
		float offsetY = lastY - ypos;

		lastX = xpos;
		lastY = ypos;

		offsetX *= sensitivity;
		offsetY *= sensitivity;

		pitch += offsetY;
		yaw += offsetX;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(front);
	}
};