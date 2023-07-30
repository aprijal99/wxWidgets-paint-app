#include <wx/wx.h>
#include "App.hpp"
#include "Frame.hpp"

bool App::OnInit()
{
  wxInitAllImageHandlers();

  Frame* frame = new Frame("Paint");
  frame->Show(true);

  return true;
}

wxIMPLEMENT_APP(App);