#pragma once

#include <wx/wx.h>

class ImagePanel : public wxPanel
{
	wxBitmap bitmap;

	void paintEvent(wxPaintEvent & evt);
	void paintNow();
	void ResizeArea();
public:
	ImagePanel(wxWindow * parent);
	void setImage(wxImage & image);

	DECLARE_EVENT_TABLE()
};