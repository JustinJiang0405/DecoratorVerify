#pragma once

#include "deco_device.h"

#include <string>
#include <vector>

namespace Deco
{
	struct PipelineConfigInfo
	{
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		VkPipelineViewportStateCreateInfo m_viewport_info;
		VkPipelineInputAssemblyStateCreateInfo m_input_assembly_info;
		VkPipelineRasterizationStateCreateInfo m_rasterization_info;
		VkPipelineMultisampleStateCreateInfo m_multi_sample_info;
		VkPipelineColorBlendAttachmentState m_color_blend_attachment;
		VkPipelineColorBlendStateCreateInfo m_color_blend_info;
		VkPipelineDepthStencilStateCreateInfo m_depth_stencil_info;
		std::vector<VkDynamicState> m_dynamic_state_enables;
		VkPipelineDynamicStateCreateInfo m_dynamic_state_info;
		VkPipelineLayout m_pipeline_layout = nullptr;
		VkRenderPass m_render_pass = nullptr;
		uint32_t m_subpass = 0;
	};

	class DecoPipeline
	{
	public:
		DecoPipeline(DecoDevice& device, const std::string& vert_file_path, const std::string& frag_file_path, const PipelineConfigInfo& config_info);
		~DecoPipeline();

		DecoPipeline(const DecoPipeline&) = delete;
		DecoPipeline& operator=(const DecoPipeline&) = delete;

		void bind(VkCommandBuffer command_buffer);

		static void defaultPipelineConfigInfo(PipelineConfigInfo& config_info);
	private:
		static std::vector<char> readFile(const std::string& file_path);

		void createGraphicsPipeline(const std::string& vert_file_path, const std::string& frag_file_path, const PipelineConfigInfo& config_info);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shader_module);

		DecoDevice& m_device;
		VkPipeline m_graphics_pipeline;
		VkShaderModule m_vert_shader_module;
		VkShaderModule m_frag_shader_module;
	};
}

