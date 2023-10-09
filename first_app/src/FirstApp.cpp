#include "FirstApp.h"

#include "deco_buffer.h"
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
	struct GlobalUbo
	{
		glm::mat4 projection_view{ 1.0f };
		glm::vec3 light_direction = glm::normalize(glm::vec3{ 1.0f, -3.0f, -1.0f });
	};

	FirstApp::FirstApp()
	{
		loadGameObjects();
	}

	FirstApp::~FirstApp() {}

	void FirstApp::run()
	{
		DecoBuffer global_ubo_buffer{
			m_deco_device,
			sizeof(GlobalUbo),
			DecoSwapChain::MAX_FRAMES_IN_FLIGHT,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			m_deco_device.properties.limits.minUniformBufferOffsetAlignment,
		};
		global_ubo_buffer.map();

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
				int frame_index = m_deco_renderer.getFrameIndex();
				FrameInfo frame_info{
					frame_index,
					frame_time,
					command_buffer,
					camera
				};

				//update
				GlobalUbo ubo{};
				ubo.projection_view = camera.getProjection() * camera.getView();
				global_ubo_buffer.writeToIndex(&ubo, frame_index);
				global_ubo_buffer.flushIndex(frame_index);

				//render
				m_deco_renderer.beginSwapChainRenderPass(command_buffer);
				simple_render_system.renderGameObjects(frame_info, m_deco_game_objects);
				m_deco_renderer.endSwapChainRenderPass(command_buffer);
				m_deco_renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(m_deco_device.device());
	}

	void FirstApp::loadGameObjects()
	{
		std::shared_ptr<DecoModel> deco_model = DecoModel::createModelFromFile(m_deco_device, "../resources/objs/flat_vase.obj");
		auto flat_vase = DecoGameObject::createGameObject();
		flat_vase.m_model = deco_model;
		flat_vase.m_transform.m_translation = { -.5f, .5f, 2.5f };
		flat_vase.m_transform.m_scale = glm::vec3(3.f);
		m_deco_game_objects.push_back(std::move(flat_vase));

		deco_model = DecoModel::createModelFromFile(m_deco_device, "../resources/objs/smooth_vase.obj");
		auto smooth_vase = DecoGameObject::createGameObject();
		smooth_vase.m_model = deco_model;
		smooth_vase.m_transform.m_translation = { .5f, .5f, 2.5f };
		smooth_vase.m_transform.m_scale = glm::vec3(3.f);
		m_deco_game_objects.push_back(std::move(smooth_vase));
	}
}