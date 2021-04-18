#ifndef SEARCH_WINDOW_H
#define SEARCH_WINDOW_H

#include <string>
#include <vector>

#include <exec/ports.h>
#include <intuition/screens.h>
#include <workbench/workbench.h>

#include "OpenScreenBase.h"
#include "CommandBase.h"
#include "CmdSearch.h"
#include "CmdFileRequester.h"
#include "WindowBase.h"

/**
 * The search window.
 *
 *
 * @author Uwe Rosner
 * @date 11/04/2021
 */
class SearchWindow : public WindowBase
{
public:
  SearchWindow(std::vector<WindowBase*>& windowArray,
              ScreenBase& screen,
              struct MsgPort* pIdcmpMsgPort,
              CmdSearch& cmdSearch,
              CommandBase& cmdCloseSearchWindow);

  virtual ~SearchWindow();

  void Refresh();

  /**
   * Opening the window.
   *
   * @returns
   * When ok: true, false if opening fails
   */
  bool open(InitialPosition initialPos = WindowBase::IP_Center);

  /**
   * Handles given IDCMP event.
   *
   * @returns
   * If this event was handled: true; if it was not handled: false..
   */
  void handleIDCMP(ULONG msgClass, UWORD msgCode, APTR pItemAddress);


private:
  CmdSearch& m_CmdSearch;
  CommandBase& m_CmdCloseSearchWindow;

  const ULONG m_MaxPathLength;

  /**
   * IDs to help to interpret the events of this window's Gadtools
   * gadgets.
   */
  enum GadgetId
  {
    GID_StrSearchText,
    GID_CycLocation,
    GID_CbxIgnoreCase,
    GID_BtnFind,
  };

  struct Gadget* m_pGadtoolsContext;
  struct Gadget* m_pGadStrSearchText;
  struct Gadget* m_pGadCycLocation;
  struct Gadget* m_pGadCbxIgnoreCase;
  struct Gadget* m_pGadBtnFind;

  //
  // The next two are called from HandleIDCMP() to get that method not
  // overblown.
  //
  void handleGadgetEvent(struct Gadget* pGadget);
  void handleVanillaKey(UWORD code);

  void find();

  void cleanup();

};


#endif
