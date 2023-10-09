#include "deco_model.h"
#include "deco_utils.h"

// libs
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

// std
#include <cassert>
#include <cstring>
#include <unordered_map>

namespace std
{
	template<>
	struct hash<Deco::DecoModel::Vertex>
	{
		size_t operator()(Deco::DecoModel::Vertex const &vertex) const
		{
			size_t seed = 0;
			Deco::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

namespace Deco
{
	DecoModel::DecoModel(DecoDevice& device, const DecoModel::Builder& builder) : m_deco_device(device)
	{
		createVertexBuffers(builder.m_vertices);
		createIndexBuffers(builder.m_indices);
	}

	DecoModel::~DecoModel()
	{
	}

	void DecoModel::createVertexBuffers(const std::vector<Vertex>& vertices)
	{
		m_vertex_count = static_cast<uint32_t>(vertices.size());
		assert(m_vertex_count >= 3 && "Vertex count must be at least 3");
		VkDeviceSize buffer_size = sizeof(vertices[0]) * m_vertex_count;

		uint32_t vertex_size = sizeof(vertices[0]);

		DecoBuffer staging_buffer
		{
			m_deco_device,
			vertex_size,
			m_vertex_count,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		staging_buffer.map();
		staging_buffer.writeToBuffer((void*)vertices.data());

		m_vertex_buffer = std::make_unique<DecoBuffer>(
			m_deco_device,
			vertex_size,
			m_vertex_count,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		// move the content from the staging buffer to the vertex buffer
		m_deco_device.copyBuffer(staging_buffer.getBuffer(), m_vertex_buffer->getBuffer(), buffer_size);
	}

	void DecoModel::createIndexBuffers(const std::vector<uint32_t>& indices)
	{
		m_index_count = static_cast<uint32_t>(indices.size());
		m_has_index_buffer = m_index_count > 0;

		if (!m_has_index_buffer)
		{
			return;
		}

		VkDeviceSize buffer_size = sizeof(indices[0]) * m_index_count;
		uint32_t index_size = sizeof(indices[0]);

		DecoBuffer staging_buffer{
			m_deco_device,
			index_size,
			m_index_count,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		staging_buffer.map();
		staging_buffer.writeToBuffer((void*)indices.data());

		m_index_buffer = std::make_unique<DecoBuffer>(
			m_deco_device,
			index_size,
			m_index_count,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		// move the content from the staging buffer to the vertex buffer
		m_deco_device.copyBuffer(staging_buffer.getBuffer(), m_index_buffer->getBuffer(), buffer_size);
	}

	std::unique_ptr<DecoModel> DecoModel::createModelFromFile(DecoDevice& device, const std::string& file_path)
	{
		Builder builder{};
		builder.loadModel(file_path);

		return std::make_unique<DecoModel>(device, builder);
	}

	void DecoModel::bind(VkCommandBuffer command_buffer)
	{
		VkBuffer buffers[] = { m_vertex_buffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);

		if (m_has_index_buffer)
		{
			vkCmdBindIndexBuffer(command_buffer, m_index_buffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void DecoModel::draw(VkCommandBuffer command_buffer)
	{
		if (m_has_index_buffer)
		{
			vkCmdDrawIndexed(command_buffer, m_index_count, 1, 0, 0, 0);
		}
		else
		{
			vkCmdDraw(command_buffer, m_vertex_count, 1, 0, 0);
		}
	}


	std::vector<VkVertexInputBindingDescription> DecoModel::Vertex::getBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> binding_descriptions(1);
		binding_descriptions[0].binding = 0;
		binding_descriptions[0].stride = sizeof(Vertex);
		binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return binding_descriptions;
	}

	std::vector<VkVertexInputAttributeDescription> DecoModel::Vertex::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attribute_descriptions{};

		attribute_descriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
		attribute_descriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
		attribute_descriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
		attribute_descriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

		return attribute_descriptions;
	}

	void DecoModel::Builder::loadModel(const std::string& filepath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
		{
			throw std::runtime_error(warn + err);
		}

		m_vertices.clear();
		m_indices.clear();

		std::unordered_map<Vertex, uint32_t> unique_vertices{};
		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};

				if (index.vertex_index >= 0)
				{
					vertex.position =
					{
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2],
					};

					vertex.color =
					{
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2],
					};
				}

				if (index.normal_index >= 0)
				{
					vertex.normal =
					{
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
					};
				}

				if (index.texcoord_index >= 0)
				{
					vertex.uv =
					{
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1],
					};
				}

				if (unique_vertices.count(vertex) == 0)
				{
					unique_vertices[vertex] = static_cast<uint32_t>(m_vertices.size());
					m_vertices.push_back(vertex);
				}
				m_indices.push_back(unique_vertices[vertex]);
			}
		}
	}

}