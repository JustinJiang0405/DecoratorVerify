#include "FirstApp.h"

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
	FirstApp::FirstApp()
	{
		loadGameObjects();
	}

	FirstApp::~FirstApp() {}

	void FirstApp::run()
	{
		SimpleRenderSystem simple_render_system{ m_deco_device, m_deco_renderer.getSwapChainRenderPass() };
		while (!m_deco_window.shouldClose())
		{
			glfwPollEvents();
			
			if (auto command_buffer = m_deco_renderer.beginFrame())
			{
				// begin offscreen shadow pass
				// render shadow casting objects
				// end offscreen shadow pass

				m_deco_renderer.beginSwapChainRenderPass(command_buffer);
				simple_render_system.renderGameObjects(command_buffer, m_deco_game_objects);
				m_deco_renderer.endSwapChainRenderPass(command_buffer);
				m_deco_renderer.endFrame();
			}
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