#include "deco_pipeline.h"
#include "deco_model.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>

//#include <direct.h>

namespace Deco
{

	DecoPipeline::DecoPipeline(DecoDevice& device, const std::string& vert_file_path, const std::string& frag_file_path, const PipelineConfigInfo& config_info) : m_device(device)
	{
		createGraphicsPipeline(vert_file_path, frag_file_path, config_info);
	}

	DecoPipeline::~DecoPipeline()
	{
		vkDestroyShaderModule(m_device.device(), m_vert_shader_module, nullptr);
		vkDestroyShaderModule(m_device.device(), m_frag_shader_module, nullptr);
		vkDestroyPipeline(m_device.device(), m_graphics_pipeline, nullptr);
	}

	void DecoPipeline::bind(VkCommandBuffer command_buffer)
	{
		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics_pipeline);
	}

	void DecoPipeline::defaultPipelineConfigInfo(PipelineConfigInfo& config_info)
	{

		config_info.m_input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		config_info.m_input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		config_info.m_input_assembly_info.primitiveRestartEnable = VK_FALSE;

		config_info.m_viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		config_info.m_viewport_info.viewportCount = 1;
		config_info.m_viewport_info.pViewports = nullptr;
		config_info.m_viewport_info.scissorCount = 1;
		config_info.m_viewport_info.pScissors = nullptr;

		config_info.m_rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		config_info.m_rasterization_info.depthClampEnable = VK_FALSE;
		config_info.m_rasterization_info.rasterizerDiscardEnable = VK_FALSE;
		config_info.m_rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
		config_info.m_rasterization_info.lineWidth = 1.0f;
		config_info.m_rasterization_info.cullMode = VK_CULL_MODE_NONE;
		config_info.m_rasterization_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
		config_info.m_rasterization_info.depthBiasEnable = VK_FALSE;
		config_info.m_rasterization_info.depthBiasConstantFactor = 0.0f;
		config_info.m_rasterization_info.depthBiasClamp = 0.0f;
		config_info.m_rasterization_info.depthBiasSlopeFactor = 0.0f;

		config_info.m_multi_sample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		config_info.m_multi_sample_info.sampleShadingEnable = VK_FALSE;
		config_info.m_multi_sample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		config_info.m_multi_sample_info.minSampleShading = 1.0f;
		config_info.m_multi_sample_info.pSampleMask = nullptr;
		config_info.m_multi_sample_info.alphaToCoverageEnable = VK_FALSE;
		config_info.m_multi_sample_info.alphaToOneEnable = VK_FALSE;

		config_info.m_color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		config_info.m_color_blend_attachment.blendEnable = VK_FALSE;
		config_info.m_color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		config_info.m_color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		config_info.m_color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
		config_info.m_color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		config_info.m_color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		config_info.m_color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

		config_info.m_color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		config_info.m_color_blend_info.logicOpEnable = VK_FALSE;
		config_info.m_color_blend_info.logicOp = VK_LOGIC_OP_COPY;
		config_info.m_color_blend_info.attachmentCount = 1;
		config_info.m_color_blend_info.pAttachments = &config_info.m_color_blend_attachment;
		config_info.m_color_blend_info.blendConstants[0] = 0.0f;
		config_info.m_color_blend_info.blendConstants[1] = 0.0f;
		config_info.m_color_blend_info.blendConstants[2] = 0.0f;
		config_info.m_color_blend_info.blendConstants[3] = 0.0f;

		config_info.m_depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		config_info.m_depth_stencil_info.depthTestEnable = VK_TRUE;
		config_info.m_depth_stencil_info.depthWriteEnable = VK_TRUE;
		config_info.m_depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
		config_info.m_depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
		config_info.m_depth_stencil_info.minDepthBounds = 0.0f;
		config_info.m_depth_stencil_info.maxDepthBounds = 1.0f;
		config_info.m_depth_stencil_info.stencilTestEnable = VK_FALSE;
		config_info.m_depth_stencil_info.front = {};
		config_info.m_depth_stencil_info.back = {};

		config_info.m_dynamic_state_enables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		config_info.m_dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		config_info.m_dynamic_state_info.pDynamicStates = config_info.m_dynamic_state_enables.data();
		config_info.m_dynamic_state_info.dynamicStateCount = static_cast<uint32_t>(config_info.m_dynamic_state_enables.size());
		config_info.m_dynamic_state_info.flags = 0;

		config_info.bindingDescriptions = DecoModel::Vertex::getBindingDescriptions();
		config_info.attributeDescriptions = DecoModel::Vertex::getAttributeDescriptions();
	}

	std::vector<char> DecoPipeline::readFile(const std::string& file_path)
	{
		//char dir[260];
		//_getcwd(dir, 260);
		//std::cout << "!!!!!!!!!path: " << dir << std::endl;

		std::ifstream file(file_path, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("failed to open file: " + file_path);
		}

		size_t file_size = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(file_size);

		file.seekg(0);
		file.read(buffer.data(), file_size);

		file.close();
		return buffer;
	}

	void DecoPipeline::createGraphicsPipeline(const std::string& vert_file_path, const std::string& frag_file_path, const PipelineConfigInfo& config_info)
	{
		assert(
			config_info.m_pipeline_layout != VK_NULL_HANDLE &&
			"Cannot create graphics pipeline:: no pipelineLayout provided in configInfo");

		assert(
			config_info.m_render_pass != VK_NULL_HANDLE &&
			"Cannot create graphics pipeline:: no renderPass provided in configInfo");

		auto vert_code = readFile(vert_file_path);
		auto frag_code = readFile(frag_file_path);

		//std::cout << "Vertex shader code size: " << vert_code.size() << std::endl;
		//std::cout << "Fragment shader code size: " << frag_code.size() << std::endl;

		createShaderModule(vert_code, &m_vert_shader_module);
		createShaderModule(frag_code, &m_frag_shader_module);

		VkPipelineShaderStageCreateInfo shader_stages[2];
		shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shader_stages[0].module = m_vert_shader_module;
		shader_stages[0].pName = "main";
		shader_stages[0].flags = 0;
		shader_stages[0].pNext = nullptr;
		shader_stages[0].pSpecializationInfo = nullptr;
		shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shader_stages[1].module = m_frag_shader_module;
		shader_stages[1].pName = "main";
		shader_stages[1].flags = 0;
		shader_stages[1].pNext = nullptr;
		shader_stages[1].pSpecializationInfo = nullptr;

		auto binding_descriptions = DecoModel::Vertex::getBindingDescriptions();
		auto attribute_descriptions = DecoModel::Vertex::getAttributeDescriptions();
		VkPipelineVertexInputStateCreateInfo vertex_input_info{};
		vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
		vertex_input_info.vertexBindingDescriptionCount = static_cast<uint32_t>(binding_descriptions.size());
		vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();
		vertex_input_info.pVertexBindingDescriptions = binding_descriptions.data();

		VkGraphicsPipelineCreateInfo pipeline_info{};
		pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_info.stageCount = 2;
		pipeline_info.pStages = shader_stages;
		pipeline_info.pVertexInputState = &vertex_input_info;
		pipeline_info.pInputAssemblyState = &config_info.m_input_assembly_info;
		pipeline_info.pViewportState = &config_info.m_viewport_info;
		pipeline_info.pRasterizationState = &config_info.m_rasterization_info;
		pipeline_info.pMultisampleState = &config_info.m_multi_sample_info;
		pipeline_info.pColorBlendState = &config_info.m_color_blend_info;
		pipeline_info.pDepthStencilState = &config_info.m_depth_stencil_info;
		pipeline_info.pDynamicState = &config_info.m_dynamic_state_info;

		pipeline_info.layout = config_info.m_pipeline_layout;
		pipeline_info.renderPass = config_info.m_render_pass;
		pipeline_info.subpass = config_info.m_subpass;

		pipeline_info.basePipelineIndex = -1;
		pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(
			m_device.device(),
			VK_NULL_HANDLE,
			1,
			&pipeline_info,
			nullptr,
			&m_graphics_pipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create graphics pipeline");
		}
	}

	void DecoPipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shader_module)
	{
		VkShaderModuleCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = code.size();
		create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(m_device.device(), &create_info, nullptr, shader_module) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module");
		}
	}

}