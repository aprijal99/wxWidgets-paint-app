#pragma once

#include <wx/wx.h>
#include <wx/wrapsizer.h>
#include <wx/splitter.h>
#include <string>
#include <vector>
#include "ColorPane.hpp"
#include "PenSizePane.hpp"
#include "DrawingCanvas.hpp"
#include "XmlSerializer.hpp"

class Frame: public wxFrame
{
public:
  Frame(const wxString& title);

private:
  wxPanel* BuildControlsPanel(wxWindow* parent);
  void SetupColorPanes(wxWindow* parent, wxSizer* sizer);
  void SetupPenPanes(wxWindow* parent, wxSizer* sizer);
  void SelectColorPane(ColorPane* pane);
  void SelectPenPane(PenSizePane* pane);
  void LoadFromXml();
  void SaveToXml();

  std::vector<ColorPane*> colorPanes{};
  std::vector<PenSizePane*> penPanes{};

  DrawingCanvas* canvas;
  XmlSerializer serializer{};

  const std::string lightBackground = "#f4f3f3";
  const std::string darkBackground = "#2c2828";
  const std::vector<std::string> niceColors = {
    "#000000", "#ffffff", "#fd7f6f", "#7eb0d5", "#b2e061", "#bd7ebe",
    "#ffb55a", "#ffee65", "#beb9db", "#fdcce5", "#8bd3c7"
  };
  const int penCount = 6;
};

Frame::Frame(const wxString& title)
  : wxFrame(nullptr, wxID_ANY, title)
{
  wxSplitterWindow* splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);
  splitter->SetMinimumPaneSize(FromDIP(150));

  auto controlsPanel = BuildControlsPanel(splitter);
  canvas = new DrawingCanvas(splitter);

  splitter->SplitVertically(controlsPanel, canvas);
  splitter->SetSashPosition(FromDIP(220));

  this->SetSize(FromDIP(1000), FromDIP(700));
  this->SetMinSize(wxSize(FromDIP(400), FromDIP(200)));

  SelectColorPane(colorPanes[0]);
  SelectPenPane(penPanes[0]);
}

void Frame::SetupColorPanes(wxWindow* parent, wxSizer* sizer)
{
  for (const auto& color: niceColors)
  {
    auto colorPane = new ColorPane(parent, wxColour(color));
    colorPane->Bind(wxEVT_LEFT_DOWN, [this, colorPane](wxMouseEvent& event) { SelectColorPane(colorPane); });

    colorPanes.push_back(colorPane);
    sizer->Add(colorPane, 0, wxRIGHT | wxBOTTOM, FromDIP(5));
  }
}

void Frame::SetupPenPanes(wxWindow* parent, wxSizer* sizer)
{
  for (int i = 0; i < penCount; i++)
  {
    auto penPane = new PenSizePane(parent, i * FromDIP(4) + FromDIP(1));
    penPane->Bind(wxEVT_LEFT_DOWN, [this, penPane](wxMouseEvent& event) { SelectPenPane(penPane); });

    penPanes.push_back(penPane);
    sizer->Add(penPane, 0, wxRIGHT | wxBOTTOM, FromDIP(5));
  }
}

wxPanel* Frame::BuildControlsPanel(wxWindow* parent)
{
  auto controlsPanel = new wxScrolled<wxPanel>(parent, wxID_ANY);
  controlsPanel->SetScrollRate(0, FromDIP(10));
  auto mainSizer = new wxBoxSizer(wxVERTICAL);

  // Set controls pane background color
  bool isDark = wxSystemSettings::GetAppearance().IsDark();
  controlsPanel->SetBackgroundColour(wxColour(isDark ? darkBackground : lightBackground));
  
  // Add 'Colors' title
  auto colorTitle = new wxStaticText(controlsPanel, wxID_ANY, "Colors");
  colorTitle->SetForegroundColour(wxColour(*wxWHITE));
  mainSizer->Add(colorTitle, 0, wxALL, FromDIP(5));

  // Color panel
  auto colorPaneSizer = new wxWrapSizer(wxHORIZONTAL);
  SetupColorPanes(controlsPanel, colorPaneSizer);
  mainSizer->Add(colorPaneSizer, 0, wxALL, FromDIP(5));

  // Add 'Pens' title
  auto penTitle = new wxStaticText(controlsPanel, wxID_ANY, "Pens");
  penTitle->SetForegroundColour(wxColour(*wxWHITE));
  mainSizer->Add(penTitle, 0, wxALL, FromDIP(5));

  // Pen size panel
  auto penPaneSizer = new wxWrapSizer(wxHORIZONTAL);
  SetupPenPanes(controlsPanel, penPaneSizer);
  mainSizer->Add(penPaneSizer, 0, wxALL, FromDIP(5));

  // Buttons
  auto exportButton = new wxButton(controlsPanel, wxID_ANY, "Export");
  exportButton->SetCursor(wxCursor(wxCURSOR_HAND));
  exportButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { canvas->ShowExportDialog(); });
  
  auto saveButton = new wxButton(controlsPanel, wxID_ANY, "Save As");
  saveButton->SetCursor(wxCursor(wxCURSOR_HAND));
  saveButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { SaveToXml(); });
  
  auto loadButton = new wxButton(controlsPanel, wxID_ANY, "Load");
  loadButton->SetCursor(wxCursor(wxCURSOR_HAND));
  loadButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) { LoadFromXml(); });
  
  mainSizer->AddStretchSpacer();
  mainSizer->Add(exportButton, 0, wxALL, FromDIP(5));
  mainSizer->Add(saveButton, 0, wxALL, FromDIP(5));
  mainSizer->Add(loadButton, 0, wxALL, FromDIP(5));
  mainSizer->AddSpacer(FromDIP(5));

  controlsPanel->SetSizer(mainSizer);

  return controlsPanel;
}

void Frame::SelectColorPane(ColorPane* pane)
{
  for (auto colorPane: colorPanes)
  {
    colorPane->selected = (colorPane == pane);
    colorPane->Refresh();
  }

  canvas->currentColor = pane->color;
}

void Frame::SelectPenPane(PenSizePane* pane)
{
  for (auto penPane: penPanes)
  {
    penPane->selected = (penPane == pane);
    penPane->Refresh();
  }

  canvas->currentWidth = pane->penWidth;
}

void Frame::LoadFromXml()
{
  wxFileDialog dialog(this, "Load", "", "", "PXZ files (*.pxz)|*.pxz", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (dialog.ShowModal() == wxID_CANCEL) return;

  wxXmlDocument doc = serializer.DecompressXml(dialog.GetPath());
  if (!doc.IsOk())
  {
    wxMessageBox("Failed to load file");
    return;
  }

  canvas->SetSquiggles(serializer.DeserializePaths(doc));
}

void Frame::SaveToXml()
{
  wxFileDialog dialog(this, "Save As", "", "", "PXZ files (*.pxz)|*.pxz", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
  if (dialog.ShowModal() == wxID_CANCEL) return;

  auto doc = serializer.SerializePaths(canvas->GetSquiggles());
  serializer.CompressXml(doc, dialog.GetPath());
}