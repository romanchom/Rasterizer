#include "stdafx.h"
#include "Renderer.h"

#include "Buffer.h"
#include "Texture.h"

#include "Triangle.h"

#include "mathUtil.h"

uint8_t NOTNULLDATA[] = { 1, 2, 3, 4, };

Renderer::Renderer() :
	mImage(1, 1, NOTNULLDATA, true)
{
}

void Renderer::saveToFile(const std::string & fileName)
{
	mImage.SaveFile(fileName, wxBitmapType::wxBITMAP_TYPE_PNG);
}

void Renderer::present(ImagePanel * imagePanel)
{
	if (mImage.GetHeight() != mRenderTarget->height() || mImage.GetWidth() != mRenderTarget->height()) {
		mImage.Resize(wxSize(mRenderTarget->width(), mRenderTarget->height()), wxPoint());
	}

	// mighty slow copy, but u can't do it better in wxWidgets
	uint8_t * rgb = mImage.GetData();
	uint8_t * rgba = reinterpret_cast<uint8_t *>(mRenderTarget->colorBegin());
	uint8_t * rgbaEnd = reinterpret_cast<uint8_t *>(mRenderTarget->colorEnd());

	while(rgba != rgbaEnd) {
		rgb[0] = rgba[0];
		rgb[1] = rgba[1];
		rgb[2] = rgba[2];

		rgb += 3;
		rgba += 4;
	}

	imagePanel->setImage(mImage);
}

void Renderer::setRenderTarget(Buffer * buffer)
{ 
	mRenderTarget = buffer;
	screenMul = vec<4>(buffer->width() / 2.0f, buffer->height() / 2.0f, 1.0f, 1.0f);
	screenAdd = vec<4>(buffer->width() / 2.0f, buffer->height() / 2.0f, 0.0f, 0.0f);
}

void Renderer::clearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	clearColor(static_cast<uint32_t>(r) |
		(static_cast<uint32_t>(g) << 8) |
		(static_cast<uint32_t>(b) << 16) |
		(static_cast<uint32_t>(a) << 24));
}

void Renderer::clearColor(uint32_t color)
{
	for (uint32_t * p = mRenderTarget->colorBegin(), *end = mRenderTarget->colorEnd(); p != end; ++p) {
		*p = color;
	}
}

void Renderer::clearDepth(float depth)
{
	for (float * p = mRenderTarget->depthBegin(), *end = mRenderTarget->depthEnd(); p != end; ++p) {
		*p = depth;
	}
}

void Renderer::setTexture(Texture * texture)
{
	mTexture = texture;
}

void Renderer::setLightDir(const vec<3>& dir)
{
	mLightDir = dir;
	mLightDir.normalize();
}

#define DRAWING_MODE 1


void Renderer::drawMesh(const Mesh & mesh)
{
	if (mVertexCache.size() < mesh.verticies.size()) {
		mVertexCache.resize(mesh.verticies.size());
	}

	mat4 mvp = mProjection * mView * mModel;

	for (int i = 0; i < mesh.verticies.size(); ++i) {
		mVertexCache[i].p = mvp * mesh.verticies[i].p;
		mVertexCache[i].uv = mesh.verticies[i].uv;
		vec<4> n; 
		n << mesh.verticies[i].n, 0.0f;
		mVertexCache[i].n = (mModel * n).head<3>();
	}

	const Vertex * verticies[3];
	for (int i = 0; i < mesh.indicies.size(); i += 3) {
		for (int j = 0; j < 3; ++j) {
			verticies[j] = &mVertexCache[mesh.indicies[i + j]];
		}
		drawTriangle(verticies);
	}
}

