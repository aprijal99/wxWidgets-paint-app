#pragma once

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <wx/settings.h>

class SelectablePane: public wxWindow
{
public:
  SelectablePane(wxWindow* parent);
  wxSize DoGetBestSize() const override
  {
    return FromDIP(wxSize(45, 45));
  }
  bool selected = false;

protected:
  virtual void DrawingContent(wxGraphicsContext* gc, const wxRect &rect, int roundness) = 0;
  
private:
  void OnPaint(wxPaintEvent& event);
};

SelectablePane::SelectablePane(wxWindow* parent)
  : wxWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
{
  SetBackgroundStyle(wxBG_STYLE_PAINT); // Mandatory for Windows OS
  Bind(wxEVT_PAINT, &SelectablePane::OnPaint, this);
}

void SelectablePane::OnPaint(wxPaintEvent& event)
{
  wxAutoBufferedPaintDC dc(this);
  dc.SetBackground(wxBrush(this->GetParent()->GetBackgroundColour()));
  dc.Clear();

  auto gc = wxGraphicsContext::Create(dc);
  if (gc)
  {
    wxRect selectionRect{0, 0, this->GetSize().GetWidth(), this->GetSize().GetHeight()};
    selectionRect.Deflate(FromDIP(1));

    wxRect contentRect = selectionRect;
    contentRect.Deflate(FromDIP(2));

    const auto roundness = FromDIP(4);

    DrawingContent(gc, contentRect, roundness);

    if (selected)
    {
      gc->SetPen(wxSystemSettings::GetAppearance().IsDark() ? *wxWHITE_PEN : *wxBLACK_PEN);
      gc->SetBrush(*wxTRANSPARENT_BRUSH);
      gc->DrawRoundedRectangle(selectionRect.GetX(), selectionRect.GetY(), selectionRect.GetWidth(), selectionRect.GetHeight(), roundness);
    }
  }

  delete gc;
}