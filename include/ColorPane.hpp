#pragma once

#include <wx/wx.h>
#include <wx/graphics.h>
#include "SelectablePane.hpp"

class ColorPane: public SelectablePane
{
public:
  ColorPane(wxWindow* parent, const wxColor& paneColor);
  wxColor color;

private:
  virtual void DrawingContent(wxGraphicsContext* gc, const wxRect &rect, int roundness) override;
};

ColorPane::ColorPane(wxWindow* parent, const wxColor& paneColor)
  : SelectablePane(parent), color(paneColor)
{
}

void ColorPane::DrawingContent(wxGraphicsContext* gc, const wxRect &rect, int roundness)
{
  gc->SetPen(wxPen(color));
  gc->SetBrush(wxBrush(color));

  gc->DrawRoundedRectangle(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight(), roundness);
}