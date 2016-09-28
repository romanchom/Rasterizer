#pragma once

class Main : public wxApp
{
public:
	Main();
	virtual ~Main();
	bool OnInit() override;

	class ImageCompressor * imageCompressor;
};
