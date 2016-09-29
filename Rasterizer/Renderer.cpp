#include "stdafx.h"
#include "Renderer.h"

#include "Buffer.h"

#include "Triangle.h"

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

float lerp(float a, float b, float t) {
	return a * (1 - t) + b * t;
}

void Renderer::fillBottomHalfTri(float x0, float x1, float y01, float x2, float y2) {
	float invslope0 = (x2 - x0) / (y2 - y01);
	float invslope1 = (x2 - x1) / (y2 - y01);

	// subtract 0.5f to round up or down
	//x2 -= 0.5f;
	//y2 -= 0.5f;

	int iy = y2;
	float fcount = y01 - y2;
	int count = fcount + 0.5f;
	for (int y = 0; y <= count; ++y)
	{
		float t = y / fcount;
		//float curx0 = x2 + invslope0 * y, curx1 = x2 + invslope1 * y;
		float curx0 = lerp(x2, x0, t), curx1 = lerp(x2, x1, t);

		for (int x = curx0, xEnd = curx1; x < xEnd; ++x) {
			mRenderTarget->color(x, y + iy) += 127;
		}
	}
}

void Renderer::fillTopHalfTri(float x0, float x1, float y01, float x2, float y2) {
	float invslope0 = (x0 - x2) / (y2 - y01);
	float invslope1 = (x1 - x2) / (y2 - y01);

	// subtract 0.5f to round up or down
	//x0 -= 0.5f;
	//x1 -= 0.5f;
	//y2 += 0.5f;
	//y01 += 0.5f;

	float fcount = y01 - y2;
	int count = fcount + 0.5f;
	int iy = y2;
	for (int y = 1; y < count; ++y)
	{
		float t = y / fcount;
		//float curx0 = x0 + invslope0 * y, curx1 = x1 + invslope1 * y;
		float curx0 = lerp(x0, x2, t), curx1 = lerp(x1, x2, t);

		for (int x = curx0, xEnd = curx1; x < xEnd; ++x) {
			mRenderTarget->color(x, y + iy) += 127;
		}
	}
}

void asdasd(Buffer * b, float yLo, float yHi, float x0, const vec<3> & pb, const vec<3> & pd)
{
	float denom0 = 1.0f / (pb.y() - yLo);
	float denom1 = 1.0f / (pd.y() - yLo);
	
	int yBegin = yLo, yEnd = ceil(yHi);

	for (int y = yLo; y < yHi; y++)
	{
		float t0 = (y - yLo) * denom0;
		float t1 = (y - yLo) * denom1;

		int sx = (int)lerp(x0, pb.x(), t0);
		int ex = (int)lerp(x0, pd.x(), t1);

		for (int x = sx; x < ex; x++)
		{
			b->color(x, y) += 127;
		}
	}
	
}

void qweqwe(Buffer * b, float yLo, float yHi, const vec<3> & pa, const vec<3> & pb, const vec<3> & pc)
{

	int yBegin = ceil(yLo), yEnd = floor(yHi);

	for (int y = yBegin; y <= yEnd; y++)
	{
		float t0 = pa.y() != pb.y() ? (y - pa.y()) / (pb.y() - pa.y()) : 1;
		float t1 = pc.y() != pb.y() ? (y - pc.y()) / (pb.y() - pc.y()) : 1;

		int sx = (int)lerp(pa.x(), pb.x(), t0);
		int ex = (int)lerp(pc.x(), pb.x(), t1);

		for (int x = sx; x < ex; x++)
		{
			b->color(x, y) += 127;
		}
	}
}

#define DRAWING_MODE 1

