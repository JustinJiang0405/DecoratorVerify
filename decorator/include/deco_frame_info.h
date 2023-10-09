#pragma once

#include "deco_camera.h"

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
	};
}