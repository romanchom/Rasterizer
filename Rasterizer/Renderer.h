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
	void setTransformMatrix();
	void drawTriangle(const class Triangle & t);
private:
	struct TriangleFillParams {
		float yLow;
		float yHigh;
		float x0;
		vec<2> vLeft;
		vec<2> vRight;
		vec<2> baryCoords[3];
	};
	void rasterizeTriangle(const class Triangle & t);
	void clipX(const class Triangle & t);
	void clipY(const class Triangle & t);
	void clipZ(const class Triangle & t);
	void fillTopHalfTri(const TriangleFillParams & t);
	void fillBottomHalfTri(const TriangleFillParams & t);
};



