#pragma once

#include "deco_device.h"
#include "deco_swap_chain.h"
#include "deco_window.h"

#include <cassert>
#include <memory>
#include <vector>

namespace Deco
{
	class DecoRenderer
	{
	public:
		DecoRenderer(DecoWindow& window, DecoDevice& device);
		~DecoRenderer();

		DecoRenderer(const DecoRenderer&) = delete;
		DecoRenderer& operator=(const DecoRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const;
		bool isFrameInProgress() const;

		VkCommandBuffer getCurrentCommandBuffer() const;

		int getFrameIndex() const;

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer command_buffer);
		void endSwapChainRenderPass(VkCommandBuffer command_buffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

	private:
		DecoWindow& m_deco_window;
		DecoDevice& m_deco_device;
		std::unique_ptr<DecoSwapChain> m_deco_swap_chain;
		std::vector<VkCommandBuffer> m_command_buffers;

		uint32_t m_current_image_index{ 0 };
		int m_current_frame_index{ 0 };
		bool  m_is_frame_started{ false };
	};
}

