#pragma once

#include <vector>
#include <string>

class Texture
{
private:
	std::vector<uint32_t> mData;
	uint32_t mWidth;
	uint32_t mHeight;
	vec<2> mSize;
public:
	Texture(const std::string & fileName);

	vec<4> sample(const vec<2> & uv);
};

