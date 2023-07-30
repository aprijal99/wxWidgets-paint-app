#include <wx/wx.h>
#include <wx/graphics.h>
#include <wx/dcbuffer.h>
#include <vector>
#include "DrawingCanvas.hpp"
#include "Path.hpp"

DrawingCanvas::DrawingCanvas(wxWindow* parent)
  : wxWindow(parent, wxID_ANY)
{
  this->SetBackgroundStyle(wxBG_STYLE_PAINT); // Mandatory for Windows OS
  this->Bind(wxEVT_PAINT, &DrawingCanvas::OnPaint, this);
  this->Bind(wxEVT_LEFT_DOWN, &DrawingCanvas::OnMouseDown, this);
  this->Bind(wxEVT_MOTION, &DrawingCanvas::OnMouseMove, this);
  this->Bind(wxEVT_LEFT_UP, &DrawingCanvas::OnMouseUp, this);
  this->Bind(wxEVT_LEAVE_WINDOW, &DrawingCanvas::OnMouseLeave, this);

  BuildContextMenu();

  this->Bind(wxEVT_CONTEXT_MENU, &DrawingCanvas::OnContextMenuEvent, this);
}

void DrawingCanvas::OnPaint(wxPaintEvent&)
{
  wxAutoBufferedPaintDC dc(this);
  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();

  wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
  if (gc)
  {
    for (const auto& s: squiggles)
    {
      auto pointsVector = s.points;
      if (pointsVector.size() > 1)
      {
        gc->SetPen(wxPen(s.color, s.width));
        gc->StrokeLines(pointsVector.size(), pointsVector.data());
      }
    }
  }

  delete gc;
}

void DrawingCanvas::OnMouseDown(wxMouseEvent&)
{
  squiggles.push_back({{}, currentColor, currentWidth});
  isDrawing = true;
}

void DrawingCanvas::OnMouseMove(wxMouseEvent& event)
{
  if (isDrawing)
  {
    auto pt = event.GetPosition();
    auto& currentSquiggle = squiggles.back();

    currentSquiggle.points.push_back(pt);
    Refresh();
  }
}

void DrawingCanvas::OnMouseUp(wxMouseEvent&)
{
  isDrawing = false;
}

void DrawingCanvas::OnMouseLeave(wxMouseEvent&)
{
  isDrawing = false;
}

void DrawingCanvas::BuildContextMenu()
{
  auto clear = contextMenu.Append(wxID_ANY, "&Clear");
  contextMenu.Append(wxID_ANY, "Save &As");

  this->Bind(
    wxEVT_MENU,
    [this](wxCommandEvent&)
    {
      this->squiggles.clear();
      this->Refresh();
    },
    clear->GetId()
  );
}

void DrawingCanvas::OnContextMenuEvent(wxContextMenuEvent& event)
{
  auto clientPos = event.GetPosition() == wxDefaultPosition
  ? wxPoint(this->GetSize().GetWidth() / 2, this->GetSize().GetHeight() / 2)
  : this->ScreenToClient(event.GetPosition());

  PopupMenu(&this->contextMenu, clientPos);
}