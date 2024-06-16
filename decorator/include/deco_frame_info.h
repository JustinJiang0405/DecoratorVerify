#pragma once

#include "deco_camera.h"
#include "deco_game_object.h"

// lib
#include <vulkan/vulkan.h>

namespace Deco
{
	struct FrameInfo
	{
		int frame_index;
		float frame_time;
		VkCommandBuffer command_buffer;
		DecoCamera& camera;
		VkDescriptorSet global_descriptor_set;
		DecoGameObject::Map& game_objects;
	};
}