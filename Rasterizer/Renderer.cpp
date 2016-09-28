#include "stdafx.h"
#include "Renderer.h"

#include "Buffer.h"

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
