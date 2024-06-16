#include "keyboard_movement_controller.h"

namespace Deco
{
	void KeyboardMovementController::moveInPlaneXZ(GLFWwindow* window, float dt, DecoGameObject& game_object)
	{
		// calculate look dir
		glm::vec3 rotate{ 0.0f };
		if (glfwGetKey(window, m_keys.look_right) == GLFW_PRESS)
		{
			rotate.y += 1.0f;
		}
		if (glfwGetKey(window, m_keys.look_left) == GLFW_PRESS)
		{
			rotate.y -= 1.0f;
		}
		if (glfwGetKey(window, m_keys.look_up) == GLFW_PRESS)
		{
			rotate.x += 1.0f;
		}
		if (glfwGetKey(window, m_keys.look_down) == GLFW_PRESS)
		{
			rotate.x -= 1.0f;
		}

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) // check if the rotate is non-zero
		{
			game_object.m_transform.m_rotation += m_look_speed * dt * glm::normalize(rotate);
		}

		game_object.m_transform.m_rotation.x = glm::clamp(game_object.m_transform.m_rotation.x, -1.5f, 1.5f);
		game_object.m_transform.m_rotation.y= glm::mod(game_object.m_transform.m_rotation.y, glm::two_pi<float>());

		// calculate move dir
		float yaw = game_object.m_transform.m_rotation.y;
		const glm::vec3 forward_dir{ sin(yaw), 0.0f, cos(yaw) };
		const glm::vec3 right_dir{ forward_dir.z, 0.0f, -forward_dir.x };
		const glm::vec3 up_dir{ 0.0f, -1.0f, 0.0f };

		glm::vec3 move_dir{ 0.0f };
		if (glfwGetKey(window, m_keys.move_forward) == GLFW_PRESS)
		{
			move_dir += forward_dir;
		}
		if (glfwGetKey(window, m_keys.move_backward) == GLFW_PRESS)
		{
			move_dir -= forward_dir;
		}
		if (glfwGetKey(window, m_keys.move_right) == GLFW_PRESS)
		{
			move_dir += right_dir;
		}
		if (glfwGetKey(window, m_keys.move_left) == GLFW_PRESS)
		{
			move_dir -= right_dir;
		}
		if (glfwGetKey(window, m_keys.move_up) == GLFW_PRESS)
		{
			move_dir += up_dir;
		}
		if (glfwGetKey(window, m_keys.move_down) == GLFW_PRESS)
		{
			move_dir -= up_dir;
		}

		if (glm::dot(move_dir, move_dir) > std::numeric_limits<float>::epsilon()) // check if the move_dir is non-zero
		{
			game_object.m_transform.m_translation += m_move_speed * dt * glm::normalize(move_dir);
		}
	}
}


