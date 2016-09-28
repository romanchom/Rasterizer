#pragma once

#include <vector>

class Buffer
{
private:
	uint32_t mWidth;
	uint32_t mHeight;
	std::vector<uint32_t> mColor;
	std::vector<float> mDepth;
public:
	Buffer(uint32_t width, uint32_t height);
	uint32_t width() const { return mWidth; }
	uint32_t height() const { return mHeight; }
	uint32_t count() const { return mColor.size(); }

	uint32_t & color(uint16_t x, uint16_t y) { return mColor[x + y * mWidth]; }
	uint32_t * colorBegin() { return mColor.data(); }
	uint32_t * colorEnd() { return mColor.data() + count(); }
	
	float & depth(uint16_t x, uint16_t y) { return mDepth[x + y * mWidth]; }
	float * depthBegin() { return mDepth.data(); }
	float * depthEnd() { return mDepth.data() + count(); }
};

