#pragma once
#include "stdint.h"

struct FrameBuffer {
	uint32_t frameBufferID;
	uint32_t textureBufferID;
	uint32_t renderBufferID;
	
	FrameBuffer();
};