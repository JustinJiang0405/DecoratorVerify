#include "first_app.h"

#include "deco_buffer.h"
#include "deco_camera.h"
#include "keyboard_movement_controller.h"
#include "simple_render_system.h"
#include "point_light_system.h"

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
		glm::mat4 projection{ 1.0f };
		glm::mat4 view{ 1.0f };
		glm::vec4 ambinetLightColor{ 1.f, 1.f, 1.f, .02f }; // w is intensity
		glm::vec3 light_position{ -1.f };
		alignas(16) glm::vec4 light_color{ 1.f }; // w is light intensity
	};

	FirstApp::FirstApp()
	{
		m_global_pool = DecoDescriptorPool::Builder(m_deco_device)
			.setMaxSets(DecoSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DecoSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}

	FirstApp::~FirstApp() {}

	void FirstApp::run()
	{
		std::vector<std::unique_ptr<DecoBuffer>> uboBuffers(DecoSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++)
		{
			uboBuffers[i] = std::make_unique<DecoBuffer>(
				m_deco_device,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT/* | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT*/);

			uboBuffers[i]->map();
		}

		auto global_set_layout = DecoDescriptorSetLayout::Builder(m_deco_device)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> global_descriptor_sets(DecoSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < global_descriptor_sets.size(); i++)
		{
			auto buffer_info = uboBuffers[i]->descriptorInfo();
			DecoDescriptorWriter(*global_set_layout, *m_global_pool)
				.writeBuffer(0, &buffer_info)
				.build(global_descriptor_sets[i]);
		}

		SimpleRenderSystem simple_render_system{ m_deco_device, m_deco_renderer.getSwapChainRenderPass(), global_set_layout->getDescriptorSetLayout() };
		PointLightSystem point_light_system{ m_deco_device, m_deco_renderer.getSwapChainRenderPass(), global_set_layout->getDescriptorSetLayout() };
		DecoCamera camera{};

		auto viewer_object = DecoGameObject::createGameObject();
		viewer_object.m_transform.m_translation.z = -2.5f;
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
					camera,
					global_descriptor_sets[frame_index],
					m_deco_game_objects
				};

				//update
				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				uboBuffers[frame_index]->writeToBuffer(&ubo);
				uboBuffers[frame_index]->flush();

				//render
				m_deco_renderer.beginSwapChainRenderPass(command_buffer);
				simple_render_system.renderGameObjects(frame_info);
				point_light_system.render(frame_info);
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
		flat_vase.m_transform.m_translation = { -.5f, .5f, 0.f };
		flat_vase.m_transform.m_scale = glm::vec3(3.f);
		m_deco_game_objects.emplace(flat_vase.getId(), std::move(flat_vase));

		deco_model = DecoModel::createModelFromFile(m_deco_device, "../resources/objs/smooth_vase.obj");
		auto smooth_vase = DecoGameObject::createGameObject();
		smooth_vase.m_model = deco_model;
		smooth_vase.m_transform.m_translation = { .5f, .5f, 0.f };
		smooth_vase.m_transform.m_scale = glm::vec3(3.f);
		m_deco_game_objects.emplace(smooth_vase.getId(), std::move(smooth_vase));

		deco_model = DecoModel::createModelFromFile(m_deco_device, "../resources/objs/quad.obj");
		auto floor = DecoGameObject::createGameObject();
		floor.m_model = deco_model;
		floor.m_transform.m_translation = { 0.f, .5f, 0.f };
		floor.m_transform.m_scale = glm::vec3(3.f);
		m_deco_game_objects.emplace(floor.getId(), std::move(floor));
	}
}