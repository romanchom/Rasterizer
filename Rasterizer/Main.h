#pragma once

#include "SampleScene.h"

class Main : public wxApp
{
private:
	SampleScene * mScene;
public:
	Main();
	virtual ~Main();
	bool OnInit() override;

	class ImageCompressor * imageCompressor;
};
