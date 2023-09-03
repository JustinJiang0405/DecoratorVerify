#include "deco_renderer.h"

#include <array>
#include <cassert>
#include <stdexcept>

namespace Deco
{
	DecoRenderer::DecoRenderer(DecoWindow& window, DecoDevice& device) : m_deco_window(window), m_deco_device(device)
	{
		recreateSwapChain();
		createCommandBuffers();
	}

	DecoRenderer::~DecoRenderer()
	{
		freeCommandBuffers();
	}

	VkRenderPass DecoRenderer::getSwapChainRenderPass() const
	{
		return m_deco_swap_chain->getRenderPass();
	}

	float DecoRenderer::getAspectRatio() const
	{
		return m_deco_swap_chain->extentAspectRatio();
	}

	bool DecoRenderer::isFrameInProgress() const
	{
		return m_is_frame_started;
	}

	VkCommandBuffer DecoRenderer::getCurrentCommandBuffer() const
	{
		assert(m_is_frame_started && "Cannot get command buffer when frame not in progress");
		return m_command_buffers[m_current_frame_index];
	}

	int DecoRenderer::getFrameIndex() const
	{
		assert(m_is_frame_started && "Cannot get frame index when frame not in progress");
		return m_current_frame_index;
	}

	VkCommandBuffer DecoRenderer::beginFrame()
	{
		assert(!m_is_frame_started && "Can't call beginFrame while already in progress");

		auto result = m_deco_swap_chain->acquireNextImage(&m_current_image_index);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		m_is_frame_started = true;

		auto command_buffer = getCurrentCommandBuffer();
		VkCommandBufferBeginInfo begin_info{};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		return command_buffer;
	}

	void DecoRenderer::endFrame()
	{
		assert(m_is_frame_started && "Can't call endFrame while frame is not in progress");

		auto command_buffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffers");
		}

		auto result = m_deco_swap_chain->submitCommandBuffers(&command_buffer, &m_current_image_index);

		if (result == VK_ERROR_OUT_OF_DATE_KHR ||
			result == VK_SUBOPTIMAL_KHR ||
			m_deco_window.wasWindowResized())
		{
			m_deco_window.resetWindowResizedFlag();
			recreateSwapChain();
		}

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image");
		}

		m_is_frame_started = false;
		m_current_frame_index = (m_current_frame_index + 1) % DecoSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void DecoRenderer::beginSwapChainRenderPass(VkCommandBuffer command_buffer)
	{
		assert(m_is_frame_started && "Can't call beginSwapChainRenderPass while frame is not in progress");
		assert(command_buffer == getCurrentCommandBuffer() && "Can't begin render on command buffer from a different frame");

		VkRenderPassBeginInfo render_pass_info{};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = m_deco_swap_chain->getRenderPass();
		render_pass_info.framebuffer = m_deco_swap_chain->getFrameBuffer(m_current_image_index);

		render_pass_info.renderArea.offset = { 0,0 };
		render_pass_info.renderArea.extent = m_deco_swap_chain->getSwapChainExtent();

		std::array<VkClearValue, 2> clear_values{};
		clear_values[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clear_values[1].depthStencil = { 1.0f, 0 };
		render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
		render_pass_info.pClearValues = clear_values.data();

		vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_deco_swap_chain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(m_deco_swap_chain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, m_deco_swap_chain->getSwapChainExtent() };
		vkCmdSetViewport(command_buffer, 0, 1, &viewport);
		vkCmdSetScissor(command_buffer, 0, 1, &scissor);
	}

	void DecoRenderer::endSwapChainRenderPass(VkCommandBuffer command_buffer)
	{
		assert(m_is_frame_started && "Can't call endSwapChainRenderPass while frame is not in progress");
		assert(command_buffer == getCurrentCommandBuffer() && "Can't end render on command buffer from a different frame");

		vkCmdEndRenderPass(command_buffer);
	}

	void DecoRenderer::recreateSwapChain()
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
			std::shared_ptr<DecoSwapChain> old_swap_chain = std::move(m_deco_swap_chain);
			m_deco_swap_chain = std::make_unique<DecoSwapChain>(m_deco_device, extent, old_swap_chain);

			if (!old_swap_chain->compareSwapFormats(*m_deco_swap_chain.get()))
			{
				throw std::runtime_error("Swap chain image(or depth) format has changed");
			}
		}

		// if render pass compatible do nothing else
		//createPipeline();

	}

	void DecoRenderer::createCommandBuffers()
	{
		m_command_buffers.resize(DecoSwapChain::MAX_FRAMES_IN_FLIGHT);

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

	void DecoRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(
			m_deco_device.device(),
			m_deco_device.getCommandPool(),
			static_cast<uint32_t>(m_command_buffers.size()),
			m_command_buffers.data());

		m_command_buffers.clear();
	}
}