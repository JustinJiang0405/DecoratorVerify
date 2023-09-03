#pragma once

#include "deco_device.h"
#include "deco_game_object.h"
#include "deco_renderer.h"
#include "deco_window.h"

#include <memory>
#include <vector>

#define MAX_FRAME_TIME 0.03f

namespace Deco
{
	class FirstApp
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;

		void run();

	private:
		void loadGameObjects();

	private:
		DecoWindow m_deco_window{ WIDTH, HEIGHT, "Hello Vulkan!" };
		DecoDevice m_deco_device{ m_deco_window };
		DecoRenderer m_deco_renderer{ m_deco_window, m_deco_device };

		std::vector<DecoGameObject> m_deco_game_objects;
	};
}

