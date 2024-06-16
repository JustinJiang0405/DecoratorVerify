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
	class PointLightSystem
	{
	public:
		PointLightSystem(DecoDevice& device, VkRenderPass render_pass, VkDescriptorSetLayout global_set_layout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void render(FrameInfo& frame_info);
	private:
		void createPipelineLayout(VkDescriptorSetLayout global_set_layout);
		void createPipeline(VkRenderPass render_pass);

	private:
		DecoDevice& m_deco_device;

		std::unique_ptr<DecoPipeline> m_deco_pipeline;
		VkPipelineLayout m_pipeline_layout;
	};
}

