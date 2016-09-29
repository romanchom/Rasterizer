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

void Renderer::fillTopFlatTri(float x0, float x1, float y01, float x2, float y2) {
	float invslope0 = (x2 - x0) / (y2 - y01);
	float invslope1 = (x2 - x1) / (y2 - y01);

	// add 0.5f to round up or down
	x2 += 0.5f;
	y2 -= 0.5f;

	int iy = y2;
	int count = y01 - y2;
	for (int y = 0; y < count; ++y)
	{
		float curx0 = x2 + invslope0 * y, curx1 = x2 + invslope1 * y;

		for (int x = curx0, xEnd = curx1; x < xEnd; ++x) {
			mRenderTarget->color(x, y + iy) += 100;
		}
	}
}

void Renderer::fillBottomFlatTri(float x0, float x1, float y01, float x2, float y2) {
	float invslope0 = (x0 - x2) / (y2 - y01);
	float invslope1 = (x1 - x2) / (y2 - y01);

	// add 0.5f to round up or down
	x0 += 0.5f;
	x1 += 0.5f;
	y2 -= 0.5f;

	int count = y01 - y2;
	int iy = y2;
	for (int y = 0; y < count; ++y)
	{
		float curx0 = x0 + invslope0 * y, curx1 = x1 + invslope1 * y;

		for (int x = curx0, xEnd = curx1; x < xEnd; ++x) {
			mRenderTarget->color(x, y + iy) += 100;
		}
	}
}

#define V0 t.v[i[0]]
#define V1 t.v[i[1]]
#define V2 t.v[i[2]]

void Renderer::drawTriangle(const Triangle & t)
{
	uint32_t i[] = { 0, 1, 2 };

	// order verticies by y coordinate
	if (V0.y() > V2.y()) std::swap(i[0], i[2]);
	if (V0.y() > V1.y()) std::swap(i[0], i[1]);
	if (V1.y() > V2.y()) std::swap(i[1], i[2]);

	float x3 = V0.x() + ((V1.y() - V0.y()) / (V2.y() - V0.y()) * (V2.x() - V0.x()));

	float x4 = V1.x();
	if (x3 > x4) std::swap(x3, x4);

	fillTopFlatTri(x3, x4, V1.y(), V0.x(), V0.y());
	fillBottomFlatTri(x3, x4, V2.y(), V2.x(), V1.y());
}
