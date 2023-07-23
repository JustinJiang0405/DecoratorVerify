#pragma once

#include "deco_device.h"
#include "deco_game_object.h"
#include "deco_pipeline.h"
#include "deco_swap_chain.h"
#include "deco_window.h"

#include <memory>
#include <vector>

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
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int image_index);
		void renderGameObjects(VkCommandBuffer command_buffer);

		void sierpinski(std::vector<DecoModel::Vertex>& vertices, int depth, glm::vec2 left, glm::vec2 right, glm::vec2 top);
	private:
		DecoWindow m_deco_window{ WIDTH, HEIGHT, "Hello Vulkan!" };
		DecoDevice m_deco_device{ m_deco_window };
		std::unique_ptr<DecoSwapChain> m_deco_swap_chain;
		std::unique_ptr<DecoPipeline> m_deco_pipeline;
		VkPipelineLayout m_pipeline_layout;
		std::vector<VkCommandBuffer> m_command_buffers;
		std::vector<DecoGameObject> m_deco_game_objects;
	};
}

