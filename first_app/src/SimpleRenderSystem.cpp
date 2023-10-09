#include "SimpleRenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <cassert>
#include <stdexcept>

namespace Deco
{
	struct SimplePushConstantData
	{
		glm::mat4 transform{ 1.0f };
		glm::mat4 normal_matrix{ 1.0f };
	};

	SimpleRenderSystem::SimpleRenderSystem(DecoDevice& device, VkRenderPass render_pass) : m_deco_device(device)
	{
		createPipelineLayout();
		createPipeline(render_pass);
	}

	SimpleRenderSystem::~SimpleRenderSystem()
	{
		vkDestroyPipelineLayout(m_deco_device.device(), m_pipeline_layout, nullptr);
	}

	void SimpleRenderSystem::createPipelineLayout()
	{
		VkPushConstantRange push_constant_range{};
		push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		push_constant_range.offset = 0;
		push_constant_range.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipeline_layout_info{};
		pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_info.setLayoutCount = 0;
		pipeline_layout_info.pSetLayouts = nullptr;
		pipeline_layout_info.pushConstantRangeCount = 1;
		pipeline_layout_info.pPushConstantRanges = &push_constant_range;

		if (vkCreatePipelineLayout(
			m_deco_device.device(),
			&pipeline_layout_info,
			nullptr,
			&m_pipeline_layout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout!");
		}
	}

	void SimpleRenderSystem::createPipeline(VkRenderPass render_pass)
	{
		assert(m_pipeline_layout != nullptr && "Cannot create pipeline before swap chain");

		PipelineConfigInfo pipeline_config{};
		DecoPipeline::defaultPipelineConfigInfo(pipeline_config);
		pipeline_config.m_render_pass = render_pass;
		pipeline_config.m_pipeline_layout = m_pipeline_layout;
		m_deco_pipeline = std::make_unique<DecoPipeline>(
			m_deco_device,
			"../shaders/simple_shader.vert.spv",
			"../shaders/simple_shader.frag.spv",
			pipeline_config);
	}

	void SimpleRenderSystem::renderGameObjects(FrameInfo& frame_info, std::vector<DecoGameObject>& game_objects)
	{
		m_deco_pipeline->bind(frame_info.command_buffer);

		auto projection_view = frame_info.camera.getProjection() * frame_info.camera.getView();

		for (auto& object : game_objects)
		{
// 			object.m_transform.m_rotation.y = glm::mod(object.m_transform.m_rotation.y + 0.0001f, glm::two_pi<float>());
// 			object.m_transform.m_rotation.x = glm::mod(object.m_transform.m_rotation.x + 0.00005f, glm::two_pi<float>());

			SimplePushConstantData push{};
			push.transform = projection_view * object.m_transform.mat4();
			push.normal_matrix = object.m_transform.normalMatrix();

			vkCmdPushConstants(
				frame_info.command_buffer,
				m_pipeline_layout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);

			object.m_model->bind(frame_info.command_buffer);
			object.m_model->draw(frame_info.command_buffer);
		}
	}
}