#include "stdafx.h"
#include "PreviewFrame.h"

#include <chrono>
#include <sstream>

#include "ImagePanel.h"
#include "Triangle.h"


wxBEGIN_EVENT_TABLE(PreviewFrame, wxFrame)
	EVT_MENU(wxID_EXIT, PreviewFrame::OnExit)
	EVT_MENU(wxID_ABOUT, PreviewFrame::OnAbout)
	EVT_MENU(ID_SAVESCREEN, PreviewFrame::OnSaveScreen)
	EVT_IDLE(PreviewFrame::OnIdle)
wxEND_EVENT_TABLE()


PreviewFrame::PreviewFrame(const wxString & title, const wxPoint & pos, const wxSize & size) :
	wxFrame(NULL, wxID_ANY, title, pos, size),
	mBuffer(512, 512)
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
	static float a = 0.0f;
	a += 0.002f;
	mRenderer.setRenderTarget(&mBuffer);
	mRenderer.clearColor(0, 0, 0, 255);
	
	const float radius = 1.2f, offset = 0.0f;
	int count = 6;
	for (int i = 0; i < count; ++i) {
		float s, c;
		Triangle t;
		t.v[0].p = vec<4>(offset, offset, 0.5f, 1);
		s = sin(a + M_PI * 2 * i / count);
		c = cos(a + M_PI * 2 * i / count);
		t.v[1].p = vec<4>(offset + radius * s, offset + radius * c, 0.5f, 1);
		s = sin(a + M_PI * 2 * (i + 1) / count);
		c = cos(a + M_PI * 2 * (i + 1) / count);
		t.v[2].p = vec<4>(offset + radius * s, offset + radius * c, 0.5f, 1);
		mRenderer.drawTriangle(t);
	}

	mRenderer.present(imagePanel);

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

