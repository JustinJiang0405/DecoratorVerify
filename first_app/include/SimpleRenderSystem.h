#pragma once

#include "deco_device.h"
#include "deco_game_object.h"
#include "deco_pipeline.h"

#include <memory>
#include <vector>

namespace Deco
{
	class SimpleRenderSystem
	{
	public:
		SimpleRenderSystem(DecoDevice& device, VkRenderPass render_pass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(VkCommandBuffer command_buffer, std::vector<DecoGameObject> &game_objects);
	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass render_pass);

		void sierpinski(std::vector<DecoModel::Vertex>& vertices, int depth, glm::vec2 left, glm::vec2 right, glm::vec2 top);
	private:
		DecoDevice& m_deco_device;

		std::unique_ptr<DecoPipeline> m_deco_pipeline;
		VkPipelineLayout m_pipeline_layout;
	};
}

