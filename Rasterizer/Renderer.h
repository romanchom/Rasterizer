#pragma once

#include "ImagePanel.h"

class Renderer
{
private:
	wxImage mImage;
	class Buffer * mRenderTarget;
public:
	Renderer();
	void saveToFile(const std::string & fileName);
	void present(ImagePanel * imagePanel);

	void setRenderTarget(class Buffer * buffer) { mRenderTarget = buffer;  }

	void clearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	void clearColor(uint32_t color);
	void clearDepth(float depth);
};



