#include "deco_descriptors.h"

// std
#include <cassert>
#include <stdexcept>

namespace Deco {

	// *************** Descriptor Set Layout Builder *********************

	DecoDescriptorSetLayout::Builder& DecoDescriptorSetLayout::Builder::addBinding(
		uint32_t binding,
		VkDescriptorType descriptorType,
		VkShaderStageFlags stageFlags,
		uint32_t count) {
		assert(bindings.count(binding) == 0 && "Binding already in use");
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding;
		layoutBinding.descriptorType = descriptorType;
		layoutBinding.descriptorCount = count;
		layoutBinding.stageFlags = stageFlags;
		bindings[binding] = layoutBinding;
		return *this;
	}

	std::unique_ptr<DecoDescriptorSetLayout> DecoDescriptorSetLayout::Builder::build() const {
		return std::make_unique<DecoDescriptorSetLayout>(m_deco_device, bindings);
	}

	// *************** Descriptor Set Layout *********************

	DecoDescriptorSetLayout::DecoDescriptorSetLayout(
		Deco::DecoDevice& deco_device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
		: m_deco_device{ deco_device }, bindings{ bindings } {
		std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
		for (auto kv : bindings) {
			setLayoutBindings.push_back(kv.second);
		}

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
		descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
		descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

		if (vkCreateDescriptorSetLayout(
			m_deco_device.device(),
			&descriptorSetLayoutInfo,
			nullptr,
			&descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	DecoDescriptorSetLayout::~DecoDescriptorSetLayout() {
		vkDestroyDescriptorSetLayout(m_deco_device.device(), descriptorSetLayout, nullptr);
	}

	// *************** Descriptor Pool Builder *********************

	DecoDescriptorPool::Builder& DecoDescriptorPool::Builder::addPoolSize(
		VkDescriptorType descriptorType, uint32_t count) {
		poolSizes.push_back({ descriptorType, count });
		return *this;
	}

	DecoDescriptorPool::Builder& DecoDescriptorPool::Builder::setPoolFlags(
		VkDescriptorPoolCreateFlags flags) {
		poolFlags = flags;
		return *this;
	}
	DecoDescriptorPool::Builder& DecoDescriptorPool::Builder::setMaxSets(uint32_t count) {
		maxSets = count;
		return *this;
	}

	std::unique_ptr<DecoDescriptorPool> DecoDescriptorPool::Builder::build() const {
		return std::make_unique<DecoDescriptorPool>(m_deco_device, maxSets, poolFlags, poolSizes);
	}

	// *************** Descriptor Pool *********************

	DecoDescriptorPool::DecoDescriptorPool(
		DecoDevice& deco_device,
		uint32_t maxSets,
		VkDescriptorPoolCreateFlags poolFlags,
		const std::vector<VkDescriptorPoolSize>& poolSizes)
		: m_deco_device{ deco_device } {
		VkDescriptorPoolCreateInfo descriptorPoolInfo{};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		descriptorPoolInfo.pPoolSizes = poolSizes.data();
		descriptorPoolInfo.maxSets = maxSets;
		descriptorPoolInfo.flags = poolFlags;

		if (vkCreateDescriptorPool(m_deco_device.device(), &descriptorPoolInfo, nullptr, &descriptorPool) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	DecoDescriptorPool::~DecoDescriptorPool() {
		vkDestroyDescriptorPool(m_deco_device.device(), descriptorPool, nullptr);
	}

	bool DecoDescriptorPool::allocateDescriptor(
		const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const {
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.pSetLayouts = &descriptorSetLayout;
		allocInfo.descriptorSetCount = 1;

		// Might want to create a "DescriptorPoolManager" class that handles this case, and builds
		// a new pool whenever an old pool fills up. But this is beyond our current scope
		if (vkAllocateDescriptorSets(m_deco_device.device(), &allocInfo, &descriptor) != VK_SUCCESS) {
			return false;
		}
		return true;
	}

	void DecoDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const {
		vkFreeDescriptorSets(
			m_deco_device.device(),
			descriptorPool,
			static_cast<uint32_t>(descriptors.size()),
			descriptors.data());
	}

	void DecoDescriptorPool::resetPool() {
		vkResetDescriptorPool(m_deco_device.device(), descriptorPool, 0);
	}

	// *************** Descriptor Writer *********************

	DecoDescriptorWriter::DecoDescriptorWriter(DecoDescriptorSetLayout& setLayout, DecoDescriptorPool& pool)
		: setLayout{ setLayout }, pool{ pool } {}

	DecoDescriptorWriter& DecoDescriptorWriter::writeBuffer(
		uint32_t binding, VkDescriptorBufferInfo* bufferInfo) {
		assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

		auto& bindingDescription = setLayout.bindings[binding];

		assert(
			bindingDescription.descriptorCount == 1 &&
			"Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pBufferInfo = bufferInfo;
		write.descriptorCount = 1;

		writes.push_back(write);
		return *this;
	}

	DecoDescriptorWriter& DecoDescriptorWriter::writeImage(
		uint32_t binding, VkDescriptorImageInfo* imageInfo) {
		assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

		auto& bindingDescription = setLayout.bindings[binding];

		assert(
			bindingDescription.descriptorCount == 1 &&
			"Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pImageInfo = imageInfo;
		write.descriptorCount = 1;

		writes.push_back(write);
		return *this;
	}

	bool DecoDescriptorWriter::build(VkDescriptorSet& set) {
		bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
		if (!success) {
			return false;
		}
		overwrite(set);
		return true;
	}

	void DecoDescriptorWriter::overwrite(VkDescriptorSet& set) {
		for (auto& write : writes) {
			write.dstSet = set;
		}
		vkUpdateDescriptorSets(pool.m_deco_device.device(), writes.size(), writes.data(), 0, nullptr);
	}

}  // namespace lve