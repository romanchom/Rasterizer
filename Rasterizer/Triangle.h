#pragma once

#include "mathUtil.h"

struct alignas(16) Vertex {
	vec<4> p;
	vec<3> n;
	uint32_t c;
	vec<2> uv;

	float & x() { return p.x(); }
	float & y() { return p.y(); }
	float & z() { return p.z(); }
	float & w() { return p.w(); }

	float x() const { return p.x(); }
	float y() const { return p.y(); }
	float z() const { return p.z(); }
	float w() const { return p.w(); }
};

struct alignas(16) Triangle {
	Vertex v[3];
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
};

template<>
inline Vertex lerp(const Vertex & a, const Vertex & b, float t) {
	Vertex v;
	v.p = lerp(a.p, b.p, t);
	v.n = lerp(a.n, b.n, t);
	v.uv = lerp(a.uv, b.uv, t);
	return v;
}
