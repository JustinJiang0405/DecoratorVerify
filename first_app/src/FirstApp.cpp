#include "FirstApp.h"

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
		glm::mat2 transform{ 1.0f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	FirstApp::FirstApp()
	{
		loadGameObjects();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
	}

	FirstApp::~FirstApp()
	{
		vkDestroyPipelineLayout(m_deco_device.device(), m_pipeline_layout, nullptr);
	}

	void FirstApp::run()
	{
		while (!m_deco_window.shouldClose())
		{
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(m_deco_device.device());
	}

	void FirstApp::loadGameObjects()
	{
#if 0
		std::vector<DecoModel::Vertex> vertices{};
		sierpinski(vertices, 7, { -0.5f, 0.5f }, { 0.5f, 0.5f }, { 0.0f, -0.5f });
		m_deco_model = std::make_unique<DecoModel>(m_deco_device, vertices);
#endif

		std::vector<DecoModel::Vertex> vertices
		{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};
	
		auto model = std::make_shared<DecoModel>(m_deco_device, vertices);

		auto triangle = DecoGameObject::createGameObject();
		triangle.m_model = model;
		triangle.m_color = { 0.1f, 0.8f, 0.1f };
		triangle.m_transform2d.m_translation.x = 0.2f;
		triangle.m_transform2d.m_scale = { 2.0f, 0.5f };
		triangle.m_transform2d.m_rotation = 0.25f * glm::two_pi<float>();

		m_deco_game_objects.push_back(std::move(triangle));
	}

	void FirstApp::createPipelineLayout()
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

	void FirstApp::createPipeline()
	{
		assert(m_deco_swap_chain != nullptr && "Cannot create pipeline before swap chain");
		assert(m_pipeline_layout != nullptr && "Cannot create pipeline before swap chain");

		PipelineConfigInfo pipeline_config{};
		DecoPipeline::defaultPipelineConfigInfo(pipeline_config);
		pipeline_config.m_render_pass = m_deco_swap_chain->getRenderPass();
		pipeline_config.m_pipeline_layout = m_pipeline_layout;
		m_deco_pipeline = std::make_unique<DecoPipeline>(
			m_deco_device,
			"../shaders/simple_shader.vert.spv",
			"../shaders/simple_shader.frag.spv",
			pipeline_config);
	}

	void FirstApp::recreateSwapChain()
	{
		auto extent = m_deco_window.getExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = m_deco_window.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(m_deco_device.device());

		if (m_deco_swap_chain == nullptr)
		{
			m_deco_swap_chain = std::make_unique<DecoSwapChain>(m_deco_device, extent);
		}
		else
		{
			m_deco_swap_chain = std::make_unique<DecoSwapChain>(m_deco_device, extent, std::move(m_deco_swap_chain));
			if (m_deco_swap_chain->imageCount() != m_command_buffers.size())
			{
				freeCommandBuffers();
				createCommandBuffers();
			}
		}

		// if render pass compatible do nothing else
		createPipeline();

	}

	void FirstApp::createCommandBuffers()
	{
		m_command_buffers.resize(m_deco_swap_chain->imageCount());

		VkCommandBufferAllocateInfo alloc_info{};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandPool = m_deco_device.getCommandPool();
		alloc_info.commandBufferCount = static_cast<uint32_t>(m_command_buffers.size());

		if (vkAllocateCommandBuffers(
			m_deco_device.device(),
			&alloc_info,
			m_command_buffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffer");
		}
	}

	void FirstApp::freeCommandBuffers()
	{
		vkFreeCommandBuffers(
			m_deco_device.device(),
			m_deco_device.getCommandPool(),
			static_cast<uint32_t>(m_command_buffers.size()),
			m_command_buffers.data());

		m_command_buffers.clear();
	}

	void FirstApp::recordCommandBuffer(int image_index)
	{
		VkCommandBufferBeginInfo begin_info{};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(m_command_buffers[image_index], &begin_info) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo render_pass_info{};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = m_deco_swap_chain->getRenderPass();
		render_pass_info.framebuffer = m_deco_swap_chain->getFrameBuffer(image_index);

		render_pass_info.renderArea.offset = { 0,0 };
		render_pass_info.renderArea.extent = m_deco_swap_chain->getSwapChainExtent();

		std::array<VkClearValue, 2> clear_values{};
		clear_values[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clear_values[1].depthStencil = { 1.0f, 0 };
		render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
		render_pass_info.pClearValues = clear_values.data();

		vkCmdBeginRenderPass(m_command_buffers[image_index], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_deco_swap_chain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(m_deco_swap_chain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, m_deco_swap_chain->getSwapChainExtent() };
		vkCmdSetViewport(m_command_buffers[image_index], 0, 1, &viewport);
		vkCmdSetScissor(m_command_buffers[image_index], 0, 1, &scissor);

		renderGameObjects(m_command_buffers[image_index]);

		vkCmdEndRenderPass(m_command_buffers[image_index]);

		if (vkEndCommandBuffer(m_command_buffers[image_index]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffers");
		}

	}

	void FirstApp::renderGameObjects(VkCommandBuffer command_buffer)
	{
		m_deco_pipeline->bind(command_buffer);

		for (auto& object : m_deco_game_objects)
		{
			object.m_transform2d.m_rotation = glm::mod(object.m_transform2d.m_rotation + 0.01f, glm::two_pi<float>());

			SimplePushConstantData push{};
			push.offset = object.m_transform2d.m_translation;
			push.color = object.m_color;
			push.transform = object.m_transform2d.mat2();

			vkCmdPushConstants(
				command_buffer,
				m_pipeline_layout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);

			object.m_model->bind(command_buffer);
			object.m_model->draw(command_buffer);
		}
	}

	void FirstApp::drawFrame()
	{
		uint32_t image_index;
		auto result = m_deco_swap_chain->acquireNextImage(&image_index);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		recordCommandBuffer(image_index);

		result = m_deco_swap_chain->submitCommandBuffers(&m_command_buffers[image_index], &image_index);

		if (result == VK_ERROR_OUT_OF_DATE_KHR ||
			result == VK_SUBOPTIMAL_KHR ||
			m_deco_window.wasWindowResized())
		{
			m_deco_window.resetWindowResizedFlag();
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image");
		}
	}

	void FirstApp::sierpinski(
		std::vector<DecoModel::Vertex>& vertices,
		int depth,
		glm::vec2 left,
		glm::vec2 right,
		glm::vec2 top)
	{
		if (depth <= 0)
		{
			vertices.push_back({ top });
			vertices.push_back({ right });
			vertices.push_back({ left });
		}
		else
		{
			auto leftTop = 0.5f * (left + top);
			auto rightTop = 0.5f * (right + top);
			auto leftRight = 0.5f * (left + right);
			sierpinski(vertices, depth - 1, left, leftRight, leftTop);
			sierpinski(vertices, depth - 1, leftRight, right, rightTop);
			sierpinski(vertices, depth - 1, leftTop, rightTop, top);
		}
	}
}