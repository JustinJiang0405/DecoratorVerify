#pragma once

#include "deco_device.h"

namespace Deco {

	class DecoBuffer {
	public:
		DecoBuffer(
			DecoDevice& device,
			VkDeviceSize instanceSize,
			uint32_t instanceCount,
			VkBufferUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryPropertyFlags,
			VkDeviceSize minOffsetAlignment = 1);
		~DecoBuffer();

		DecoBuffer(const DecoBuffer&) = delete;
		DecoBuffer& operator=(const DecoBuffer&) = delete;

		VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		void unmap();

		void writeToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

		void writeToIndex(void* data, int index);
		VkResult flushIndex(int index);
		VkDescriptorBufferInfo descriptorInfoForIndex(int index);
		VkResult invalidateIndex(int index);

		VkBuffer getBuffer() const { return buffer; }
		void* getMappedMemory() const { return mapped; }
		uint32_t getInstanceCount() const { return m_instance_count; }
		VkDeviceSize getInstanceSize() const { return m_instance_size; }
		VkDeviceSize getAlignmentSize() const { return m_instance_size; }
		VkBufferUsageFlags getUsageFlags() const { return m_usage_flags; }
		VkMemoryPropertyFlags getMemoryPropertyFlags() const { return m_memory_property_flags; }
		VkDeviceSize getBufferSize() const { return m_buffer_size; }

	private:
		static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

		DecoDevice& m_device;
		void* mapped = nullptr;
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;

		VkDeviceSize m_buffer_size;
		uint32_t m_instance_count;
		VkDeviceSize m_instance_size;
		VkDeviceSize m_alignment_size;
		VkBufferUsageFlags m_usage_flags;
		VkMemoryPropertyFlags m_memory_property_flags;
	};

}  // namespace Deco