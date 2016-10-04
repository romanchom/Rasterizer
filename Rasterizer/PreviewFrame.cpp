#include "stdafx.h"
#include "PreviewFrame.h"

#include <chrono>
#include <sstream>

#include "ImagePanel.h"
#include "Scene.h"


wxBEGIN_EVENT_TABLE(PreviewFrame, wxFrame)
	EVT_MENU(wxID_EXIT, PreviewFrame::OnExit)
	EVT_MENU(wxID_ABOUT, PreviewFrame::OnAbout)
	EVT_MENU(ID_SAVESCREEN, PreviewFrame::OnSaveScreen)
	EVT_IDLE(PreviewFrame::OnIdle)
wxEND_EVENT_TABLE()


PreviewFrame::PreviewFrame(const wxString & title, const wxPoint & pos, const wxSize & size) :
	wxFrame(NULL, wxID_ANY, title, pos, size),
	mScene(nullptr)
{
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(wxID_EXIT);
	menuFile->Append(ID_SAVESCREEN, "Save screen");

	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);

	CreateStatusBar();

	wxBoxSizer * verticalSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(verticalSizer);

	imagePanel = new ImagePanel(this);

	verticalSizer->Add(imagePanel, 1, wxEXPAND | wxALL, 5);

	SetStatusText("Welcome to wxWidgets!");

}

void PreviewFrame::OnExit(wxCommandEvent & event)
{
	Close(true);
}

void PreviewFrame::OnAbout(wxCommandEvent & event)
{
	wxLogMessage("Hello world from wxWidgets!");
}

void PreviewFrame::OnSaveScreen(wxCommandEvent & event)
{
	mRenderer.saveToFile("printScreen.png");
}

std::chrono::high_resolution_clock::time_point lastFrame;

void PreviewFrame::OnIdle(wxIdleEvent & event)
{
	if (mScene != nullptr) {
		mScene->draw(mRenderer);
		mRenderer.present(imagePanel);
	}
	wxPaintEvent unused;
	OnPaint(unused);
	event.RequestMore();

	auto now = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> dur = now - lastFrame;
	lastFrame = now;

	std::ostringstream str;
	str << "FPS: " << 1.0 / dur.count();

	SetStatusText(str.str().c_str());
}

