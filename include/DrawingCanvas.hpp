#pragma

#include <wx/wx.h>
#include <vector>
#include "Path.hpp"

class DrawingCanvas: public wxWindow
{
public:
  DrawingCanvas(wxWindow* parent);
  ~DrawingCanvas() noexcept {};

  int currentWidth{1};
  wxColour currentColor{*wxBLACK};

private:
  void OnPaint(wxPaintEvent&);
  void OnMouseDown(wxMouseEvent&);
  void OnMouseMove(wxMouseEvent&);
  void OnMouseUp(wxMouseEvent&);
  void OnMouseLeave(wxMouseEvent&);

  bool isDrawing{};
  std::vector<Path> squiggles;

  wxMenu contextMenu;
  void BuildContextMenu();
  void OnContextMenuEvent(wxContextMenuEvent&);
};