void Renderer::drawTriangle(const Vertex ** verticies)
{
	// early backface culling
	{
		vec<4> temp[3];
		for (int i = 0; i < 3; ++i) {
			temp[i] = verticies[i]->p / verticies[i]->p.w();
		}
		vec<4> ab = temp[1] - temp[0];
		vec<4> ac = temp[2] - temp[0];
		float crossZ = ab[0] * ac[1] - ab[1] * ac[0];
		if (crossZ <= 0) return;
	}

	static const int MAX_VERTS = 9;
	static const float EPSILON = 1.0E-20;
	Vertex buff0[MAX_VERTS], buff1[MAX_VERTS];
	Vertex * srcPoly = buff0, * dstPoly = buff1;

	uint32_t srcCount = 3;
	uint32_t dstCount = 0;

	for (int i = 0; i < 3; ++i) {
		srcPoly[i] = *verticies[i];
	}

	vec<4> planeNormals[6] = {
		{ -1,  0,  0, 1 },	// X plane
		{  1,  0,  0, 1 },	// -X plane
		{  0, -1,  0, 1 },	// Y plane
		{  0,  1,  0, 1 },	// -Y plane
		{  0,  0, -1, 1 },	// Z plane
		{  0,  0,  1, 0 },	// -Z plane <- z > 0
	};

	// for each plane
	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < srcCount; ++j) {

			float dot = srcPoly[j].p.dot(planeNormals[i]);
			if (dot > 0) {
				// point is on non culled side
				dstPoly[dstCount] = srcPoly[j];
				++dstCount;
			}
			float denom = dot - srcPoly[(j + 1) % srcCount].p.dot(planeNormals[i]);
			if (std::abs(denom) > EPSILON) {
				float t = dot / denom;
				// line segment connecting two verticies intersects the hyperplane
				if (t > 0.0f && t < 1.0f) {
					dstPoly[dstCount] = lerp(srcPoly[j], srcPoly[(j + 1) % srcCount], t);
					++dstCount;
				}
			}
		}
		srcCount = dstCount;
		dstCount = 0;
		if (srcCount < 3) {
			break; // no vaild triangle, no point in culling it even more
		}
		std::swap(srcPoly, dstPoly);
	}

	if(srcCount >= 3) {
		for (int i = 0; i < srcCount; ++i) {
			vec<4> & v = srcPoly[i].p;
			float w = v.w();
			// homogeneous division
			v /= w;
			// divide attributes by w as well to interpolate them correctly wrt perspective
			srcPoly[i].uv /= w;
			srcPoly[i].n /= w;
			// restore w
			v.w() = w;
			// transform to screen coordinates
			v = v.cwiseProduct(screenMul) + screenAdd;
		}
		Triangle t;
		t.v[0] = srcPoly[0];

		while (srcCount > 2) {
			t.v[1] = srcPoly[srcCount - 2];
			t.v[2] = srcPoly[srcCount - 1];
			rasterizeTriangle(t);
			--srcCount;
		}
	}
}


void Renderer::rasterizeTriangle(const Triangle & t)
{
#if DRAWING_MODE == 1 // two triangles
	uint32_t indicies[3] = { 0, 1, 2 };
	vec<2> baryCoords[3] = { { 1, 0 },{ 0, 1 },{ 0, 0 } };

	// order verticies by y coordinate
	if (t.v[0].y() > t.v[2].y()) std::swap(indicies[0], indicies[2]);
	if (t.v[indicies[0]].y() > t.v[indicies[1]].y()) std::swap(indicies[0], indicies[1]);
	if (t.v[indicies[1]].y() > t.v[indicies[2]].y()) std::swap(indicies[1], indicies[2]);
	const Vertex * v0 = t.v + indicies[0], *v1 = t.v + indicies[1], *v2 = t.v + indicies[2];

	float invSlope0 = 0, invSlope1 = 0;

	/*if (v1->y() - v0->y() > 0)*/ invSlope0 = (v1->x() - v0->x()) / (v1->y() - v0->y());
	/*if (v2->y() - v0->y() > 0)*/ invSlope1 = (v2->x() - v0->x()) / (v2->y() - v0->y());

	TriangleFillParams params;
	params.yLow = v0->y();
	params.yHigh = v1->y();
	params.v[0] = *v0;
	params.v[1] = *v1;
	params.v[2] = *v2;
	if (invSlope0 > invSlope1) {
		std::swap(params.v[1], params.v[2]);
	}

	fillBottomHalfTri(params);

	params.yLow = params.yHigh;
	params.yHigh = v2->y();
	params.v[0] = *v2;
	params.v[1] = *v1;
	params.v[2] = *v0;
	if (invSlope0 > invSlope1) {
		std::swap(params.v[1], params.v[2]);
	}

	fillTopHalfTri(params);


#else // half-space
#define V0 t.v[0]
#define V1 t.v[1]
#define V2 t.v[2]
	float xd0 = V1.x() - V0.x();
	float xd1 = V2.x() - V1.x();
	float xd2 = V0.x() - V2.x();

	float yd0 = V1.y() - V0.y();
	float yd1 = V2.y() - V1.y();
	float yd2 = V0.y() - V2.y();

	auto xminmax = std::minmax({ V0.x(), V1.x(), V2.x() });
	auto yminmax = std::minmax({ V0.y(), V1.y(), V2.y() });

	int xmin = xminmax.first;
	int xmax = xminmax.second;

	for (int y = yminmax.first, ymax = yminmax.second; y < ymax; ++y) {
		for (int x = xmin; x < xmax; ++x) {
			if ((((y - V0.y()) * xd0 - (x - V0.x()) * yd0) < 0) &&
				(((y - V1.y()) * xd1 - (x - V1.x()) * yd1) < 0) &&
				(((y - V2.y()) * xd2 - (x - V2.x()) * yd2) < 0)) {
				mRenderTarget->color(x, y) += 100;
			}
		}
	}
#endif
}


