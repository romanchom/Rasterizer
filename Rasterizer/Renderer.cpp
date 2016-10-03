#include "stdafx.h"
#include "Renderer.h"

#include "Buffer.h"

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


#define DRAWING_MODE 1


void Renderer::drawTriangle(const Triangle & t)
{
	static const int MAX_VERTS = 9;
	static const float EPSILON = 1.0E-20;
	Vertex buff0[MAX_VERTS], buff1[MAX_VERTS];
	Vertex * srcPoly = buff0, * dstPoly = buff1;

	uint32_t srcCount = 3;
	uint32_t dstCount = 0;

	for (int i = 0; i < 3; ++i) {
		srcPoly[i] = t.v[i];
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
			float denom = (srcPoly[j].p - srcPoly[(j + 1) % srcCount].p).dot(planeNormals[i]);
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
			// transform to screen coordinates
			v = v.cwiseProduct(screenMul) + screenAdd;
			float w = v.w();
			// homogeneous division
			v /= w;
			// restore w
			v.w() = w;
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

	if (v1->y() - v0->y() > 0) invSlope0 = (v1->x() - v0->x()) / (v1->y() - v0->y());
	if (v2->y() - v0->y() > 0) invSlope1 = (v2->x() - v0->x()) / (v2->y() - v0->y());

	TriangleFillParams params;
	params.yLow = v0->y();
	params.yHigh = v1->y();
	params.x0 = v0->x();
	params.vLeft = v1->p.head<2>();
	params.vRight = v2->p.head<2>();
	for (int i = 0; i < 3; ++i) {
		params.baryCoords[i] = baryCoords[indicies[i]];
	}
	if (invSlope0 > invSlope1) {
		std::swap(params.vLeft, params.vRight);
		std::swap(params.baryCoords[1], params.baryCoords[2]);
	}

	fillBottomHalfTri(params);

	params.yLow = params.yHigh;
	params.yHigh = v2->y();
	params.x0 = v2->x();
	params.vLeft = v1->p.head<2>();
	params.vRight = v0->p.head<2>();
	params.baryCoords[0] = baryCoords[indicies[2]];
	params.baryCoords[1] = baryCoords[indicies[1]];
	params.baryCoords[2] = baryCoords[indicies[0]];
	if (invSlope0 > invSlope1) {
		std::swap(params.vLeft, params.vRight);
		std::swap(params.baryCoords[1], params.baryCoords[2]);
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


void Renderer::fillBottomHalfTri(const TriangleFillParams & t) {
	float denom0 = t.vLeft.y() - t.yLow;
	float denom1 = t.vRight.y() - t.yLow;
	if (denom0 != 0.0) denom0 = 1.0f / denom0;
	if (denom1 != 0.0) denom1 = 1.0f / denom1;

	int yBegin = floor(t.yLow), yEnd = ceil(t.yHigh);

	for (int y = yBegin; y < yEnd; y++)
	{
		float t1 = (y - t.yLow) * denom1;
		float t0 = (y - t.yLow) * denom0;

		int sx = (int)lerp(t.x0, t.vLeft.x(), t0);
		int ex = (int)lerp(t.x0, t.vRight.x(), t1);
		vec<2> bcL = lerp(t.baryCoords[0], t.baryCoords[1], t0);
		vec<2> bcR = lerp(t.baryCoords[0], t.baryCoords[2], t1);

		for (int x = sx; x < ex; x++)
		{
			vec<2> baryCoords = lerp(bcL, bcR, (x - sx) / float(ex - sx));
			int r = 255 * baryCoords.x();
			int g = 255 * baryCoords.y();
			int b = (255 - r - g);
			mRenderTarget->color(x, y) = r + (g << 8) + (b << 16);
		}
	}
}

void Renderer::fillTopHalfTri(const TriangleFillParams & t) {
	float denom0 = t.yHigh - t.vLeft.y();
	float denom1 = t.yHigh - t.vRight.y();
	if (denom0 != 0.0) denom0 = 1.0f / denom0;
	if (denom1 != 0.0) denom1 = 1.0f / denom1;

	int yBegin = ceil(t.yLow), yEnd = floor(t.yHigh);

	for (int y = yBegin; y <= yEnd; y++)
	{
		float t0 = (y - t.vLeft.y()) * denom0;
		float t1 = (y - t.vRight.y()) * denom1;

		int sx = (int)lerp(t.vLeft.x(), t.x0, t0);
		int ex = (int)lerp(t.vRight.x(), t.x0, t1);
		vec<2> bcL = lerp(t.baryCoords[1], t.baryCoords[0], t0);
		vec<2> bcR = lerp(t.baryCoords[2], t.baryCoords[0], t1);

		for (int x = sx; x < ex; x++)
		{
			vec<2> baryCoords = lerp(bcL, bcR, (x - sx) / float(ex - sx));
			int r = 255 * baryCoords.x();
			int g = 255 * baryCoords.y();
			int b = (255 - r - g);
			mRenderTarget->color(x, y) = r + (g << 8) + (b << 16);
		}
	}
}