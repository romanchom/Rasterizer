#pragma once

#include <vector>
#include <string>

enum Addressing {
	CLAMP,
	REPEAT,
};

enum Sampling {
	NEAREST,
	LINEAR,
};

class Texture
{
private:
	std::vector<uint32_t> mData;
	uint32_t mWidth;
	uint32_t mHeight;
	vec<2> mSize;
	Addressing mAddressing;
	Sampling mSampling;
	uint32_t readSample(const vec<2> & uv);
public:
	Texture(const std::string & fileName);
	void setAddressing(Addressing a) { mAddressing = a; }
	void setSampling(Sampling s) { mSampling = s; }
	vec<4> sample(const vec<2> & uv);
};

