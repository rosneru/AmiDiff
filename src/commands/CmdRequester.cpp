#include <clib/exec_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/intuition_protos.h>
#include <intuition/intuition.h>
#include "CmdRequester.h"


CmdRequester::CmdRequester(Array<WindowBase*>& windowArray,
                           const SimpleString& message,
                           const char* pTitle,
                           const char* pButtons)
  : CommandBase(windowArray),
    m_Message(message),
    m_pTitle(pTitle),
    m_pButtons(pButtons)
{

}

CmdRequester::~CmdRequester()
{

}

void CmdRequester::Execute(struct Window* pActiveWindow)
{
  // Disable the "About..." item (this command) in all menus
  DisableInAllWindowMenus();

  // Set the busy pointer for all windows
  enableBusyPointerForAllWindows();

  // Display the about dialog
  showRequester(pActiveWindow);

  // Set the normal pointer for all windows
  disableBusyPointerForAllWindows();

  // Enable the "About..." item (this command) in all menus
  EnableInAllWindowMenus();
}

long CmdRequester::showRequester(struct Window* pActiveWindow)
{
  if((m_pTitle == NULL) || (m_pButtons == NULL))
  {
    return 0;
  }

  //
  // Build the EasyRequest manually. This low-level approach allows
  // better control of the requests behavior. For example it gives
  // access to the event loop where the windows messages can be
  // received. In this way it can be detected if any of the windows has
  // been reseized and it can be repainted.
  //

  struct EasyStruct easyStruct;
  easyStruct.es_StructSize = sizeof(easyStruct);
  easyStruct.es_Flags = 0;
  easyStruct.es_Title = (UBYTE*)m_pTitle;
  easyStruct.es_TextFormat = (UBYTE*)m_Message.C_str();
  easyStruct.es_GadgetFormat = (UBYTE*)m_pButtons;

  struct Window* pRequesterWindow = BuildEasyRequestArgs(pActiveWindow, 
                                                         &easyStruct, 
                                                         0, 
                                                         NULL);

  if (pRequesterWindow == NULL)
  {
    return 0;
  }

  ULONG requestWindowFlags = 1UL << pRequesterWindow->UserPort->mp_SigBit;

  ULONG activeWindowFlags = 0;
  if (pActiveWindow != NULL)
  {
    activeWindowFlags = 1UL << pActiveWindow->UserPort->mp_SigBit;
  }

  long input = -1;
  do
  {
    ULONG flags = Wait(requestWindowFlags | activeWindowFlags);
    if (flags & activeWindowFlags)
    {
      struct IntuiMessage* pMsg;
      while ((pMsg = GT_GetIMsg(pActiveWindow->UserPort)) != NULL)
      {
        switch (pMsg->Class)
        {
          // One of the windows has been resized
          case IDCMP_NEWSIZE:
            for(size_t i = 0; i < m_Windows.Size(); i++)
            {
              if(m_Windows[i]->IntuiWindow() == pMsg->IDCMPWindow)
              {
                // Re-paint the resized window
                m_Windows[i]->Resized();
                break;
              }
            }
            break;

          // One of the windows must be refreshed
          case IDCMP_REFRESHWINDOW:
            GT_BeginRefresh(pMsg->IDCMPWindow);
            GT_EndRefresh(pMsg->IDCMPWindow, TRUE);
            break;
        }

        GT_ReplyIMsg(pMsg);
      }
    }

    if (flags & requestWindowFlags)
    {
      input = SysReqHandler(pRequesterWindow, NULL, FALSE);
    }

  } while (input < 0);

  FreeSysRequest(pRequesterWindow);
  return input;
}
