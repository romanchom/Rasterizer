#pragma once

#include "Scene.h"
#include "Buffer.h"
#include "Mesh.h"
#include "Texture.h"

class SampleScene : public Scene
{
private:
	Buffer mBuffer;
	Mesh asteroid;
	Mesh house;
	Mesh sphere;
	Texture stoneTexture;
	Texture grassTexture;
public:
	SampleScene();
	virtual ~SampleScene() {};
	void draw(class Renderer & renderer) override;
};

