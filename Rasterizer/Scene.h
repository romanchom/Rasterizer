#pragma once


class Scene {
public:
	virtual ~Scene() {};
	virtual void draw(class Renderer & renderer) = 0;
};