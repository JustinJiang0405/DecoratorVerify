#pragma once

#include "deco_device.h"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <memory>
#include <string>
#include <vector>

namespace Deco {

	class DecoSwapChain {
	public:
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		DecoSwapChain(DecoDevice& device_ref, VkExtent2D window_extent);
		DecoSwapChain(DecoDevice& device_ref, VkExtent2D window_extent, std::shared_ptr<DecoSwapChain> previous);

		~DecoSwapChain();

		DecoSwapChain(const DecoSwapChain&) = delete;
		DecoSwapChain& operator=(const DecoSwapChain&) = delete;

		VkFramebuffer getFrameBuffer(int index) { return m_swap_chain_frame_buffers[index]; }
		VkRenderPass getRenderPass() { return m_render_pass; }
		VkImageView getImageView(int index) { return m_swap_chain_image_views[index]; }
		size_t imageCount() { return m_swap_chain_images.size(); }
		VkFormat getSwapChainImageFormat() { return m_swap_chain_image_format; }
		VkExtent2D getSwapChainExtent() { return m_swap_chain_extent; }
		uint32_t width() { return m_swap_chain_extent.width; }
		uint32_t height() { return m_swap_chain_extent.height; }

		float extentAspectRatio() {
			return static_cast<float>(m_swap_chain_extent.width) / static_cast<float>(m_swap_chain_extent.height);
		}
		VkFormat findDepthFormat();

		VkResult acquireNextImage(uint32_t* image_index);
		VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* image_index);

		bool compareSwapFormats(const DecoSwapChain& swap_chain) const {
			return swap_chain.m_swap_chain_depth_format == m_swap_chain_depth_format &&
				swap_chain.m_swap_chain_image_format == m_swap_chain_image_format;
		}

	private:
		void init();
		void createSwapChain();
		void createImageViews();
		void createDepthResources();
		void createRenderPass();
		void createFramebuffers();
		void createSyncObjects();

		// Helper functions
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(
			const std::vector<VkSurfaceFormatKHR>& available_formats);
		VkPresentModeKHR chooseSwapPresentMode(
			const std::vector<VkPresentModeKHR>& available_presentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		VkFormat m_swap_chain_image_format;
		VkFormat m_swap_chain_depth_format;
		VkExtent2D m_swap_chain_extent;

		std::vector<VkFramebuffer> m_swap_chain_frame_buffers;
		VkRenderPass m_render_pass;

		std::vector<VkImage> m_depth_images;
		std::vector<VkDeviceMemory> m_depth_image_memorys;
		std::vector<VkImageView> m_depth_image_views;
		std::vector<VkImage> m_swap_chain_images;
		std::vector<VkImageView> m_swap_chain_image_views;

		DecoDevice& device;
		VkExtent2D m_window_extent;

		VkSwapchainKHR m_swap_chain;
		std::shared_ptr<DecoSwapChain> m_old_swap_chain;

		std::vector<VkSemaphore> m_image_available_semaphores;
		std::vector<VkSemaphore> m_render_finished_semaphores;
		std::vector<VkFence> m_in_flight_fences;
		std::vector<VkFence> m_images_in_flight;
		size_t m_current_frame = 0;
	};

}  // namespace Deco