#include "stdafx.h"
#include "Texture.h"
#include "mathUtil.h"

Texture::Texture(const std::string & fileName)
{
	wxImage image(fileName);
	mWidth = image.GetWidth();
	mHeight = image.GetHeight();
	mSize = vec<2>(mWidth, mHeight);

	uint32_t count = mWidth * mHeight;
	mData.resize(count);
	for (uint32_t i = 0; i < count; ++i) {
		uint32_t rgb = *reinterpret_cast<uint32_t *>(image.GetData() + i * 3) & 0x00FFFFFF;
		mData[i] = rgb;
	}
}

vec<4> Texture::sample(const vec<2>& uv)
{
	
	vec<2> pixelCoords = uv;
	pixelCoords = pixelCoords.cwiseProduct(mSize);

	Eigen::Vector2i intUv = pixelCoords.cast<int>();

	uint32_t intColor = mData[intUv.x() + intUv.y() * mWidth];

	return colorToVec(intColor);
}
