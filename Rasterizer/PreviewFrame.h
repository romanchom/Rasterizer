#pragma once

#include "ImagePanel.h"
#include "Buffer.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Texture.h"

enum {
	ID_SAVESCREEN = 1,
};

class PreviewFrame : public wxFrame{
private:
	ImagePanel * imagePanel;
	Buffer mBuffer;
	Renderer mRenderer;
	Mesh cyllinder;
	Texture texture;

	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnSaveScreen(wxCommandEvent& event);
	void OnIdle(wxIdleEvent &event);
public:
	PreviewFrame(const wxString& title, const wxPoint& position, const wxSize& size);
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

	wxDECLARE_EVENT_TABLE();

};

