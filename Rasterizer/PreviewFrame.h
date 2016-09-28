#pragma once

#include "ImagePanel.h"
#include "Buffer.h"
#include "Renderer.h"

enum {
	ID_SAVESCREEN = 1,
};

class PreviewFrame : public wxFrame{
private:
	ImagePanel * imagePanel;
	Buffer mBuffer;
	Renderer mRenderer;


	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnSaveScreen(wxCommandEvent& event);
	void OnIdle(wxIdleEvent &event);
public:
	PreviewFrame(const wxString& title, const wxPoint& position, const wxSize& size);

	wxDECLARE_EVENT_TABLE();
};

