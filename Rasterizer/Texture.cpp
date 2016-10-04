#include "stdafx.h"
#include "Texture.h"
#include "mathUtil.h"

inline uint32_t Texture::readSample(const vec<2> & uv)
{
	vec<2> pixelCoords = uv;
	switch (mAddressing) {
	case CLAMP:
		for (int i = 0; i < 2; ++i) {
			pixelCoords[i] = std::max(0.0f, std::min(pixelCoords[i], mSize[i]));
		}
		break;
	case REPEAT:
		for (int i = 0; i < 2; ++i) {
			pixelCoords[i] = modDown(pixelCoords[i], mSize[i]);
		}
		break;
	}
	Eigen::Vector2i intUv = pixelCoords.cast<int>();
	return mData[intUv.x() + intUv.y() * mWidth];
}

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

	switch (mSampling) {
	case NEAREST:
		return colorToVec(readSample(pixelCoords));
		break;
	case LINEAR:
		vec<4> samples[4];
		samples[0] = colorToVec(readSample(pixelCoords));
		samples[1] = colorToVec(readSample(pixelCoords + vec<2>(1.0f, 0.0f)));
		samples[2] = colorToVec(readSample(pixelCoords + vec<2>(0.0f, 1.0f)));
		samples[3] = colorToVec(readSample(pixelCoords + vec<2>(1.0f, 1.0f)));
		float dummy;
		float fracX = modf(pixelCoords.x(), &dummy);
		float fracY = modf(pixelCoords.y(), &dummy);
		
		return lerp(lerp(samples[0], samples[1], fracX), lerp(samples[2], samples[3], fracX), fracY);

		break;
	}

}
