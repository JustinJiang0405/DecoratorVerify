#pragma once

#include "deco_device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace Deco
{
	class DecoModel
	{
	public:
		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

	public:
		DecoModel(DecoDevice &device, const std::vector<Vertex>& vertices);
		~DecoModel();

		DecoModel(const DecoModel&) = delete;
		DecoModel& operator=(const DecoModel&) = delete;

		void bind(VkCommandBuffer command_buffer);
		void draw(VkCommandBuffer command_buffer);

	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);

	private:
		DecoDevice m_deco_device;
		VkBuffer m_vertex_buffer;
		VkDeviceMemory m_vertex_buffer_memory;
		uint32_t m_vertex_count;
	};
}