#include "stdafx.h"
#include "SampleScene.h"
#include "Renderer.h"

SampleScene::SampleScene() :
	mBuffer(512, 512),
	asteroid("asset/asteroid.obj"),
	house("asset/house.obj"),
	sphere("asset/sphere.obj"),
	stoneTexture("asset/stone.jpg"),
	grassTexture("asset/grass.jpg")
{
	stoneTexture.setAddressing(CLAMP);
	stoneTexture.setSampling(LINEAR);
	grassTexture.setAddressing(REPEAT);
	grassTexture.setSampling(NEAREST);
}

void SampleScene::draw(Renderer & renderer)
{
	static float a = 0.0f;
	a += 0.002f;
	renderer.setRenderTarget(&mBuffer);
	renderer.clearColor(0, 0, 0, 255);
	renderer.clearDepth(0);


	float f = 200.0f;
	float n = 0.01f;
	mat4 perspective;
	perspective <<
		1, 0, 0, 0,
		0, -1, 0, 0,
		0, 0, n / (f - n), n * f / (f - n),
		0, 0, 1, 0;

	// global transformations
	Eigen::Affine3f transform;
	transform.setIdentity();
	transform.prerotate(Eigen::AngleAxisf(-0.3f, vec<3>::UnitX()));
	transform.pretranslate(vec<3>(0, -5, 20));
	
	renderer.setView(transform.matrix());
	renderer.setProjection(perspective);
	renderer.setLightDir(vec<3>(1, 1, -1));

	
	// first animated asteroid
	transform.setIdentity();
	transform.prerotate(Eigen::AngleAxisf(a, vec<3>::UnitY()));
	transform.pretranslate(vec<3>(0, 0, -5));
	renderer.setModel(transform.matrix());
	renderer.setTexture(&stoneTexture);
	renderer.drawMesh(asteroid);

	transform.setIdentity();
	transform.prerotate(Eigen::AngleAxisf(-a, vec<3>::UnitY()));
	transform.pretranslate(vec<3>(10, 0, 0));
	renderer.setModel(transform.matrix());
	renderer.setTexture(&grassTexture);
	renderer.drawMesh(asteroid);

	transform.setIdentity();
	transform.pretranslate(vec<3>(-10, 0, 0));
	renderer.setModel(transform.matrix());
	renderer.setTexture(&grassTexture);
	renderer.drawMesh(house);

	for (float x = -7.5f; x < 10; x += 5) {
		transform.setIdentity();
		transform.pretranslate(vec<3>(x, 15 + cos(a * 10 + x * 0.2f) * 3, 0));
		renderer.setModel(transform.matrix());
		renderer.drawMesh(sphere);
	}




}
