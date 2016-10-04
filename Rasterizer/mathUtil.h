#pragma once


template<typename T>
T lerp(const T & a, const T & b, float t) {
	return a * (1 - t) + b * t;
}

inline uint32_t vecToColor(const vec<4> & c) {
	uint32_t ret = 0;
	for (int i = 0; i < 4; ++i) {
		float v = c[i];
		v = std::max(0.0f, v);
		v = std::min(1.0f, v);
		v *= 255;
		ret |= static_cast<uint32_t>(v) << (8 * i);
	}
	return ret;
}

inline vec<4> colorToVec(uint32_t c) {
	vec<4> ret;
	for (int i = 0; i < 4; ++i) {
		ret[i] = ((c >> i * 8) & 0xFF) / 255.0f;
	}
	return ret;
}