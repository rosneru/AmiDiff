#include <string.h>

#include <clib/alib_protos.h>
#include <clib/asl_protos.h>
#include <clib/dos_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/icclass.h>
#include <libraries/asl.h>
#include <libraries/dos.h>
#include "WindowBase.h"

WindowBase::WindowBase(AppScreen* p_pAppScreen)
  : m_pAppScreen(p_pAppScreen),
    m_pWindow(NULL)
{

}

WindowBase::~WindowBase()
{
  Close();
}


void WindowBase::Close()
{
  if(m_pWindow != NULL)
  {
    CloseWindow(m_pWindow);
    m_pWindow = NULL;
  }
}

const char* WindowBase::Title()
{
  return m_Title.C_str();
}

void WindowBase::SetTitle(SimpleString p_NewTitle)
{
  m_Title = p_NewTitle;

  // Call intuition function to set the window title
  // Note the ~0 inverts the value and is a value of -1
  SetWindowTitles(m_pWindow, m_Title.C_str(), (STRPTR) ~0);
}

struct Window* WindowBase::IntuiWindow()
{
  return m_pWindow;
}
