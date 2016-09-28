#include "stdafx.h"

#include "ImagePanel.h"


BEGIN_EVENT_TABLE(ImagePanel, wxPanel)
	EVT_PAINT(ImagePanel::paintEvent)
END_EVENT_TABLE()


ImagePanel::ImagePanel(wxWindow * parent) :
	wxPanel(parent)
{
	bitmap.Create(512, 512, 8);
	ResizeArea();
}


void ImagePanel::paintEvent(wxPaintEvent & evt)
{
	paintNow();
}

void ImagePanel::paintNow()
{
	wxClientDC dc(this);
	dc.DrawBitmap(bitmap, 0, 0, false);
}

void ImagePanel::ResizeArea()
{
	SetMinSize(bitmap.GetSize());
}

void ImagePanel::setImage(wxImage & image)
{
	bitmap = wxBitmap(image);
	ResizeArea();
	paintNow();
}
