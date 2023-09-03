#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace Deco
{
	class DecoWindow
	{
	public:
		DecoWindow(int width, int height, std::string window_name);
		~DecoWindow();

		DecoWindow(const DecoWindow&) = delete;
		DecoWindow& operator=(const DecoWindow&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(m_window); }
		VkExtent2D getExtent()  { return {static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height)}; }
		bool wasWindowResized() { return m_frame_buffer_resized; }
		void resetWindowResizedFlag() { m_frame_buffer_resized = false; }
		GLFWwindow* getGLFWwindow() const { return m_window; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		static void frameBufferResizeCallback(GLFWwindow* window, int width, int height);
		void initWindow();

	private:
		GLFWwindow* m_window;

		int m_width;
		int m_height;
		bool m_frame_buffer_resized{ false };

		std::string m_window_name;
	};
}