inline void Renderer::fillBottomHalfTri(TriangleFillParams & t) {
	float denom0 = t.v[1].y() - t.yLow;
	float denom1 = t.v[2].y() - t.yLow;
	if (denom0 != 0.0) denom0 = 1.0f / denom0;
	if (denom1 != 0.0) denom1 = 1.0f / denom1;
	const vec<2> baryConst[3] = { { 1, 0 },{ 0, 1 },{ 0, 0 } };

	int yBegin = floor(t.yLow), yEnd = ceil(t.yHigh);

	for (int y = yBegin; y < yEnd; y++)
	{
		float t1 = (y - t.yLow) * denom1;
		float t0 = (y - t.yLow) * denom0;

		t.xBegin = (int)lerp(t.v[0].x(), t.v[1].x(), t0);
		t.xEnd = (int)lerp(t.v[0].x(), t.v[2].x(), t1);
		t.bcL = lerp(baryConst[0], baryConst[1], t0);
		t.bcR = lerp(baryConst[0], baryConst[2], t1);

		t.currentY = y;

		scanline(t);
	}
}

inline void Renderer::fillTopHalfTri(TriangleFillParams & t) {
	float denom0 = t.yHigh - t.v[1].y();
	float denom1 = t.yHigh - t.v[2].y();
	if (denom0 != 0.0) denom0 = 1.0f / denom0;
	if (denom1 != 0.0) denom1 = 1.0f / denom1;
	const vec<2> baryConst[3] = { { 1, 0 },{ 0, 1 },{ 0, 0 } };

	int yBegin = ceil(t.yLow), yEnd = floor(t.yHigh);

	for (int y = yBegin; y <= yEnd; y++)
	{
		float t0 = (y - t.v[1].y()) * denom0;
		float t1 = (y - t.v[2].y()) * denom1;

		t.xBegin = (int)lerp(t.v[1].x(), t.v[0].x(), t0);
		t.xEnd = (int)lerp(t.v[2].x(), t.v[0].x(), t1);
		t.bcL = lerp(baryConst[1], baryConst[0], t0);
		t.bcR = lerp(baryConst[2], baryConst[0], t1);

		t.currentY = y;

		scanline(t);
	}
}

inline void Renderer::scanline(const TriangleFillParams & t)
{
	float lerpMul = 1.0f / static_cast<float>(t.xEnd - t.xBegin);

	for (int x = t.xBegin; x < t.xEnd; x++)
	{
		vec<3> baryCoords;
		baryCoords.topRows<2>() = lerp(t.bcL, t.bcR, (x - t.xBegin) * lerpMul);
		baryCoords[2] = 1.0f - baryCoords[0] - baryCoords[1];

		vec<4> interPos = t.v[0].p * baryCoords[0] + t.v[1].p * baryCoords[1] + t.v[2].p * baryCoords[2];
		
		float depth = interPos.z();
		if (mRenderTarget->depth(x, t.currentY) > depth) continue;
		mRenderTarget->depth(x, t.currentY) = depth;

		float w = interPos.w();
		interPos *= w;
		vec<2> uv = t.v[0].uv * baryCoords[0] + t.v[1].uv * baryCoords[1] + t.v[2].uv * baryCoords[2];
		uv *= w;
		vec<3> n = t.v[0].n * baryCoords[0] + t.v[1].n * baryCoords[1] + t.v[2].n * baryCoords[2];
		n *= w;

		float light = std::max(0.0f, mLightDir.dot(n));

		mRenderTarget->color(x, t.currentY) = vecToColor(mTexture->sample(uv) * light);
	}
}