void Renderer::drawTriangle(const Triangle & t)
{
#if DRAWING_MODE == 1 // two triangles
#define V0 t.v[i[0]]
#define V1 t.v[i[1]]
#define V2 t.v[i[2]]
	uint32_t i[] = { 0, 1, 2 };

	// order verticies by y coordinate
	if (V0.y() > V2.y()) std::swap(i[0], i[2]);
	if (V0.y() > V1.y()) std::swap(i[0], i[1]);
	if (V1.y() > V2.y()) std::swap(i[1], i[2]);

	float invSlope0 = 0, invSlope1 = 0;

	if (V1.y() - V0.y() > 0) invSlope0 = (V1.x() - V0.x()) / (V1.y() - V0.y());
	if (V2.y() - V0.y() > 0) invSlope1 = (V2.x() - V0.x()) / (V2.y() - V0.y());

	if (invSlope0 > invSlope1)
	{
		asdasd(mRenderTarget, V0.y(), V1.y(), V0.x(), V2, V1);
		qweqwe(mRenderTarget, V1.y(), V2.y(), V0, V2, V1);
	}
	else
	{
		asdasd(mRenderTarget, V0.y(), V1.y(), V0.x(), V1, V2);
		qweqwe(mRenderTarget, V1.y(), V2.y(), V1, V2, V0);
	}



	//float x3 = V0.x() + ((V1.y() - V0.y()) / (V2.y() - V0.y()) * (V2.x() - V0.x()));
	/*float x3 = lerp(V0.x(), V2.x(), (V1.y() - V0.y()) / (V2.y() - V0.y()));

	float x4 = V1.x();
	if (x3 > x4) std::swap(x3, x4);

	fillBottomHalfTri(x3, x4, V1.y(), V0.x(), V0.y());
	fillTopHalfTri(x3, x4, V2.y(), V2.x(), V1.y());*/
#elif DRAWING_MODE == 2 // different approach to two triangles
#define V0 t.v[i[0]]
#define V1 t.v[i[1]]
#define V2 t.v[i[2]]
#define VA t.v[i[a]]
#define VB t.v[i[b]]

	uint32_t i[] = { 0, 1, 2 };

	// order verticies by y coordinate
	if (V0.y() > V2.y()) std::swap(i[0], i[2]);
	if (V0.y() > V1.y()) std::swap(i[0], i[1]);
	if (V1.y() > V2.y()) std::swap(i[1], i[2]);

	//float x3 = V0.x() + ((V1.y() - V0.y()) / (V2.y() - V0.y()) * (V2.x() - V0.x()));
	float x3 = lerp(V0.x(), V2.x(), (V1.y() - V0.y()) / (V2.y() - V0.y()));


	float ea[4], eb[4];
	for (int a = 0; a < 3; ++a) {
		int b = (a + 1) % 3;
		float denom = VB.y() - VA.y();
		ea[a] = (VB.x() - VA.x()) / denom;
		eb[a] = (VB.y() * VA.x() - VA.y() * VB.x()) / denom + 1.0f;
	}
	ea[3] = ea[2];
	eb[3] = eb[2];

	int ys[3] = { V0.y() + 1.0f, V1.y() + 0.5f, V2.y() + 0.0f };
	auto xminmax = std::minmax({ t.v[0].x(), t.v[1].x(), t.v[2].x() });

	if (x3 > V1.x()) {
		std::swap(ea[0], ea[2]);
		std::swap(eb[0], eb[2]);
		std::swap(ea[1], ea[3]);
		std::swap(eb[1], eb[3]);
	}

	for (int y = ys[0]; y < ys[1]; ++y) {
		for (int x = ea[2] * y + eb[2], xEnd = ea[0] * y + eb[0]; x < xEnd; ++x) {
			mRenderTarget->color(x, y) += 100;
		}
	}
	for (int y = ys[1]; y < ys[2]; ++y) {
		for (int x = ea[3] * y + eb[3], xEnd = ea[1] * y + eb[1]; x < xEnd; ++x) {
			mRenderTarget->color(x, y) += 100;
		}
	}
	
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

	auto xminmax = std::minmax({V0.x(), V1.x(), V2.x()});
	auto yminmax = std::minmax({V0.y(), V1.y(), V2.y()});

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
