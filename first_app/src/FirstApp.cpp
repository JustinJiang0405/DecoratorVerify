#include "FirstApp.h"

#include "deco_camera.h"
#include "KeyboardMovementController.h"
#include "SimpleRenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <chrono>
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
		DecoCamera camera{};
		//camera.setViewDirection(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 1.0f));
		//camera.setViewTarget(glm::vec3(-2.0f, -2.f, 2.f), glm::vec3(0.0f, 0.0f, 2.5f));

		auto viewer_object = DecoGameObject::createGameObject();
		KeyboardMovementController camera_controller{};

		auto current_time = std::chrono::high_resolution_clock::now();

		while (!m_deco_window.shouldClose())
		{
			glfwPollEvents();

			auto new_time = std::chrono::high_resolution_clock::now();
			float frame_time = std::chrono::duration<float, std::chrono::seconds::period>(new_time - current_time).count();
			current_time = new_time;

			frame_time = glm::min(frame_time, MAX_FRAME_TIME);

			camera_controller.moveInPlaneXZ(m_deco_window.getGLFWwindow(), frame_time, viewer_object);
			camera.setViewYXZ(viewer_object.m_transform.m_translation, viewer_object.m_transform.m_rotation);

			float aspect = m_deco_renderer.getAspectRatio();
			//camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
			camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);

			
			if (auto command_buffer = m_deco_renderer.beginFrame())
			{
				// begin offscreen shadow pass
				// render shadow casting objects
				// end offscreen shadow pass

				m_deco_renderer.beginSwapChainRenderPass(command_buffer);
				simple_render_system.renderGameObjects(command_buffer, m_deco_game_objects, camera);
				m_deco_renderer.endSwapChainRenderPass(command_buffer);
				m_deco_renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(m_deco_device.device());
	}

	// temporary helper function, creates a 1x1x1 cube centered at offset
	std::unique_ptr<DecoModel> createCubeModel(DecoDevice& device, glm::vec3 offset)
	{
		std::vector<DecoModel::Vertex> vertices{

			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

	};
		for (auto& v : vertices) {
			v.position += offset;
		}
		return std::make_unique<DecoModel>(device, vertices);
}

	void FirstApp::loadGameObjects()
	{
		std::shared_ptr<DecoModel> deco_model = createCubeModel(m_deco_device, { 0.0f, 0.0f, 0.0f });

		auto cube = DecoGameObject::createGameObject();
		cube.m_model = deco_model;
		cube.m_transform.m_translation = { 0.0f, 0.0f, 2.5f };
		cube.m_transform.m_scale = { 0.5f, 0.5f, 0.5f };
		m_deco_game_objects.push_back(std::move(cube));
	}
}