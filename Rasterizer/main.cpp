#include "stdafx.h"

#include "Main.h"

#include "PreviewFrame.h"

Main::Main() 
{
}

bool Main::OnInit()
{
	wxInitAllImageHandlers();

	PreviewFrame * frame = new PreviewFrame("Hello World", wxPoint(50, 50), wxSize(1200, 900));
	mScene = new SampleScene();
	frame->setScene(mScene);
	frame->Show(true);
	return true;
}

Main::~Main()
{
	delete mScene;
}


wxIMPLEMENT_APP(Main);