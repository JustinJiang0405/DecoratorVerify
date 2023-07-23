#include "deco_window.h"

#include <stdexcept>
namespace Deco
{
	DecoWindow::DecoWindow(int width, int height, std::string window_name) : m_width(width), m_height(height), m_window_name(window_name)
	{
		initWindow();
	}

	void DecoWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create window surface");
		}
	}

	DecoWindow::~DecoWindow()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void DecoWindow::frameBufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto deco_window = reinterpret_cast<DecoWindow*>(glfwGetWindowUserPointer(window));
		deco_window->m_frame_buffer_resized = true;
		deco_window->m_width = width;
		deco_window->m_height = height;
	}

	void DecoWindow::initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_window = glfwCreateWindow(m_width, m_height, m_window_name.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(m_window, this);

		glfwSetFramebufferSizeCallback(m_window, frameBufferResizeCallback);
	}

}