#pragma once

#include "deco_device.h"
#include "deco_buffer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace Deco
{
	class DecoModel
	{
	public:
		struct Vertex
		{
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex& other) const
			{
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}
		};

		struct Builder
		{
			std::vector<Vertex> m_vertices{};
			std::vector<uint32_t> m_indices{};

			void loadModel(const std::string& file_path);
		};

	public:
		DecoModel(DecoDevice &device, const DecoModel::Builder& builder);
		~DecoModel();

		DecoModel(const DecoModel&) = delete;
		DecoModel& operator=(const DecoModel&) = delete;

		static std::unique_ptr<DecoModel> createModelFromFile(DecoDevice& device, const std::string& file_path);

		void bind(VkCommandBuffer command_buffer);
		void draw(VkCommandBuffer command_buffer);

	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

	private:
		DecoDevice m_deco_device;

		// vertex buffer
		std::unique_ptr<DecoBuffer> m_vertex_buffer;
		uint32_t m_vertex_count;

		// index buffer
		bool m_has_index_buffer{ false };
		std::unique_ptr<DecoBuffer> m_index_buffer;
		uint32_t m_index_count;
	};
}