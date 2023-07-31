#pragma

#include <wx/wx.h>
#include <wx/graphics.h>
#include <vector>
#include "Path.hpp"

class DrawingCanvas: public wxWindow
{
public:
  DrawingCanvas(wxWindow* parent);
  ~DrawingCanvas() noexcept {};
  void ShowExportDialog();

  const std::vector<Path>& GetSquiggles() { return squiggles; }
  void SetSquiggles(const std::vector<Path>&& squiggles)
  {
    this->squiggles = std::move(squiggles);
    Refresh();
  }

  int currentWidth{1};
  wxColour currentColor{*wxBLACK};

private:
  void DrawOnContext(wxGraphicsContext* gc);
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