#pragma once

#include "deco_device.h"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace Deco {

	class DecoDescriptorSetLayout {
	public:
		class Builder {
		public:
			Builder(Deco::DecoDevice& deco_device) : m_deco_device{ deco_device } {}

			Builder& addBinding(
				uint32_t binding,
				VkDescriptorType descriptorType,
				VkShaderStageFlags stageFlags,
				uint32_t count = 1);
			std::unique_ptr<DecoDescriptorSetLayout> build() const;

		private:
			Deco::DecoDevice& m_deco_device;
			std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
		};

		DecoDescriptorSetLayout(
			DecoDevice& deco_device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
		~DecoDescriptorSetLayout();
		DecoDescriptorSetLayout(const DecoDescriptorSetLayout&) = delete;
		DecoDescriptorSetLayout& operator=(const DecoDescriptorSetLayout&) = delete;

		VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

	private:
		DecoDevice& m_deco_device;
		VkDescriptorSetLayout descriptorSetLayout;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

		friend class DecoDescriptorWriter;
	};

	class DecoDescriptorPool {
	public:
		class Builder {
		public:
			Builder(DecoDevice& deco_device) : m_deco_device{ deco_device } {}

			Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
			Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
			Builder& setMaxSets(uint32_t count);
			std::unique_ptr<DecoDescriptorPool> build() const;

		private:
			DecoDevice& m_deco_device;
			std::vector<VkDescriptorPoolSize> poolSizes{};
			uint32_t maxSets = 1000;
			VkDescriptorPoolCreateFlags poolFlags = 0;
		};

		DecoDescriptorPool(
			DecoDevice& deco_device,
			uint32_t maxSets,
			VkDescriptorPoolCreateFlags poolFlags,
			const std::vector<VkDescriptorPoolSize>& poolSizes);
		~DecoDescriptorPool();
		DecoDescriptorPool(const DecoDescriptorPool&) = delete;
		DecoDescriptorPool& operator=(const DecoDescriptorPool&) = delete;

		bool allocateDescriptor(
			const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

		void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

		void resetPool();

	private:
		DecoDevice& m_deco_device;
		VkDescriptorPool descriptorPool;

		friend class DecoDescriptorWriter;
	};

	class DecoDescriptorWriter {
	public:
		DecoDescriptorWriter(DecoDescriptorSetLayout& setLayout, DecoDescriptorPool& pool);

		DecoDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
		DecoDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

		bool build(VkDescriptorSet& set);
		void overwrite(VkDescriptorSet& set);

	private:
		DecoDescriptorSetLayout& setLayout;
		DecoDescriptorPool& pool;
		std::vector<VkWriteDescriptorSet> writes;
	};

}  // namespace Deco