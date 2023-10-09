#pragma once

#include "deco_camera.h"
#include "deco_device.h"
#include "deco_game_object.h"
#include "deco_pipeline.h"
#include "deco_frame_info.h"

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

		void renderGameObjects(FrameInfo& frame_info, std::vector<DecoGameObject> &game_objects);
	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass render_pass);

	private:
		DecoDevice& m_deco_device;

		std::unique_ptr<DecoPipeline> m_deco_pipeline;
		VkPipelineLayout m_pipeline_layout;
	};
}

