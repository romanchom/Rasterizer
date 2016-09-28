#include "stdafx.h"
#include "Buffer.h"

Buffer::Buffer(uint32_t width, uint32_t height) :
	mWidth(width),
	mHeight(height),
	mColor(width * height),
	mDepth(width * height)
{}
