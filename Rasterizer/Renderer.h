#pragma once

#include <vector>
#include "ImagePanel.h"
#include "Mesh.h"

class alignas(16) Renderer
{
private:
	wxImage mImage;
	class Buffer * mRenderTarget;
	vec<4> screenAdd;
	vec<4> screenMul;
	std::vector<Vertex> mVertexCache;
	mat4 mTransform;
public:
	Renderer();
	void saveToFile(const std::string & fileName);
	void present(ImagePanel * imagePanel);

	void setRenderTarget(class Buffer * buffer);

	void clearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	void clearColor(uint32_t color);
	void clearDepth(float depth);
	void setTransformMatrix(const mat4 & matrix);
	void drawMesh(const Mesh & mesh);
	void drawTriangle(const Vertex ** verticies);
private:
	struct TriangleFillParams {
		float yLow;
		float yHigh;
		float x0;
		vec<2> vLeft;
		vec<2> vRight;
		vec<2> baryCoords[3];
		Vertex v[3];
	};
	void rasterizeTriangle(const struct Triangle & t);
	void fillTopHalfTri(const TriangleFillParams & t);
	void fillBottomHalfTri(const TriangleFillParams & t);
	void scanline();

};



