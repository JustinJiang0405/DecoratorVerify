#pragma once

#include "deco_game_object.h"
#include "deco_window.h"

namespace Deco
{
	class KeyboardMovementController
	{
	public:
		struct KeyMappings
		{
			int move_left = GLFW_KEY_A;
			int move_right = GLFW_KEY_D;
			int move_forward = GLFW_KEY_W;
			int move_backward = GLFW_KEY_S;
			int move_up = GLFW_KEY_E;
			int move_down = GLFW_KEY_Q;
			int look_left = GLFW_KEY_LEFT;
			int look_right = GLFW_KEY_RIGHT;
			int look_up = GLFW_KEY_UP;
			int look_down = GLFW_KEY_DOWN;
		};

		void moveInPlaneXZ(GLFWwindow* window, float dt, DecoGameObject& game_object);

		KeyMappings m_keys{};
		float m_move_speed{ 3.0f };
		float m_look_speed{ 1.5f };

	private:

	};
}