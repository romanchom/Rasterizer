#pragma once

#include <vector>
#include "ImagePanel.h"
#include "Mesh.h"

class alignas(16) Renderer
{
private:
	wxImage mImage;
	class Buffer * mRenderTarget;
	class Texture * mTexture;
	vec<4> screenAdd;
	vec<4> screenMul;
	std::vector<Vertex> mVertexCache;
	mat4 mModel;
	mat4 mView;
	mat4 mProjection;
	vec<3> mLightDir;
public:
	Renderer();
	void saveToFile(const std::string & fileName);
	void present(ImagePanel * imagePanel);

	void setRenderTarget(class Buffer * buffer);

	void clearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	void clearColor(uint32_t color);
	void clearDepth(float depth);
	void setModel(const mat4 & m) { mModel = m; }
	void setView(const mat4 & m) { mView = m; }
	void setProjection(const mat4 & m) { mProjection = m; }
	void setTexture(class Texture * texture);
	void setLightDir(const vec<3> & dir);
	void drawMesh(const Mesh & mesh);
	void drawTriangle(const Vertex ** verticies);
private:
	struct TriangleFillParams {
		float yLow;
		float yHigh;
		Vertex v[3];
		int currentY;
		int xBegin;
		int xEnd;
		vec<2> bcL;
		vec<2> bcR;
	};
	void rasterizeTriangle(const struct Triangle & t);
	void fillTopHalfTri(TriangleFillParams & t);
	void fillBottomHalfTri(TriangleFillParams & t);
	void scanline(const TriangleFillParams & t);

};



